
#define VERSION "MQ135_Air_Quality_2024_0807"

/*
  This software incorporates the MQ135 library to do perform two functions:
  1. When the #define CALIBRATE statement is active, calibarate the MQ135 sensor 
  2. When the #define CALIBRATE statement is inactive, read the MQ135 sensor and report the CO2 ppm data on the following platforms:
  
    computer monitor via USB connection
    OLED display via I2C connection
    Thingspeak.com via WIFI
    Home Assistant via WIFI and MQTT

  NOTE - The device automatically ignores the platforms for which it is not credentialed or connected
*/    
 
//////////////////////////////////////////////////////////////  
//*******     Setup secrets.h file               ***********//
//                                                          //
//////////////////////////////////////////////////////////////

  #include <secrets.h>   
  
/* 
  secrets.h file contents follow (optional):

    #define SECRET_WIFI_SSID           "your wifi ssid"
    #define SECRET_WIFI_PASSWORD       "your wifi password" 

  Home Assistant or other MQTT broker (optional):
    #define SECRET_MQTT_HOST "your mqtt host"  //12/29/2022
    #define SECRET_MQTT_PORT 1883
    #define SECRET_MQTT_USER_NAME "your mqtt user name"
    #define SECRET_MQTT_USER_PWD  "your mqtt password" 

  Thingspeak API key (optional):
    #define SECRET_THINGSPEAK_API_KEY = "xxxxx"
*/
////////////////////////////////////////////////////////////////////

//Tested on esp8266 (NodeMCU 0.9 (ESP-12 Module)

//#define CALIBRATE         // Comment out if not in calibration mode
#define PIN_MQ135 A0      // Analog pin on the ESP8266, ESP32, etc which connects to the analog output of the sesor. 
#define RZERO 51.5        // Measured resistance from calibration (8/8/2024 75F, 63% RH in my case)
unsigned long reportPeriod_msec = 15000;        // Report period in msec

//Sensor stuff
#include "MQ135.h"
MQ135 gasSensor(PIN_MQ135, RZERO);

//Wifi stuff:
#include <ESP8266WiFi.h>
#define WIFI_SSID SECRET_WIFI_SSID  
#define WIFI_PASSWORD SECRET_WIFI_PASSWORD
WiFiClient client;

//Display stuff
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Timezone stuff
#include <ezTime.h>
#define MY_TIMEZONE "America/New_York"               // <<<<<<< use Olson format: https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
#define TIMEZONE_EEPROM_OFFSET 0                     // location-to-timezone info - saved in case eztime server is down
Timezone myTZ;

//MQTT stuff for Home Assistant
#include <PubSubClient.h>
PubSubClient mqttClient(client);
#define MQTT_USER_NAME SECRET_MQTT_USER_NAME         
#define MQTT_PASSWORD SECRET_MQTT_USER_PWD            
#define MQTT_SERVER SECRET_MQTT_HOST
#define DEVICE_NAME "mq135"
#define LWT_TOPIC "mq135/status/LWT"             // MQTT Last Will & Testament
#define MQ135_TOPIC "aqi/mq135"
#define VERSION_TOPIC "mq135/report/version"     // report software version at connect
#define RECV_COMMAND_TOPIC "mq135/cms/#"
#define MSG_BUFFER_SIZE 512                          // for MQTT message payload (growth)
char msg[MSG_BUFFER_SIZE];
unsigned long lastReconnectAttempt = 0;
unsigned long lastPublish = 0, lastRead = 0, lastPressErrReport = 0;
unsigned long tempNow, lastPublishNow, sensorReadNow, mqttNow; //

//ThingSpeak stuff
const char* server = "api.thingspeak.com";
String THINGSPEAK_API_KEY = SECRET_THINGSPEAK_API_KEY;  //does not compile as #define WHY???

//   ***********************
//   **  MQTT reconnect() **
//   ***********************

boolean reconnect()
{
  // PubSubClient::connect(const char *id, const char *user, const char *pass, const char* willTopic, uint8_t willQos, boolean willRetain, const char* willMessage)
  if (mqttClient.connect(DEVICE_NAME, MQTT_USER_NAME, MQTT_PASSWORD, LWT_TOPIC, 2, true, "Disconnected"))
  {
    Serial.print(F("MQTT connected to "));
    Serial.println(F(MQTT_SERVER));

    // Publish MQTT announcements...
    mqttClient.publish(LWT_TOPIC, "Connected", true); // let broker know we're connected
    Serial.printf("\n%s MQTT SENT: %s/Connected\n", myTZ.dateTime("[H:i:s.v]").c_str(), LWT_TOPIC);

    mqttClient.publish(VERSION_TOPIC, VERSION, true); // report firmware version
    Serial.printf("%s MQTT SENT: Firmware %s\n", myTZ.dateTime("[H:i:s.v]").c_str(), VERSION);

    // Subscribe
    mqttClient.subscribe(RECV_COMMAND_TOPIC);
  }
  return mqttClient.connected();
}

//   ***********************
//   **  MQTT callback()  **
//   ***********************

void callback(char *topic, byte *payload, unsigned int length)
{
  strncpy(msg, (char *)payload, length);
  msg[length] = (char)NULL; // terminate the string
  Serial.printf("\n%s MQTT RECVD: %s/%s \n", myTZ.dateTime("[H:i:s.v]").c_str(), topic, msg);

  // All commands must be prefixed with RECV_COMMAND_TOPIC
  //   reboot         - reboots device
  //   report_period_ms/<new_value>
                            
  if (strstr(topic, "report_period_ms")) // report period in msec
  {
    if (atoi(msg) >= 1)
    {
      Serial.printf("reportPeriod_msec set to %s\n", msg);
      reportPeriod_msec = atoi(msg);
    }
  }

  if (strstr(topic, "reboot"))
  {
    Serial.println(F("MQTT reboot command received.  Rebooting..."));
    delay(5000);
    mqttClient.disconnect();
    delay(1000);
    WiFi.disconnect();
    delay(1000);
    ESP.restart();
  }
  msg[0] = (char)NULL; // clear msg
}

//   ***********************
//   **  setup()  **
//   ***********************
void setup()
{

  Serial.begin(115200);

  //Setup OLED Display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //initialize with the I2C addr 0x3C (128x64)
  display.clearDisplay();
  delay(10);
  display.clearDisplay();
  display.setCursor(0,0);  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println("Connecting to ");
  display.setTextSize(2);
  display.print(WIFI_SSID);
  display.display();

  //Setup wifi
  Serial.println("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  //Update display
  display.clearDisplay();
  display.setCursor(0,0);  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("WiFi connected");
  display.display();

  //Setup mqtt
  mqttClient.setBufferSize(MSG_BUFFER_SIZE);
  mqttClient.setServer(MQTT_SERVER, 1883);
  mqttClient.setCallback(callback);
  lastReconnectAttempt = 0;

  delay(4000);
}
 
//   ***********************
//   **  loop()  **
//   ***********************
void loop()
{
  #ifdef CALIBRATE
    //****Read R Zero
    float value = gasSensor.getRZero();
    Serial.print("R Zero Ohms: ");  
    Serial.println(value);
//    Serial.println();
  #else
     //****Read air quality sensor
    float value = gasSensor.getPPM();
    Serial.print("Air Quality: "); 
    Serial.println(value);
//    Serial.println();
    Serial.print("Resistance: "); 
  #endif
  
  //****Display AQI value on OLED
  display.clearDisplay();
  display.setCursor(0,0);  //oled display
  display.setTextSize(1);
  display.setTextColor(WHITE);
  #ifdef CALIBRATE
    display.println("Calibration");
  #else
    display.println("Air Quality Index");
  #endif  
  display.setCursor(0,20);  //oled display
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println(value);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  #ifdef CALIBRATE
   display.println(" OHMS");
  #endif
//  display.display();

  //***verify mqtt connection
  if (!mqttClient.connected())
  {
    mqttNow = millis();
    if (mqttNow - lastReconnectAttempt > 1000)
    {
      Serial.printf("[%s] Waiting for MQTT...\n", myTZ.dateTime(RFC3339).c_str());
      lastReconnectAttempt = mqttNow;
      // Attempt to reconnect
      if (reconnect())
      {
        lastReconnectAttempt = 0;
      }
    }
  }
  else
  {
    // Client connected
    mqttClient.loop();
  }
  
  //****Publish MQTT for Home Assistant
  lastPublishNow = millis();
  if(mqttClient.connected() ){
    sprintf(msg, "%.2f", value);
    mqttClient.publish(MQ135_TOPIC, msg, true);
    lastPublish = millis(); 
  }      

  //****Publish Thingspeak
  if (client.connect(server, 80)) // "184.106.153.149" or api.thingspeak.com
  {
    String postStr = THINGSPEAK_API_KEY;
    postStr += "&field1=";
    postStr += String(value);
    postStr += "r\n";
    
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + THINGSPEAK_API_KEY + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    
    Serial.println("Data Sent to Thingspeak");
  }

  //****Pause to throttle data rate thingspeak needs minimum 15 sec delay between updates.
  client.stop();
  Serial.println("Waiting...");
  delay(reportPeriod_msec);
}


#define VERSION "MQ135_Air_Quality_2024_0822"

/*
   IMPORTANT - read project description at
   https://github.com/Bobbo117/MQ135-Air-Quality-Sensor

  This software incorporates the MQ135 library to do perform two functions:
  1. When the #define CALIBRATE statement is active, calibarate the MQ135 sensor 
  2. When the #define CALIBRATE statement is inactive, read the MQ135 sensor and report the CO2 ppm data on the following platforms:
  
    computer monitor via USB connection
    OLED display via I2C connection
    Thingspeak.com via WIFI if the #define THINGSPEAK statement is enabled
    Home Assistant via WIFI and MQTT if the #define MQTT statement is enabled

*/    
 
//////////////////////////////////////////////////////////////  
//*******     Setup secrets.h file               ***********//
//                                                          //
//////////////////////////////////////////////////////////////

  #include <secrets.h>   //<--*** if you want to report to Thingspeak or MQTT
  
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
//////////////////////////////////////////////////////////////  
//*******         Compile-time Options           ***********//
//        (Disable unwanted options with leading //)
//////////////////////////////////////////////////////////////

//#define CALIBRATE_         // Comment out if not in calibration mode
#define OLED_
#define WIFI_
#define THINGSPEAK_
#define MQTT_

//////////////////////////////////////////////////////////////  
//*******         Compile-time Parameters           ***********//
// IMPORTANT - read project description at
// https://github.com/Bobbo117/MQ135-Air-Quality-Sensor
//////////////////////////////////////////////////////////////

#define PIN_MQ135 A0       // Analog pin on the ESP8266 which connects to the analog output of the sesor. 
#define RZERO 9.47 //5.93 //51.5         // Measured resistance from calibration (8/8/2024 75F, 63% RH in my case)
#define RLOAD 1.612        // Measured resistance Kohms pin A0 to GND with power off

//////////////////////////////////////////////////////////////  
//*******         Libraries          ***********//
// Use the Arduino Library Manager to install libraries for selected compile-time options 
// 
//////////////////////////////////////////////////////////////

//***Select one microprocessor:  (Disable unwanted options with leading //)
//#define ARDUINO_
//#define ESP32_
#define ESP8266_      //Only this option has been verified

//***Sensor library
#include "MQ135.h"        // <-- from Arduino Library Manager
MQ135 gasSensor(PIN_MQ135, RZERO, RLOAD);

//***Wifi libraries
#ifdef WIFI_
  #ifdef ESP32_
    #include <WiFi.h>        
   #endif    
  #ifdef ESP8266_
    #include <ESP8266WiFi.h>
  #endif  
  #define WIFI_SSID SECRET_WIFI_SSID  
  #define WIFI_PASSWORD SECRET_WIFI_PASSWORD
  WiFiClient client;
#endif

//***Display libraries
#ifdef OLED_
    #include <Wire.h>
    #include "SSD1306Ascii.h"
    #include "SSD1306AsciiWire.h"
    #define I2C_ADDRESS 0x3C            // 0X3C+SA0 - 0x3C or 0x3D
    #define OLED_RESET     -1           // Reset pin # (or -1 if sharing Arduino reset pin)
    SSD1306AsciiWire oled;
#endif

#ifdef MQTT_
  //***Timezone libraries
  #include <ezTime.h>
  #define MY_TIMEZONE "America/New_York"               // <<<<<<< use Olson format: https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
  #define TIMEZONE_EEPROM_OFFSET 0                     // location-to-timezone info - saved in case eztime server is down
  Timezone myTZ;

 //***MQTT libraries and parameters, topics
  #include <PubSubClient.h>
  PubSubClient mqttClient(client);
  #define MQTT_USER_NAME SECRET_MQTT_USER_NAME         
  #define MQTT_PASSWORD SECRET_MQTT_USER_PWD            
  #define MQTT_SERVER SECRET_MQTT_HOST
  #define DEVICE_NAME "mq135"
  #define LWT_TOPIC "mq135/status/LWT"             // MQTT Last Will & Testament
  #define MQ135_TOPIC "aqi/mq135"                  // MQTT topic for sensor
  #define VERSION_TOPIC "mq135/report/version"     // report software version at connect
  #define RECV_COMMAND_TOPIC "mq135/cms/#"
  #define MSG_BUFFER_SIZE 512                          // for MQTT message payload (growth)
  char msg[MSG_BUFFER_SIZE];
  unsigned long lastReconnectAttempt = 0;
  unsigned long lastPublish = 0, lastRead = 0, lastPressErrReport = 0;
  unsigned long tempNow, lastPublishNow, sensorReadNow, mqttNow; //
#endif

//***ThingSpeak parameters
#ifdef THINGSPEAK_
  const char* server = "api.thingspeak.com";
  String THINGSPEAK_API_KEY = SECRET_THINGSPEAK_API_KEY;  
#endif

//***Variable Definitions

  unsigned long reportPeriod_msec = 15000;        // Report period in msec
  float ppmArray[8];                             // array used to store and calculate rolling average of ppm data
  int ppmArrayLength = 8;                        // array length 
  int validArrayData = 0;                        // number of entries in the array which are valid data for use in rolling average
  
//   ***********************
//   **  MQTT reconnect() **
//   ***********************

#ifdef MQTT_
  boolean reconnect()
  {
    // PubSubClient::connect(const char *id, const char *user, const char *pass, const char* willTopic, uint8_t willQos, boolean willRetain, const char* willMessage)
    if (mqttClient.connect(DEVICE_NAME, MQTT_USER_NAME, MQTT_PASSWORD, LWT_TOPIC, 2, true, "Disconnected"))
    {
      Serial.print(F("MQTT connected to "));
      Serial.println(F(MQTT_SERVER));
  
      // Publish MQTT announcements...
      mqttClient.publish(LWT_TOPIC, "Connected", true); // let broker know we're connected
      Serial.printf("%s MQTT SENT: %s/Connected\n", myTZ.dateTime("[H:i:s.v]").c_str(), LWT_TOPIC);
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
#endif

//   ***********************
//   **  setup()  **
//   ***********************

void setup()
{
  Serial.begin(115200);
  ppmArray[0]=1;                                      // ppmArray[0] is index pointing to first data location

  //***Setup OLED Display
  #ifdef OLED_
    Wire.begin();
    #if OLED_RESET  >= 0
      oled.begin(&Adafruit128x64, I2C_ADDRESS, OLED_RESET);
    #else 
      oled.begin(&Adafruit128x64, I2C_ADDRESS);
    #endif
    oled.clear();
    oled.setFont(fixed_bold10x15);
    oled.setCursor(0,0); 
    #ifdef WIFI_
      oled.println ("Connecting..");
      oled.println(WIFI_SSID);
    #endif  
  #endif
  
  //***Setup wifi
  #ifdef WIFI_
    Serial.println("Connecting.. ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WIFI connected");

    #ifdef OLED_
      oled.println("connected");
    #endif
  #endif    

  //***Setup mqtt
  #ifdef MQTT_
    mqttClient.setBufferSize(MSG_BUFFER_SIZE);
    mqttClient.setServer(MQTT_SERVER, 1883);
    mqttClient.setCallback(callback);
    lastReconnectAttempt = 0;
  #endif
  
  delay(4000);
}
 
//   ***********************
//   **  loop()  **
//   ***********************

void loop()
{            
  //***print to Serial Monitor
  #ifdef CALIBRATE_
    //****Read sensor resistance
    float value = gasSensor.getRZero();
    Serial.print("R Zero Ohms: ");  
    Serial.println(value);
  #else
     //****Read air quality sensor
    float value = gasSensor.getPPM();
    Serial.print("Air Quality: "); 
    Serial.println(value);
  #endif

  //***Store in array
  ppmArray[int(ppmArray[0])]=value;
  //Serial.print(ppmArray[0]); Serial.print(": ");Serial.println(ppmArray[int(ppmArray[0])]);
  ppmArray[0]++;
  if (int(ppmArray[0]) > ppmArrayLength-1)
  {
    ppmArray[0]=1;
  }  
  if (validArrayData < ppmArrayLength-1)
  {
    validArrayData ++;
  }
  
  //***Compute rolling average
  float total = 0;
  for (int i=1; i< validArrayData+1; i++)
  {
    total=total+ppmArray[i];   
  }
  float avg = total/(validArrayData);   
  Serial.print("Rolling Average ");Serial.print(validArrayData); Serial.print(": ");Serial.println(avg);
  
  //****Display AQI value on OLED
  #ifdef OLED_
    oled.clear();
    oled.setCursor(0,0); 
    #ifdef CALIBRATE_
      oled.println("Calibration");
      //oled.println("");
      oled.print("R0: ");
      oled.println(value);
      oled.print("Average "); oled.print(validArrayData); oled.println(":");
      oled.print(avg);;oled.print(" kOhms");
    #else
      oled.println("Air Quality Index");
      oled.print(int(value));oled.println(" PPM");
      oled.print("Average "); oled.print(validArrayData); oled.println(":");
      oled.print(int(avg));oled.print(" PPM");
    #endif   
  #endif

  value=avg;                      // Report the avg to MQTT, Thingspeak to eliminate jitter
  
  //***verify mqtt connection
  #ifdef MQTT_
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
  
    //****Publish MQTT
    lastPublishNow = millis();
    if(mqttClient.connected() )
    {
      sprintf(msg, "%.2f", value);
      mqttClient.publish(MQ135_TOPIC, msg, true);
      lastPublish = millis(); 
    }      
  #endif
  
  //****Publish Thingspeak
  #ifdef THINGSPEAK_
    if (client.connect(server, 80)) // "184.106.153.149" or api.thingspeak.com
    {
      String httpPost = THINGSPEAK_API_KEY+"&field1="+String(value);
      client.print("POST /update HTTP/1.1\n");
      client.print("Host: api.thingspeak.com\n");
      client.print("Connection: close\n");
      client.print("X-THINGSPEAKAPIKEY: " + THINGSPEAK_API_KEY + "\n");
      client.print("Content-Type: application/x-www-form-urlencoded\n");
      client.print("Content-Length: ");
      client.print(httpPost.length());
      client.print("\n\n");
      client.print(httpPost);
      client.stop();
      Serial.println("Published Thingspeak");
    }
  #endif
  
  //****Pause to throttle data rate.
  Serial.print(reportPeriod_msec/1000);Serial.println(" second pause..");Serial.println("");
  delay(reportPeriod_msec);
}

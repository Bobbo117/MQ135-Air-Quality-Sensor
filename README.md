# Project Description

This project connects the MQ135 sensor board to an Arduino-based controller to calibrate it outdoors and then monitor inside air quality, primarily CO2, using one or any combination of the following options:

a. Computer via usb to Arduino IDE
  
b. OLED display
  
c. Thingspeak.com
  
d. Home Assistant via MQTT

# MQ135 Air Quality Sensor Board

![image](https://github.com/user-attachments/assets/be24d9ab-758d-421a-aa32-b21a0685b0c2) ![image](https://github.com/user-attachments/assets/d0af170c-036d-4ee3-a305-74bce83af16a)

The MQ135 board has several components, including the MQ135 sensor, a power LED, an alarm LED, a variable resistor to adjust the alarm level that triggers the alarm LED and the D0 pin, a Load Resistor (RL), as well as additional circuitry associated with the digital output.  

The board has 4 pins:  Vcc (5v), GND, A0 (Analog voltage), and D0 (digital signal).  

The digital signal is low when the voltage exceeds the trigger level set by the potentiometer.

# MQ135 Air Quality Sensor

The MQ135 sensor ([datasheet here](https://www.olimex.com/Products/Components/Sensors/Gas/SNS-MQ135/resources/SNS-MQ135.pdf)) uses the presence of certain gases to change the electrical resistance of the sensor. 

The following excerpt from the datasheet depicts the internal structure of the sensor. The sensor itself does not include the load resister RL shown in the diagram or any of the board components.

![image](https://github.com/user-attachments/assets/4af4b039-8945-414b-9be0-84ea8c5ca75d)

The MQ135 sensor itself has 6 pins labeled A,A B,B and H,H.  The H pins connect to the heater coil, which measures 35 ohms without power.  

The A's are connected to each other, as are the B's.  They are the terminations for Rs, the sensor resistance.  

The A's are connected to Vcc (5 volts).  

The B's are connected to a board pin labeled A0 and to a fixed load resistor RL on the board whose other end is connected to the ground pin of the board. 

The following excerpt from the sensor datasheet specifies sensitivity characteristics. 

![image](https://github.com/user-attachments/assets/bfa19113-d05b-4a2e-a1c8-9897aea4ffdc)

Consider that the [CO2.Earth](https://www.co2.earth/) website indicates clean outside air contains more than 400 ppm CO2, and that inside air can exceed 1000 ppm easily.  

![image](https://github.com/user-attachments/assets/dbd9c7ad-27ea-407b-a662-c83ac738bfca)


The MQ135 properties suggest possible utility as a CO2 detector as well as a general air quality monitor.  

# MQ135 Board Concept of Operation

Each board may be slightly different.  All resistance measurements in this discussion pertain to my specific board.  

The characteristic curve of the sensor shows that increasing ppm reduces sensor resistance.  

The resistance of the sensor and the RL tied to A0 pin form a voltage divider whose voltage increases with increasing PPM (decreasing Rs). 

The sensor resistance, connected to pins Vcc and A0, and the Load resistance, connected between pins A0 and GND, form a voltage divider between Vcc and GND whose midpoint is pin A0. 

When the unit is powered up, the sensor resistance decreases as the concentration of detected gases (mostly CO2 under normal conditions) increases, causing the voltage across the load resistance at pin A0 to increase.  

Additional board circuitry enables setting a potentiometer to determine the level at which the D0 pin and LED trigger.

1. Measure the sensor resistance of the MQ135 board (without power applied) between the VCC and the A0 pin.  Mine measures 1750 ohms (1.750 Kohms).
   
2. Measure the load resistance between the A0 and GND pin.  Mine measures 1720 ohms (1.720 Kohm). You will need to enter the load resistance value in KOhms in the '#define RLOAD' statement in the 
 MQ135_Air_Quality.ino sotware.

# Hardware Connections

Use any arduino compatable controller you want - esp8266, esp32, or arduino board - by setting flags in the software.  I used the Lolin d1 mini esp8266.  The pinouts are shown below:

![image](https://github.com/user-attachments/assets/deb7757e-022d-40d1-9cd0-f9b4820fb36a)

![image](https://github.com/user-attachments/assets/30f0dc68-d9a5-49dc-9d5f-92cf92265db8)

Power the ESP8266 via USB connection.

MQ135 pins:

1. VCC to esp8266 Vcc (5v)
2. GND to esp8266 GND
3. A0 to esp8266 A0 via voltage divider *
4. D0 unused

* 170K and 330k voltage divider is used to reduce MQ135 5v max output from A0 to ESP8266 or esp32 3.3v input.  The voltage divider is not necessary if using an Arduino board that can handle 5v input voltage.
  
OLED Display pins (optional):  

![image](https://github.com/user-attachments/assets/74b7fedc-3a7a-4865-a162-2cde44a472d6)

1. VCC to esp8266 3.3v 
2. GND to esp8266 GND
3. SDA to esp8266 SDA
4. SCL to esp8266 SCL

# MQ135 Sensor Board Calibration

The datasheet suggests calibrating under strict conditions that would require an environmental chamber and with a specific load resistance (20K ohms).  

Since we don't have a chamber, and the board comes with a fixed load resistance (1.7K ohms) that may not comply, we calibrate the unit outdoors under clear conditions and hope for the best.  

Try to pick a day when outside temperature and humidity conditions match normal inside conditions. 

Also, consider using the EPA's ([AirNow App](https://www.airnow.gov/airnow-mobile-app/)) to verify air quality.

![airnow 13](https://github.com/Bobbo117/MQ135-Air-Quality-Sensor/blob/main/Images/EPA%20AQI%2013.jpg)

# Software

Use the Library Manager of the Arduine IDE to download the MQ135 library.  

The MQ135_Air_Quality.ino software incorporates the Arduino MQ135 library to perform two functions:
  
1. To Calibrate the sensor, Execute the following:

   a. Activate the #define CALIBRATE statement by removing the preceding '//'.
   
   b. Replace the #define RLOAD value with the measured resistance in kOhms of your MQ135 board pin A0 to GND, as described in the Concept of Operation section above.
   
   c. Plug in the system outdoors for a couple hours to attain a reference resistance R0 for fresh air.

   The value for RZERO will be displayed on the report platforms (computer, OLED, HA, ThingSpeak as connected).

   Average the most recent numbers after a couple hours and then replace the number in the software statement '#define RZERO xx.xx' with the new value.

   The OLED display will show the R0 value and the rolling average of the last 7 values:

   ![OLED Calibration](https://github.com/Bobbo117/MQ135-Air-Quality-Sensor/blob/main/Images/OLED%20Calibration.jpg)
           
3. To use the sensor to monitor air quality ececute the following:

   a. Deactivate the #define CALIBRATE statement by replacing the preceding '//'.

   b. Replace the '#define RZERO xx.xxx' with the new value.

   c. The OLED display will show the latest ppm measurement as well as the average of the last 7 measurements.

   ![OLED AQI](https://github.com/Bobbo117/MQ135-Air-Quality-Sensor/blob/main/Images/OLED%20AQI.jpg)
   
NOTES 

  1. The MQ135 must be powered on for 48 hours before first use.

# Results

Examples of MQ135 measurements after calibration (Calibration outside conditions approximate the [CO2.Earth]([co2.earth](https://www.co2.earth/)) website. Inside conditions include 2 persons):

    Outside Maine - 75 F 63% RH 415 ppm
    Maine Kithen - windows open 500 ppm
    Maine Kithen - windows shut 600 ppm
    Maine Kithen - oven on     1200 ppm   
    
    Maine basement - quiet      650 ppm
    Maine basement - humidex    600 ppm

(Humidex is a device which draws air from the upstairs to expel damp air from the finished basement to the outside.)
    
My Shark Clean Sense IQ Model HP102 Air Purifier in the basement indicates 97 - 100 % clean air    

An interesting scenario follows.  The sensor is in the basement.  The first floor is vacant, we sleep on the second floor with the window open.  A little after 4:14 a.m., I get up to pee.  We see a bump in CO2 in the basement!  I try to get back to sleep, but give up around 4:48, when I go to the first floor to make coffee and into the basement and turn on the humidex, then head back upstairs.  We see a reduction in ppm due to the humidex:

![Coffee Humidex](https://github.com/Bobbo117/MQ135-Air-Quality-Sensor/blob/main/Images/HA%20AQI%20Coffee%20Humidex.jpg)

Then arond 5:34, I head down for a second cup.  While I'm there, I empty the dishwasher, creating quite the uptick:

![Humidex Coffee](https://github.com/Bobbo117/MQ135-Air-Quality-Sensor/blob/main/Images/HA%20AQI%20Humidex%20Coffee.jpg)

The above two screenshots are from Home Assistant.  The same can be created on a free Thingspeak account.

Are these measurement accurate?  I don't know.  Are they useful? I think so, because they reflect trends if not precise concentrations.  

BREAKING NEWS UPDATE: Today the MQ135 was hovering around 16,000 and the Shark was indicating 78% (4, 18, & 21 ppm of pm1.0, pm2.5, & pm10 particles).  I checked the EPA's AirNow App and found outside AQI is 60 (moderate) due to wildfires. Cool!

![a](https://github.com/Bobbo117/MQ135-Air-Quality-Sensor/blob/main/Images/HA%20AQI%2017K.jpg)

![b](https://github.com/Bobbo117/MQ135-Air-Quality-Sensor/blob/main/Images/EPA%20AQI%2060.jpg)

# Further Discussion

The microcontroller analog input used for this project needs to be 10 bits beccause the MQ135 library uses the max value 1023 instead of Vcc in the calculation.

# Someday:

Determine accuracy
    
Integrate the MQ-2, 3, 4, 5, 6, 7, 8, 9 sensors

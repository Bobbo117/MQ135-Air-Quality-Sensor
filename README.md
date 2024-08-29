# Project Description

This project connects the MQ135 sensor board to an Arduino-based controller to calibrate your specific board outdoors and then monitor inside air quality, primarily CO2, using one or any combination of the following options:

a. Computer via usb to Arduino IDE
  
b. OLED display
  
c. Thingspeak.com
  
d. Home Assistant via MQTT

# MQ135 Air Quality Sensor Board

The MQ135 board has 4 pins: Vcc (5v), GND, A0 (Analog voltage), and D0 (digital signal).

![image](https://github.com/user-attachments/assets/d0af170c-036d-4ee3-a305-74bce83af16a)

It has several components, including the MQ135 sensor, a power LED, an alarm LED, a variable resistor to adjust the alarm level that triggers the alarm LED and the D0 pin, a Load Resistor (RL), as well as additional circuitry associated with the digital output.  

The digital pin goes low, and an alarm LED lights, when the voltage exceeds the trigger level set by the potentiometer.

![image](https://github.com/user-attachments/assets/be24d9ab-758d-421a-aa32-b21a0685b0c2) 

The MQ135 sensot has an explosion shield for protection in case the internal heater creates unwanted artifacts.

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

Consider also that our home environment generally does not contain levels of the other gases that this sensor detects.  If that is true, then the MQ135 properties suggest possible utility as a CO2 detector as well as a general air quality monitor.  

# MQ135 Board Concept of Operation

Each board may be slightly different.  All resistance measurements in this discussion pertain to my specific board.  

The characteristic curve of the sensor shows that increasing ppm reduces sensor resistance.  

The resistance of the sensor and the RL tied to A0 pin form a voltage divider whose voltage increases with increasing PPM (decreasing Rs):

![image](https://github.com/Bobbo117/MQ135-Air-Quality-Sensor/blob/main/Images/SensorDiagram.jpg)

The sensor resistance, connected to pins Vcc and A0, and the Load resistance, connected between pins A0 and GND, form a voltage divider between Vcc and GND whose midpoint is pin A0. 

You can get feel for this by doing the folowing:

1. Measure the sensor resistance of the MQ135 board (without power applied) between the VCC and the A0 pin.  Mine measures 1750 ohms (1.750 Kohms).
   
2. Measure the load resistance between the A0 and GND pin.  Mine measures 1720 ohms (1.720 Kohm). You will need to enter the load resistance value in KOhms in the '#define RLOAD' statement in the 
 MQ135_Air_Quality.ino sotware.

When the unit is powered up, the sensor resistance decreases as the concentration of detected gases (mostly CO2 under normal conditions) increases, causing the voltage across the load resistance at pin A0 to increase.  

Additional board circuitry enables setting a potentiometer to determine the level at which the D0 pin and LED trigger.

# Hardware Connections

![image](https://github.com/Bobbo117/MQ135-Air-Quality-Sensor/blob/main/Images/Ckt%20Diagram.jpg)

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
  
OLED Display pins:  

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

   a. Measure the load resistance between the A0 and GND pin.  Mine measures 1720 ohms (1.720 Kohm). 

   b. Activate the #define CALIBRATE statement by removing the preceding '//'.
   
   c. Replace the #define RLOAD value with the measured resistance in kOhms of your MQ135 board pin A0 to GND, as described in the Concept of Operation section above.

   d. Recompile the software and download to the microcontroller.
   
   e. Plug in the system outdoors for a few hours until the computed R0 stabilizes to attain a reference resistance R0 for fresh air.

   The value for RZERO will be displayed on the report platforms (computer, OLED, HA, ThingSpeak as connected).

   The OLED display will show the R0 value and the rolling average of the last 7 values:

   ![OLED Calibration](https://github.com/Bobbo117/MQ135-Air-Quality-Sensor/blob/main/Images/OLED%20Calibration.jpg)

   f. Replace the value in the software statement '#define RZERO xx.xx' with the new value.
           
3. To use the sensor to monitor air quality ececute the following:

   a. Deactivate the #define CALIBRATE statement by replacing the preceding '//'.

   b. Replace the '#define RZERO xx.xxx' with the new value.

   c. Recompile the software and download to the microcontroller.

   d. The OLED display will show the latest ppm measurement as well as the average of the last 7 measurements.

   ![OLED AQI](https://github.com/Bobbo117/MQ135-Air-Quality-Sensor/blob/main/Images/OLED%20AQI.jpg)

# Results

Examples of MQ135 measurements after calibration (Calibration outside conditions approximate the [CO2.Earth]([co2.earth](https://www.co2.earth/)) website. Inside conditions include 2 persons):

    Outside Maine - 75 F 63% RH 415 ppm
    Maine Kitchen - windows open 500 ppm
    Maine Kitchen - windows shut 600 ppm
    Maine Kitchen - oven on      900 ppm   
    
    Maine basement - quiet      600 ppm
    Maine basement - humidex    550 ppm

(Humidex is a device which draws air from the upstairs to expel damp air from the finished basement to the outside.)
    
My Shark Clean Sense IQ Model HP102 Air Purifier in the basement indicates 97 - 100 % clean air    

# Scenario 1

The sensor is in the basement. The first floor is vacant, we sleep on the second floor with the window open.  

A little after 4:14 a.m., I got up to pee.  We see a bump in CO2 in the basement, as indicated on the timeline screenshot below.

I tried to get back to sleep, but gave up around 4:48, when I went to the first floor to make coffee, creating another bump in CO2, then head back upstairs.  

The humidex activated around that time, causing a steep reduction in ppm, as seen in the timeline below:

![Coffee Humidex](https://github.com/Bobbo117/MQ135-Air-Quality-Sensor/blob/main/Images/HA%20AQI%20Coffee%20Humidex.jpg)

Then arond 5:34, I headed back down for a second cup.  

While there, I spent a couple extra minutes emptying the dishwasher, which created quite the uptick in CO2 (Screenshot below).

![Humidex Coffee](https://github.com/Bobbo117/MQ135-Air-Quality-Sensor/blob/main/Images/HA%20AQI%20Humidex%20Coffee.jpg)

The above two screenshots are from Home Assistant.  The same can be created on a free Thingspeak account.

# Scenario 2

On August 14, the MQ135 was hovering around 2,500 and the Shark was indicating 78% (4, 18, & 21 ppm of pm1.0, pm2.5, & pm10 particles).

I checked the EPA's AirNow App and found outside AQI is 60 (moderate) due to wildfires. Cool!

![b](https://github.com/Bobbo117/MQ135-Air-Quality-Sensor/blob/main/Images/EPA%20AQI%2060.jpg)

# Home Assistant Implementation Option

As an option, this project interfaces with Home Assistant (HA) via topic "aqi/mq135".  

To implement it in HA, open the file editor to mqtt.yaml, and enter the following under the sensor section:

"- unique_id: mq135"
  
  name: "MQ135"
  
  state_topic: "aqi/mq135"
  
  qos: 0
  
  unit_of_measurement: "ppm"
  
Save the file and open the dashboard User Interface (Lovelace in my case) to create a gauge card: 


![image](https://github.com/user-attachments/assets/3cf2f138-5b63-41d2-93ad-2f90cacaa56a)


Create a History Graph Card with the following parameters (or not!):


![image](https://github.com/user-attachments/assets/66477e56-1246-45c4-81e2-772616c45629)


# Further Discussion

The MQ135 must be powered on for 48 hours before first use.

The MQ135 will need a few minutes for the heater to warm up enough to provide normal data any time it has been powered off.

The digital threshhold can be set by adjusting the potentiometer to light the LED when the trigger concentration exists.

The microcontroller analog input used for this project needs to be 10 bits because the MQ135 library uses the max value 1023 instead of Vcc in the calculation.

This project makes no effort to correct for temperature or humidity.

# Observations

CO2 tracking seems reasonable during clear days as indicated on the EPA's AirNow App.

When the AirNow App indicates rising PM2.5 fine particles, the MQ135 response is dominated by these particles to the extent that there is no discernable sensitivity to varying CO2.

# Conclusions

How accurate are the MQ135 measurements?  I don't know.  Are they useful? I think so, because they reflect trends if not precise concentrations.

# Someday:

Determine accuracy
    
Integrate the MQ-2, 3, 4, 5, 6, 7, 8, 9 sensors as appropriate to isolate specific gases

# Attributions and References

For details about the MQ135.h library parameters see:

http://davidegironi.blogspot.com/2014/01/cheap-co2-meter-using-mq135-sensor-with.html

https://hackaday.io/project/3475-sniffing-trinket/log/12363-mq135-arduino-library

https://gml.noaa.gov/ccgg/trends/gl_data.html

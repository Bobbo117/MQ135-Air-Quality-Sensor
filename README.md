# MQ135 Air Quality Sensor Board

![image](https://github.com/user-attachments/assets/be24d9ab-758d-421a-aa32-b21a0685b0c2) ![image](https://github.com/user-attachments/assets/d0af170c-036d-4ee3-a305-74bce83af16a)


The MQ135 sensor ([datasheet here](https://www.olimex.com/Products/Components/Sensors/Gas/SNS-MQ135/resources/SNS-MQ135.pdf)) uses the presence of certain gases to change the electrical resistance of the sensor.  
  
The following excerpt from the datasheet datasheet specifies sensitivity characteristics to up to 200 ppm. Consider that the [CO2.Earth](https://www.co2.earth/) website indicates clean outside air contains more than 400 ppm CO2, and that inside air can exceed 1000 ppm easily.  The MQ135 properties suggest possible utility as a CO2 detector as well as a general air quality monitor.  

![image](https://github.com/user-attachments/assets/bfa19113-d05b-4a2e-a1c8-9897aea4ffdc)


# MQ135 Board Concept of Operation
1. Measure the sensor resistance of the MQ135 board (without power applied) between the VCC and the A0 pin.  Mine measures 1750 ohms (1.750 Kohms).
   
2. Measure the load resistance between the A0 and GND pin.  Mine measures 1720 ohms (1.720 Kohm). YOU WILL NEED TO ENTER THE LOAD RESISTANCE VALUE IN KOHMS FOR THE #DEFINE RLOAD STATEMENT IN THE MQ135_Air_Quality.ino SOFTWARE.

These two resistors form a voltage divider between Vcc and GND whose midpoint is pin A0.  When the unit is powered up, the sensor resistance decreases as the concentration of detected gases (mostly CO2 normally) increases, causing the voltage across the load resistance at pin A0 to increase.  

Additional board circuitry enables setting a potentiometer to determine the level at which the D0 pin and LED trigger.

# MQ135 Sensor Board Calibration
The datasheet suggests calibrating under strict conditions that would require an environmental chamber and with a specific load resistance.  Since we don't have a chamber, and the board comes with a fixed load resistance the does not comply, we calibrate the unit outdoors under clear conditions and hope for the best.  Try to pick a day when outside temperature and humidity conditions match normal inside conditions.  Also, consider using the EPA's AirNow App to verify air quality.

# Hardware Connections

MQ135 pins:

1. VCC to esp8266 Vcc (5v)
2. GND to esp8266 GND
3. A0 to esp8266 A0 via voltage divider *
4. D0 unused

* 170K and 330k voltage divider is used to reduce MQ135 5v max output from A0 to ESP8266 3.3v input.
  
OLED Display pins (optional):

1. VCC to esp8266 3.3v 
2. GND to esp8266 GND
3. SDA to esp8266 SDA
4. SCL to esp8266 SCL

Power the ESP8266 via USB connection.

# Software

Use the Library Manager of the Arduine IDE to download the MQ135 library.  

The MQ135_Air_Quality.ino software incorporates the Arduino MQ135 library to perform two functions:
  
1. When the #define CALIBRATE statement is active, calibrate the MQ135 sensor while running the system outdoors for a couple hours
   to attain a reference resistance R0 for fresh air.  Replace the number in the software statement '#define RZERO 51.5' with the new value.
   Consider using the EPA's AirNow App to verify the outside air quality.
           
2. When the #define CALIBRATE statement is inactivated by commenting it out, read the MQ135 sensor and report the CO2 ppm data on the following platforms:
  
a. computer via USB to Arduino IDE
  
b. OLED display via I2C if one is present
  
c. Thingspeak.com via WIFI if the #define THINGSPEAK line is not disabled
  
d. Home Assistant via MQTT if the #define MQTT is not disabled

NOTES 

  1. The MQ135 must be powered on for 24 hours before first use.

# Results

Examples of MQ135 measurements after calibration (Calibration outside conditions approximate the [CO2.Earth]([co2.earth](https://www.co2.earth/)) website. Inside conditions include 2 persons):

    Outside Maine - 75 F 63% RH 415 ppm
    Maine Kithen - windows open 550 ppm
    Maine Kithen - windows shut 800 ppm
    Maine Kithen - oven on     1500 ppm   
    
    Maine basement - quiet     1450 ppm
    Maine basement - humidex   1320 ppm

(Humidex is a device which draws air from the upstairs to expel damp air from the basement to the outside.)

Maine basement dehumidifier on - 2300 ppm initially, settling to 1375 ppm steady state after target humidity is reached and the humidex catches up to the new blast of exhaust from the dehumidifier.
    
Shark Clean Sense IQ Model HP102 Air Purifier in basement indicates 97 - 100 % clean air    

Are these measurement accurate?  I don't know.  Are they useful? I think so, because they reflect trends if not precise concentrations.  

BREAKING NEWS UPDATE: Today the MQ135 was hovering around 12,000 and the Shark was indicating 78% (4, 18, & 21 ppm of pm1.0, pm2.5, & pm10).  I checked the EPA's AirNow App and found outside AQI is 60 (moderate) due to wildfires. Cool!


# Further Discussion

The MQ135 board has several components, including the MQ135 sensor, a power LED, an alarm LED, a variable resistor to adjust the alarm level that triggers the alarm LED and the D0 pin, a Load Resistor (RL), as well as some circuitry associated with the digital output.  Each board may be slightly different.  All resistance measurements in this discussion pertain to my specific board.  The MQ135 sensor itself has 6 pins labeled A,A B,B and H,H.  The H pins connect to the heater coil, which measures 35 ohms without power.  The A's are connected to each other, as are the B's.  They are the terminations for Rs, the sensor resistance.  The A's are connected to Vcc (5 volts).  The B's are connected to a board pin labeled A0 and to a fixed load resistor RL whose other end is connected to the ground pin of the board.  The characteristic curve shows that increasing ppm reduces sensor resistance.  The resistance of the sensor and the RL tied to A0 pin form a voltage divider whose voltage increases with increasing PPM (decreasing Rs).  The comparable Rs and RL values insure optimum use of the 10-bit A/D Converter. 

It is worth noting that the value of RL plays no role in calculating the AQI, because the AQI is calculated using the ratio Rs (sensor resistance in normal environment) and Ro (sensor resistance in clean air): 
The voltage divider relationship is VL/Vcc = RL/(RL + Rs).  Solving for Rs yields Rs = RL(Vcc/Vs - 1).  So the initial resistance Ro = RL(VCC/Vo-1).  The quantity Rs/Ro results in RL being cancelled out because it is in the numerator and denominator. Yay!
That said, the measured value of RL in KOhms needs to be entered in the software becuasse it is used to calculate the value of Rs.

Finally, the esp8266 or esp32 used for this project needs to be 10 bits if you want to use the getResistance command beccause the MQ135 library uses the max value 1023 instead of Vcc in the calculation.

# Someday:

Determine accuracy
    
Integrate the MQ-2, 3, 4, 5, 6, 7, 8, 9 sensors

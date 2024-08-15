# ReadMe
  The MQ135 sensor ([datasheet](https://www.olimex.com/Products/Components/Sensors/Gas/SNS-MQ135/resources/SNS-MQ135.pdf)) uses the presence of certain gases to change the electrical resistance of the sensor.  
  
  Though the datasheet only specifies sensitivity characteristics to 200 ppm, the MQ135 properties suggest utility as a CO2 detector as well as a general air quality monitor.  For comparison, consider that the [CO2.Earth]([co2.earth](https://www.co2.earth/)) website indicates clean 
  outside air contains above 400 ppm CO2.  Inside air can exceed 1000 ppm easily.

# Operation Concept  
1. Measure the load resistance between the A0 and GND pin of the MQ135 board (without power applied).  Mine measures 1720 ohms (1.720 Kohm). This load resistor is not variable.
2. Next, measure the sensor resistance between the VCC and GND pin.  Mine measures 1750 ohms (1.750 Kohms).  These two resistors form a voltage divider between Vcc and GND whose midpoint is pin A0.

When the unit is powered up, the sensor resistance decreases as the concentration of detected gases (mostly CO2 hopefully) increases, causing the voltage across the load resistance at pin A0 to increase.

Additional board circuitry enables setting a potentiometer to determine the level at which the D0 pin and LED trigger.

# Hardware Connections

MQ135 pins:

    VCC to esp8266 Vcc (5v)
    GND to esp8266 GND
    A0 to esp8266 A0 via voltage divider *
    D0 unused

* 170K and 330k voltage divider is used to reduce MQ135 5v max output from A0 to ESP8266 3.3v input.
  
OLED Display pins (optional):

    VCC to esp8266 3.3v 
    GND to esp8266 GND
    SDA to esp8266 SDA
    SCL to esp8266 SCL

Power ESP8266 via USB connection.

# Software

  This software incorporates the Arduino MQ135 library to perform two functions:
  
  1. When the #define CALIBRATE statement is active, calibrate the MQ135 sensor by running the system outdoors for a couple hours
     to attain a reference resistance R0 for fresh air.  Replace the number in the software statement '#define RZERO 51.5' with the new value.
     Consider using the EPA's AirNow App to verify the outside air quality.
           
  2. When the #define CALIBRATE statement is inactivated by commenting it out, read the MQ135 sensor and report the CO2 ppm data on the following platforms:
  
    a. computer via USB to Arduino IDE
  
    b. OLED display via I2C
  
    c. Thingspeak.com via WIFI
  
    d. Home Assistant via MQTT

  NOTES 
  
  1. The software ignores the platforms for which it is not credentialed or connected.
  2. The MQ135 must be powered on for 24 hours before first use.

# Results

Examples of MQ135 measurements after calibration (Calibration outside conditions approximate the [CO2.Earth]([co2.earth](https://www.co2.earth/)) website. Inside conditions include 2 persons):

    Outside Maine - 75 F 63% RH 415 ppm
    Maine Kithen - windows open 550 ppm
    Maine Kithen - windows shut 800 ppm
    Maine Kithen - stove on    1500 ppm   
    
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
That said, the measured value of RL in KOhms needs to be entered in the software becuasse it is used to measure Rs.

Finally, the esp8266 or esp32 used for this project needs to be 10 bits if you want to use the getResistance command beccause the MQ135 library uses the max value 1023 instead of Vcc in the calculation.

# Someday:

Determine accuracy
    
Integrate the MQ-2, 3, 4, 5, 6, 7, 8, 9 sensors

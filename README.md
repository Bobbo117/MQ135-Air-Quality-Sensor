# ReadMe
  The MQ135 sensor detects ([datasheet](https://www.olimex.com/Products/Components/Sensors/Gas/SNS-MQ135/resources/SNS-MQ135.pdf)) uses the presence of certain gases to change the electrical resistance of the sensor.  
  Though the datasheet only specifies sensitivity characteristics to 200 ppm, the MQ135 properties suggest utility as a CO2 detector.  For comparison, consider that CO2.Earth indicates clean outside air contains around 400 ppm CO2; inside air can reach 1000 ppm 
  easily.
  
  The MQ sensor family can be procured here:

https://www.amazon.com/dp/B0978KXFCQ/ref=sspa_dk_detail_1?pd_rd_i=B0978KXFCQ&pd_rd_w=Rdxbx&content-id=amzn1.sym.8c2f9165-8e93-42a1-8313-73d3809141a2&pf_rd_p=8c2f9165-8e93-42a1-8313-73d3809141a2&pf_rd_r=3DREN4AM4GMVXSB16463&pd_rd_wg=sWHBk&pd_rd_r=c1e97e35-5eb3-4f6e-9ffd-abd54153791b&s=industrial&sp_csd=d2lkZ2V0TmFtZT1zcF9kZXRhaWw&th=1


  
  This software incorporates the Arduino MQ135 library to perform two functions:
  1. When the #define CALIBRATE statement is active, calibrate the MQ135 sensor by running the system outdoors for a couple hours
     to attain a reference resistance R0 for fresh air.  Replace the number in the software statement '#define RZERO 51.5' with the new value.        
  2. When the #define CALIBRATE statement is inactivated by commenting it out, read the MQ135 sensor and report the CO2 ppm data on the following platforms:
  
    a. computer monitor via USB Arduino
  
    b. OLED display via I2C
  
    c. Thingspeak.com via WIFI
  
    d. Home Assistant via MQTT

  NOTE - The software ignores the platforms for which it is not credentialed or connected.



Examples of MQ135 measurements after calibration (Calibration outside conditions approximate CO2.Earth website. Inside conditions include 2 persons):

    Outside Maine - 75 F 63% RH 415 ppm 
    Maine Kithen - windows open 550 ppm
    Maine Kithen - windows shut 800 ppm
    Maine Kithen - stove on    1500 ppm     
    
    Maine basement - quiet     1450 ppm
    Maine basement - humidex   1320 ppm 

(Humidex is an air exchanger which introduces fresh air to the basement)

Maine basement - dehumidifier on 1650 ppm initially, settling to 1375 ppm steady state when the humidex catches up to the new blast of exhaust from the dehumidifier.
    
Shark Clean Sense IQ Model HP102 Air Purifier in basement indicates 97 - 100 % clean air    

Are these measurement accurate?  I don't know.  Are they useful? I think so, because they reflect trends if not precise concentrations.  

# Someday:

Determine accuracy
    
Integrate the MQ-2, 3, 4, 5, 6, 7, 8, 9 sensors

# ReadMe
  This software incorporates the MQ135 library to perform two functions:
  1. When the #define CALIBRATE statement is active, calibrate the MQ135 sensor by running the system outdoors for a couple hours
     to attain a reference resistance R0 for fresh air.  This value replaces the number in the software statemnent '#define RZERO 51.5'        
  2. When the #define CALIBRATE statement is inactivated by commenting it out, read the MQ135 sensor and report the CO2 ppm data on the following platforms:
  
    a. computer monitor via USB Arduino
  
    b. OLED display via I2C
  
    c. Thingspeak.com via WIFI
  
    d. Home Assistant via MQTT

  NOTE - The software automatically ignores the platforms for which it is not credentialed or connected

The MQ sensor family can be procured here:

https://www.amazon.com/dp/B0978KXFCQ/ref=sspa_dk_detail_1?pd_rd_i=B0978KXFCQ&pd_rd_w=Rdxbx&content-id=amzn1.sym.8c2f9165-8e93-42a1-8313-73d3809141a2&pf_rd_p=8c2f9165-8e93-42a1-8313-73d3809141a2&pf_rd_r=3DREN4AM4GMVXSB16463&pd_rd_wg=sWHBk&pd_rd_r=c1e97e35-5eb3-4f6e-9ffd-abd54153791b&s=industrial&sp_csd=d2lkZ2V0TmFtZT1zcF9kZXRhaWw&th=1

Examples of MQ135 measurements after calibration (Calibration outside conditions approximate CO2.Earth website. Inside conditions include 2 persons):

    Outside Maine - 75 F 63% RH 415 ppm 
    Maine Kithen - windows open 550 ppm
    Maine Kithen - windows shut 800 ppm
    Maine Kithen - stove on    1500 ppm     
    
    Maine basement - quiet     1450 ppm
    Maine basement - humidex   1320 ppm 

    (humidex is an air exchanger which introduces fresh air to the basement)
    Maine basement - dehumidifier on 1650 ppm initially, settling to 1375 ppm steady state
    
Shark Clean Sense IQ Model HP102 Air Purifier in basement indicates 97 - 100 % clean air    

The measurements confirm the concensus that these sensors are useful for reflecting gas concentration trends, not precise concentrations.  

# Someday:

    Integrate the MQ-2, 3, 4, 5, 6, 7, 8, 9 sensors

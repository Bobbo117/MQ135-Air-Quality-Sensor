# ReadMe
  This software incorporates the MQ135 library to perform two functions:
  1. When the #define CALIBRATE statement is active, calibarate the MQ135 sensor 
  2. When the #define CALIBRATE statement is inactive, read the MQ135 sensor and report the CO2 ppm data on the following platforms:
  
    computer monitor via USB connection
    OLED display via I2C connection
    Thingspeak.com via WIFI
    Home Assistant via WIFI and MQTT

  NOTE - The software automatically ignores the platforms for which it is not credentialed or connected

The MQ sensor family can be procured here:

https://www.amazon.com/dp/B0978KXFCQ/ref=sspa_dk_detail_1?pd_rd_i=B0978KXFCQ&pd_rd_w=Rdxbx&content-id=amzn1.sym.8c2f9165-8e93-42a1-8313-73d3809141a2&pf_rd_p=8c2f9165-8e93-42a1-8313-73d3809141a2&pf_rd_r=3DREN4AM4GMVXSB16463&pd_rd_wg=sWHBk&pd_rd_r=c1e97e35-5eb3-4f6e-9ffd-abd54153791b&s=industrial&sp_csd=d2lkZ2V0TmFtZT1zcF9kZXRhaWw&th=1

Examples of MQ135 measurements after calibration:

    Outside Maine - 75 F 63% RH 415 ppm (Calibration conditions approximate CO2.Earth website)
    Maine Kithen - windows open 550 ppm (2 people rattling around the house providing CO2)
    Maine Kithen - windows shut 800 ppm
    Maine Kithen - stove on    1500 ppm     
    
    Maine basement - appliances off  1700 ppm (6 am, appliances off overnight)
    Maine basement - humidex on      1500 ppm (humidex is an air exchanger which introduces fresh air to the basement)
    Maine basement - dehumidifier on 1800 ppm
    
Shark Clean Sense IQ Model HP102 Air Purifier in basement indicates 97 - 100 % clean air    

The measurements confirm the concensus that these sensors are useful for reflecting gas concentration trends, not precise concentrations.  

# Someday:

    Integrate the MQ-2, 3, 4, 5, 6, 7, 8, 9 sensors

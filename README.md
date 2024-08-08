# ReadMe
  This software incorporates the MQ135 library to do perform two functions:
  1. When the #define CALIBRATE statement is active, calibarate the MQ135 sensor 
  2. When the #define CALIBRATE statement is inactive, read the MQ135 sensor and report the CO2 ppm data on the following platforms:
  
    computer monitor via USB connection
    OLED display via I2C connection
    Thingspeak.com via WIFI
    Home Assistant via WIFI and MQTT

  NOTE - The device automatically ignores the platforms for which it is not credentialed or connected

The MQ sensor family can be procured here:

https://www.amazon.com/dp/B0978KXFCQ/ref=sspa_dk_detail_1?pd_rd_i=B0978KXFCQ&pd_rd_w=Rdxbx&content-id=amzn1.sym.8c2f9165-8e93-42a1-8313-73d3809141a2&pf_rd_p=8c2f9165-8e93-42a1-8313-73d3809141a2&pf_rd_r=3DREN4AM4GMVXSB16463&pd_rd_wg=sWHBk&pd_rd_r=c1e97e35-5eb3-4f6e-9ffd-abd54153791b&s=industrial&sp_csd=d2lkZ2V0TmFtZT1zcF9kZXRhaWw&th=1

These sensors are useful for reflecting gas concentration trends; they are not for measuring precise concentrations.  

Examples of MQ135 measurements:

    Outside Florida balmy day    300
    Inside Florida windows shut 3500
    Outside Maine hot humid day 1200
    Inside Maine windows open   2200
    Inside Maine windows shut   3200
    Maine basement 9,000 - 30,000

Shark Clean Sense IQ Model HP102 Air Purifier in basement indicates 97 - 100 % clean air    


# Someday:

    Calibrate the sensor somewhat

    Integrate the MQ-2, 3, 4, 5, 6, 7, 8, 9 sensors

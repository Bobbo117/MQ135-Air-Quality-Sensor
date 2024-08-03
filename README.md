# ReadMe
Read and display the analog pin of MQ135 Air Quality Sensor 

This software, written for esp8266 (NodeMCU 0.9 (ESP-12 Module), simply reads the analog pin from the MQ-135 and displays the raw value on:

    Computer monitor
    
    OLED display
    
    Thingspeak
    
    Home Assistant 
    
There is no attempt to calibrate the device.

The MQ sensor family can be procured here:

https://www.amazon.com/dp/B0978KXFCQ/ref=sspa_dk_detail_1?pd_rd_i=B0978KXFCQ&pd_rd_w=Rdxbx&content-id=amzn1.sym.8c2f9165-8e93-42a1-8313-73d3809141a2&pf_rd_p=8c2f9165-8e93-42a1-8313-73d3809141a2&pf_rd_r=3DREN4AM4GMVXSB16463&pd_rd_wg=sWHBk&pd_rd_r=c1e97e35-5eb3-4f6e-9ffd-abd54153791b&s=industrial&sp_csd=d2lkZ2V0TmFtZT1zcF9kZXRhaWw&th=1

# Someday:

    Calibrate the sensor

    Integrate the MQ-2, 3, 4, 5, 6, 7, 8, 9 sensors

## Greenhouse Monitor
### This code can be used with a LDR, temperature sensor, humidity sensor as well as an ADC, DAC and RTC to continuously monitor light, temperature and humidity. 

The no_blynk folder contains the code which can be run without bynk. 
 - compile using: make
 - run using: make run 

The with_blynk folder contains code to do the same thing while also sending data to blynk. 
 - compile using: ./build.sh raspberry
 - run using: sudo ./blynk --token=P__wky18yTk1LYj21wz8SZaoA_HRW-iq
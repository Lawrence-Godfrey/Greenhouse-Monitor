/**
 * @file       main.cpp
 * @author     Volodymyr Shymanskyy
 * @license    This project is released under the MIT License (MIT)
 * @copyright  Copyright (c) 2015 Volodymyr Shymanskyy
 * @date       Mar 2015
 * @brief
 */

//#define BLYNK_DEBUG
#define BLYNK_PRINT stdout
#ifdef RASPBERRY
  #include <BlynkApiWiringPi.h>
#else
  #include <BlynkApiLinux.h>
#endif
#include <BlynkSocket.h>
#include <BlynkOptionsParser.h>
#include "main.h"

static BlynkTransportSocket _blynkTransport;
BlynkSocket Blynk(_blynkTransport);

static const char *auth, *serv;
static uint16_t port;

#include <BlynkWidgets.h>

unsigned int uptime;
long lastInterruptTime = 0; //Used for button debounce
int freq_index = 0;
bool stopped = false;
long dif;
long end_value;
long start_value;
long time_since_last_alarm;

WidgetTerminal terminal(V6);


BlynkTimer tmr;

BLYNK_WRITE(V1)
{
    printf("Got a value: %s\n", param[0].asStr());
}

void setup()
{
    Blynk.begin(auth, serv, port);
    tmr.setInterval(1000, [](){
      Blynk.virtualWrite(V0, BlynkMillis()/1000);
    });
}

void initGPIO(void){
    wiringPiSetup();

    mcp3004Setup(BASE, SPI_CHAN); // 3004 and 3008 are the same 4/8 channels
    wiringPiSPISetup(1,100);
    for(int j=0; j < sizeof(BTNS)/sizeof(BTNS[0]); j++){
            pinMode(BTNS[j], INPUT);
            pullUpDnControl(BTNS[j], PUD_DOWN);
        }

    pinMode(LED,OUTPUT);    
 
    if(wiringPiISR(4,INT_EDGE_RISING,&dismiss) < 0 || wiringPiISR(17,INT_EDGE_RISING, &change_freq) < 0 || wiringPiISR(27,INT_EDGE_RISING,&stop) < 0 || wiringPiISR(22,INT_EDGE_RISING, &reset) < 0)
    {
        printf("error ISR");
    }

}

void loop()
{
    Blynk.run();
    auto end = std::chrono::system_clock::now(); 

    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    float light = analogRead(BASE + 0);
    float humidity = analogRead(BASE + 1)*3.3/1023;
    float temp = analogRead(BASE + 2)*3.3/(float)1023;

    temp = temp - 0.5;  
    temp = temp/0.01;

    float vout = (light/(float)1023)*humidity;
    
    auto epoch2 = end.time_since_epoch();
    auto value2 = std::chrono::duration_cast<std::chrono::seconds>(epoch2);
    end_value = value2.count();

    dif =end_value-start_value; 

    struct tm * timeinfo;
    char buffer [80];

    time (&end_time);
    timeinfo = localtime (&end_time);

    strftime (buffer,80,"%T",timeinfo);

    unsigned char vout_array[2];

    char ch1 = (int)((vout/3.3)*255);
    ch1 = (ch1>>4);
    char ch2 = (int)((vout/3.3)*255);
    ch2 = (ch2<<2);
            
    vout_array[0] = 0b00110000 | ch1; 
    //Set next 8 bit packet
    vout_array[1] =0b11111100 & ch2;
    wiringPiSPIDataRW (1, vout_array, 2);

    if((vout < 0.65 || vout > 2.65) && (((dif-time_since_last_alarm)/(float)60)>0.2))
    {
        time_since_last_alarm = dif;
        digitalWrite (LED, HIGH);
        Blynk.virtualWrite(V7,255);
    }
    
    if(!stopped)
    {
        std::cout <<  buffer << " | " << setw(6) << dif <<" s | "<<setprecision(2)<< setw(8)<< humidity << " | " << setw(9)<< temp <<" C | "<<setprecision(4)<< setw(5) << light << " | "<<setprecision(2) << setw(4)<< vout<< " V "<< "\n";
        char line[80];
        int linef = sprintf(line," | %02d s | %1.2f | %1.2f C | %1.2f | %1.2f\n",dif,humidity, temp, light, vout);
        
        terminal.write(buffer);
        terminal.write(line);
        Blynk.virtualWrite(V1, dif);
        Blynk.virtualWrite(V2, light);
        Blynk.virtualWrite(V3, humidity);
        Blynk.virtualWrite(V4, temp);
        Blynk.virtualWrite(V5, vout);
    }

    
    //   Blynk.virtualWrite(V6, dif ," s | ", humidity , " | " ,  temp ," C | ", light , " | ", vout, " V \n" )
    //   terminal.write( dif ," s | ", humidity , " | " ,  temp ," C | ", light , " | ", vout, " V \n");
    
      
    
    
    tmr.run();
}


int main(int argc, char* argv[])
{
    parse_options(argc, argv, auth, serv, port);
    setup();
    initGPIO();
    terminal.clear();

    auto start = std::chrono::system_clock::now();
    auto epoch = start.time_since_epoch();
    auto value = std::chrono::duration_cast<std::chrono::seconds>(epoch);
    start_value = value.count();

    time_since_last_alarm = (-1*3*60);

    // char line[80];
    // int linef = sprintf(line,"_______________________________________________________________\n");
    // terminal.write(line,80);
    // terminal.write("RTC TIME | Sys Time | Humidity | Temperature | Light | DAC out \n");
    // terminal.write("_______________________________________________________________\n");

        
    while(true) {
        loop();
        delay (FREQ[freq_index]);
    }

    return 0;
}

void dismiss()
{
    long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
        digitalWrite (LED, LOW);
        Blynk.virtualWrite(V7,0);

    }

    lastInterruptTime = interruptTime;

}
void change_freq()
{
    long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
        if(freq_index<2)
        {
            freq_index++; 
        }
        else
        {
            freq_index = 0;
        }
    }
    lastInterruptTime = interruptTime;
}
void stop()
{
    long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
        stopped = !stopped;
    }
    lastInterruptTime = interruptTime;
}
void reset()
{
    long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
        start_value=end_value;
    }
    lastInterruptTime = interruptTime;

}


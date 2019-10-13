#include "main.h"

using namespace std;

long lastInterruptTime = 0; //Used for button debounce
int freq_index = 0; // keeps track of which time delay is being used
bool stopped = false; // keeps track of wether stop button has been pressed
long dif; // difference between start time and current time (in seconds)
long end_value; // current time
long start_value; // time that script started
long time_since_last_alarm; // time since previous alarm sounded


void initGPIO(void){
    wiringPiSetup();

    mcp3004Setup(BASE, SPI_CHAN); // 3004 and 3008 are the same 4/8 channels
    wiringPiSPISetup(1,100);
    for(int j=0; j < sizeof(BTNS)/sizeof(BTNS[0]); j++){
            pinMode(BTNS[j], INPUT);
            pullUpDnControl(BTNS[j], PUD_DOWN);
        }

    pinMode(LED,OUTPUT);    
 
    if(wiringPiISR(7,INT_EDGE_RISING,&dismiss) < 0 || wiringPiISR(0,INT_EDGE_RISING, &change_freq) < 0 || wiringPiISR(2,INT_EDGE_RISING,&stop) < 0 || wiringPiISR(3,INT_EDGE_RISING, &reset) < 0)
    {
        printf("error ISR");
    }
}

int main(void){
	initGPIO();
    printf("_______________________________________________________________\n");
    printf("RTC TIME | Sys Time | Humidity | Temperature | Light | DAC out \n");
    printf("_______________________________________________________________\n");
    fflush (stdout) ;


    // get time and convert to long data type
    auto start = std::chrono::system_clock::now();
    auto epoch = start.time_since_epoch();
    auto value = std::chrono::duration_cast<std::chrono::seconds>(epoch);
    start_value = value.count();

    time_since_last_alarm = (-1*3*60); // set time since last alarm to negative 3 minutes to begin with

    
    for(;;){            
        // get current time and convert to long
        auto end = std::chrono::system_clock::now(); 
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);
        auto epoch2 = end.time_since_epoch();
        auto value2 = std::chrono::duration_cast<std::chrono::seconds>(epoch2);
        end_value = value2.count();

        //get data from ADC
        float light = analogRead(BASE + 0);
        float humidity = analogRead(BASE + 1)*3.3/1023;
        float temp = analogRead(BASE + 2)*3.3/(float)1023;

        // calibrate to get correct temperature
        temp = temp - 0.5;  
        temp = temp/0.01;

        // calculate output voltage
        float vout = (light/(float)1023)*humidity;
        
        // calculate runtime
        dif =end_value-start_value; 
   
        // get system time and convert to char buffer
        struct tm * timeinfo;
        char buffer [80];
        time (&end_time);
        timeinfo = localtime (&end_time);
        strftime (buffer,80,"%T",timeinfo);

        //create char buffer of 16 bits and shift vout into both parts of buffer
        unsigned char vout_array[2];
        char ch1 = (int)((vout/3.3)*255);
        ch1 = (ch1>>4);
        char ch2 = (int)((vout/3.3)*255);
        ch2 = (ch2<<2);
        vout_array[0] = 0b00110000 | ch1; 
        //Set next 8 bit packet
        vout_array[1] =0b11111100 & ch2;
        //output buffer to DAC
        wiringPiSPIDataRW (1, vout_array, 2);

        // set LED high under these conditions and if the time since last alarm is more than 3 minutes
        if((vout < 0.65 || vout > 2.65) && (((dif-time_since_last_alarm)/(float)60)>3))
        {
            time_since_last_alarm = dif;
            digitalWrite (LED, HIGH);
        }
        
        //output to terminal if not stopped
        if(!stopped)
        {
           std::cout <<  buffer << " | " << setw(6) << dif <<" s | "<<setprecision(2)<< setw(8)<< humidity << " | " << setw(9)<< temp <<" C | "<<setprecision(4)<< setw(5) << light << " | "<<setprecision(2) << setw(4)<< vout<< " V "<< "\n";
        }
        
        delay (FREQ[freq_index]);
        
    }

    return 0;
}

void dismiss() //dismiss alarm
{
    long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
        digitalWrite (LED, LOW);
    }

    lastInterruptTime = interruptTime;

}
void change_freq() //increase frequency 
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
void stop() // stop outputing
{
    long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
        stopped = !stopped;
    }
    lastInterruptTime = interruptTime;
}
void reset() // reset system
{
    long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
        start_value=end_value;

    }
    lastInterruptTime = interruptTime;

}
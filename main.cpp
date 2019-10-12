#include "main.h"

long lastInterruptTime = 0; //Used for button debounce

using namespace std;

void initGPIO(void){
    wiringPiSetup();

    mcp3004Setup(BASE, SPI_CHAN); // 3004 and 3008 are the same 4/8 channels

    for(int j=0; j < sizeof(BTNS)/sizeof(BTNS[0]); j++){
            pinMode(BTNS[j], INPUT);
            pullUpDnControl(BTNS[j], PUD_DOWN);
        }

    pinMode(LED,OUTPUT);    
 
    if(wiringPiISR(7,INT_EDGE_RISING,&button_1) < 0 || wiringPiISR(0,INT_EDGE_RISING, &button_2) < 0 || wiringPiISR(2,INT_EDGE_RISING,&button_3) < 0 || wiringPiISR(3,INT_EDGE_RISING, &button_4) < 0)
    {
        printf("error ISR");
    }
}

int main(void){
	initGPIO();
    printf ("Running"); 
    fflush (stdout) ;
   
    printf("Temp | Light | Humidity\n");
    
    auto start = std::chrono::system_clock::now();
        

    for(;;){            
        // Some computation here
        auto end = std::chrono::system_clock::now();

        std::chrono::duration<double> elapsed_seconds = end-start;
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);

        std::cout << "finished computation at " << std::ctime(&end_time)
                << "elapsed time: " << elapsed_seconds.count() << "s\n";

        float light = analogRead(BASE + 0);
        float humidity = analogRead(BASE + 1)*3.3/1023;
        float temp = analogRead(BASE + 2)*3.3/(float)1023;

        temp = temp - 0.5;  
        temp = temp/0.01;

        float vout = (light/(float)1023)*humidity;

        if(vout < 0.65 || vout > 2.65)
        {
            digitalWrite (LED, HIGH);
        }
        else if(vout > 0.65 && vout <2.65)
        {
            digitalWrite (LED, LOW);
        }

        printf("%f Celsius | %f | %f | %f\n", temp,light,humidity,vout);
        fflush (stdout);
        delay (10);
        
    }

    return 0;
}

void button_1()
{
    long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
        printf("button 1 pressed");  
    }

    lastInterruptTime = interruptTime;

}
void button_2()
{
    long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
        printf("button 2 pressed"); 
    }
    lastInterruptTime = interruptTime;
}
void button_3()
{
    long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
        printf("button 3 pressed");
    }
    lastInterruptTime = interruptTime;
}
void button_4()
{
    long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
        printf("button 4 pressed"); 
    }
    lastInterruptTime = interruptTime;

}
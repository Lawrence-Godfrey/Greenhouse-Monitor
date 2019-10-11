#include "main.h"

long lastInterruptTime = 0; //Used for button debounce

using namespace std;

void initGPIO(void){
    wiringPiSetup();
    printf("setup");
    
    const int BTNS[] = {7,0,2,3};

    for(int j=0; j < sizeof(BTNS)/sizeof(BTNS[0]); j++){
            pinMode(BTNS[j], INPUT);
            pullUpDnControl(BTNS[j], PUD_DOWN);
        }

 
    if(wiringPiISR(7,INT_EDGE_RISING,&button_1) < 0 || wiringPiISR(0,INT_EDGE_RISING, &button_2) < 0 || wiringPiISR(2,INT_EDGE_RISING,&button_3) < 0 || wiringPiISR(3,INT_EDGE_RISING, &button_4) < 0)
    {
        fprintf (stderr, "Unable to setup ISR: %s\n") ;
    }

}

int main(void){
	initGPIO();
    printf("running");
    for(;;){
        printf(".");
        delay (100);
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
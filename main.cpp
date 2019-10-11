#include "main.h"

long lastInterruptTime = 0; //Used for button debounce

using namespace std;

void initGPIO(void){
    wiringPiSetup();
    
    const int BTNS[] = {7,11,12,13};

    for(int j=0; j < sizeof(BTNS)/sizeof(BTNS[0]); j++){
            pinMode(BTNS[j], INPUT);
            pullUpDnControl(BTNS[j], PUD_UP);
        }

    int wiringPiISR(BTNS[1],INT_EDGE_RISING,button_1(void));
	int wiringPiISR(BTNS[2],INT_EDGE_RISING, button_2(void));
    int wiringPiISR(BTNS[3],INT_EDGE_RISING,button_3(void));
	int wiringPiISR(BTNS[4],INT_EDGE_RISING, button_4(void));

}

int main(void){
	initGPIO();
}

void button_1(void)
{
    long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
        printf("button 1 pressed");  
    }

    lastInterruptTime = interruptTime;

}
void button_2(void)
{
    long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
        printf("button 2 pressed"); 
    }
    lastInterruptTime = interruptTime;
}
void button_3(void)
{
    long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
        printf("button 3 pressed");
    }
    lastInterruptTime = interruptTime;
}
void button_4(void)
{
    long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
        printf("button 4 pressed"); 
    }
    lastInterruptTime = interruptTime;

}
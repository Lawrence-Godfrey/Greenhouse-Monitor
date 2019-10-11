#ifndef MAIN_H
#define MAIN_H

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h> //For printf functions
#include <stdlib.h> // For system functions
#include <vector>
#include <wiringPiSPI.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <iostream>

void initGPIO(void);
void button_1();
void button_2();
void button_3();
void button_4();

#endif
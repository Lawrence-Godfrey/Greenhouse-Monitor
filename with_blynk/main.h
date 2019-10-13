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
#include <mcp3004.h>
#include <iomanip>
#include <chrono>
#include <ctime>  

#define BASE 100
#define SPI_CHAN 0

const int FREQ[] = {1000,2000,5000};
const int BTNS[] = {4,17,27,22};
const int LED = 23;

void initGPIO(void);
void dismiss();
void change_freq();
void stop();
void reset();

#endif
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include "initGPIO.h"
#include "wiringPi.h"
#define GPFSEL0 0
#define GPFSEL1 1
#define GPSET0 8
#define GPCLR0 11
#define GPLEV0 14
#define LATCH 9
#define DATA 10
#define CLOCK 11

#define INP_GPIO(g) *(gpioPtr+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpioPtr+((g)/10)) |= (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpioPtr+(((g)/10))) |= (((a)<=3? (a)+4: (a)==4? 3:2) << (((g)%10)*3))

int main()
{
	// Get GPIO ptr
	unsigned int *gpioPtr = getGPIOPtr();  
	printf("pointer address: %p\n", gpioPtr);
	
	// Set clock to output
    //INP_GPIO(CLOCK);
    OUT_GPIO(CLOCK);
    
	//gpioPtr[GPFSEL0] &= ~(0b111 << (9*3));
	//gpioPtr[GPFSEL0] |= (0b001 << (9*3));
    
	// Set lactch to input
    INP_GPIO(LATCH);
    //OUT_GPIO(LATCH);
    
	//gpioPtr[GPFSEL1] &= ~(0b111 << (0*3));
		
	//Set DATA to output
    INP_GPIO(DATA);
    
	//gpioPtr[GPFSEL1] &= ~(0b111 << (1*3));
	//gpioPtr[GPFSEL1] |= (0b001 << (1*3));
    
    // Initialize running bool
    bool running = true;
    
    while (running){


		// Increment GPIO ptr
		unsigned int buttons = 0;
    
		// Write 1 to CLOCK
		//gpioPtr = gpioPtr + 28;
		int pinValue = (gpioPtr[GPSET0] &= (1 << CLOCK)) > 0;
		printf("Address 1: %d\nPin Value: %d\n\n", gpioPtr, pinValue);
    
		// Write 1 to LATCH
		pinValue = (gpioPtr[GPSET0] &= (1 << LATCH)) > 0;
		printf("Address 2: %d\nPin Value: %d\n\n", gpioPtr, pinValue);
    
		// Wait 12 micro seconds
		delayMicroseconds(1000000);
    
		// Write 0 to LATCH
		pinValue = (gpioPtr[0] &= (1 << LATCH)) > 0;
		printf("Address 3: %d\nPin Value: %d\n\n", gpioPtr, pinValue);
    
		// Initialize counter
		int i = 1;
		
        printf("enter Loop: \n");
		// Set bool to true
		while (i <= 16) {
                
			// Counter equals 1
            //printf("i 1: %d\n", i);
            
			// Wait 6 micro seconds
			delayMicroseconds(12); 

    
			// Write 0 to clock
			pinValue = (gpioPtr[GPCLR0] &= (1 << CLOCK)) > 0;
			//printf("Clock Address: %d\nPin Value: %d\n\n", gpioPtr, pinValue);
    
			// Wait 6 micro seconds
			delayMicroseconds(12);
    
			// Read bit i
			pinValue = (gpioPtr[GPLEV0] >> DATA) & 1;
			printf("Address i: %d\nPin Value: %d\n\n", gpioPtr, pinValue);
    
			// Set buttons register
			buttons |= (pinValue << i);
            printf("buttons value: %d\n", buttons);
        
			// Write 0 to CLOCK
			pinValue = (gpioPtr[GPCLR0] &= (1 << LATCH)) > 0;
			//printf("Address: %d\nPin Value: %d\n\n", gpioPtr, pinValue);
    
			// Increment i
			i += 1;
            //printf("i 2: %d\n", i);
		}
        printf("outside loop\n");
		pinValue = (buttons &= 1) > 0;
		if (pinValue == 1) {
			printf("B is being pressed...");
		}

    }
}

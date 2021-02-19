
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include "initGPIO.h"
#include "wiringPi.h"

#define GPFSEL0 0
#define GPFSEL1 1
#define GPSET0 7
#define GPCLR0 10
#define GPLEV0 13
#define LATCH 9
#define DATA 10
#define CLOCK 11

int main()
{
	// Get GPIO ptr
	unsigned int *gpioPtr = getGPIOPtr();  
	printf("pointer address: %p\n", gpioPtr);
	
	// Set pin 9 to output
	gpioPtr[GPFSEL0] &= ~(0b111 << (9*3));
	gpioPtr[GPFSEL0] |= (0b001 << (9*3));
	    		
	// Set pin 10 to input
	gpioPtr[GPFSEL1] &= ~(0b111 << (0*3));
		
	//Set pin 11 to output
	gpioPtr[GPFSEL1] &= ~(0b111 << (1*3));
	gpioPtr[GPFSEL1] |= (0b001 << (1*3));
    
    // Initialize running bool
    bool running = true;
    
    while (running){

		// Increment GPIO ptr
		unsigned int buttons = 0;
    
		// Write 1 to CLOCK
		int pinValue = (gpioPtr[GPSET0] &= (1 << CLOCK)) > 0;
    
		// Write 1 to LATCH
		pinValue = (gpioPtr[GPSET0] &= (1 << LATCH)) > 0;
    
		// Wait 12 micro seconds
		delayMicroseconds(12);
    
		// Write 0 to LATCH
		pinValue = (gpioPtr[GPCLR0] &= (1 << LATCH)) > 0;
		printf("Address: %d\nPin Value: %d\n\n", gpioPtr, pinValue);
    
		// Initialize counter
		int i = 1;
		
		// Set bool to true
		while (i <= 16) {
                		
			// Wait 6 micro seconds
			delayMicroseconds(6);

			// Write 0 to clock
			pinValue = (gpioPtr[GPCLR0] &= (1 << CLOCK)) > 0;
			//printf("Address: %d\nPin Value: %d\n\n", gpioPtr, pinValue);
    
			// Wait 6 micro seconds
			delayMicroseconds(6);
    
			// Read bit i
			pinValue = (gpioPtr[GPLEV0] >> DATA) & 1;
			//printf("Address: %d\nPin Value: %d\n\n", gpioPtr, pinValue);
    
			// Set buttons register
			buttons |= (pinValue << i);
        
			// Write 0 to CLOCK
			pinValue = (gpioPtr[GPCLR0] &= (1 << LATCH)) > 0;
			//printf("Address: %d\nPin Value: %d\n\n", gpioPtr, pinValue);
    
			// Increment i
			i += 1;
		}
		pinValue = (buttons &= 1) > 0;
		if (pinValue == 1) {
			printf("B is being pressed...");
		}
    }
}

#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include "initGPIO.h"
#define GPSET0 0
#define GPCLR0 0
#define GPLEV0 0
#define GPFSEL0 0
#define GPFSEL1 1
#define LATCH 9
#define DATA 10
#define CLOCK 11
#define CLO_REG 0xFE003004
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
		gpioPtr = gpioPtr + 28;
		int pinValue = (gpioPtr[GPSET0] &= (1 << CLOCK)) > 0;
		printf("Address: %d\nPin Value: %d\n\n", gpioPtr, pinValue);
    
		// Write 1 to LATCH
		pinValue = (gpioPtr[GPSET0] &= (1 << LATCH)) > 0;
		printf("Address: %d\nPin Value: %d\n\n", gpioPtr, pinValue);
    
		// Wait 12 micro seconds
		unsigned *clo = (unsigned*) CLO_REG;
		unsigned c = *clo + 12; // micro seconds
		while (c >* clo);
    
		// Increment GPIO ptr
		//gpioPtr = gpioPtr + 12;

		// Write 0 to LATCH
		//pinValue = (gpioPtr[GPCLR0] &= (1 << LATCH)) > 0;
		//printf("Address: %d\nPin Value: %d\n\n", gpioPtr, pinValue);
    
		// Initialize counter
		//int i;
		
		// Set bool to true
		//while (i <= 16) {
        
			//printf("loop is running\n");
        
			// Counter equals 1
			//i = 1;
        
			// Get system clock
			//*clo = (unsigned*) CLO_REG;
		
			// Wait 6 micro seconds
			//c = *clo + 6; // micro seconds
			//while (c >* clo);
    
			// Write 0 to clock
			//pinValue = (gpioPtr[GPCLR0] &= (1 << CLOCK)) > 0;
			//printf("Address: %d\nPin Value: %d\n\n", gpioPtr, pinValue);
    
			// Wait 6 micro seconds
		//	c = *clo + 6; // micro seconds
			//while (c >* clo);
    
			// Increment GPIO ptr
			//gpioPtr = gpioPtr + 12;
    
			// Read bit i
			//pinValue = (gpioPtr[GPLEV0] >> DATA) & 1;
			//printf("Address: %d\nPin Value: %d\n\n", gpioPtr, pinValue);
    
			// Set buttons register
			//buttons &= pinValue;
    
			// Decrement GPIO ptr
			//gpioPtr = gpioPtr - 12;
    
			// Write 0 to CLOCK
			//pinValue = (gpioPtr[GPCLR0] &= (1 << LATCH)) > 0;
	//		printf("Address: %d\nPin Value: %d\n\n", gpioPtr, pinValue);
    
			// Increment i
		//	i += 1;
		//}
    }
}

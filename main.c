
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
	
		// Initialize counter
		int i = 1;
		
		// Set bool to true
		while (i <= 16) {
                		
			// Wait 6 micro seconds
			delayMicroseconds(6);

			// Write 0 to CLOCK
			pinValue = (gpioPtr[GPCLR0] &= (1 << CLOCK)) > 0;
    
			// Wait 6 micro seconds
			delayMicroseconds(6);
    
			// Read bit i
			pinValue = (gpioPtr[GPLEV0] >> DATA) & 1;
    
			// Set buttons register
			buttons |= (pinValue << i);
        
			// Write 1 to CLOCK
			pinValue = (gpioPtr[GPSET0] &= (1 << CLOCK)) > 0;
    
			// Increment i
			i += 1;
		}
		
		// Print buttons register
		i = 0;
		while (i < 32){
			pinValue = (buttons >> i) & 1;
			printf("%d", pinValue);
			if (i == 31) {
				printf("\n");
			}
			i += 1;
		}
		// Check if B is being pressed
		pinValue = (buttons >> 0) & 1
		;
		if (pinValue == 1) {
			printf("B is being pressed...");
		}
		
		// Check if Y is being pressed
		pinValue = (buttons >> 1) & 1;
		if (pinValue == 1) {
			printf("Y is being pressed...");
		}
		
		// Check if SELECT is being pressed
		pinValue = (buttons >> 2) & 1;
		if (pinValue == 1) {
			printf("SELECT is being pressed...");
		}
		
		// Check if START is being pressed
		pinValue = (buttons >> 3) & 1;
		if (pinValue == 1) {
			printf("START is being pressed...");
		}
		
		// Check if UP is being pressed
		pinValue = (buttons >> 4) & 1;
		if (pinValue == 1) {
			printf("UP is being pressed...");
		}
		
		// Check if DOWN is being pressed
		pinValue = (buttons >> 5) &  1;
		if (pinValue == 1) {
			printf("DOWN is being pressed...");
		}
		
		// Check if LEFT is being pressed
		pinValue = (buttons >> 6) & 1;
		if (pinValue == 1) {
			printf("LEFT is being pressed...");
		}
		
		// Check if RIGHT is being pressed
		pinValue = (buttons >> 7) & 1;
		if (pinValue == 1) {
			printf("RIGHT is being pressed...");
		}
		
		// Check if A is being pressed
		pinValue = (buttons >> 8) & 1;
		if (pinValue == 1) {
			printf("A is being pressed...");
		}
		
		// Check if X is being pressed
		pinValue = (buttons >> 9) & 1;
		if (pinValue == 1) {
			printf("X is being pressed...");
		}
		
		// Check if L is being pressed
		pinValue = (buttons >> 10) & 1;
		if (pinValue == 1) {
			printf("L is being pressed...");
		}
		
		// Check if R is being pressed
		pinValue = (buttons >> 11) & 1;
		if (pinValue == 1) {
			printf("R is being pressed...");
		}
		
    }
}

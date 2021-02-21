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
    //OUT_GPIO(CLOCK);
    
	gpioPtr[GPFSEL0] &= ~(0b111 << (9*3));
	gpioPtr[GPFSEL0] |= (0b001 << (9*3));
    
	// Set lactch to input
    //INP_GPIO(LATCH);
    //OUT_GPIO(LATCH);
    
	gpioPtr[GPFSEL1] &= ~(0b111 << (0*3));
		
	//Set DATA to output
    //INP_GPIO(DATA);
    
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

			// Write 0 to clock
			pinValue = (gpioPtr[GPCLR0] &= (1 << CLOCK)) > 0;
    
			// Wait 6 micro seconds
			delayMicroseconds(6);
    
			// Read bit i
			pinValue = (gpioPtr[GPLEV0] >> DATA);
            printf("pinvalue: %d\n", pinValue);
    
			// Set buttons register
			buttons |= (pinValue << i);
        
			// Write 0 to CLOCK
			pinValue = (gpioPtr[GPCLR0] &= (1 << CLOCK)) > 0;
    
			// Increment i
			i += 1;
		}
        
        printf("buttons reg: %d\n", buttons);
		
        /*
		// Check if B is being pressed
		pinValue = (buttons &= 1);
		if (pinValue == 0) {
			printf("B is being pressed...");
		}
		
		// Check if Y is being pressed
		pinValue = (buttons &= 4);
		if (pinValue == 0) {
			printf("Y is being pressed...");
		}
		
		// Check if SELECT is being pressed
		//pinValue = (buttons &= 8);
		if (pinValue == 1) {
			printf("SELECT is being pressed...");
		}
		
		// Check if START is being pressed
		//pinValue = (buttons &= 16);
		if (pinValue == 1) {
			printf("START is being pressed...");
		}
		
		// Check if UP is being pressed
		//pinValue = (buttons &= 32);
		if (pinValue == 1) {
			printf("UP is being pressed...");
		}
		
		// Check if DOWN is being pressed
		//pinValue = (buttons &= 64);
		if (pinValue == 1) {
			printf("DOWN is being pressed...");
		}
		
		// Check if LEFT is being pressed
		//pinValue = (buttons &= 128);
		if (pinValue == 1) {
			printf("LEFT is being pressed...");
		}
		
		// Check if RIGHT is being pressed
		//pinValue = (buttons &= 256);
		if (pinValue == 1) {
			printf("RIGHT is being pressed...");
		}
		
		// Check if L is being pressed
		//pinValue = (buttons &= 512);
		if (pinValue == 1) {
			printf("L is being pressed...");
		}
		
		// Check if R is being pressed
		//pinValue = (buttons &= 1028);
		if (pinValue == 1) {
			printf("R is being pressed...");
		}
        */
    }
}
		

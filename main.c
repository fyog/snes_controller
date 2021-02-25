#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include "initGPIO.h"
#include "wiringPi.h"

// Constants
#define GPFSEL0 0
#define GPFSEL1 1
#define GPSET0 7
#define GPCLR0 10
#define GPLEV0 13
#define LATCH 9
#define DATA 10
#define CLOCK 11

// Macros
#define INP_GPIO(g) *(gpioPtr+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpioPtr+((g)/10)) |= (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpioPtr+(((g)/10))) |= (((a)<=3? (a)+4: (a)==4? 3:2) << (((g)%10)*3))

// Global variables
unsigned int *gpioPtr;

// Initializes the GPIO pins so that the SNES controller can communicate with the rPi.
//
// argument(s): none
// returns: nothing
void initSNES() {
	
	// Get GPIO ptr
	gpioPtr = getGPIOPtr();  
	printf("pointer address: %p\n", gpioPtr);
	
	// Set LATCH to output
	INP_GPIO(LATCH);
	OUT_GPIO(LATCH);
    
	// Set CLOCK to output
	INP_GPIO(CLOCK);
	OUT_GPIO(CLOCK);
    
	// Set DATA to input
	INP_GPIO(DATA);
}

// Checks to see if any of the SNES controller buttons are being pressed.
//
// argument(s): array of integers
// returns: nothing
void checkButtons(int buttons_arr[]) {
	
	// Check if B is being pressed
	if (buttons_arr[0] == 0) {
		printf("B is being pressed...\n");
	}
		
	// Check if Y is being pressed
	if (buttons_arr[1] == 0) {
		printf("Y is being pressed...\n");
	}        
		
	// Check if SELECT is being pressed
	if (buttons_arr[2] == 0) {
		printf("SELECT is being pressed...\n");
	}
	
	// Check if START is being pressed
	if (buttons_arr[3] == 0) {
		printf("START is being pressed...\n");
	}
	
	// Check if UP is being pressed
	if (buttons_arr[4] == 0) {
		printf("UP is being pressed...\n");
	}
	
	// Check if DOWN is being pressed
	if (buttons_arr[5] == 0) {
		printf("DOWN is being pressed...\n");
	}
	
	// Check if LEFT is being pressed
	if (buttons_arr[6] == 0) {
		printf("LEFT is being pressed...\n");
	}
	
	// Check if RIGHT is being pressed
	if (buttons_arr[7] == 0) {
		printf("RIGHT is being pressed...\n");
	}
	
	// Check if A is being pressed
	if (buttons_arr[8] == 0) {
		printf("A is being pressed...\n");
	}
	
	// Check if X is being pressed
	if (buttons_arr[9] == 0) {
		printf("X is being pressed...\n");
	}
		
	// Check if L is being pressed
	if (buttons_arr[10] == 0) {
		printf("L is being pressed...\n");
	}
		
	// Check if R is being pressed
	if (buttons_arr[11] == 0) {
		printf("R is being pressed...\n");
	}
	system("clear");
}

// Starts execution of the SNES controller driver
//
// argument(s): none
// returns: nothing
void runDriver() {
	
	// Running loop
	while (true){

		// Initialize array that will be used to track button presses
		int buttons_arr[20];
    
		// Write 1 to LATCH
		gpioPtr[GPSET0] = (1 << LATCH);
    
		// Wait 12 micro seconds
		delayMicroseconds(12);
    
		// Write 0 to LATCH
		gpioPtr[GPCLR0] = (1 << LATCH);
        
		// Write 1 to CLOCK
		gpioPtr[GPSET0] = 1 << CLOCK;
	
		// Initialize counter
		int i = 1;
		
		// Set bool to true
		while (i <= 16) {
                		
			// Wait 6 micro seconds
			delayMicroseconds(6);

			// Write 0 to CLOCK
			gpioPtr[GPCLR0] = 1 << CLOCK;
    
			// Wait 6 micro seconds
			delayMicroseconds(6);
    
			// Read bit i
			int pinValue = (gpioPtr[GPLEV0] >> DATA) & 1;    
        
			// Write 1 to CLOCK
			gpioPtr[GPSET0] = (1 << CLOCK);
            
			// Set buttons_arr
			buttons_arr[i-1] = pinValue;
    
			// Increment i
			i += 1;
		}
		checkButtons(buttons_arr);
	}
}

// Main method.
//
// argument(s): none
// returns: nothing
int main() {
	
	// Initialize pins
	initSNES();
	
	// Run the driver
	runDriver();
}

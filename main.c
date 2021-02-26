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
bool running = true;

// Prints the authors of the program to the console
//
// argument(s): none
// returns: nothing
void print_Authors() {
	printf("Created by: Matthew Newton and Eric Ross\n");
}

// Initializes a GPIO line
//
// argument(s): GPIO pin number, function code
// returns: nothing
void init_GPIO(int pinNumber, int functionCode) {
	if (functionCode == 0b000) {
		INP_GPIO(pinNumber);
	} else {
		INP_GPIO(pinNumber);
		OUT_GPIO(pinNumber);
	}
}

// Writes a bit to the LATCH line
//
// argument(s): bit value
// returns: nothing
void write_Latch(int bit) {
	if (bit == 0) {
		gpioPtr[GPCLR0] = (1 << LATCH);
	} else if (bit == 1) {
		gpioPtr[GPSET0] = (1 << LATCH);
	}

}

// Writes a bit to the CLOCK line
//
// argument(s): bit value
// returns: nothing
void write_Clock(int bit) {
	if (bit == 0) {
		gpioPtr[GPCLR0] = (1 << CLOCK);
	} else if (bit == 1) {
		gpioPtr[GPSET0] = (1 << CLOCK);
	}

}

// Reads a bit from the DATA line
//
// argument(s): none
// returns: read bit value
int read_Data() {
	int pinValue = (gpioPtr[GPLEV0] >> DATA) & 1;    
	return pinValue;
}

// Initializes the GPIO pins so that the SNES controller can communicate with the rPi
//
// argument(s): none
// returns: nothing
void init_SNES() {
	
	// Clear the console
	system("clear");

	// Ask for input
	printf("Please press a button...\n");
	printf("(Press START to exit)\n");
	
	// Wait 2 second
	delay(2000);
	
	// Get GPIO ptr
	gpioPtr = getGPIOPtr();  
	
	// Set LATCH to output
	init_GPIO(LATCH, 0b001);
    
	// Set CLOCK to output
	init_GPIO(CLOCK, 0b001);
    
	// Set DATA to input
	init_GPIO(DATA, 0b000);
}

// Checks to see if any of the SNES controller buttons are being pressed
//
// argument(s): array of integers
// returns: nothing
void print_Message(int buttons_arr[]) {
	
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
		printf("START is being pressed...\n");\
		running = false;
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
void read_SNES() {
	
	// Running loop
	while (running){

		// Initialize array that will be used to track button presses
		int buttons_arr[16];
    
		// Write 1 to LATCH
		write_Latch(1);
		
		// Wait 12 micro seconds
		delayMicroseconds(12);
    
		// Write 0 to LATCH
		write_Latch(0);
	
		// Write 1 to CLOCK
		write_Clock(1);
		
		// Initialize counter
		int i = 1;
		
		// Set bool to true
		while (i <= 16) {
                		
			// Wait 6 micro seconds
			delayMicroseconds(6);

			// Write 0 to CLOCK
			write_Clock(0);
			
			// Wait 6 micro seconds
			delayMicroseconds(6);
    
			// Read bit i
			int pinValue = read_Data();
			
			// Write 1 to CLOCK
			write_Clock(1);
			
			// Set buttons_arr
			buttons_arr[i-1] = pinValue;
    
			// Increment i
			i += 1;
		}
		 print_Message(buttons_arr);
	}
}

// Main method
//
// argument(s): none
// returns: nothing
int main() {
	
	// Initialize pins
	init_SNES();
	
	// Run the driver
	read_SNES();
}

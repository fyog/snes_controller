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

#define CLOCK 11
#define LATCH 9
#define DATA 10

#define INP_GPIO(g) *(gpioPtr+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpioPtr+((g)/10)) |= (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpioPtr+(((g)/10))) |= (((a)<=3? (a)+4: (a)==4? 3:2) << (((g)%10)*3))



unsigned int *gpioPtr;

int main()
{
	// Get GPIO ptr
	
	gpioPtr = getGPIOPtr();  
	printf("pointer address: %p\n", gpioPtr);
	
    //set latch to output
    //gpioPtr[LATCH/10] = (gpioPtr[LATCH/10] & ~(0b111 << ((LATCH%10)*3) | (GPFSEL1 << ((LATCH % 10)*3))));
    INP_GPIO(LATCH);
    OUT_GPIO(LATCH);
    
    //set clock to output
    //gpioPtr[CLOCK/10] = (gpioPtr[CLOCK/10] & ~(0b111 << ((CLOCK%10)*3) | (GPFSEL1 << ((CLOCK % 10)*3))));
    INP_GPIO(CLOCK);
    OUT_GPIO(CLOCK);
    
    //set data to input
    //gpioPtr[DATA/10] = (gpioPtr[DATA/10] & ~(0b111 << ((DATA%10)*3) | (GPFSEL0 << ((DATA % 10)*3))));
    INP_GPIO(DATA);
    
    
    // Initialize running bool
    bool running = true;
    //unsigned int buttons = 0;
    //static int buttons_arr[32];
    
    while (running){

		// Increment GPIO ptr
		//unsigned int buttons;
        int pinValue;
        static int buttons_arr[20];
    
    
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

			// Write 0 to clock
			gpioPtr[GPCLR0] = 1 << CLOCK;
    
			// Wait 6 micro seconds
			delayMicroseconds(6);
    
			// Read bit i
			pinValue = (gpioPtr[GPLEV0] >> DATA) & 1;
			//printf("pinvalue: %d i = %d\n", pinValue, i);
    
        
			// Write 0 to CLOCK
			gpioPtr[GPSET0] = (1 << CLOCK);
            
            // Set buttons register
			//buttons |= (pinValue << i);
            buttons_arr[i-1] = pinValue;
    
			// Increment i
			i += 1;
		}
        
/*
        for(int x = 0; x < 20; x++){
            printf("%d", buttons_arr[x]);

        }
        printf("\n");
*/
		//printf("buttons reg: %d\n", buttons);
		
		// Print buttons register
        /*
		i = 1;
		while (i <= 32){
			pinValue = (buttons >> i) & 1;
			printf("%d", pinValue);
			i += 1;
		}
		printf("\n");
        */

        delay(10);
		// Check if B is being pressed
		if (buttons_arr[0] == 0) {
			printf("B is being pressed...\n");
		}
		
		// Check if Y is being pressed
		if (buttons_arr[1] == 0) {
			printf("Y is being pressed...\n");
		}
        //will put in the rest of the code to detect button once clock is fixed
        
		
		// Check if SELECT is being pressed
		//pinValue = (buttons >> 3) & 1;
		if (buttons_arr[2] == 0) {
			printf("SELECT is being pressed...\n");
		}
		
		// Check if START is being pressed
		//pinValue = (buttons >> 4) & 1;
		if (buttons_arr[3] == 0) {
			printf("START is being pressed...\n");
		}
		
		// Check if UP is being pressed
		//pinValue = (buttons >> 5) & 1;
		if (buttons_arr[4] == 0) {
			printf("UP is being pressed...\n");
		}
		
		// Check if DOWN is being pressed
		//pinValue = (buttons >> 6) &  1;
		if (buttons_arr[5] == 0) {
			printf("DOWN is being pressed...\n");
		}
		
		// Check if LEFT is being pressed
		//pinValue = (buttons >> 7) & 1;
		if (buttons_arr[6] == 0) {
			printf("LEFT is being pressed...\n");
		}
		
		// Check if RIGHT is being pressed
		//pinValue = (buttons >> 8) & 1;
		if (buttons_arr[7] == 0) {
			printf("RIGHT is being pressed...\n");
		}
		
		// Check if A is being pressed
		//pinValue = (buttons >> 9) & 1;
		if (buttons_arr[8] == 0) {
			printf("A is being pressed...\n");
		}
		
		// Check if X is being pressed
		//pinValue = (buttons >> 10) & 1;
		if (buttons_arr[9] == 0) {
			printf("X is being pressed...\n");
		}
		
		// Check if L is being pressed
		//pinValue = (buttons >> 11) & 1;
		if (buttons_arr[10] == 0) {
			printf("L is being pressed...\n");
		}
		
		// Check if R is being pressed
		//pinValue = (buttons >> 12) & 1;
		if (buttons_arr[11] == 0) {
			printf("R is being pressed...\n");
		}
        
		
    }
}
		

// This program is a driver for a SNES controller. It makes use of the Raspberry Pi's GPIO lines. It was completed
// to fulfill the requirements laid out by Project Part 1, CPSC 359.
//
// Authors: Eric Ross (UCID: 10178538), Matthew Newton (UCID: 30094756)

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>



// Main method
//
// argument(s): none
// returns: nothing
int main() {
	
	int board[32][32];
	bool running = true;
	
	// Initialize pins
	init_SNES();

	//Initialize board
	init_Board(&board[0][0]);
	
	// Print board
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 32; j++) {
			printf("%d", *((&board[0][0]+i*32) + j));
		} 
		printf("\n");
	}
	
	// Delay for 2 seconds
	delay(2000);
	
	// Run the controller driver
	while (running) {
		read_SNES();
	}
	printf("successfully exited\n");

}

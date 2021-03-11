// This program is a driver for a SNES controller. It makes use of the Raspberry Pi's GPIO lines. It was completed
// to fulfill the requirements laid out by Project Part 1, CPSC 359.
//
// Authors: Eric Ross (UCID: 10178538), Matthew Newton (UCID: 30094756)

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

struct Player {
	//char* name;
	int locationX;
	int locationY;
};

//Global variables
struct Player playerOne;
int board[32][32];

// Initializes the player
//
// argument(s): x location of the player, y location of the player
// returns: nothing
void init_Player(int locationX_par, int locationY_par) {
	
	//char f; 
	//playerOne.name = &f;
	playerOne.locationX = locationX_par;
	playerOne.locationY = locationY_par;
	board[locationY_par][locationX_par] = 1;
}

// Updates the player's position
//
// argument(s): none
// returns: nothing
void update_Player() {
	board[playerOne.locationY][playerOne.locationX] = 1;
}
	
// Moves the player down
//
// argument(s): current y position of the player
// returns: nothing
void down(int currentY) {
	if (currentY < 31) {
		board[playerOne.locationY][playerOne.locationX] = 0;
		playerOne.locationY = currentY + 1;
	} 
}

// Moves the player up
//
// argument(s): current y position of the player
// returns: nothing
void up (int currentY) {
	if (currentY > 0) {
		board[playerOne.locationY][playerOne.locationX] = 0;
		playerOne.locationY = currentY - 1;
	}
}

// Moves the player left
//
// argument(s): current x position of the player
// returns: nothing
void left(int currentX) {
	if (currentX > 0) {
		board[playerOne.locationY][playerOne.locationX] = 0;
		playerOne.locationX = currentX - 1;
	} 
}

// Moves the player right
//
// argument(s): current x position of the player
// returns: nothing
void right(int currentX) {
	if (currentX < 31) {
		board[playerOne.locationY][playerOne.locationX] = 0;
		playerOne.locationX = currentX + 1;
	} 
}

// Main method
//
// argument(s): none
// returns: nothing
int main() {
	
	// Create running boolean
	bool running = true;
	
	// Initialize pins
	init_SNES();

	// Initialize the board
	init_Board(board);
	// Initialize player
	init_Player(0, 31);
	int sensitivity = 0;
	
	// Get start time
	time_t startTime = time(NULL);
	
	// Game loop
	while (running) {
		
		// Get current time
		time_t currentTime = time(NULL);
		
		// Print time elapsed
		int timeElapsed = currentTime - startTime;
		int timeRemaining = 90 - timeElapsed;
		printf("%d s\n", timeRemaining);
		
		// Read from the controller
		int* buttons_arr = read_SNES();
		
		// React to button presses
		if (sensitivity % 40 == 0) {
			if (buttons_arr[4] == 0) {
				up(playerOne.locationY);
			} 
			if (buttons_arr[5] == 0) {
				down(playerOne.locationY);
			} 
			if (buttons_arr[6] == 0) {
				left(playerOne.locationX);
			} 
			if (buttons_arr[7] == 0) {
				right(playerOne.locationX);
			}
			if (buttons_arr[3] == 0) {
				running = false;
			}
		}
		
		// Update the player's position
		update_Player(&board[0][0]);
		
		// Print the board
		print_Board(&board[0][0]);
		
		// Print the player's current location
		printf("Location X: %d\nLocation Y: %d\n", playerOne.locationX, playerOne.locationY);
		
		// Check for win
		if (playerOne.locationY == 0) {
			printf("You win!\n");
			running = false;
		}
		
		// Increment sensitivity counter
		sensitivity += 1;
	}
	
	// Print exit message
	printf("Successfully exited.\n");

}

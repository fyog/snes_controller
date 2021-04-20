// This program runs the game Frogger. It makes use of the Raspberry Pi's GPIO lines. It was completed
// to fulfill the requirements laid out by Project Part 2, CPSC 359.
//
// Authors: Eric Ross (UCID: 10178538), Matthew Newton (UCID: 30094756)

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include "controller_driver.h"
#include "board.h"
#include <pthread.h>

// Structure definitions
struct Player {
	//char* name;
	int locationX;
	int locationY;
};

struct Obstacle {
	int locationX;
	int locationY;
};

struct Transporter {
	int locationX;
	int locationY;
};

//Global variables
struct Player playerOne;
char board[32][32];
bool playerOnLog;

// Player methods -------------------------------------------------------------------------------

// Initializes the player
//
// argument(s): x location of the player, y location of the player
// returns: nothing
void init_Player(int locationX_par, int locationY_par ) {
	//char f; 
	//playerOne.name = &f;
	playerOne.locationX = locationX_par;
	playerOne.locationY = locationY_par;
	board[locationY_par][locationX_par] = 'X';
}

// Updates the player's position
//
// argument(s): none
// returns: nothing
void update_Player() {
	board[playerOne.locationY][playerOne.locationX] = 'X';
}
	
// Moves the player down
//
// argument(s): current y position of the player
// returns: nothing
void down(int currentY) {
	if (currentY < 31) {
		board[playerOne.locationY][playerOne.locationX] = '-';
		playerOne.locationY = currentY + 1;
	} 
}

// Moves the player up
//
// argument(s): current y position of the player
// returns: nothing
void up (int currentY) {
	if (currentY > 0) {
		board[playerOne.locationY][playerOne.locationX] = '-';
		playerOne.locationY = currentY - 1;
	}
}

// Moves the player left
//
// argument(s): current x position of the player
// returns: nothing
void left(int currentX) {
	if (currentX > 0) {
		board[playerOne.locationY][playerOne.locationX] = '-';
		playerOne.locationX = currentX - 1;
	} 
}

// Moves the player right
//
// argument(s): current x position of the player
// returns: nothing
void right(int currentX) {
	if (currentX < 31) {
		board[playerOne.locationY][playerOne.locationX] = '-';
		playerOne.locationX = currentX + 1;
	} 
}

// Obstacle methods -----------------------------------------------------------------------------

struct Obstacle init_Obstacle(int positionX_par, int positionY_par) {
	struct Obstacle obstacle;
	obstacle.locationX = positionX_par;
	obstacle.locationY = positionY_par;
	board[positionY_par][positionX_par] = '@';
	return obstacle;
}

// Move method
struct Obstacle move_Obstacle(struct Obstacle obstacle, bool up, bool down, bool left, bool right) {
	if (up == true) {
		board[obstacle.locationY][obstacle.locationX] = '-';
		obstacle.locationY += 1;
	} else if (down == true) {
		board[obstacle.locationY][obstacle.locationX] = '-';
		obstacle.locationY -= 1;
	} else if (left == true) {
		board[obstacle.locationY][obstacle.locationX] = '-';
		obstacle.locationX -= 1;
	} else if (right == true) {
		board[obstacle.locationY][obstacle.locationX] = '-';
		obstacle.locationX += 1;
	}
	return obstacle;
}

// Update method
void update_Obstacle(struct Obstacle obstacle) {
	board[obstacle.locationY][obstacle.locationX] = '@';
}	

// Transporter methods -------------------------------------------------------------------------

struct Transporter init_Transporter(int positionX_par, int positionY_par) {
	struct Transporter transporter;
	transporter.locationX = positionX_par;
	transporter.locationY = positionY_par;
	board[transporter.locationY][transporter.locationX] = 'U';
	return transporter;
}

// Move method
struct Transporter move_Transporter(struct Transporter transporter_par, int up, int down, int left, int right) {
	if (up > 0) {
	board[transporter_par.locationY][transporter_par.locationX] = '-';
	transporter_par.locationY += up;
		if (playerOnLog) {
			playerOne.locationY += up;
		}
	} else if (down > 0) {
		board[transporter_par.locationY][transporter_par.locationX] = '-';
		transporter_par.locationY -= down;
		if (playerOnLog) {
			playerOne.locationY -= down;
		}
	} else if (left > 0) {
		board[transporter_par.locationY][transporter_par.locationX] = '-';
		transporter_par.locationX -= left;
		if (playerOnLog) {
			playerOne.locationX -= left;
		}
	} else if (right > 0) {
		board[transporter_par.locationY][transporter_par.locationX] = '-';
		transporter_par.locationX += right;
		if (playerOnLog) {
			playerOne.locationX += right;
		}
	 }
	return transporter_par;
}

// Update method
void update_Transporter(struct Transporter transporter) {
	if (playerOnLog) {
		board[transporter.locationY][transporter.locationX] = 'O';
	} else if (!playerOnLog) {
		board[transporter.locationY][transporter.locationX] = 'U';
	}
}

// Game loop -----------------------------------------------------------------------------------

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
	
	// Initialize controller thread
	//pthread_t controller_Thread;
	
	//pthread_create(&controller_Thread, NULL, run, NULL);
	// Initialize the obstacles
	struct Obstacle obstacle_one = init_Obstacle(16, 16);
	
	// Initialize the transporters
	struct Transporter transporter_one = init_Transporter(10, 10);
	
	// Initialize sensitivity counter
	int sensitivity = 0;
	
	// Get start time
	time_t startTime = time(NULL);
	
	// Game loop
	while (running) {
		
		// Get current time
		time_t currentTime = time(NULL);
		
		// Read from the controller
	int* buttons_arr = read_SNES();
		
		// React to button presses
		if (sensitivity % 20 == 0) {
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
		
		// Print time remaining
		int timeElapsed = currentTime - startTime;
		int timeRemaining = 90 - timeElapsed;
		printf("%d s\n", timeRemaining);
	
		// Alter the obstacle's position
		if (sensitivity % 20 == 0) {
			//board[obstacle_one.locationY][obstacle_one.locationX] = '-';
			//obstacle_one.locationY += 1;
			obstacle_one = move_Obstacle(obstacle_one, 1, 0, 0, 0);
		} else if (sensitivity % 20 == 10) {
			//board[obstacle_one.locationY][obstacle_one.locationX] = '-';
			//obstacle_one.locationY -= 1;
			obstacle_one = move_Obstacle(obstacle_one, 1, 0, 0, 0);

		}
		
		// Alter the transporter's position
		if (sensitivity % 100 == 0) {
			//board[transporter_one.locationY][transporter_one.locationX] = '-';
			//transporter_one.locationY += 1;
			transporter_one = move_Transporter(transporter_one, 1, 0, 0, 0);
			if (playerOnLog) {
				board[playerOne.locationY][playerOne.locationX] = '-';
				playerOne.locationY = transporter_one.locationY;
			}
		} else if (sensitivity % 100 == 50) {
			//board[transporter_one.locationY][transporter_one.locationX] = '-';
			transporter_one = move_Transporter(transporter_one, 0, 1, 0, 0);
			//transporter_one.locationY -= 1;
			if (playerOnLog) {
				board[playerOne.locationY][playerOne.locationX] = '-';
				playerOne.locationY = transporter_one.locationY;
			}
		}
		
		// Update the player's position
		update_Player(board);
		
		// Update the obstacle
		update_Obstacle(obstacle_one);
	
		// Update the transporter
		update_Transporter(transporter_one);
		
		// Print the board
		print_Board(board);
		
		// Check for collisions
		if (playerOne.locationX == obstacle_one.locationX && playerOne.locationY == obstacle_one.locationY) {
			running = false;
			printf("You died! Try again\n");
		}
		
		// Check if player on log
		if (playerOne.locationX == transporter_one.locationX && playerOne.locationY == transporter_one.locationY) {
			playerOnLog = true;
		} else {
			playerOnLog = false;
		}
		
		// Check for win
		if (playerOne.locationY == 0) {
			printf("You win!\n");
			running = false;
		}
		
		// Check for out of time
		if (timeRemaining <= 0) {
			printf("Out of time!\n");
			running = false;
		}
		
		// Increment sensitivity counter
		sensitivity += 1;
	}
	
	// Print exit message
	printf("Successfully exited.\n");

}

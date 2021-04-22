// This program runs the game Frogger. It makes use of the Raspberry Pi's GPIO lines. It was completed
// to fulfill the requirements laid out by Project Part 2, CPSC 359.
//
// Authors: Eric Ross (UCID: 10178538), Matthew Newton (UCID: 30094756)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <sys/mman.h>
#include "framebuffer.h"
#include "controller_driver.h"
#include "board.h"
#include <pthread.h>
#include "frog.h"
#include "arrow.h"
#include "castle.h"
#include "log.h"
#include "frogPlusLog.h"
#include "rArrow.h"
#include "mainMenu.h"

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

typedef struct {
	short int color;
	int x, y;
} Pixel;

struct fbs fbstruct;

//Global variables
struct Player playerOne;
char board[21][50];
bool playerOnLog;
int drawColours[21][40];
char currentChar;

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
	currentChar = board[playerOne.locationY][playerOne.locationX];
	board[playerOne.locationY][playerOne.locationX] = 'X';
	
}
	
// Moves the player down
//
// argument(s): current y position of the player
// returns: nothing
void down(int currentY) {
	if (currentY < 20) {
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
	if (currentX > 5) {
		board[playerOne.locationY][playerOne.locationX] = '-';
		playerOne.locationX = currentX - 1;
	} 
}

// Moves the player right
//
// argument(s): current x position of the player
// returns: nothing
void right(int currentX) {
	if (currentX < 44) {
		board[playerOne.locationY][playerOne.locationX] = '-';
		playerOne.locationX = currentX + 1;
	} 
}

// Obstacle methods -----------------------------------------------------------------------------

// Initialize obstacle
struct Obstacle init_Obstacle(int positionX_par, int positionY_par, char representation) {
	struct Obstacle obstacle;
	obstacle.locationX = positionX_par;
	obstacle.locationY = positionY_par;
	board[positionY_par][positionX_par] = representation;
	return obstacle;
}


// Move method
struct Obstacle move_Obstacle(struct Obstacle obstacle, int up, int down, int left, int right) {
	
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
void update_Obstacle(struct Obstacle obstacle, char char_representation) {
	board[obstacle.locationY][obstacle.locationX] = char_representation;
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

// Graphics methods ---------------------------------------------------------------------------

/* Draw a pixel */
void drawPixel(Pixel *pixel){
	long int location = (pixel->x +fbstruct.xOff) * (fbstruct.bits/8) +
                       (pixel->y+fbstruct.yOff) * fbstruct.lineLength;
	*((unsigned short int*)(fbstruct.fptr + location)) = pixel->color;
}

/*
*Draw Map
*
*
*/
void init_map(){
	fbstruct = initFbInfo();
	
	short int *frogPtr=(short int *) frog_map.pixel_data;
	short int *arrowPtr = (short int *) arrow_map.pixel_data;
	short int *castlePtr = (short int *) castle_map.pixel_data;
	short int *logPtr = (short int *) log_map.pixel_data;
	short int *lPfPtr = (short int *) frogPlusLog_map.pixel_data;
	short int *rArrowPtr = (short int *) rightArrow_map.pixel_data;
	
	Pixel *pixel;
	pixel = malloc(sizeof(pixel));
	
	int ycount = 0;
	int xcount = 0;
	short int i = 0;
	short int j = 0;
	short int last = 0;
	
	for (int y = 0; y < 672; y++)
	{
		if(y % 32 == 0 && ycount < 20 && y != 0){
			ycount++;
		}
	
		if(y % 32 == 0){
			j = 0;
		}
		last = 32 * j;
		j++;
		
		for (int x = 0; x < 1280; x++) 
		{	
				if(x % 32 == 0 && xcount < 41 && x != 0){
					xcount++;                                                          
				}
				
				if(x % 32 == 0){    
					i = last;                                                   
				}
				
				//printf("outside i: %d\n", i);
				
				if(board[ycount][xcount+5] == 'X'){
					pixel->color = frogPtr[i]; //0x00FF; //alienPtr[i]; // pixel
					pixel->x = x;
					pixel->y = y;
					//printf("i: %d\n", i);
				}else if(board[ycount][xcount+5] == '@'){
					pixel->color = arrowPtr[i]; 
					pixel->x = x;
					pixel->y = y;
				}else if(board[ycount][xcount+5] == '#'){
					pixel->color = castlePtr[i]; 
					pixel->x = x;
					pixel->y = y;
				}else if(board[ycount][xcount+5] == 'U'){
					pixel->color = logPtr[i]; 
					pixel->x = x;
					pixel->y = y;
				}else if(board[ycount][xcount+5] == 'O'){
					pixel->color = lPfPtr[i]; 
					pixel->x = x;
					pixel->y = y;
				}else if(board[ycount][xcount+5] == 'R'){
					pixel->color = rArrowPtr[i]; 
					pixel->x = x;
					pixel->y = y;
				}else{
					pixel->color = drawColours[ycount][xcount]; // pixel
					pixel->x = x;
					pixel->y = y;
				}
				i++;
	
				drawPixel(pixel);
		}
		xcount = 0;
		
	}
	/* free pixel's allocated memory */
	free(pixel);
	pixel = NULL;
	munmap(fbstruct.fptr, fbstruct.screenSize);

	
}

void block_draw(int mul, int mul2, int colour){
	fbstruct = initFbInfo();
	
	Pixel *pixel;
	pixel = malloc(sizeof(pixel));
	int xstart = 32 * mul;
	int ystart = 32 * mul2;
	//printf("xstart %d\n", xstart);
	//printf("ystart %d\n", ystart);
	
	for (int y = ystart; y < ystart + 32; y++)
	{
		for (int x = xstart; x < xstart + 32; x++) 
		{	
				pixel->color = colour; // black pixel
				pixel->x = x;
				pixel->y = y;
	
				drawPixel(pixel);
		}
	}
	/* free pixel's allocated memory */
	free(pixel);
	pixel = NULL;
	munmap(fbstruct.fptr, fbstruct.screenSize);
	
}

void draw_map(){
	
	for(int i = 0; i < 21; i++){
		for(int j = 0; j < 40; j++){
			if(board[i][j+5] == '-'){
				drawColours[i][j] = 0x0000;
				//printf("colour 1: %d\n", drawColours[i][j]);
				//block_draw(i, j, 0x0000);//drawColours[i][j] = 0xF800;
			}else {
				//init_map(i, j, 0xF800);
				drawColours[i][j] = 0x00FF;
				//printf("colour 2: %d\n", drawColours[i][j]);
			}
			/*
			else{
				//init_map(i, j, 0xF800);
				drawColours[i][j] = 0xF800;
				//printf("colour 2: %d\n", drawColours[i][j]);
			}
			*/
		}
	}
	//init_map();
	
}

void draw_mainMenu(int selected){
	/* initialize + get FBS */
	fbstruct = initFbInfo();
	
	short int *mainMenuPtr=(short int *) mainMenu_map.pixel_data;
	
	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	int i=0;
	for (int y = 0; y < 672; y++)//30 is the image height
	{
		for (int x = 0; x < 1280; x++) // 30 is image width
		{	
				pixel->color = mainMenuPtr[i]; 
				pixel->x = x;
				pixel->y = y;
				drawPixel(pixel);
				i++;
		}
	}
	i = 0;
	int ystart = 320;
	int yend = 352;
	if(selected == 1){
		ystart = 420;
		yend = 452;
	}
	for (int y = ystart; y < yend; y++)//30 is the image height
	{
		for (int x = 468; x < 500; x++) // 30 is image width
		{	
				pixel->color = 0x00FF; 
				pixel->x = x;
				pixel->y = y;
				drawPixel(pixel);
				i++;
		}
	}
	/* free pixel's allocated memory */
	free(pixel);
	pixel = NULL;
	munmap(fbstruct.fptr, fbstruct.screenSize);
}

void mainMenu(){
	
	int selector = 0;
//	int* buttons_arr = read_SNES();
	
	while(1){
		int* buttons_arr = read_SNES();
		if(buttons_arr[4] == 0){
			selector = 0;
		}else if(buttons_arr[5] == 0){
			selector = 1;
		}
		draw_mainMenu(selector);
		if(selector == 0 && buttons_arr[8] == 0){
			break;
		}else if(selector == 1 && buttons_arr[8] == 0){
			printf("Thanks for playing!\n");
			exit(1);
		}
	}
	
}
//

// Main method  --------------------------------------------------------------------------------

// Main
//
// argument(s): none
// returns: nothing
int main() {
	

	// Create running boolean
	bool running = true;

restart: 
	running = true;
	// Initialize pins
	init_SNES();
	
	mainMenu();
	//draw_mainMenu();

	// Initialize the board
	init_Board(*board);
	
	// Initialize player
	init_Player(25, 20);
	
	//initialize map
	//init_map();
	
	// Initialize controller thread
	//pthread_t controller_Thread;
	
	//pthread_create(&controller_Thread, NULL, run, NULL);
	
	// Initialize the obstacles
	struct Obstacle obstacle_one = init_Obstacle(45, 16, '@'); // right to left, row 16
	struct Obstacle obstacle_two = init_Obstacle(0, 17, 'R'); // left to right, row 17
	struct Obstacle obstacle_three = init_Obstacle(45, 18, '@'); // right to left, row 18
	struct Obstacle obstacle_four = init_Obstacle(0, 19, 'R'); // left to right, row 19
	struct Obstacle obstacle_five = init_Obstacle(0, 16, '@'); //left to right, row 16
	struct Obstacle obstacle_six = init_Obstacle(0, 17, 'R'); // left to right, row 17
	struct Obstacle obstacle_seven = init_Obstacle(45, 18, '@'); // right to left, row 18
	struct Obstacle obstacle_eight = init_Obstacle(4, 19, 'R'); // left to right, row 18

	
	// Initialize the transporters
	struct Transporter transporter_one = init_Transporter(0, 9);
	struct Transporter transporter_two = init_Transporter(45, 8);
	struct Transporter transporter_three = init_Transporter(0, 7);
	struct Transporter transporter_four = init_Transporter(45, 6);

	
	// Initialize sensitivity counter
	int sensitivity = 0;
	
	//initial map
	draw_map();
	
	// Get start time
	time_t startTime = time(NULL);
	
	// Game loop...
	while (running) {
		
	// Get current time
	time_t currentTime = time(NULL);
		
		// Read from the controller
	int* buttons_arr = read_SNES();
		
		// React to button presses
		if (sensitivity % 4 == 0) {
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
				running = false; // bring up menu eventually
			}
		}
		
		// Print time remaining
		int timeElapsed = currentTime - startTime;
		int timeRemaining = 90 - timeElapsed;
		//printf("%d s\n", timeRemaining);
		
// Obstacle movement -----------------------------------------------------------------------------
	
		// Alter the 1st obstacle's position (right to left), row 16
		if (sensitivity % 5 == 0) {
			if (obstacle_one.locationX <= 0) {
				board[obstacle_one.locationY][obstacle_one.locationX] = '-';
				int delay_Selector = rand() % 5;
				obstacle_one.locationX = 44 + delay_Selector;
			} else {
				obstacle_one = move_Obstacle(obstacle_one, 0, 0, 1, 0);
			}
		}
		
		// Alter the 2nd obstacle's position (left to right), row 17
		if (sensitivity % 10 == 0) {
			if (obstacle_two.locationX >= 45) {
				board[obstacle_two.locationY][obstacle_two.locationX] = '-';
				int delay_Selector = rand() % 5;
				obstacle_two.locationX = 0 + delay_Selector;
			} else {
				obstacle_two = move_Obstacle(obstacle_two, 0, 0, 0, 1);
			}
		} 
		
		// Alter the 3rd obstacle's position (right to left)
		if (sensitivity % 5 == 0) {
			if (obstacle_three.locationX <= 0) {
				board[obstacle_three.locationY][obstacle_three.locationX] = '-';
				int delay_Selector = rand() % 5;
				obstacle_three.locationX = 44 + delay_Selector;
			} else {
				obstacle_three = move_Obstacle(obstacle_three, 0, 0, 1, 0);
			}
		}
		
		// Alter the 4th obstacle's position (left to right)
		if (sensitivity % 2 == 0) {
			if (obstacle_four.locationX >= 45) {
				board[obstacle_four.locationY][obstacle_four.locationX] = '-';
				int delay_Selector = rand() % 5;
				obstacle_four.locationX = 0 + delay_Selector;
			} else {
				obstacle_four = move_Obstacle(obstacle_four, 0, 0, 0, 1);
			}
		}
		
		// Alter the 5th obstacle's position (right to left), row 16
		if (sensitivity % 5 == 0) {
			if (obstacle_five.locationX <= 0) {
				board[obstacle_five.locationY][obstacle_five.locationX] = '-';
				int delay_Selector = rand() % 5;
				obstacle_five.locationX = 44 + delay_Selector;
			} else {
				obstacle_five = move_Obstacle(obstacle_five, 0, 0, 1, 0);
			}
		}
		
		// Alter the 6th obstacle's position (left to right), row 17
		if (sensitivity % 10 == 0) {
			if (obstacle_six.locationX >= 45) {
				board[obstacle_six.locationY][obstacle_six.locationX] = '-';
				int delay_Selector = rand() % 5;
				obstacle_six.locationX = 0 + delay_Selector;
			} else {
				obstacle_six = move_Obstacle(obstacle_six, 0, 0, 0, 1);
			}
		}
		// Alter the 7th obstacle's position (right to left)
		if (sensitivity % 5 == 0) {
			if (obstacle_seven.locationX <= 0) {
				board[obstacle_seven.locationY][obstacle_seven.locationX] = '-';
				int delay_Selector = rand() % 5;
				obstacle_seven.locationX = 0 + delay_Selector;
			} else {
				obstacle_seven = move_Obstacle(obstacle_seven, 0, 0, 1, 0);
			}
		}
		// Alter the 8th obstacle's position (left to right), row 18
		if (sensitivity % 2 == 0) {
			if (obstacle_eight.locationX >= 45) {
				board[obstacle_eight.locationY][obstacle_eight.locationX] = '-';
				int delay_Selector = rand() % 5;
				obstacle_eight.locationX = 0 + delay_Selector;
			} else {
				obstacle_eight = move_Obstacle(obstacle_eight, 0, 0, 0, 1);
			}
		}
// Transporter movement ---------------------------------------------------------------------------

		// Alter the 1st transporter's position
		if (sensitivity % 10 == 0) {
			if (playerOne.locationX >= 45) {
				printf("You fell off the edge of the map!");
				running = false;
			}
			if (transporter_one.locationX >= 45) {
				board[transporter_one.locationY][transporter_one.locationX] = '-';
				int delay_Selector = rand() % 5;
				transporter_one.locationX = 0 + delay_Selector;
			} else {
				transporter_one = move_Transporter(transporter_one, 0, 0, 0, 1);
				if (playerOnLog) {
					board[playerOne.locationY][playerOne.locationX] = '-';
					playerOne.locationY = transporter_one.locationY;
				}
			}
		}
		
		// Alter the 2nd transporter's position
		if (sensitivity % 10 == 0) {
			if (playerOne.locationX >= 45) {
				printf("You fell off the edge of the map!");
				running = false;
			}
			if (transporter_two.locationX <= 0) {
				board[transporter_two.locationY][transporter_two.locationX] = '-';
				int delay_Selector = rand() % 5;
				transporter_two.locationX = 0 + delay_Selector;
			} else {
				transporter_two = move_Transporter(transporter_two, 0, 0, 1, 0);
				if (playerOnLog) {
					board[playerOne.locationY][playerOne.locationX] = '-';
					playerOne.locationY = transporter_two.locationY;
				}	
			}
		}
		
		// Alter the 3rd transporter's position
		if (sensitivity % 10 == 0) {
			if (playerOne.locationX >= 45) {
				printf("You fell off the edge of the map!");
				running = false;
			}
			if (transporter_three.locationX >= 45) {
				board[transporter_three.locationY][transporter_three.locationX] = '-';
				int delay_Selector = rand() % 5;
				transporter_three.locationX = 0 + delay_Selector;
			} else {
				transporter_three = move_Transporter(transporter_three, 0, 0, 0, 1);
				if (playerOnLog) {
				board[playerOne.locationY][playerOne.locationX] = '-';
				playerOne.locationY = transporter_three.locationY;
				}
			}
		}
		
		// Alter the 4th transporter's position
		if (sensitivity % 10 == 0) {
			if (playerOne.locationX >= 45) {
				printf("You fell off the edge of the map!");
				running = false;
			}
			if (transporter_four.locationX >= 45) {
				board[transporter_four.locationY][transporter_four.locationX] = '-';
				int delay_Selector = rand() % 5;
				transporter_four.locationX = 0 + delay_Selector;
			} else {
				transporter_four = move_Transporter(transporter_four, 0, 0, 1, 0);
				if (playerOnLog) {
				board[playerOne.locationY][playerOne.locationX] = '-';
				playerOne.locationY = transporter_four.locationY;
				}
			}
		}
		
	


// Update components ------------------------------------------------------------------------------

		// Update the player's position
		update_Player(board);
		
		// Update the obstacles
		update_Obstacle(obstacle_one, '@');
		update_Obstacle(obstacle_two, 'R');
		update_Obstacle(obstacle_three, '@');
		update_Obstacle(obstacle_four, 'R');
		update_Obstacle(obstacle_five, '@');
		update_Obstacle(obstacle_six, 'R');
		update_Obstacle(obstacle_seven, '@');
		update_Obstacle(obstacle_eight, 'R');

		// Update the transporter
		update_Transporter(transporter_one);
		update_Transporter(transporter_two);
		update_Transporter(transporter_three);
		update_Transporter(transporter_four);

		
		// Print the board 
		//print_Board(board); // for text-based
		
		//init_map();
		//draw_map(); // for graphics
		init_map();
		
// Collision detection ---------------------------------------------------------------------------


		if (playerOne.locationX == obstacle_one.locationX && playerOne.locationY == obstacle_one.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_two.locationX && playerOne.locationY == obstacle_two.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_three.locationX && playerOne.locationY == obstacle_three.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_four.locationX && playerOne.locationY == obstacle_four.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_five.locationX && playerOne.locationY == obstacle_five.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_six.locationX && playerOne.locationY == obstacle_six.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_seven.locationX && playerOne.locationY == obstacle_seven.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_eight.locationX && playerOne.locationY == obstacle_eight.locationY) {
			running = false;
		}
		if(running == false){
			goto restart;
		}
		
		// Check if player on log
		if (playerOne.locationX == transporter_one.locationX && playerOne.locationY == transporter_one.locationY) {
			playerOnLog = true;
		} else {
			playerOnLog = false;
		}

// Final checks ----------------------------------------------------------------------------------
		
		// Check for water collision
		if (currentChar == '/') {
			running = false;
			printf ("You drowned!\n");
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
	printf("Successfully exited.\n"); // on exit
}


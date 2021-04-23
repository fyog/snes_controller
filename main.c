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
#include "wiringPi.h"
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
#include "car1.h"
#include "car2.h"
#include "gameOver.h"
#include "gameMenu.h"
#include "gameWin.h"


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
bool running = true;
int lives;

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
struct Obstacle set_Position(struct Obstacle obstacle, int sensitivity, int speed, bool direction){
	if (direction) {
		if (sensitivity % speed == 0) {
			if (obstacle.locationX <= 0) {
				board[obstacle.locationY][obstacle.locationX] = '-';
				int delay_Selector = rand() % 5;
				obstacle.locationX = 44 + delay_Selector;
			} else {
				obstacle = move_Obstacle(obstacle, 0, 0, 1, 0);
			}
		}
	} else if (!direction) {
		if (sensitivity % speed == 0) {
			if (obstacle.locationX >= 45) {
				board[obstacle.locationY][obstacle.locationX] = '-';
				int delay_Selector = rand() % 5;
				obstacle.locationX = 0 + delay_Selector;
			} else {
				obstacle = move_Obstacle(obstacle, 0, 0, 0, 1);
			}
		} 
	}
	return obstacle;
}
		

// Update method
void update_Obstacle(struct Obstacle obstacle, char representation) {
	board[obstacle.locationY][obstacle.locationX] = representation;
}	

// Transporter methods -------------------------------------------------------------------------


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
	short int *car1Ptr = (short int *) car1_map.pixel_data;
	short int *car2Ptr = (short int *) car2_map.pixel_data;
	
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
				}else if(board[ycount][xcount+5] == '%'){
					pixel->color = car1Ptr[i]; 
					pixel->x = x;
					pixel->y = y;
				}else if(board[ycount][xcount+5] == '$'){
					pixel->color = car2Ptr[i]; 
					pixel->x = x;
					pixel->y = y;
				}else if(board[ycount][xcount+5] == 'P'){
					pixel->color = 0x00FF; 
					pixel->x = x;
					pixel->y = y;
				}else if(board[ycount][xcount+5] == '/'){
					pixel->color = 0x00FF; 
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
				drawColours[i][j] = 0x0FF0;
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

void draw_gameMenu(int selected){
	/* initialize + get FBS */
	fbstruct = initFbInfo();
	
	short int *gameMenuPtr=(short int *) gameMenu_map.pixel_data;
	
	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	int i=0;
	for (int y = 168; y < 504; y++)//30 is the image height
	{
		for (int x = 320; x < 960; x++) // 30 is image width
		{	
				pixel->color = gameMenuPtr[i]; 
				pixel->x = x;
				pixel->y = y;
				drawPixel(pixel);
				i++;
		}
	}
	i = 0;
	int ystart = 280;
	int yend = 312;
	if(selected == 1){
		ystart = 350;
		yend = 382;
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

/*
void *count(void *a){
	
}

*/

void drawGameOver(){
	/* initialize + get FBS */
	fbstruct = initFbInfo();
	
	short int *gameOverPtr=(short int *) gameOver_map.pixel_data;
	
	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	int i=0;
	for (int y = 0; y < 672; y++)//30 is the image height
	{
		for (int x = 0; x < 1280; x++) // 30 is image width
		{	
				pixel->color = gameOverPtr[i]; 
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

void drawGameWin(){
	/* initialize + get FBS */
	fbstruct = initFbInfo();
	
	short int *gameWinPtr=(short int *) gameWin_map.pixel_data;
	
	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	int i=0;
	for (int y = 0; y < 672; y++)//30 is the image height
	{
		for (int x = 0; x < 1280; x++) // 30 is image width
		{	
				pixel->color = gameWinPtr[i]; 
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

void gameMenu(){
	int selector = 0;
	
	delay(500);
	
	while(1){
		int* buttons_arr = read_SNES();
		if(buttons_arr[4] == 0){
			selector = 0;
		}else if(buttons_arr[5] == 0){
			selector = 1;
		}
		draw_gameMenu(selector);
		if(selector == 0 && buttons_arr[8] == 0){
			running = false;
			break;
		}else if(selector == 1 && buttons_arr[8] == 0){
			//printf("Thanks for playing!\n");
			running = false;
			break;
		}else if(buttons_arr[3] == 0){
			break;
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
	
//
	// Create running boolean
	running = true;

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
	
	// Level 1
	struct Obstacle obstacle_one = init_Obstacle(46, 16, '@'); // right to left, row 16
	struct Obstacle obstacle_two = init_Obstacle(0, 17, 'R'); // left to right, row 17
	struct Obstacle obstacle_three = init_Obstacle(46, 18, '@'); // right to left, row 18
	struct Obstacle obstacle_four = init_Obstacle(0, 19, 'R'); // left to right, row 19
	struct Obstacle obstacle_five = init_Obstacle(0, 16, '@'); //left to right, row 16
	struct Obstacle obstacle_six = init_Obstacle(0, 17, 'R'); // left to right, row 17
	struct Obstacle obstacle_seven = init_Obstacle(46, 18, '@'); // right to left, row 18
	struct Obstacle obstacle_eight = init_Obstacle(4, 19, 'R'); // left to right, row 18
	
	// Level 2
	struct Obstacle obstacle_one_one = init_Obstacle(46, 14, '$'); // right to left, row 16
	struct Obstacle obstacle_two_two = init_Obstacle(0, 13, '%'); // left to right, row 17
	struct Obstacle obstacle_three_three = init_Obstacle(46, 12, '$'); // right to left, row 18
	struct Obstacle obstacle_four_four = init_Obstacle(0, 11, '%'); // left to right, row 19
	struct Obstacle obstacle_five_five = init_Obstacle(0, 14, '$'); //left to right, row 16
	struct Obstacle obstacle_six_six = init_Obstacle(0, 13, '%'); // left to right, row 17
	struct Obstacle obstacle_seven_seven = init_Obstacle(46, 12, '$'); // right to left, row 18
	struct Obstacle obstacle_eight_eight = init_Obstacle(4, 11, '%'); // left to right, row 18
	
	// Level 3
	struct Obstacle obstacle_one_one_one = init_Obstacle(46, 9, 'U'); // right to left, row 9
	struct Obstacle obstacle_two_two_two = init_Obstacle(0, 8, 'U'); // left to right, row 8
	struct Obstacle obstacle_three_three_three = init_Obstacle(46,7, 'U'); // right to left, row 7
	struct Obstacle obstacle_four_four_four = init_Obstacle(0, 6, 'U'); // left to right, row 6
	struct Obstacle obstacle_five_five_five = init_Obstacle(0, 9, 'U'); //left to right, row 9
	struct Obstacle obstacle_six_six_six = init_Obstacle(0, 8, 'U'); // left to right, row 8 
	struct Obstacle obstacle_seven_seven_seven = init_Obstacle(46, 7, 'U'); // right to left, row 7
	struct Obstacle obstacle_eight_eight_eight = init_Obstacle(4, 6, 'U'); // left to right, row 6
	
	// Level 4
	struct Obstacle obstacle_one_one_one_one = init_Obstacle(46, 4, 'U'); // right to left, row 4
	struct Obstacle obstacle_two_two_two_two = init_Obstacle(0, 3, 'U'); // left to right, row 3
	struct Obstacle obstacle_three_three_three_three = init_Obstacle(46,2, 'U'); // right to left, row 2
	struct Obstacle obstacle_four_four_four_four = init_Obstacle(0, 1, 'U'); // left to right, row 1
	struct Obstacle obstacle_five_five_five_five = init_Obstacle(0, 4, 'U'); //left to right, row 4
	struct Obstacle obstacle_six_six_six_six = init_Obstacle(0, 3, 'U'); // left to right, row 3
	struct Obstacle obstacle_seven_seven_seven_seven = init_Obstacle(46, 2, 'U'); // right to left, row 2
	struct Obstacle obstacle_eight_eight_eight_eight = init_Obstacle(4, 1, 'U'); // left to right, row 1
	
	
	
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
				//running = false; // bring up menu eventually
				gameMenu();
			}
		}
		
		// Print time remaining
		int timeElapsed = currentTime - startTime;
		int timeRemaining = 90 - timeElapsed;
		//printf("%d s\n", timeRemaining);
		if(timeElapsed == 25 || timeElapsed == 45){
			//struct Obstacle valuePack = 
			init_Obstacle(20, 15, 'P');
			
		}
		
// Obstacle movement (Level 1) --------------------------------------------------------------------
	
		// Alter the 1st obstacle's position (right to left), row 15
		obstacle_one = set_Position(obstacle_one, sensitivity, 5, true);
		
		// Alter the 2nd obstacle's position (left to right), row 14
		obstacle_two = set_Position(obstacle_two, sensitivity, 5, false);

		// Alter the 3rd obstacle's position (right to left), row 13
		obstacle_three = set_Position(obstacle_three, sensitivity, 5, true);

		// Alter the 4th obstacle's position (left to right), row 12
		obstacle_four = set_Position(obstacle_four, sensitivity, 5, false);
		
		// Alter the 5th obstacle's position (right to left), row 15
		obstacle_five = set_Position(obstacle_five, sensitivity, 5, true);

		// Alter the 6th obstacle's position (left to right), row 14
		obstacle_six = set_Position(obstacle_six, sensitivity, 5, false);

		// Alter the 7th obstacle's position (right to left), row 13
		obstacle_seven = set_Position(obstacle_seven, sensitivity, 5, true);

		// Alter the 8th obstacle's position (left to right), row 12
		obstacle_eight = set_Position(obstacle_eight, sensitivity, 5, false);
		
// Obstacle movement (Level 2) --------------------------------------------------------------------
	
		// Alter the 1st obstacle's position (right to left), row 16
		obstacle_one_one = set_Position(obstacle_one_one, sensitivity, 4, true);
		
		// Alter the 2nd obstacle's position (left to right), row 17
		obstacle_two_two = set_Position(obstacle_two_two, sensitivity, 4, false);

		// Alter the 3rd obstacle's position (right to left)
		obstacle_three_three = set_Position(obstacle_three_three, sensitivity, 4, true);

		// Alter the 4th obstacle's position (left to right)
		obstacle_four_four = set_Position(obstacle_four_four, sensitivity, 4, false);
		
		// Alter the 5th obstacle's position (right to left), row 16
		obstacle_five_five = set_Position(obstacle_five_five, sensitivity, 4, true);

		// Alter the 6th obstacle's position (left to right), row 17
		obstacle_six_six = set_Position(obstacle_six_six, sensitivity, 4, false);

		// Alter the 7th obstacle's position (right to left)
		obstacle_seven_seven = set_Position(obstacle_seven_seven, sensitivity, 4, true);

		// Alter the 8th obstacle's position (left to right), row 18
		obstacle_eight_eight = set_Position(obstacle_eight_eight, sensitivity, 4, false);
		
// Obstacle movement (Level 3) --------------------------------------------------------------------
	
		// Alter the 1st obstacle's position (right to left), row 16
		obstacle_one_one_one = set_Position(obstacle_one_one_one, sensitivity, 3, true);
		
		// Alter the 2nd obstacle's position (left to right), row 17
		obstacle_two_two_two = set_Position(obstacle_two_two_two, sensitivity, 3, false);

		// Alter the 3rd obstacle's position (right to left)
		obstacle_three_three_three = set_Position(obstacle_three_three_three, sensitivity, 3, true);

		// Alter the 4th obstacle's position (left to right)
		obstacle_four_four_four = set_Position(obstacle_four_four_four, sensitivity, 3, false);
		
		// Alter the 5th obstacle's position (right to left), row 16
		obstacle_five_five_five = set_Position(obstacle_five_five_five, sensitivity, 3, true);

		// Alter the 6th obstacle's position (left to right), row 17
		obstacle_six_six_six = set_Position(obstacle_six_six_six, sensitivity, 3, false);

		// Alter the 7th obstacle's position (right to left)
		obstacle_seven_seven_seven = set_Position(obstacle_seven_seven_seven, sensitivity, 3, true);

		// Alter the 8th obstacle's position (left to right), row 18
		obstacle_eight_eight_eight = set_Position(obstacle_eight_eight_eight, sensitivity, 3, false);

// Obstacle movement (Level 4) --------------------------------------------------------------------
	
		// Alter the 1st obstacle's position (right to left), row 16
		obstacle_one_one_one_one = set_Position(obstacle_one_one_one_one, sensitivity, 2, true);
		
		// Alter the 2nd obstacle's position (left to right), row 17
		obstacle_two_two_two_two = set_Position(obstacle_two_two_two_two, sensitivity, 2, false);

		// Alter the 3rd obstacle's position (right to left)
		obstacle_three_three_three_three = set_Position(obstacle_three_three_three_three, sensitivity, 2, true);

		// Alter the 4th obstacle's position (left to right)
		obstacle_four_four_four_four = set_Position(obstacle_four_four_four_four, sensitivity, 2, false);
		
		// Alter the 5th obstacle's position (right to left), row 16
		obstacle_five_five_five_five = set_Position(obstacle_five_five_five_five, sensitivity, 2, true);

		// Alter the 6th obstacle's position (left to right), row 17
		obstacle_six_six_six_six = set_Position(obstacle_six_six_six_six, sensitivity, 2, false);

		// Alter the 7th obstacle's position (right to left)
		obstacle_seven_seven_seven_seven = set_Position(obstacle_seven_seven_seven_seven, sensitivity, 2, true);

		// Alter the 8th obstacle's position (left to right), row 18
		obstacle_eight_eight_eight_eight = set_Position(obstacle_eight_eight_eight_eight, sensitivity, 2, false);
		
// Update game state ------------------------------------------------------------------------------

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
		
		// Level 2
		update_Obstacle(obstacle_one_one, '$');
		update_Obstacle(obstacle_two_two, '%');
		update_Obstacle(obstacle_three_three, '$');
		update_Obstacle(obstacle_four_four, '%');
		update_Obstacle(obstacle_five_five, '$');
		update_Obstacle(obstacle_six_six, '%');
		update_Obstacle(obstacle_seven_seven, '$');
		update_Obstacle(obstacle_eight_eight, '%');
		
		// Level 3
		update_Obstacle(obstacle_one_one_one, 'U');
		update_Obstacle(obstacle_two_two_two, 'U');
		update_Obstacle(obstacle_three_three_three, 'U');
		update_Obstacle(obstacle_four_four_four, 'U');
		update_Obstacle(obstacle_five_five_five, 'U');
		update_Obstacle(obstacle_six_six_six, 'U');
		update_Obstacle(obstacle_seven_seven_seven, 'U');
		update_Obstacle(obstacle_eight_eight_eight, 'U');
		
		// Level 4
		update_Obstacle(obstacle_one_one_one_one, 'U');
		update_Obstacle(obstacle_two_two_two_two, '@');
		update_Obstacle(obstacle_three_three_three_three, '$');
		update_Obstacle(obstacle_four_four_four_four, 'U');
		update_Obstacle(obstacle_five_five_five_five, '$');
		update_Obstacle(obstacle_six_six_six_six, 'R');
		update_Obstacle(obstacle_seven_seven_seven_seven, '@');
		update_Obstacle(obstacle_eight_eight_eight_eight, 'U');
		

		// Print the board 
		//print_Board(board); // for text-based
		
		//init_map();
		//draw_map(); // for graphics
		init_map();
		
// Collision detection ---------------------------------------------------------------------------
	// NOTE: brute force af
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
		
		if (playerOne.locationX == obstacle_one_one.locationX && playerOne.locationY == obstacle_one_one.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_two_two.locationX && playerOne.locationY == obstacle_two_two.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_three_three.locationX && playerOne.locationY == obstacle_three_three.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_four_four.locationX && playerOne.locationY == obstacle_four_four.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_five_five.locationX && playerOne.locationY == obstacle_five_five.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_six_six.locationX && playerOne.locationY == obstacle_six_six.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_seven_seven.locationX && playerOne.locationY == obstacle_seven_seven.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_eight_eight.locationX && playerOne.locationY == obstacle_eight_eight.locationY) {
			running = false;
		}
		
		if (playerOne.locationX == obstacle_one_one_one.locationX && playerOne.locationY == obstacle_one_one_one.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_two_two_two.locationX && playerOne.locationY == obstacle_two_two_two.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_three_three_three.locationX && playerOne.locationY == obstacle_three_three_three.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_four_four_four.locationX && playerOne.locationY == obstacle_four_four_four.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_five_five_five.locationX && playerOne.locationY == obstacle_five_five_five.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_six_six_six.locationX && playerOne.locationY == obstacle_six_six_six.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_seven_seven_seven.locationX && playerOne.locationY == obstacle_seven_seven_seven.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_eight_eight_eight.locationX && playerOne.locationY == obstacle_eight_eight_eight.locationY) {
			running = false;
		}
		
		if (playerOne.locationX == obstacle_one_one_one_one.locationX && playerOne.locationY == obstacle_one_one_one_one.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_two_two_two_two.locationX && playerOne.locationY == obstacle_two_two_two_two.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_three_three_three_three.locationX && playerOne.locationY == obstacle_three_three_three_three.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_four_four_four_four.locationX && playerOne.locationY == obstacle_four_four_four_four.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_five_five_five_five.locationX && playerOne.locationY == obstacle_five_five_five_five.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_six_six_six_six.locationX && playerOne.locationY == obstacle_six_six_six_six.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_seven_seven_seven_seven.locationX && playerOne.locationY == obstacle_seven_seven_seven_seven.locationY) {
			running = false;
		} else if (playerOne.locationX == obstacle_eight_eight_eight_eight.locationX && playerOne.locationY == obstacle_eight_eight_eight_eight.locationY) {
			running = false;
		}
		if(running == false){
			drawGameOver();
			delay(1000);
			goto restart;
		}
		
		

// Final checks ----------------------------------------------------------------------------------
		
		// Check for water collision
		
				
		// Check for win
		if (playerOne.locationY == 0) {
			printf("You win!\n");
			drawGameWin();
			delay(1000);
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

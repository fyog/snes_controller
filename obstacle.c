#include <stdio.h>
#include <stdbool.h>

struct Obstacle {
	int locationX;
	int locationY;
};

// Global variables
extern char board[32][32];

struct Obstacle init_Obstacle (int positionX_par, int positionY_par) {
	struct Obstacle obstacle;
	obstacle.locationX = positionX_par;
	obstacle.locationY = positionY_par;
	board[positionY_par][positionX_par] = '@';
	return obstacle;
}

// Move method
void move(struct Obstacle obstacle, bool up, bool down, bool left, bool right) {
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
}

// Update method
void update_Obstacle(struct Obstacle obstacle) {
	board[obstacle.locationY][obstacle.locationX] = '@';
}


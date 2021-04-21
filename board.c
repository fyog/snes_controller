#include <stdio.h>

int BOARD_HEIGHT = 21;
int BOARD_WIDTH = 50;

// Initializes the board
//
// argument(s): pointer of type int, which points to the board
// returns: nothing
void init_Board(char* board) {
	for (int i = 0; i < BOARD_HEIGHT; i++) {
		for (int j = 0; j < BOARD_WIDTH; j++) {
			if (i == 0) {
				*((board + i * BOARD_WIDTH) + j) = '#';
			} else {
				*((board + i * BOARD_WIDTH) + j) = '-';
			}
		}
	}
}

// Prints the board
//
// argument(s): pointer of type int, which points to the board
// returns: nothing
void print_Board(char* board) {
	for (int i = 0; i < BOARD_HEIGHT; i++) {
		for (int j = 0; j < BOARD_WIDTH; j++) {
			printf("%c", *((board + i * BOARD_WIDTH) + j));
		}
		printf("\n");
	}
}

#include <stdio.h>

// Initializes the board
//
// argument(s): pointer of type int, which points to the board
// returns: nothing
void init_Board(int* board) {
	for (int i = 0; i < 32; i++) {
		for (int j=0; j < 32; j++) {
			if (i == 31 && j == 0) {
				*((board + i * 32) + j) = 9;
			} else {
				*((board + i * 32) + j) = 0;
			}
		}
	}
}

// Prints the board
//
// argument(s): pointer of type int, which points to the board
// returns: nothing
void print_Board(int* board) {
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 32; j++) {
			printf("%d", *((board + i * 32) + j));
		}
		printf("\n");
	}
}

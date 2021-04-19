#include <stdio.h>

// Initializes the board
//
// argument(s): pointer of type int, which points to the board
// returns: nothing
void init_Board(char* board) {
	for (int i = 0; i < 32; i++) {
		for (int j=0; j < 32; j++) {
			if (i == 0) {
				*((board + i * 32) + j) = '#';
			} else {
				*((board + i * 32) + j) = '-';
			}
		}
	}
}

// Prints the board
//
// argument(s): pointer of type int, which points to the board
// returns: nothing
void print_Board(char* board) {
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 32; j++) {
			printf("%c", *((board + i * 32) + j));
		}
		printf("\n");
	}
}

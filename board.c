#include <stdio.h>

// Print's the board
//
// argument(s): pointer of type intt, which points to the board
// returns: nothing
void print_Board(int* board) {
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 32; j++) {
			printf("%d", *((board + i * 32) + j));
		}
		printf("\n");
	}
}

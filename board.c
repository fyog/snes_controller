#include <stdio.h>

int* init_Board(int* board) {
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 32; j++) {
			*((board + i * 32) + j) = 0;
		} 
	}
}

//void print_Board(int* board) {
//	for (int i = 0; i < 32; i++) {
//		for (int j = 0; j < 32; j++) {
//			printf("%d", *((board + i * 32) + j));
//		}
//	}
//}

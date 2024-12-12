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
struct fbs {
	char *fptr;		// framebuffer pointer
	int xOff; 					// x offset
	int yOff;					// x offset
	int bits;					// bits per pixel
	int lineLength;				// Line Length
	float screenSize;			// Screen Size
};

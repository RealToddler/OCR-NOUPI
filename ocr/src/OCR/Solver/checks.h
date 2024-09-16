#ifndef CHECKS_H
#define CHECKS_H

#define MAX_COLS 16

typedef struct {
	int x;
	int y;
} tTuple;

typedef struct {
	tTuple t1;
	tTuple t2;
} cCoords;

tTuple find_first_letter(int x, int y, int rows, int cols, char grid[rows][cols] ,
		const char* word);

cCoords check_diags(int rows, int cols, char grid[rows][cols] , 
		const char* word, tTuple first_coords, int word_size);

cCoords check_horizontal(int rows, int cols, char grid[rows][cols] ,
		const char* word, tTuple first_coords, int word_size);

cCoords check_vertical(int rows, int cols, char grid[rows][cols] ,
		const char* word, tTuple first_coords, int word_size);

#endif

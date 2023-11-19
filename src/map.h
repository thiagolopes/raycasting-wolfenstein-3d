#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Grid Grid;
typedef struct Cel Cel;

struct Cel{
  int raw_value;
};

struct Grid {
  Cel *cels;
  size_t rows;
  size_t cols;
};

Grid grid_new(size_t rows, size_t cols);
Grid grid_load(char* path);

int grid_area(Grid grid);
bool grid_index_valid(int index);

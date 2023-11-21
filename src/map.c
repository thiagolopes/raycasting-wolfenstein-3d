#include "map.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 255
#define FILE_READ "r"
#define SPACE 32
#define NEW_LINE 10

Grid grid_new(size_t rows, size_t cols) {
    Cel** c = (Cel**)calloc(rows, sizeof(Cel*));
    for (int i = 0; i < rows; i++) {
        c[i] = (Cel*)calloc(cols, sizeof(Cel));
    }
    Grid g = {c, rows, cols};
    return g;
}

void grid_update_size(Grid* grid, size_t rows, size_t cols) {
    return;
}

void grid_push(Grid grid, size_t row, size_t col, Cel cel) {
    if (grid_index_valid(grid, row, col)) {
        grid.cels[row][col] = cel;
    } else {
        printf("[ERROR] Grid push invalid: %lo %lo\n", row, col);
        exit(EXIT_FAILURE);
    }
}

bool grid_index_valid(Grid grid, int row, int col) {
    if ((row < 0) || (col < 0)) {
        return false;
    }
    if ((row >= grid.rows) || (col >= grid.cols)) {
        return false;
    }
    return true;
}

int grid_area(Grid grid) {
    return grid.cols * grid.rows;
}

Grid grid_load(char* path) {
    FILE* f;
    char* line = {0};

    f = fopen(path, FILE_READ);
    if (!f) {
        printf("[ERROR] Cound not open the file map: %s", path);
    }

    char   c    = 0;
    size_t rows = 0, cols = 0;
    Grid   grid = grid_new(BUFFER_SIZE, BUFFER_SIZE);
    do {
        c = (char)fgetc(f);
        if ((c != SPACE) && (c != NEW_LINE)) {
            grid_push(grid, rows, cols, (Cel){atoi(&c)});
            cols++;
            printf("%c ", c);
        }
        if (c == NEW_LINE) {
            cols = 0;
            rows++;
            printf("\n");
        }
    } while (c != EOF);

    fclose(f);
    return grid;
}

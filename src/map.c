#include "map.h"
#define BUFFER_SIZE 255
#define FILE_READ "r"

Grid grid_new(size_t rows, size_t cols) {
};

Grid grid_load(char* path) {
    FILE* f;
    char* line = {0};

    f = fopen(path, FILE_READ);
    if(!f) {
        printf("[ERROR] Cound not open the file map: %s", path);
    }

    char file_c = 0;
    size_t rows, cols;
    do {
        file_c = fgetc(f);
        printf("%c", file_c);
    }
    while(file_c != EOF);

    fclose(f);
    Grid g = {0};
    return g;
};

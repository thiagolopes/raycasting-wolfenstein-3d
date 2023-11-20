#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include <stdlib.h>
#include <SDL2/SDL_opengl.h>

typedef struct Texture Texture;
struct Texture {
    unsigned int id;
    bool repeat;
    char* name;
    int width;
    int height;
    int bpp;
    unsigned char *data;
};

typedef struct TextureContainer TextureContainer;
struct TextureContainer {
    unsigned int len;
    unsigned int capacity;
    Texture* textures;
};

Texture texture_new(char* path, bool repeat);
void texture_bind(Texture t);
void texture_unbind();
void texture_free(Texture* t);

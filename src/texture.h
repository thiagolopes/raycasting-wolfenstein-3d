#pragma once

typedef struct Texture {
  unsigned int id;
  char* name;
} Texture;

typedef struct TextureContainer {
  unsigned int len;
  unsigned int capacity;
  Texture* textures;
} TextureContainer;


void texture_bind(Texture t);
void texture_unbind(Texture t);

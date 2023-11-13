#pragma once

typedef struct Texture Texture;
struct Texture {
  unsigned int id;
  char* name;
};

typedef struct TextureContainer TextureContainer;
struct TextureContainer {
  unsigned int len;
  unsigned int capacity;
  Texture* textures;
};

void texture_bind(Texture t);
void texture_unbind(Texture t);

#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture texture_new(char* path, bool repeat) {
    Texture t = {0};
    stbi_set_flip_vertically_on_load(1);

    t.data = stbi_load(path, &t.width, &t.height, &t.bpp, STBI_rgb_alpha);
    if (!t.data) {
        printf("[ERROR] Do not able to load file: %s", path);
        exit(EXIT_FAILURE);
    }

    /* init texture from file pixels to GPU MEMORY*/
    glGenTextures(1, &t.id);
    glBindTexture(GL_TEXTURE_2D, t.id);

    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t.width, t.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, t.data);
    glBindTexture(GL_TEXTURE_2D, t.id);
    return t;
};

void texture_bind(Texture t){
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, t.id);
}

void texture_unbind(){
  glDisable(GL_TEXTURE_2D);
}

void texture_free(Texture* t){
  stbi_image_free(t->data);
}

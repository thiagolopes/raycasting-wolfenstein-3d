#include <GL/gl.h>

static void draw_vertical_view(int texture, double texture_of, int pixel_start, int pixel_end, int vertical_pixel) {
    glColor3f(1, 1, 1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_LINES);

    glTexCoord2d(texture_of, 1.0);
    glVertex2i(vertical_pixel, pixel_start);

    glTexCoord2d(texture_of, 0.0);
    glVertex2i(vertical_pixel, pixel_end);

    glEnd();
    glDisable(GL_TEXTURE_2D);
}

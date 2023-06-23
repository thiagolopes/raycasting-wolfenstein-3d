#include <GL/gl.h>

void draw_rect(Rectangle rect, Color_t color) {
    glBegin(GL_QUADS);
    glColor4ub(color.r, color.g, color.b, color.a);
    glVertex2i(rect.x, rect.y);
    glVertex2i(rect.x + rect.width, rect.y);
    glVertex2i(rect.x + rect.width, rect.y + rect.height);
    glVertex2i(rect.x, rect.y + rect.height);
    glEnd();
}
void draw_vertical_line(float posX, float posY, float posY_end, int texture_id, float texture_offset, Color_t color) {
    glColor4ub(color.r, color.g, color.b, color.a);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glBegin(GL_LINES);

    glTexCoord2d(texture_offset, 1.0);
    glVertex2i(posX, posY);

    glTexCoord2d(texture_offset, 0.0);
    glVertex2i(posX, posY_end);

    glEnd();

    glDisable(GL_TEXTURE_2D);
}

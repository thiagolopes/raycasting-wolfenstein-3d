#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void ray_horizontal(int plx, int ply, int rtr[]){
  /* 60 degree */
  float angle = 1.0471975511965976;
  int degree_angle = 60;

  int px=0, py=0, dx, dy, n=64;

  if (degree_angle < 180){
    /* up */
    py = ((int)(ply/n) * n - 1);
    dy = -64;
  }
  if (degree_angle > 180){
    exit(0);
    /* down */
    py = ((int)(ply/n) * n + n);
    dy = 64;
  }
  px = (int) plx + (int)((ply - py) / tan(angle));
  dx = (int) (n / tan(angle));

  rtr[0] = px;
  rtr[1] = py;
  rtr[2] = dx;
  rtr[3] = dy;
  rtr[4] = (int)px/n;
  rtr[5] = (int)py/n;
}

int main(){
  int i=6;
  int rt[i];
  ray_horizontal(96,224,rt);
  printf("%i %i %i %i %i %i\n", rt[0], rt[1], rt[2], rt[3], rt[4], rt[5]);

  int test_rt[] = {115, 191, 36, -64, 1, 2};
  for (int x=0; x<i; x++){
    assert(rt[x] == test_rt[x] && "Passed");
  }
}

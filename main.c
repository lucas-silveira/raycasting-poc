#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#define PI 3.1415926535
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533 // 1 degree in radians

int mapWidth = 8, mapHeight = 8, tile = 64;
int map[] = {
  1,1,1,1,1,1,1,1,
  1,0,1,0,0,0,0,1,
  1,0,1,0,0,0,0,1,
  1,0,1,0,0,0,0,1,
  1,0,0,0,0,0,0,1,
  1,0,0,0,0,1,0,1,
  1,0,0,0,0,0,0,1,
  1,1,1,1,1,1,1,1,
};

void drawMap2d() {
  int x, y, x2, y2;
  for (y=0; y<mapHeight; y++) {
    for (x=0; x<mapWidth; x++) {
      if (map[y*mapWidth+x]==1) {
        glColor3f(1, 1, 1);
      } else {
        glColor3f(0, 0, 0);
      }

      x2=x*tile; y2=y*tile;
      glBegin(GL_QUADS);
      glVertex2i(x2+1, y2+1);
      glVertex2i(x2+1, y2+tile-1);
      glVertex2i(x2+tile-1, y2+tile-1);
      glVertex2i(x2+tile-1, y2+1);
      glEnd();
    }
  }
}

float px, py, pdx, pdy, pa; // player position

void drawPlayer() {
  glColor3f(1, 1, 0);
  glPointSize(8);
  glBegin(GL_POINTS);
  glVertex2i(px, py);
  glEnd();

  glLineWidth(3);
  glBegin(GL_LINES);
  glVertex2i(px, py);
  glVertex2i(px+pdx*5, py+pdy*5);
  glEnd();
}

float dist(float ax, float ay, float bx, float by, float ang) {
  return (sqrt((bx-ax)*(bx-ax) + (by-ay)*(by-ay)));
}

void drawRays3d() {
  int r, mx, my, mp, depth;
  float rx, ry, ra, xo, yo, distT;
  ra = pa-DR*30;

  if (ra < 0) {
    ra += 2*PI;
  }
  if (ra > 2*PI) {
    ra -= 2*PI;
  }

  for (r = 0; r < 60; r++) {
    // Check horizontal lines
    depth = 0;
    float distH=1000000, hx = px, hy = py;
    float aTan=-1/tan(ra);

    // Looking up
    if (ra>PI) {
      ry = (((int)py>>6)<<6)-0.0001;
      rx = (py-ry)*aTan+px;
      yo = -tile;
      xo = -yo*aTan;
    }
    // Looking down
    if (ra<PI) {
      ry = (((int)py>>6)<<6)+tile;
      rx = (py-ry)*aTan+px;
      yo = tile;
      xo = -yo*aTan;
    }
    // Looking straight left or right
    if (ra == 0 || ra == PI) {
      rx = px;
      ry = py;
      depth = 8;
    }
    while (depth < 8 ) {
      mx = (int)rx>>6;
      my = (int)ry>>6;
      mp = my*mapWidth+mx;

      if (mp > 0 && mp < mapWidth*mapHeight && map[mp] > 0) {
        hx = rx;
        hy = ry;
        distH = dist(px, py, hx, hy, ra);
        depth = 8;
      } else {
        rx += xo;
        ry += yo;
        depth++;
      }
    }

    // Check vertical lines
    depth = 0;
    float distV=1000000, vx = px, vy = py;
    float nTan=-tan(ra);
    // Looking left
    if (ra>P2 && ra<P3) { 
      rx = (((int)px>>6)<<6)-0.0001;
      ry = (px-rx)*nTan+py;
      xo = -tile;
      yo = -xo*nTan;
    }
    // Looking right
    if (ra<P2 || ra>P3) {
      rx = (((int)px>>6)<<6)+tile;
      ry = (px-rx)*nTan+py;
      xo = tile;
      yo = -xo*nTan;
    }
    // Looking straight up or down
    if (ra == 0 || ra == PI) {
      rx = px;
      ry = py;
      depth = 8;
    }
    while (depth < 8 ) {
      mx = (int)rx>>6;
      my = (int)ry>>6;
      mp = my*mapWidth+mx;

      if (mp > 0 && mp < mapWidth*mapHeight && map[mp] > 0) {
        vx = rx;
        vy = ry;
        distV = dist(px, py, vx, vy, ra);
        depth = 8;
      }
      else {
        rx += xo;
        ry += yo;
        depth++;
      }
    }

    // Vertical wall hit
    if (distV<distH) {
      rx = vx;
      ry = vy;
      distT = distV;
      glColor3f(0.9, 0, 0);
    }
    // Horizontal wall hit
    if (distH<distV) {
      rx = hx;
      ry = hy;
      distT = distH;
      glColor3f(0.7, 0, 0);
    }

    glLineWidth(1);
    glBegin(GL_LINES);
    glVertex2i(px, py);
    glVertex2i(rx, ry);
    glEnd();

    // Draw 3D Walls
    float ca = pa-ra;
    if (ca < 0) {
      ca += 2*PI;
    }
    if (ca > 2*PI) {
      ca -= 2*PI;
    }
    distT *= cos(ca);

    float lineH = (tile*320)/distT;
    if (lineH>320) lineH = 320;
    float lineO = 160-lineH/2;
    glLineWidth(8);
    glBegin(GL_LINES);
    glVertex2i(r*8+530, lineO);
    glVertex2i(r*8+530, lineH+lineO);
    glEnd();

    ra += DR;
    if (ra < 0) {
      ra += 2*PI;
    }
    if (ra > 2*PI) {
      ra -= 2*PI;
    }
  }
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawMap2d();
  drawPlayer();
  drawRays3d();
  glutSwapBuffers();
}

void buttons(unsigned char key, int x, int y) {
  if (key=='a') {
    pa -= 0.1;
    if (pa < 0) pa += 2*PI;

    pdx = cos(pa)*5;
    pdy = sin(pa)*5;
  }
  if (key=='d') {
    pa += 0.1;
    if (pa > 2*PI) pa -= 2*PI;

    pdx = cos(pa)*5;
    pdy = sin(pa)*5;
  }
  if (key=='w') {
    px += pdx;
    py += pdy;
  }
  if (key=='s') {
    px -= pdx;
    py -= pdy;
  }
  glutPostRedisplay();
}

void init() {
  glClearColor(0.3,0.3,0.3,0.3);
  gluOrtho2D(0, 1024, 512, 0);
  px=300; py=300;
  pdx = cos(pa)*5;
  pdy = sin(pa)*5;
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(1024, 512);
  glutCreateWindow("Raycasting");
  init();
  glutDisplayFunc(display);
  glutKeyboardFunc(buttons);

  glutMainLoop();
}
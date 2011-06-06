#ifndef RG_GEOMETRY
#define RG_GEOMETRY

/* Standard libraries */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* OpenGL libraries */
#ifdef WIN32
  #include <windows.h>
  #include <gl/gl.h>
  #include <gl/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif

#define TWOPI           6.283185307179586476925287
#define PI              3.141592653589793238462643
#define PID2            1.570796326794896619231322

typedef struct {
   double x,y,z;
} XYZ;

extern void MakeSphere (void);
extern void MakeBox (void);
extern void colorCube (void);
extern void polygon (int a, int b, int c, int d);

#endif

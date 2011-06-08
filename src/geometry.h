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

#define DTOR            0.0174532925
#define RTOD            57.2957795
#define TWOPI           6.283185307179586476925287
#define PI              3.141592653589793238462643
#define PID2            1.570796326794896619231322

/* Model types */
enum MODEL_TYPE
{
	MESH = 1,
	SPHERE,
	BOX,
	BOXCOLOR,
	PULSAR,
	KNOT,
	TRITORUS,
	LORENZ,
	NONE
};

typedef struct {
   double x,y,z;
} XYZ;

void MakeModel(MODEL_TYPE modeltype);

void MakeMesh(void);
extern void MakeSphere (void);
extern void MakeBox (void);
void MakePulsar(void);
void MakeKnot(void);
void MakeTriTorus(void);
void MakeLorenz(void);
XYZ  TriTorusEval(double,double);

extern void polygon (int a, int b, int c, int d);
extern void colorCube (void);

/* Util */
void Normalise(XYZ *);
XYZ  CalcNormal(XYZ,XYZ,XYZ);

#endif

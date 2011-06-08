#include "geometry.h"

GLUquadricObj *quadratic; /* Storage For Our Quadratic Objects ( NEW ) */
XYZ origin = {0.0,0.0,0.0};
// float rotatespeed = 0.5;     /* Each object can autorotate      */


void MakeModel(MODEL_TYPE modeltype)
{
  switch(modeltype)
  {
    case MESH:
      MakeMesh();
      break;
    case BOX:
      MakeBox();
      break;
    case BOXCOLOR:
      colorCube();
      break;
    case SPHERE:
      MakeSphere();
      break;
    case PULSAR:
      MakePulsar();
      break;
    case KNOT:
      MakeKnot();
      break;
    case TRITORUS:
      MakeTriTorus();
      break;
    case LORENZ:
      MakeLorenz();
      break;
    default:
      MakeBox();
      break;
  }
}

void polygon(int a, int b, int c, int d)
{
  double vertices[][3]={{-1,-1, 1}, {-1, 1, 1}, { 1, 1, 1}, { 1,-1, 1},
                        {-1,-1,-1}, {-1, 1,-1}, { 1, 1,-1}, { 1,-1,-1}};
  glBegin(GL_POLYGON);
    glVertex3dv(vertices[a]);
    glVertex3dv(vertices[b]);
    glVertex3dv(vertices[c]);
    glVertex3dv(vertices[d]);
  glEnd();
}

void colorCube(void)
{
  glColor3f(1,0,0);
  glNormal3f(1,0,0);
  polygon(2,3,7,6);
           
  glColor3f(0,1,0);
  glNormal3f(0,1,0);
  polygon(1,2,6,5);
                    
  glColor3f(0,0,1);
  glNormal3f(0,0,1);
  polygon(0,3,2,1);
  
  glColor3f(1,0,1);
  glNormal3f(0,-1,0);
  polygon(3,0,4,7);
                                      
  glColor3f(1,1,0);
  glNormal3f(0,0,-1);
  polygon(4,5,6,7);

  glColor3f(0,1,1);
  glNormal3f(-1,0,0);
  polygon(5,4,0,1);
}

/* 
 * Create the geometry for a wireframe box
 */
void MakeBox(void)
{
  XYZ pmin = {-2,-2,-2}, pmax = {2, 2, 2};

  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_LINE_STRIP);
  {
    glVertex3f(pmin.x, pmin.y,pmin.z);
    glVertex3f(pmax.x, pmin.y,pmin.z);
    glVertex3f(pmax.x, pmin.y,pmax.z);
    glVertex3f(pmin.x, pmin.y,pmax.z);
    glVertex3f(pmin.x, pmin.y,pmin.z);
    glVertex3f(pmin.x, pmax.y,pmin.z);
    glVertex3f(pmax.x, pmax.y,pmin.z);
    glVertex3f(pmax.x, pmax.y,pmax.z);
    glVertex3f(pmin.x, pmax.y,pmax.z);
    glVertex3f(pmin.x, pmax.y,pmin.z);
  }
  glEnd();
  
  glBegin(GL_LINES);
  glVertex3f(pmax.x,pmin.y,pmin.z); glVertex3f(pmax.x,pmax.y,pmin.z);
  glEnd();
  glBegin(GL_LINES);
  glVertex3f(pmax.x,pmin.y,pmax.z); glVertex3f(pmax.x,pmax.y,pmax.z);
  glEnd();
  glBegin(GL_LINES);
  glVertex3f(pmin.x,pmin.y,pmax.z); glVertex3f(pmin.x,pmax.y,pmax.z);
  glEnd();
}

/*
 * Create the geometry for a sphere
 */
void MakeSphere(void)
{
   int i,j,n=32;
   double t1,t2,t3,r=3;
   XYZ e,p,c={0,0,0};

   glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
   glColor3f(1.0,1.0,1.0);
   for (j=0;j<n/2;j++) {
      t1 = -PID2 + j * PI / (n/2);
      t2 = -PID2 + (j + 1) * PI / (n/2);

      glBegin(GL_QUAD_STRIP);
      for (i=0;i<=n;i++) {
         t3 = i * TWOPI / n;

         e.x = cos(t1) * cos(t3);
         e.y = sin(t1);
         e.z = cos(t1) * sin(t3);
         p.x = c.x + r * e.x;
         p.y = c.y + r * e.y;
         p.z = c.z + r * e.z;
         glNormal3f(e.x,e.y,e.z);
         glTexCoord2f(i/(double)n,2*j/(double)n);
         glVertex3f(p.x,p.y,p.z);

         e.x = cos(t2) * cos(t3);
         e.y = sin(t2);
         e.z = cos(t2) * sin(t3);
         p.x = c.x + r * e.x;
         p.y = c.y + r * e.y;
         p.z = c.z + r * e.z;

         glNormal3f(e.x,e.y,e.z);
         glTexCoord2f(i/(double)n,2*(j+1)/(double)n);
         glVertex3f(p.x,p.y,p.z);

      }
      glEnd();
   }
}

/*
   Create the geometry for the knot
*/
void MakeKnot(void)
{
   int i,n=200;
   double x,y,z;
   double mu;

   glLineWidth(2.0);
   glColor3f(1.0,1.0,1.0);

   glBegin(GL_LINE_STRIP);
   for (i=0;i<=n;i++) {
      mu = i * TWOPI / (double)n;
      x = 10 * (cos(mu) + cos(3*mu)) + cos(2*mu) + cos(4*mu);
      y = 6 * sin(mu) + 10 * sin(3*mu);
      z = 4 * sin(3*mu) * sin(5*mu/2) + 4*sin(4*mu) - 2 * sin(6*mu);
      glVertex3f(x/4,y/4,z/4);
   }
   glEnd();

   glLineWidth(1.0);
}

/*
   Create the geometry for the lorenz attractor
*/
void MakeLorenz(void)
{
   int i,n=10000;
   double x0=0.1,y0=0,z0=0,x1,y1,z1;
   double h = 0.005;
   double a = 10.0;
   double b = 28.0;
   double c = 8.0 / 3.0;

   glColor3f(1.0,1.0,1.0);
   glBegin(GL_POINTS);
   for (i=0;i<=n;i++) {
      x1 = x0 + h * a * (y0 - x0);
      y1 = y0 + h * (x0 * (b - z0) - y0);
      z1 = z0 + h * (x0 * y0 - c * z0);
      x0 = x1;
      y0 = y1;
      z0 = z1;
      if (i > 100)
         glVertex3f((x0-0.95)/5,(y0-1.78)/5,(z0-26.7)/5);
   }
   glEnd();
}

/*
   Create the geometry for a tritorus
*/
void MakeTriTorus(void)
{
   int m = 51;
   int i,j,k;
   double u,v,u1,v1,delta=0.001;
   XYZ p[4],n[4],p1,p2;

   glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
   glColor3f(1.0,1.0,1.0);

   glBegin(GL_QUADS);
   for (i=0;i<m;i++) {
      for (j=0;j<m;j++) {

         u  = -PI + i * TWOPI / m;
         u1 = -PI + (i+1) * TWOPI / m;
         v  = -PI + j * TWOPI / m;
         v1 = -PI + (j+1) * TWOPI / m;

         p[0] = TriTorusEval(u,v);
         p1 = TriTorusEval(u+delta,v);
         p2 = TriTorusEval(u,v+delta);
         n[0] = CalcNormal(p[0],p1,p2);

         p[1] = TriTorusEval(u1,v);
         p1 = TriTorusEval(u1+delta,v);
         p2 = TriTorusEval(u1,v+delta);
         n[1] = CalcNormal(p[1],p1,p2);

         p[2] = TriTorusEval(u1,v1);
         p1 = TriTorusEval(u1+delta,v1);
         p2 = TriTorusEval(u1,v1+delta);
         n[2] = CalcNormal(p[2],p1,p2);

         p[3] = TriTorusEval(u,v1);
         p1 = TriTorusEval(u+delta,v1);
         p2 = TriTorusEval(u,v1+delta);
         n[3] = CalcNormal(p[3],p1,p2);

         for (k=0;k<4;k++) {
            glNormal3f(n[k].x,n[k].y,n[k].z);
            glVertex3f(2.5*p[k].x,2.5*p[k].y,2.5*p[k].z);
         }
      }
   }
   glEnd();
}
XYZ TriTorusEval(double u,double v)
{
   XYZ p;

   p.x = sin(u) * (1 + cos(v));
   p.y = sin(u + 2 * PI / 3) * (1 + cos(v + 2 * PI / 3));
   p.z = sin(u + 4 * PI / 3) * (1 + cos(v + 4 * PI / 3));

   return(p);
}

/*
   Create the geometry for the mesh
*/
void MakeMesh(void)
{
   int i,j,n=1,w=4;
   static double rotateangle = 0.0;

   glColor3f(1.0,1.0,1.0);
   glBegin(GL_LINES);
   for (i=-w;i<=w;i+=n) {
      for (j=-w;j<w;j+=n) {
         glVertex3f(0.0,(double)i,(double)j);
         glVertex3f(0.0,(double)i,(double)j+n);
      }
   }
   for (j=-w;j<=w;j+=n) {
      for (i=-w;i<w;i+=n) {
         glVertex3f(0.0,(double)i,(double)j);
         glVertex3f(0.0,(double)i+n,(double)j);
      }
   }
   glEnd();
}


/*
   Create the geometry for the pulsar
*/
void MakePulsar(void)
{
   int i,j,k;
   double cradius = 1;            /* Final radius of the cone */
   double clength = 5;            /* Cone length */
   double sradius = 1.3;         /* Final radius of sphere */
   double r1=1.4,r2=1.8;         /* Min and Max radius of field lines */
   double x,y,z;
   XYZ p[4],n[4];
   static double rotateangle = 0.0;

   glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

   /* Top level rotation  - spin */
   glPushMatrix();
   glRotatef(rotateangle,0.0,1.0,0.0);

   /* Rotation about spin axis */
   glPushMatrix();
   glRotatef(45.0,0.0,0.0,1.0);

   quadratic = gluNewQuadric();			// Create A Pointer To The Quadric Object ( NEW )
   gluQuadricNormals(quadratic, GLU_SMOOTH);	// Create Smooth Normals ( NEW )
   gluQuadricTexture(quadratic, GL_TRUE);		// Create Texture Coords ( NEW )
   /* Light in center */
   gluSphere(quadratic, 0.9, 16, 8);

   /* Spherical center */
   for (i=0;i<360;i+=5) {
      for (j=-80;j<80;j+=5) {

         p[0].x = sradius * cos(j*DTOR) * cos(i*DTOR);
         p[0].y = sradius * sin(j*DTOR);
         p[0].z = sradius * cos(j*DTOR) * sin(i*DTOR);
         n[0] = p[0];

         p[1].x = sradius * cos((j+5)*DTOR) * cos(i*DTOR);
         p[1].y = sradius * sin((j+5)*DTOR);
         p[1].z = sradius * cos((j+5)*DTOR) * sin(i*DTOR);
         n[1] = p[1];

         p[2].x = sradius * cos((j+5)*DTOR) * cos((i+5)*DTOR);
         p[2].y = sradius * sin((j+5)*DTOR);
         p[2].z = sradius * cos((j+5)*DTOR) * sin((i+5)*DTOR);
         n[2] = p[2];

         p[3].x = sradius * cos(j*DTOR) * cos((i+5)*DTOR);
         p[3].y = sradius * sin(j*DTOR);
         p[3].z = sradius * cos(j*DTOR) * sin((i+5)*DTOR);
         n[3] = p[3];

         glBegin(GL_POLYGON);
         if (i % 20 == 0)
            glColor3f(1.0,1.0,1.0);
         else
            glColor3f(0.5,0.5,0.5);
         for (k=0;k<4;k++) {
            glNormal3f(n[k].x,n[k].y,n[k].z);
            glVertex3f(p[k].x,p[k].y,p[k].z);
         }
         glEnd();
      }
   }

   /* Draw the cones */
   for (j=-1;j<=1;j+=2) {
      for (i=0;i<360;i+=10) {
         
         p[0]   = origin;
         n[0]   = p[0];
         n[0].y = -1;

         p[1].x = cradius * cos(i*DTOR);
         p[1].y = j*clength;
         p[1].z = cradius * sin(i*DTOR);
         n[1]   = p[1];
         n[1].y = 0;

         p[2].x = cradius * cos((i+10)*DTOR);
         p[2].y = j*clength;
         p[2].z = cradius * sin((i+10)*DTOR);
         n[2]   = p[2];
         n[2].y = 0;

         glBegin(GL_POLYGON);
         if (i % 30 == 0)
            glColor3f(0.2,0.2,0.2);
         else
            glColor3f(0.5,0.5,0.5);
         for (k=0;k<3;k++) {
            glNormal3f(n[k].x,n[k].y,n[k].z);
            glVertex3f(p[k].x,p[k].y,p[k].z);
         }
         glEnd();
      }
   }

   /* Draw the field lines */
   for (j=0;j<360;j+=20) {
      glPushMatrix();
      glRotatef((double)j,0.0,1.0,0.0);
      glBegin(GL_LINE_STRIP);
      glColor3f(0.7,0.7,0.7);
      for (i=-140;i<140;i++) {
         x = r1 + r1 * cos(i*DTOR);
         y = r2 * sin(i*DTOR);
         z = 0;
         glVertex3f(x,y,z);   
      }   
      glEnd();
      glPopMatrix();      
   }

   glPopMatrix(); /* Pulsar axis rotation */
   glPopMatrix(); /* Pulsar spin */
//   rotateangle += rotatespeed;
}

/*
   Normalise a vector
*/
void Normalise(XYZ *p)
{
   double length;

   length = sqrt(p->x * p->x + p->y * p->y + p->z * p->z);
   if (length != 0) {
      p->x /= length;
      p->y /= length;
      p->z /= length;
   } else {
      p->x = 0;
      p->y = 0;
      p->z = 0;
   }
}

/*
   Calculate the unit normal at p given two other points
   p1,p2 on the surface. The normal points in the direction
   of p1 crossproduct p2
*/
XYZ CalcNormal(XYZ p,XYZ p1,XYZ p2)
{
   XYZ n,pa,pb;

   pa.x = p1.x - p.x;
   pa.y = p1.y - p.y;
   pa.z = p1.z - p.z;
   pb.x = p2.x - p.x;
   pb.y = p2.y - p.y;
   pb.z = p2.z - p.z;
   Normalise(&pa);
   Normalise(&pb);

   n.x = pa.y * pb.z - pa.z * pb.y;
   n.y = pa.z * pb.x - pa.x * pb.z;
   n.z = pa.x * pb.y - pa.y * pb.x;
   Normalise(&n);

   return(n);
}



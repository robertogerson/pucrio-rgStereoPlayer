#include "geometry.h"

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


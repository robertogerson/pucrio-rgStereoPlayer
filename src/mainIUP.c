/**
 * @file mainIUP.c
 *
 * @author Roberto Azevedo <rg.albuquerque@gmail.com>
 *
 * @date
 *    Criado em: 05 de Junho de 2011.
 */

/* Standard Libraries */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/* IUP (GUI) Library */
#include <iup.h>            /* IUP functions */
#include <iupgl.h>          /* IUP Canvas and OpengGL */
#include <iupcontrols.h>    /* buttons controls */

/* OpenGL Library */
#ifdef WIN32
  #include <windows.h>
  #include <gl/gl.h>
  #include <gl/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif /* WIN32 */

#include "anaglyph.h"

Ihandle *mainDlg;           /* Main Window */
Ihandle *controlsFrm;          /* Controls over canvas */
Ihandle *canvas;            /* OpengGL canvas */

anaglyph_handle *anaglyph;

typedef struct {
   double x,y,z;
} XYZ;
#define TWOPI           6.283185307179586476925287
#define PI              3.141592653589793238462643
#define PID2            1.570796326794896619231322

int width = 800, height = 600;

int exit_cb(void)
{
  IupDestroy(canvas);
  IupDestroy(controlsFrm);
  IupDestroy(mainDlg);

  return IUP_CLOSE;
}

static void polygon(int a, int b, int c, int d)
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

static void colorCube(void)
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

void drawScene (void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
/*  glPushMatrix();  saves current model view in a stack */
//    glTranslatef( 0.0f, 0.0f , 0.0f );
//    glScalef( 1.0f, 1.0f, 1.0f );
/*    glRotatef(t,0,0,1); */
/*    colorCube(); */
      MakeBox();
      MakeSphere();
/*  glPopMatrix(); */
 
}

int iod_valuechanged_cb(Ihandle *ih)
{
  anaglyph->IOD =  IupGetFloat(ih, IUP_VALUE);
}

int repaint_cb(Ihandle *self)
{
  IupGLMakeCurrent(self); /* Make self the current GL Context */
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  /* White */
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  anaglyph_draw(anaglyph);
  IupGLSwapBuffers(self);  /* change the back buffer with the front buffer */

  return IUP_DEFAULT; /* returns the control to the main loop */
}

int idle_cd(void)
{
//  t += 1;
  repaint_cb(canvas);
  return IUP_DEFAULT; /* return to the IUP main loop */
}

/* 
 * Function called in the event of changes in the width or in the height of 
 * the canvas.
 */
int resize_cb(Ihandle *self, int new_width, int new_height)
{
  IupGLMakeCurrent(self);  /* Make the canvas current in OpenGL */

  /* define the entire canvas as the viewport  */
  glViewport(0, 0, new_width, new_height);

  /* transformation applied to each vertex */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();           /* identity, i. e. no transformation */

  /* projection transformation (orthographic in the xy plane) */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  /* update canvas size and repaint */
  width = new_width;
  height = new_height;
  repaint_cb(canvas);

  return IUP_DEFAULT; /* return to the IUP main loop */
}

Ihandle *createFrmControls()
{
  Ihandle *frm, *vbox, *anag,
          *renderTypeFrm,
          *topbottom, *sidebyside, *renderType,
          *iod_v;

  anag = IupToggle ("&Anaglyph", ""),
  topbottom = IupToggle ("&Top/Bottom", ""),
  sidebyside = IupToggle ("&Side-by-side", ""),

  renderType = IupRadio (
    IupVbox (
      anag,
      topbottom,
      sidebyside,
      NULL
    )
  );

  IupSetHandle("anaglyph", anag);
  IupSetHandle("topbottom", topbottom);
  IupSetHandle("sidebyside", sidebyside);

  IupSetAttribute(renderType, "VALUE", "anaglyph");
  IupSetAttribute(anag, "TIP",   "Two state button - Exclusive - RADIO");
  IupSetAttribute(topbottom, "TIP",   "Two state button - Exclusive - RADIO");
  IupSetAttribute(sidebyside, "TIP",   "Two state button - Exclusive - RADIO");

  renderTypeFrm = IupFrame( renderType );
  IupSetAttribute (renderTypeFrm, "TITLE", "RenderType");

  
  iod_v = IupVal("Horizontal");
  IupSetCallback(iod_v, "VALUECHANGED_CB",  (Icallback)iod_valuechanged_cb);
  IupSetAttribute(iod_v, "MIN", "-3.0");
  IupSetAttribute(iod_v, "MAX", "5.0");

  vbox = IupVbox
  (
    IupFill(),
    renderTypeFrm,
    IupFill(),
    iod_v,
    IupButton ("A&bout", ""),
    NULL
  );
  
  frm = IupDialog (vbox);
  IupSetAttributes(frm, "SIZE=100, TITLE=RGStereoPlayer Controls, \
                        RESIZE=NO, MINBOX=NO, MAXBOX=NO");
  return frm;
}

/* Initialize GUI elements */
int init(void)
{
  Ihandle *hbox;

  anaglyph = anaglyph_new_handle (width, height, drawScene);

  canvas = IupGLCanvas("repaint_cb"); /* Create Main Canvas */
  IupSetFunction("repaint_cb", (Icallback) repaint_cb);
  /* define the canvas' size in pixels */
  IupSetAttribute (canvas, IUP_RASTERSIZE, "800x600");
  
  /* define that this OpenGL _canvas has double buffer (front and back) */
  IupSetAttribute(canvas,IUP_BUFFER,IUP_DOUBLE);

  /* bind callback actions with callback functions */
  IupSetCallback(canvas, "RESIZE_CB",(Icallback) resize_cb);

  controlsFrm = createFrmControls();
/*  IupSetAttribute (controlsFrm, "OPACITY", "140"); */
  IupShowXY(controlsFrm, 0, 0); /* show mainDlg */

  hbox = IupHbox (
//    controlsFrm,
    canvas,
    NULL
  );

  /* Create the Main Window */
  mainDlg = IupDialog(hbox);
  IupSetAttribute (mainDlg, "TITLE", "RGStereoPlayer"); 

  IupSetCallback (mainDlg, "CLOSE_CB", (Icallback) exit_cb);
  IupSetFunction (IUP_IDLE_ACTION, (Icallback) idle_cd); 
  
//  IupShowXY(mainDlg, IUP_CENTER, IUP_CENTER); /* show mainDlg */
  IupShow(mainDlg); /* show mainDlg */
  
  return 1;
}

/* Main function */
int main(int argc, char *argv[])
{
  IupOpen(&argc, &argv); /* Initalize IUP */
  IupGLCanvasOpen();

  if( init() )
    IupMainLoop();

  IupClose();
}

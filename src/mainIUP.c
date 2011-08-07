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
#include "geometry.h"

Ihandle *mainDlg;           /* Main Window */
Ihandle *controlsFrm;          /* Controls over canvas */
Ihandle *canvas;            /* OpengGL canvas */

anaglyph_handle *anaglyph;

int width = 810, height = 610;
int speed = 1.0;
GLdouble angle = 0.0;

MODEL_TYPE modeltype;

int exit_cb(void)
{
  IupDestroy(canvas);
  IupDestroy(controlsFrm);
  IupDestroy(mainDlg);

  return IUP_CLOSE;
}

void drawScene (void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPushMatrix();  /* saves current model view in a stack */
   // glTranslatef(0.0, 0.0, -3.0);
    glScalef(0.7, 0.7, 0.7);
    glRotatef(angle, 0, 1, 0);
    MakeModel(modeltype);
  glPopMatrix();
 
}

/* Callback called when the user wants to change the Interocular Distance */
int iod_valuechanged_cb(Ihandle *ih)
{
  anaglyph->IOD =  IupGetFloat(ih, IUP_VALUE);

  return IUP_DEFAULT;
}

/* Callback called when the user wants to stop/start the rotation */
int rotating_valuechanged_cb(Ihandle *ih)
{
  speed = IupGetInt(ih, IUP_VALUE);

  return IUP_DEFAULT;
}

/* Callback to repaint the canvas */
int repaint_cb(Ihandle *self)
{
  IupGLMakeCurrent(self); /* Make self the current GL Context */

  /* Should be in another place (it is need only in the begining*/
  glEnable(GL_DEPTH_TEST); 

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  /* Black */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  anaglyph_draw(anaglyph);

  IupGLSwapBuffers(self);  /* change the back buffer with the front buffer */
  return IUP_DEFAULT; /* returns the control to the main loop */
}

int idle_cd(void)
{
  angle += speed;
  repaint_cb(canvas);
  return IUP_DEFAULT; /* return to the IUP main loop */
}

/* Prevents the control window from being closed */
int controls_close_cb(Ihandle *ih)
{
  return IUP_IGNORE;
}

/* Handle the user change the render type */
int rendertype_valuechanged_cb(Ihandle *self, char *t, int i, int v)
{
//  printf ("Item %d - %s - %s\n", i, t, v == 0 ? "deselected" : "selected" );
  if(v == 1){ //selected
    switch (i)
    {
      case 1:
        anaglyph->stereo_method = STEREO_OFF_AXIS;
        break;
      case 2:
        anaglyph->stereo_method = STEREO_OFF_AXIS_COLOR;
        break;
      case 3:
        anaglyph->stereo_method = STEREO_TOE_IN;
        break;
      case 4:
        anaglyph->stereo_method = STEREO_TOE_IN_COLOR;
        break;
      default: 
        anaglyph->stereo_method = STEREO_OFF_AXIS;
        break;
    }
  }
  return IUP_DEFAULT;
}

/* Handle the user change the geometry type */
int geometrytype_valuechanged_cb(Ihandle *self, char *t, int i, int v)
{
  printf ("Item %d - %s - %s\n", i, t, v == 0 ? "deselected" : "selected" );
  if(v == 1) //selected
  {
    modeltype = (MODEL_TYPE)(i);
  }
  return IUP_DEFAULT;
}

/* Handle the user changing the anaglyph method */
int anaglyphmethod_valuechanged_cb(Ihandle *self, char *t, int i, int v)
{
  printf ("Item %d - %s - %s\n", i, t, v == 0 ? "deselected" : "selected" );
  if(v == 1) //selected
  {
    anaglyph->anaglyph_method = (ANAGLYPH_METHOD) (i-1);
  }
  return IUP_DEFAULT;
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

  /* projection transformation (orthographic in the xy plane) */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(45, new_width/new_height, 1.0, 30.0);
  
  /* transformation applied to each vertex */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();           /* identity, i. e. no transformation */

  /* update canvas size and repaint */
  width = new_width;
  height = new_height;
 
  anaglyph_set_size(anaglyph, new_width, new_height);
  
  repaint_cb(canvas);

  return IUP_DEFAULT; /* return to the IUP main loop */
}

/*
 * Create the controls that user can use to change the scene.
 */
Ihandle *createFrmControls()
{
  Ihandle *frm, *vbox,
          *renderTypeList, *renderTypeFrm,
          *anaglyphMethodList, *anaglyphMethodFrm,
          *geometryTypeList, *geometryTypeFrm,
          *iod_v,
          *rotating_control;

  
  renderTypeList = IupList("render_type");

  IupSetAttributes(renderTypeList, "1=\"Anaglyph (offaxis)\", \
                                    2=\"Anaglyph (offaxis with color)\", \
                                    3=\"Anaglyph (toe-in)\", \
                                    4=\"Anaglyph (toe-in with color)\", \
                                    VISIBLEITEMS=4, \
                                    EXPAND=\"HORIZONTAL\""); 

/*  IupSetAttributes(renderTypeList, "1=\"Anaglyph (offaxis)\", \
                                    2=\"Anaglyph (toe-in)\", \
                                    3=\"Top/Bottom\", \
                                    4=\"Side-by-side\", \
                                    VISIBLEITEMS=4, \
                                    EXPAND=\"HORIZONTAL\""); */
  IupSetCallback( renderTypeList, "ACTION", 
                  (Icallback)rendertype_valuechanged_cb);


  renderTypeFrm = IupFrame( renderTypeList );
  IupSetAttribute (renderTypeFrm, "TITLE", "Render Type:");

  anaglyphMethodList = IupList("anaglyph_method");
  IupSetAttributes(anaglyphMethodList,  "1=\"True Anaglyphs\",\
                                     2=\"Gray Anaglyphs\", \
                                     3=\"Color Anaglyphs\", \
                                     4=\"Half Color Anaglyphs\", \
                                     5=\"Optimized Anaglyphs\", \
                                     VISIBLEITEMS=6, \
                                     EXPAND=\"HORIZONTAL\"");

  IupSetCallback( anaglyphMethodList, "ACTION", 
                  (Icallback)anaglyphmethod_valuechanged_cb);

  anaglyphMethodFrm = IupFrame( anaglyphMethodList );
  IupSetAttribute (anaglyphMethodFrm, "TITLE", "Anglyph Method:");

  geometryTypeList = IupList("geometry_type");
  IupSetAttributes(geometryTypeList, "1=\"Mesh\", \
                                    2=\"Sphere\", \
                                    3=\"Box\", \
                                    4=\"Pulsar\", \
                                    5=\"Knot\", \
                                    6=\"Tritoruz\", \
                                    7=\"Lorenz\", \
                                    8=\"Box Color\", \
                                    9=\"RGB Cube\", \
                                    VISIBLEITEMS=9, \
                                    EXPAND=\"HORIZONTAL\"");

  IupSetCallback( geometryTypeList, "ACTION", 
                  (Icallback)geometrytype_valuechanged_cb);

  geometryTypeFrm = IupFrame( geometryTypeList );
  IupSetAttribute (geometryTypeFrm, "TITLE", "Geometry:");

  /* Interocular distance */
  iod_v = IupVal("Horizontal");
  IupSetCallback(iod_v, "VALUECHANGED_CB",  (Icallback)iod_valuechanged_cb);
  IupSetAttribute(iod_v, "MIN", "-3.0");
  IupSetAttribute(iod_v, "MAX", "5.0");
  IupSetAttribute (iod_v, "VALUE", "0.5");
  /* End interocular distance */
  
  rotating_control = IupToggle("Rotating", "");
  IupSetCallback( rotating_control, "VALUECHANGED_CB", 
                  (Icallback) rotating_valuechanged_cb);
  IupSetAttribute (rotating_control, "VALUE", "ON");

  /* All together */
  vbox = IupVbox
  (
    IupFill(),
    renderTypeFrm,
    IupFill(),
    geometryTypeFrm,
    IupFill(),
    anaglyphMethodFrm,
    IupFill(),
    rotating_control,
    IupSetAttributes(IupHbox
      (
        IupLabel("Interocular Distance:"),
        iod_v,
        NULL
      ), "ALIGNMENT=ACENTER"
    ),
    IupButton ("A&bout", ""),
    NULL
  );
  
  frm = IupDialog (vbox);
  IupSetAttributes(frm, "TITLE=RGStereoPlayer Controls, \
                        MINBOX=NO, MAXBOX=NO");

  /* Disable close button of the control dialog */
  IupSetCallback( frm, "CLOSE_CB", (Icallback) controls_close_cb);

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
  /* Enable depth */
  IupSetAttribute(canvas, "DEPTH_SIZE", "1.0");
  /* bind callback actions with callback functions */
  IupSetCallback(canvas, "RESIZE_CB",(Icallback) resize_cb);

  IupGLMakeCurrent(canvas);


  controlsFrm = createFrmControls();
/*  IupSetAttribute (controlsFrm, "OPACITY", "140"); */
  IupShowXY(controlsFrm, 0, 0); /* show mainDlg */

  hbox = IupHbox (
    canvas,
    NULL
  );

  /* Create the Main Window */
  mainDlg = IupDialog(hbox);
  IupSetAttribute (mainDlg, "TITLE", "RGStereoPlayer"); 

  IupSetCallback (mainDlg, "CLOSE_CB", (Icallback) exit_cb);
  IupSetFunction (IUP_IDLE_ACTION, (Icallback) idle_cd); 
  
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

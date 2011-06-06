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

int width = 800, height = 600;
int speed = 0;
GLdouble angle = 0.0;

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
    //glTranslatef( 0.0f, 0.0f , 0.0f );
    //glScalef( 1.0f, 1.0f, 1.0f );
    glRotatef(angle, 0, 1, 0);
 // colorCube();
    MakeBox();
//    MakeSphere();
  glPopMatrix();
 
}

/* Callback called when the user wants to change the Interocular Distance */
int iod_valuechanged_cb(Ihandle *ih)
{
  anaglyph->IOD =  IupGetFloat(ih, IUP_VALUE);
}

/* Callback called when the user wants to stop/start the rotation */
int rotating_valuechanged_cb(Ihandle *ih)
{
  speed = IupGetInt(ih, IUP_VALUE);
}

/* Callback to repaint the canvas */
int repaint_cb(Ihandle *self)
{
  IupGLMakeCurrent(self); /* Make self the current GL Context */
 
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  /* White */
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

/* Prevents the control window from being closed */
int rendertype_valuechanged_cb(Ihandle *ih)
{
  int type = IupGetInt(ih, IUP_VALUE);
  if(type)
    anaglyph->anaglyph_method = ANAGLYPH_OFF_AXIS;
  else
    anaglyph->anaglyph_method = ANAGLYPH_TOE_IN;
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
  repaint_cb(canvas);

  return IUP_DEFAULT; /* return to the IUP main loop */
}

Ihandle *createFrmControls()
{
  Ihandle *frm, *vbox, *anag_toein, *anag_offaxis,
          *renderTypeFrm,
          *topbottom, *sidebyside, *renderType,
          *iod_v,
          *rotating_control;

  anag_offaxis = IupToggle ("&Anaglyph (offaxis)", ""),
  anag_toein = IupToggle ("&Anaglyph (Toe-in)", ""),
  topbottom = IupToggle ("&Top/Bottom", ""),
  sidebyside = IupToggle ("&Side-by-side", ""),

  renderType = IupRadio (
    IupVbox (
      anag_offaxis,
      anag_toein,
      topbottom,
      sidebyside,
      NULL
    )
  );

  IupSetHandle("anaglyph_offaxis", anag_offaxis);
  IupSetHandle("anaglyph_toein", anag_toein);
  IupSetHandle("topbottom", topbottom);
  IupSetHandle("sidebyside", sidebyside);

  IupSetAttribute(renderType, "VALUE", "anaglyph_offaxis");
  IupSetAttribute(anag_offaxis, "TIP",   "Two state button - Exclusive - RADIO");
  IupSetAttribute(anag_toein, "TIP",   "Two state button - Exclusive - RADIO");
  IupSetAttribute(topbottom, "TIP",   "Two state button - Exclusive - RADIO");
  IupSetAttribute(sidebyside, "TIP",   "Two state button - Exclusive - RADIO");
  IupSetCallback(anag_offaxis, "VALUECHANGED_CB", (Icallback)rendertype_valuechanged_cb);

  renderTypeFrm = IupFrame( renderType );
  IupSetAttribute (renderTypeFrm, "TITLE", "RenderType");
  
  iod_v = IupVal("Horizontal");
  IupSetCallback(iod_v, "VALUECHANGED_CB",  (Icallback)iod_valuechanged_cb);
  IupSetAttribute(iod_v, "MIN", "-3.0");
  IupSetAttribute(iod_v, "MAX", "5.0");
  IupSetAttribute (iod_v, "VALUE", "0.5");

  rotating_control = IupToggle("Rotating", "");
  IupSetCallback(rotating_control, "VALUECHANGED_CB", (Icallback) rotating_valuechanged_cb);
  IupSetAttribute (rotating_control, "VALUE", "ON");

  vbox = IupVbox
  (
    IupFill(),
    renderTypeFrm,
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

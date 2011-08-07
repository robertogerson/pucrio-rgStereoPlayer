#ifndef RG_ANAGLYPH
#define RG_ANAGLYPH

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

#define DTR 0.0174532925

static unsigned char *left_eye, *right_eye, *merged_image;

enum STEREO_METHOD
{
  STEREO_TOE_IN = 0, /* Enable the anaglyph toe-in projection method. */
  STEREO_TOE_IN_COLOR, /* Enable the anaglyph toe-in with color projection method. */
  STEREO_OFF_AXIS,    /* Enabe the anaglyph Off-axis projection  method.*/
  STEREO_OFF_AXIS_COLOR    /* Enabe the anaglyph Off-axis with color projection  method.*/
};

enum ANAGLYPH_METHOD
{
  ANAGLYPH_TRUE = 0,
  ANAGLYPH_GRAY,
  ANAGLYPH_COLOR,
  ANAGLYPH_HALF_COLOR,
  ANAGLYPH_OPTIMIZED
};

typedef struct
{
  GLdouble leftFrustum;
  GLdouble rightFrustum;
  GLdouble bottomFrustum;
  GLdouble topFrustum;
  GLdouble modelTranslation;
} camera;

typedef struct 
{
  float depthZ;    

  double fovy;      /* Field of view in y-axis */
  double nearZ;     /* near clipping plane */
  double farZ;      /* far clipping plane */
  double screenZ;   /* screen projection plane */
  double IOD; /* intraocular distance */

  double screen_width;
  double screen_height;

  void (*drawSceneFunc) (void);
  
  STEREO_METHOD stereo_method;
  ANAGLYPH_METHOD anaglyph_method;

  camera leftCam, rightCam;
} anaglyph_handle;



/*
 * Create a new handle to configure anaglyph generation.
 *
 * @param width         The width of the screen.
 * @param height        The height of the screen.
 * @param drawSceneFunc The function that is responsible to draw the scene.
 */
extern 
  anaglyph_handle *anaglyph_new_handle ( int width, int height,
                     void (*drawSceneFunc) (void));

/*
 * Draw the scene.
 * @param config An anaglyph handle that config the generation of the anaglyph
 *                images.
 */
extern
  void anaglyph_draw (anaglyph_handle *config);

/*
 * Set the size of the scene.
 * @param config An anaglyph handle that config the generation of the anaglyph
 *                images.
 * @param width the new width of the scene.
 * @param height the new height of the scene.
 */
extern
  void anaglyph_set_size(anaglyph_handle *config, int width, int height);

#endif

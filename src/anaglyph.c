#include "anaglyph.h"

anaglyph_handle *anaglyph_new_handle ( int width, int height,
                                void (*drawSceneFunc) (void))
{
  anaglyph_handle *config = 
              (anaglyph_handle*)malloc(sizeof(anaglyph_handle));
  
  /* Default values */
  config->depthZ = -10.0;
  config->fovy = 45;
  config->screen_width = width;
  config->screen_height = height;

  config->nearZ = 3.0;
  config->farZ = 30.0;
  config->screenZ = 10.0;
  config->IOD = 0.5;
  config->drawSceneFunc = drawSceneFunc;

  config->anaglyph_method = ANAGLYPH_OFF_AXIS;
  config->anaglyph_method = ANAGLYPH_TOE_IN;
}

void anaglyph_draw_toein(const anaglyph_handle *config)
{
  glDrawBuffer(GL_BACK); /* draw into both back buffers */
  
  /* clear color and depth buffers*/
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glDrawBuffer(GL_BACK_LEFT); /* draw into back left buffer */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity(); /* reset modelview matrix */
  gluLookAt(  (-1)*config->IOD/2, /* set camera position  x=-IOD/2 */
              0.0,            /*                      y=0.0 */
              0.0,            /*                      z=0.0 */
              0.0,            /*set camera "look at"  x=0.0 */
              0.0,            /*                      y=0.0 */
              config->screenZ,/*                      z=screenplane */
              0.0,            /* set camera up vector x=0.0 */
              1.0,            /*                      y=1.0 */
              0.0);           /*                      z=0.0 */

  // TODO: This must be configurable
  glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
  glPushMatrix();
  {
    glTranslatef(0.0, 0.0, config->depthZ); /* translate to screenplane */
    config->drawSceneFunc();
  }
  glPopMatrix();
  glDrawBuffer(GL_BACK_RIGHT);  /* draw into back right buffer */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();             /* reset modelview matrix */
  gluLookAt( config->IOD/2,
             0.0,
             0.0,
             0.0,
             0.0,
             config->screenZ, /* as for left buffer with camera position at: */
             0.0,
             1.0,
             0.0);   /*                     (IOD/2, 0.0, 0.0) */

  // TODO: This must be configurable
  glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
  glPushMatrix();
  {
    glTranslatef(0.0, 0.0, config->depthZ); /* translate to screenplane */
    config->drawSceneFunc();
  }
  glPopMatrix();
}

void setFrustum (anaglyph_handle *config)
{
  /* sets top of frustum based on fovy and near clipping plane */
  double top = config->nearZ * tan( DTR* config->fovy/2);
  double aspect = config->screen_width / config->screen_height;
  double right = aspect*top;  /* sets right of frustum based on aspect ratio */
  double frustumshift = (config->IOD/2)* config->nearZ / config->screenZ;

  config->leftCam.topFrustum = top;
  config->leftCam.bottomFrustum = -top;
  config->leftCam.leftFrustum = -right + frustumshift;
  config->leftCam.rightFrustum = right + frustumshift;
  config->leftCam.modelTranslation = config->IOD/2;
  
  config->rightCam.topFrustum = top;
  config->rightCam.bottomFrustum = -top;
  config->rightCam.leftFrustum = -right - frustumshift;
  config->rightCam.rightFrustum = right - frustumshift;
  config->rightCam.modelTranslation = (-1)*config->IOD/2;

}

void anaglyph_draw_offaxis(anaglyph_handle *config)
{ 
  setFrustum (config);  /* Just need call this when the viewport changes */

  glDrawBuffer(GL_BACK);  /* draw into both back buffers */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /* clear color and depth buffers */

  glDrawBuffer(GL_BACK_LEFT); /* draw into back left buffer */
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity(); /* reset projection matrix */

  /* set left view frustum */
  glFrustum(config->leftCam.leftFrustum, config->leftCam.rightFrustum,
      config->leftCam.bottomFrustum, config->leftCam.topFrustum,
      config->nearZ, config->farZ);
  glTranslatef(config->leftCam.modelTranslation, 0.0, 0.0); /* translate to cancel parallax */

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // TODO: This must be configurable
  glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
  glPushMatrix();
  {
    glTranslatef(0.0, 0.0, config->depthZ); /* translate to screenplane */
    config->drawSceneFunc();
  }
  glPopMatrix();
  
  glDrawBuffer(GL_BACK_RIGHT);  /* draw into back right buffer */
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity(); /* reset projection matrix */

  /* set right view frustum */
  glFrustum(config->rightCam.leftFrustum, config->rightCam.rightFrustum,   
      config->rightCam.bottomFrustum, config->rightCam.topFrustum,
      config->nearZ, config->farZ);
  
  glTranslatef(config->rightCam.modelTranslation, 0.0, 0.0); /* translate to cancel parallax */
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // TODO: This must be configurable
  glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
  glPushMatrix();
  {
    glTranslatef(0.0, 0.0, config->depthZ); /* translate to screenplane */
    config->drawSceneFunc();
  }
  glPopMatrix();
}

void anaglyph_draw (anaglyph_handle *config)
{
  switch (config->anaglyph_method)
  {
    case ANAGLYPH_TOE_IN:
      anaglyph_draw_toein(config);
      break;
    case ANAGLYPH_OFF_AXIS:
      anaglyph_draw_offaxis(config);
      break;
    default: 
      printf("Unknow ANAGLYPH Draw Method.\n");
  }
}


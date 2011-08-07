#include "anaglyph.h"

anaglyph_handle *anaglyph_new_handle ( int width, int height,
                                void (*drawSceneFunc) (void))
{
  anaglyph_handle *config = 
              (anaglyph_handle*)malloc(sizeof(anaglyph_handle));
  
  /* Default values */
  config->depthZ = -10.0;
  config->fovy = 45;

  config->drawSceneFunc = drawSceneFunc;
  config->nearZ = 3.0;
  config->farZ = 30.0;
  config->screenZ = 10.0;
  config->IOD = 0.5;

  config->stereo_method = STEREO_OFF_AXIS;
  config->anaglyph_method = ANAGLYPH_COLOR;

  anaglyph_set_size(config, width, height);

  return config;
}

void anaglyph_set_size(anaglyph_handle *config, int width, int height)
{
  config->screen_width = width;
  config->screen_height = height;

  printf("%d %d\n", width, height);

  if(left_eye != NULL)
  {
    free(left_eye);
    left_eye = NULL;
  }

  if(right_eye != NULL)
  {
    free(right_eye);
    right_eye = NULL;
  }

  if(merged_image != NULL)
  {
    free(merged_image);
    merged_image = NULL;
  }

  left_eye = (unsigned char *) malloc(sizeof(unsigned char)*4*width*height);
  right_eye = (unsigned char *) malloc(sizeof(unsigned char)*4*width*height);
  merged_image = (unsigned char *) malloc(sizeof(unsigned char)*4*width*height);

  return;
}

void merge_left_and_right(const anaglyph_handle *config)
{
  int i, j, pos;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for(i = 0; i < config->screen_height; i++)
  {
    for(j = 0; j < config->screen_width; j++)
    {
      pos = (i*config->screen_width*4) + (j*4);

      switch(config->anaglyph_method)
      {
        case ANAGLYPH_TRUE:
          //True Anaglyphs
          merged_image[pos] = 0.299*left_eye[pos] +
                              0.587*left_eye[pos+1] +
                              0.114*left_eye[pos+2];
          
          merged_image[pos+1] = 0;
          
          merged_image[pos+2] = 0.299*right_eye[pos] +
                                0.587*right_eye[pos+1] +
                                0.114*right_eye[pos+2];
        break;

        case ANAGLYPH_GRAY:
          //Gray Anaglyphs
          merged_image[pos] = 0.299*left_eye[pos] +
                              0.587*left_eye[pos+1] +
                              0.114*left_eye[pos+2];

          merged_image[pos+1] = 0.299*right_eye[pos] +
                                0.587*right_eye[pos+1] +
                                0.114*right_eye[pos+2];

          merged_image[pos+2] = 0.299*right_eye[pos] +
                                0.587*right_eye[pos+1] +
                                0.114*right_eye[pos+2];
        break;

        case ANAGLYPH_COLOR:
        //Color Anaglyphs
          merged_image[pos] = left_eye[pos];
          merged_image[pos+1] = right_eye[pos+1];
          merged_image[pos+2] = right_eye[pos+2];
        break;

        case ANAGLYPH_HALF_COLOR:
          //Half color Anaglyphs
          merged_image[pos] = 0.299*left_eye[pos] +
                              0.587*left_eye[pos+1] +
                              0.114*left_eye[pos+2];
          merged_image[pos+1] = right_eye[pos+1];
          merged_image[pos+2] = right_eye[pos+2];
        break;

        case ANAGLYPH_OPTIMIZED:
          //Optimized Anaglyphs
          merged_image[pos] = 0.1*left_eye[pos] +
                              0.6*left_eye[pos+1] +
                              0.3*left_eye[pos+2];
          merged_image[pos+1] = right_eye[pos+1];
          merged_image[pos+2] = right_eye[pos+2];

        break;
     }
    }
  }
  glDrawPixels(config->screen_width, config->screen_height, GL_RGBA, GL_UNSIGNED_BYTE, 
                merged_image);
  glFlush();
}

void anaglyph_draw_toein(const anaglyph_handle *config)
{
  /* I just need do this when change the viewport */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective( config->fovy,
                  config->screen_width/config->screen_height, 
                  0.1,
                  10000.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  glDrawBuffer(GL_BACK); /* draw into back buffer */
  glReadBuffer(GL_BACK); /* read from back buffer */
  
  /* clear color and depth buffers*/
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClear(GL_ACCUM_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity(); /* reset modelview matrix */
  
  gluLookAt(  (-1)*config->IOD/2, /* set camera position  x=-IOD/2 */
              0.0,            /*                      y=0.0 */
              0.0,            /*                      z=0.0 */
              0.0,            /*set camera "look at"  x=0.0 */
              0.0,            /*                      y=0.0 */
              (-1)*config->screenZ,/*                      z=screenplane */
              0.0,            /* set camera up vector x=0.0 */
              1.0,            /*                      y=1.0 */
              0.0);           /*                      z=0.0 */

  //TODO: This must be configurable
  glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
  glPushMatrix();
  {
    glTranslatef(0.0, 0.0, config->depthZ); /* translate to screenplane */
    config->drawSceneFunc();
  }
  glPopMatrix();
  glFlush();

  /* Write over accumulation buffer */
  glAccum(GL_ACCUM, 1.0);

  // Remove the previous color mask.
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

  glDrawBuffer(GL_BACK);  /* draw into back buffer */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();             /* reset modelview matrix */
  gluLookAt( config->IOD/2,
             0.0,
             0.0,
             0.0,
             0.0,
             (-1)*config->screenZ, /* as for left buffer with camera position at: */
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
  glFlush();
  
  // Remove the previous color mask.
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

  glAccum(GL_ADD, 1.0);
  glAccum(GL_RETURN, 1.0);
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

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glDrawBuffer(GL_BACK);  /* draw into both back buffers */
  glReadBuffer(GL_BACK);  /* read from both back buffers */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /* clear color and depth buffers */
  glClear(GL_ACCUM_BUFFER_BIT);

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

  // Remove the previous color mask.
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

void anaglyph_draw_toein_color(const anaglyph_handle *config)
{
  /* I just need do this when change the viewport */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective( config->fovy,
                  config->screen_width/config->screen_height, 
                  0.1,
                  10000.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  glDrawBuffer(GL_BACK); /* draw into back buffer */
  glReadBuffer(GL_BACK); /* read from back buffer */

  /* clear color and depth buffers*/
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClear(GL_ACCUM_BUFFER_BIT);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity(); /* reset modelview matrix */
  
  gluLookAt(  (-1)*config->IOD/2, /* set camera position  x=-IOD/2 */
              0.0,            /*                      y=0.0 */
              0.0,            /*                      z=0.0 */
              0.0,            /*set camera "look at"  x=0.0 */
              0.0,            /*                      y=0.0 */
              (-1)*config->screenZ,/*                      z=screenplane */
              0.0,            /* set camera up vector x=0.0 */
              1.0,            /*                      y=1.0 */
              0.0);           /*                      z=0.0 */

  glPushMatrix();
  {
    glTranslatef(0.0, 0.0, config->depthZ); /* translate to screenplane */
    config->drawSceneFunc();
  }
  glPopMatrix();
  glFlush();

  glReadPixels(0, 0, config->screen_width, config->screen_height, GL_RGBA, 
                GL_UNSIGNED_BYTE, left_eye);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();             /* reset modelview matrix */
  gluLookAt( config->IOD/2,
             0.0,
             0.0,
             0.0,
             0.0,
             (-1)*config->screenZ, /* as for left buffer with camera position at: */
             0.0,
             1.0,
             0.0);   /*                     (IOD/2, 0.0, 0.0) */

  glPushMatrix();
  {
    glTranslatef(0.0, 0.0, config->depthZ); /* translate to screenplane */
    config->drawSceneFunc();
  }
  glPopMatrix();
  glFlush();
  glReadPixels(0, 0, config->screen_width, config->screen_height, GL_RGBA,
                GL_UNSIGNED_BYTE, right_eye);

  merge_left_and_right(config);
}

void anaglyph_draw_offaxis_color(anaglyph_handle *config)
{
  setFrustum (config);  /* Just need call this when the viewport changes */

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glDrawBuffer(GL_BACK);  /* draw into both back buffers */
  glReadBuffer(GL_BACK);  /* draw into both back buffers */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /* clear color and depth buffers */
  glClear(GL_ACCUM_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity(); /* reset projection matrix */

  /* set left view frustum */
  glFrustum(config->leftCam.leftFrustum, config->leftCam.rightFrustum,
      config->leftCam.bottomFrustum, config->leftCam.topFrustum,
      config->nearZ, config->farZ);
  glTranslatef(config->leftCam.modelTranslation, 0.0, 0.0); /* translate to cancel parallax */

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  glPushMatrix();
  {
    glTranslatef(0.0, 0.0, config->depthZ); /* translate to screenplane */
    config->drawSceneFunc();
  }
  glPopMatrix();
  glReadPixels(0, 0, config->screen_width, config->screen_height, GL_RGBA, 
                GL_UNSIGNED_BYTE, left_eye);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity(); /* reset projection matrix */

  /* set right view frustum */
  glFrustum(config->rightCam.leftFrustum, config->rightCam.rightFrustum,   
      config->rightCam.bottomFrustum, config->rightCam.topFrustum,
      config->nearZ, config->farZ);
  
  glTranslatef(config->rightCam.modelTranslation, 0.0, 0.0); /* translate to cancel parallax */
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glPushMatrix();
  {
    glTranslatef(0.0, 0.0, config->depthZ); /* translate to screenplane */
    config->drawSceneFunc();
  }
  glPopMatrix();
  glReadPixels(0, 0, config->screen_width, config->screen_height, GL_RGBA, 
                GL_UNSIGNED_BYTE, right_eye);

  merge_left_and_right(config);
}

void anaglyph_draw (anaglyph_handle *config)
{
  switch (config->stereo_method)
  {
    case STEREO_TOE_IN:
      anaglyph_draw_toein(config);
      break;
    case STEREO_TOE_IN_COLOR:
      anaglyph_draw_toein_color(config);
      break;
    case STEREO_OFF_AXIS:
      anaglyph_draw_offaxis(config);
      break;
    case STEREO_OFF_AXIS_COLOR:
      anaglyph_draw_offaxis_color(config);
      break;
   default: 
      printf("Unknow STEREO Draw Method.\n");
  }
}


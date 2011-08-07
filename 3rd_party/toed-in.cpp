#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

int screenwidth = 800, screenheight = 600;
//toed-in stereo
float depthZ = -5.0;                                      //depth of the object drawing

double fovy = 45;                                          //field of view in y-axis
double aspect = double(screenwidth)/double(screenheight);  //screen aspect ratio
double nearZ = 3.0;                                        //near clipping plane
double farZ = 30.0;                                        //far clipping plane
double screenZ = -10.0;                                     //screen projection plane
double IOD = 0.5;                                          //intraocular distance

typedef struct {
   double x,y,z;
} XYZ;

/* 
 * Create the geometry for a wireframe box
 */
void MakeBox(void)
{
  XYZ pmin = {-1,-1,-1}, pmax = {1, 1, 1};

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

void init(void)
{
  glViewport (0, 0, screenwidth, screenheight);            //sets drawing viewport
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fovy, aspect, nearZ, farZ);               //sets frustum using gluPerspective
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

GLvoid display(void)
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

//  glDrawBuffer(GL_BACK);                                   //draw into both back buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);      //clear color and depth buffers

//  glDrawBuffer(GL_BACK_LEFT);                              //draw into back left buffer
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();                                        //reset modelview matrix
  gluLookAt(-IOD/2,                                        	//set camera position  x=-IOD/2
            0.0,                                           //                     y=0.0
            0.0,                                           //                     z=0.0
            0.0,                                           //set camera "look at" x=0.0
            0.0,                                           //                     y=0.0
            screenZ,                                       //                     z=screenplane
            0.0,                                           //set camera up vector x=0.0
            1.0,                                           //                     y=1.0
            0.0);                                          //                     z=0.0
  glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
  glPushMatrix();
  {
    glTranslatef(0.0, 0.0, depthZ);                        //translate to screenplane
    MakeBox();
  }
  glPopMatrix();

  glDrawBuffer(GL_BACK_RIGHT);                             //draw into back right buffer
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();                                        //reset modelview matrix
  gluLookAt(IOD/2, 0.0, 0.0, 0.0, 0.0, screenZ,            //as for left buffer with camera position at:
            0.0, 1.0, 0.0);                                //                     (IOD/2, 0.0, 0.0)

  glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
  glPushMatrix();
  {
    glTranslatef(0.0, 0.0, depthZ);                        //translate to screenplane
    MakeBox();
  }
  glPopMatrix();

  glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_ACCUM | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Primeiro Programa");
	glutReshapeWindow(screenwidth, screenheight);
	glutDisplayFunc(display);
	init();
	glutMainLoop();
	return 0;
}

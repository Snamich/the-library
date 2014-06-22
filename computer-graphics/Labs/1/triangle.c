#ifdef __APPLE__
#include "/usr/local/include/GL/glew.h"
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <GL/glew.h>
#include <GL/glut.h>
#endif

const GLfloat vertex1[3]={-1.0f,-1.0f,0.0f};
const GLfloat vertex2[3]={1.0f,-1.0f,0.0f};
const GLfloat vertex3[3]={0.0f,1.5f,0.0f};

void RenderScene(void)
{
  //clear all pixels
  glClear(GL_COLOR_BUFFER_BIT);

  //draw blue isosceles triangle
  glColor3f(0.0f, 0.0f, 1.0f);
  glBegin(GL_LINE_LOOP);
  glVertex3fv(vertex1);
  glVertex3fv(vertex2);
  glVertex3fv(vertex3);
  glEnd();
  glFlush();
}

void init(void)
{
  //Set background color as white
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  //initialize viewing values
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-2.0f, 2.0f, -2.0f, 2.0f, -1.0f, 1.0f);
}

int main(int argc, char** argv)
{
  //declare initial window size, position, and display mode
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100, 100);

  //open window with "Triangle" in its title bar
  glutCreateWindow("Triangle");
  glewInit();
  glutDisplayFunc(RenderScene);
  init();

  glutMainLoop();
  return 0;
}

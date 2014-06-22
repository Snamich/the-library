#include "common/Angel.h"
#include "lab7.h"
#include "Mesh.cpp"
#include <ctime>
#include <sys/time.h>
#include <vector>

typedef Angel::vec4 color4;
typedef Angel::vec4 point4;

// Camera controls
GLint ModelViewLoc;
vec4 eye, at, up;
int oldX, oldY;
int CAMERA_STATE;
const int REST = -1, ZOOM = 0, PAN = 1, LOOK = 2, ORBIT = 3;

std::vector<Mesh> Models;

//----------------------------------------------------------------------------
// Camera functions

void camera_look_around()
{
  static float i = 0.0;
  i = 0.0;
  struct timeval t1, t2;
  gettimeofday( &t1, NULL );
  float radius = length( eye - at );

  while( i < 360.0 ) {
    gettimeofday( &t2, NULL );
    float timeDelta = (t2.tv_usec - t1.tv_usec) / 1000.0;
    gettimeofday( &t1, NULL );
    float theta = i * DegreesToRadians;

    at = vec4( radius * cos(theta), 0, radius * sin(theta), 1.0 );
    mat4 N = LookAt( eye, at, up );
    glUniformMatrix4fv( ModelViewLoc, 1, GL_TRUE, N );

    glutPostRedisplay();

    i += 5.0 * timeDelta;
    printf( "I: %f\n", i );

    printf( "TimeDelta: %f\n", timeDelta );
  }
}

void camera_orbit()
{
  static float i = 0.0;
  i = 0.0;
  struct timeval t1, t2;
  gettimeofday( &t1, NULL );
  float radius = length( eye - at );

  while( i < 360.0 ) {
    gettimeofday( &t2, NULL );
    float timeDelta = (t2.tv_usec - t1.tv_usec) / 1000.0;
    gettimeofday( &t1, NULL );
    float theta = i * DegreesToRadians;

    eye = vec4( radius * cos(theta), 0, radius * sin(theta), 1.0 );
    mat4 N = LookAt( eye, at, up );
    glUniformMatrix4fv( ModelViewLoc, 1, GL_TRUE, N );

    glutPostRedisplay();

    i += 5.0 * timeDelta;
    printf( "I: %f\n", i );

    printf( "TimeDelta: %f\n", timeDelta );
  }
}

void camera_reset()
{
  eye = vec4( 0.0, 0.0, 25.0, 1.0 );
  at = vec4( 0.0, 0.0, 0.0, 1.0 );
  up = vec4( 0.0, 1.0, 0.0, 0.0 );

  mat4 N = LookAt( eye, at, up );
  glUniformMatrix4fv( ModelViewLoc, 1, GL_TRUE, N );

  glutPostRedisplay();
}

//----------------------------------------------------------------------------
// GLUT callbacks

void display_cb()
{
  // clear the window
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // draw the points
  for( int i = 0; i < Models.size(); ++i ) {
    Models[i].Render();
    Models[i].BB.Render();
  }
  
  glFlush();
}

void keyboard_cb( unsigned char key, int x, int y )
{
  switch ( key ) {
  case 033:
    exit( EXIT_SUCCESS );
    break;
  case 97:
    camera_look_around();
    break;
  case 115:
    camera_orbit();
    break;
  case 100:
    camera_reset();
    break;
  } 
}

void motion_cb( int x, int y )
{
  mat4 M = mat4();
  int deltaX = x - oldX, deltaY = y - oldY;

  switch( CAMERA_STATE ) {
  case ZOOM:
    M = Translate( 0.0, 0.0, deltaY );
    eye = M * eye;
    break;
  case PAN:
    M = Translate( deltaX, deltaY, 0.0 );
    eye = M * eye;
    at = M * at;
    break;
  }

  oldX = x;
  oldY = y;

  mat4 N = LookAt( eye, at, up );
  glUniformMatrix4fv( ModelViewLoc, 1, GL_TRUE, N );

  glutPostRedisplay();
}

void mouse_cb( int button, int state, int x, int y )
{
  oldX = x;
  oldY = y;

  if( state != GLUT_DOWN ) {
    CAMERA_STATE = REST;
    return;
  }

  if( button == GLUT_RIGHT_BUTTON ) {
    CAMERA_STATE = ZOOM;
  } else if( button == GLUT_LEFT_BUTTON ) {
    // check for selected model
    // if not, must be panning
    CAMERA_STATE = PAN;
  }
}

//----------------------------------------------------------------------------
// GLUI callbacks

void camera_cb( int ID )
{
  switch( ID ) {
  case 0: // look around
    CAMERA_STATE = LOOK;
    break;
  case 1: // orbit
    CAMERA_STATE = ORBIT;
    break;
  }
}

//----------------------------------------------------------------------------
// Main and init

int main( int argc, char **argv )
{
  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_RGBA );
  glutInitWindowSize( 500, 500 );
  glutInitWindowPosition( 100, 100 );

  glutCreateWindow( "Lab 7" );
  glewExperimental = GL_TRUE;
  glewInit();
  init();

  glutDisplayFunc( display_cb );
  glutKeyboardFunc( keyboard_cb );
  glutMotionFunc( motion_cb );
  glutMouseFunc( mouse_cb );

  glutMainLoop();
  return 0;
}

void init()
{
  // Read model in
  Mesh woman;
  woman.LoadMesh( "woman" );
  Models.push_back( woman );

  // Load shaders and use the resulting shader program
  GLuint program = InitShader( "shaders/vshader.glsl", "shaders/fshader.glsl" );
  glUseProgram( program );

  // Initialize shader lighting parameters
  point4 light_position( 0.0, 0.0, 1.0, 0.0 );
  color4 light_ambient( 0.5, 0.5, 0.5, 1.0 );
  color4 light_diffuse( 1.0, 1.0, 1.0, 1.0 );

  // Material properties
  color4 material_diffuse( 0.0, 0.0, 1.0, 1.0 );

  // Calculate products
  color4 ambient_product = light_ambient;
  color4 diffuse_product = light_diffuse * material_diffuse;

  // Pass in light products
  glUniform4fv( glGetUniformLocation( program, "AmbientProduct" ),
                1, ambient_product );
  glUniform4fv( glGetUniformLocation( program, "DiffuseProduct" ),
                1, diffuse_product );
  glUniform4fv( glGetUniformLocation( program, "LightPosition" ),
                1, light_position );

  // Set our vertex projection matrix
  //    mat4 orthoProjection = Ortho( -20.0, 20.0, -20.0, 20.0, -20.0, 20.0 );
  mat4 orthoProjection = Perspective( 45.0, 1.0, 20.0, -20.0 );
  GLuint loc = glGetUniformLocation( program, "Projection" );
  glUniformMatrix4fv( loc, 1, GL_TRUE, orthoProjection );

  // Set up our camera
  eye = vec4( 0.0, 0.0, 25.0, 1.0 );
  at = vec4( 0.0, 0.0, 0.0, 1.0 );
  up = vec4( 0.0, 1.0, 0.0, 0.0 );
  mat4 N = LookAt( eye, at, up );
  ModelViewLoc = glGetUniformLocation( program, "ModelView" );
  glUniformMatrix4fv( ModelViewLoc, 1, GL_TRUE, N );

  glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
}

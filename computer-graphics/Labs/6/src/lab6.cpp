#include "common/Angel.h"
#include "Mesh.cpp"
#include <vector>

typedef vec4 color4;
typedef vec4 point4;

Mesh woman;
bool Selected = false;

GLuint MaterialColorLoc;
mat4 orthoProjection, N;

//----------------------------------------------------------------------------

void
init( void )
{
  // Read model in
  woman.LoadMesh( "woman" );

  // Load shaders and use the resulting shader program
  GLuint program = InitShader( "shaders/vshader.glsl", "shaders/fshader.glsl" );
  glUseProgram( program );

  GLuint loc;

  // Initialize shader lighting parameters
  point4 light_position( 0.0, 0.0, 1.0, 0.0 );
  color4 light_ambient( 0.5, 0.5, 0.5, 1.0 );
  color4 light_diffuse( 1.0, 1.0, 1.0, 1.0 );

  // Calculate products
  color4 ambient_product = light_ambient;

  // Pass in light products
  glUniform4fv( glGetUniformLocation( program, "AmbientProduct" ),
                1, ambient_product );
  glUniform4fv( glGetUniformLocation( program, "DiffuseProduct" ),
                1, light_diffuse );
  glUniform4fv( glGetUniformLocation( program, "LightPosition" ),
                1, light_position );
  MaterialColorLoc = glGetUniformLocation( program, "MaterialColor" );

  // Set our vertex projection matrix
  orthoProjection = Ortho( -20.0, 20.0, -20.0, 20.0, -20.0, 20.0 );
  loc = glGetUniformLocation( program, "Projection" );
  glUniformMatrix4fv( loc, 1, GL_TRUE, orthoProjection );

  // Set up our camera
  vec4 eye( 0.0, 0.0, 1.0, 1.0 );
  vec4 at( 0.0, 0.0, 0.0, 1.0 );
  vec4 up( 0.0, 1.0, 0.0, 0.0 );
  N = LookAt( eye, at, up );
  glUniformMatrix4fv( glGetUniformLocation( program, "ModelView" ),
                      1, GL_TRUE, N );

  glEnable( GL_DEPTH );
  glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
}

//----------------------------------------------------------------------------

void
display( void )
{
  // clear the window
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  if( Selected ) {
    glUniform4fv( MaterialColorLoc, 1, vec4( 0.0, 0.0, 0.0, 1.0 ) );
  } else {
    glUniform4fv( MaterialColorLoc, 1, woman.GetMaterialColor() );
  }

  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  woman.Render();
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  
  glFlush();
}

//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{
  switch ( key ) {
  case 033:
    exit( EXIT_SUCCESS );
    break;
  }
}

//----------------------------------------------------------------------------

vec3 GetOGLPos( int x, int y )
{
  GLint viewport[4];
  GLdouble modelview[16];
  GLdouble projection[16];
  GLfloat winX, winY, winZ;
  GLdouble posX, posY, posZ;

  glGetIntegerv( GL_VIEWPORT, viewport );

  for( int i = 0, k = 0; i < 4; ++i ) {
    for( int j = 0; j < 4; ++j, ++k ) {
      modelview[k] = N[j][i];
      projection[k] = orthoProjection[j][i];
    }
  }

  winX = (float)x;
  winY = (float)viewport[3] - (float)y;
  glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

  gluUnProject( winX, winY, 0.5, modelview, projection, viewport, &posX, &posY, &posZ );
 
  return vec3( posX, posY, posZ );
}

void mouse_cb( int button, int state, int x, int y )
{
  if( state != GLUT_DOWN )
    return;
  if( button == GLUT_RIGHT_BUTTON )
    exit( 0 );

  vec3 test = GetOGLPos( x, y );

  if( woman.IntersectP( test ) ) {
    Selected = true;
  } else {
    Selected = false;
  }

  glutPostRedisplay();
}

int
main( int argc, char **argv )
{
  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH );
  glutInitWindowSize( 500, 500 );
  glutInitWindowPosition( 100, 100 );

  glutCreateWindow( "Lab 6" );
  glewExperimental = GL_TRUE;
  glewInit();
  init();

  glutMouseFunc( mouse_cb );
  glutDisplayFunc( display );
  glutKeyboardFunc( keyboard );

  glutMainLoop();
  return 0;
}

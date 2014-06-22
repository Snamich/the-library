#include "common/Angel.h"
#include "read_and_load.cpp"
#include <vector>

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

std::vector<Angel::vec3> verts;
std::vector<Angel::vec3> triangles;

//----------------------------------------------------------------------------

void
init( void )
{
  // Read model in
  loadModel( "data/woman.coor", "data/woman.poly", &verts, &triangles );

  // Create a vertex array object
  GLuint vao[1];
  glGenVertexArrays( 1, vao );
  glBindVertexArray( vao[0] );

  // Create and initialize a buffer object
  GLuint buffer;
  glGenBuffers( 1, &buffer );
  glBindBuffer( GL_ARRAY_BUFFER, buffer );
  glBufferData( GL_ARRAY_BUFFER, triangles.size() * sizeof(vec3),
                triangles.data(), GL_STATIC_DRAW );

  // Load shaders and use the resulting shader program
  GLuint program = InitShader( "shaders/vshader.glsl", "shaders/fshader.glsl" );
  glUseProgram( program );

  // Initialize the vertex position attribute from the vertex shader
  GLuint loc = glGetAttribLocation( program, "vPosition" );
  glEnableVertexAttribArray( loc );
  glVertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, 0,
                         BUFFER_OFFSET(0) );

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
  mat4 orthoProjection = Ortho( -2.0, 2.0, -2.0, 2.0, -1.0, 1.0 );
  loc = glGetUniformLocation( program, "Projection" );
  glUniformMatrix4fv( loc, 1, GL_TRUE, orthoProjection );

  // Set up our camera
  vec4 eye( 1.0, 2.0, 0.5, 1.0 );
  vec4 at( 0.0, 0.0, 0.0, 1.0 );
  vec4 up( 0.0, 1.0, 0.0, 0.0 );
  mat4 N = LookAt( eye, at, up );
  glUniformMatrix4fv( glGetUniformLocation( program, "ModelView" ),
                      1, GL_TRUE, N );

  glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
}

//----------------------------------------------------------------------------

void
display( void )
{
  // clear the window
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // draw the points
  glDrawArrays( GL_TRIANGLES, 0, triangles.size() );
  
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

int
main( int argc, char **argv )
{
  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_RGBA );
  glutInitWindowSize( 500, 500 );
  glutInitWindowPosition( 100, 100 );

  glutCreateWindow( "Lab 5" );
  glewExperimental = GL_TRUE;
  glewInit();
  init();

  glutDisplayFunc( display );
  glutKeyboardFunc( keyboard );

  glutMainLoop();
  return 0;
}

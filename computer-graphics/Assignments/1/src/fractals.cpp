#include "fractals.h"

vec3 points[4000];
int mainWindowID, Index = 0, seed;

// fshader variables
vec4 fColor = vec4( 1.0, 0.0, 0.0, 1.0 );
GLuint fColorLoc;

// GLUI live variables
int recursiveLevels = 2, color;

// Starting geometry
vec3 vertices[3] = {
  vec3( -1.5, -1.0, 0.0 ), vec3( 1.5, -1.0, 0.0 ), vec3( 0.0, 1.0, 0.0 )
};

void triangle( const vec3& a, const vec3& b, const vec3& c )
{
  points[Index] = a; Index++;
  points[Index] = b; Index++;
  points[Index] = c; Index++;
}

vec3 getMidpoint( const vec3& a, const vec3& b, float clamp )
{
  float displacementAmount;
  int displacementSign;

  displacementSign = rand() & 1;
  displacementAmount = (rand() / (double)RAND_MAX) / clamp;
  displacementAmount *= displacementSign < 1 ? -1 : 1;
  vec3 midpoint = (a + b) / 2.0;
  midpoint[1] += displacementAmount;

  return midpoint;
}

void fractal( int levels )
{
  // Set our seed
  srand( seed );

  fractalOuter( vertices[0], vertices[1], vertices[2], 2.0, levels );
}

void fractalLeft( const vec3& a, const vec3& b, const vec3& c,
                  const vec3& mid1, float clamp, int levels )
{
  if( levels > 0 ) {
    vec3 mid0 = getMidpoint( a, b, clamp );
    vec3 mid2 = getMidpoint( a, c, clamp );
    fractalRecurs( a, b, c, mid0, mid1, mid2, clamp, levels );
  } else {
    triangle( a, b, c );
  }
}

void fractalRight( const vec3& a, const vec3& b, const vec3& c,
                   const vec3& mid2, float clamp, int levels )
{
  if( levels > 0 ) {
    vec3 mid0 = getMidpoint( a, b, clamp );
    vec3 mid1 = getMidpoint( b, c, clamp );
    fractalRecurs( a, b, c, mid0, mid1, mid2, clamp, levels );
  } else {
    triangle( a, b, c );
  }
}

void fractalTop( const vec3& a, const vec3& b, const vec3& c,
                 const vec3& mid0, float clamp, int levels )
{
  if( levels > 0 ) {
    vec3 mid1 = getMidpoint( b, c, clamp );
    vec3 mid2 = getMidpoint( a, c, clamp );
    fractalRecurs( a, b, c, mid0, mid1, mid2, clamp, levels );
  } else {
    triangle( a, b, c );
  }
}

void fractalOuter( const vec3& a, const vec3& b, const vec3& c, float clamp, int levels )
{
  if( levels > 0 ) {
    vec3 mid0 = getMidpoint( a, b, clamp );
    vec3 mid1 = getMidpoint( b, c, clamp );
    vec3 mid2 = getMidpoint( a, c, clamp );
    fractalRecurs( a, b, c, mid0, mid1, mid2, clamp, levels );
  } else {
    triangle( a, b, c );
  }
}

void fractalInner( const vec3& a, const vec3& b, const vec3& c,
                   const vec3& mid0, const vec3& mid1, const vec3& mid2,
                   float clamp, int levels )
{
  if( levels > 0 ) {
    fractalRecurs( a, b, c, mid0, mid1, mid2, clamp, levels );
  } else {
    triangle( a, b, c );
  }
}

void fractalRecurs( const vec3& a, const vec3& b, const vec3& c,
                    const vec3& mid0, const vec3& mid1, const vec3& mid2,
                    float clamp, int levels )
{
  vec3 mida = getMidpoint( mid0, mid2, clamp * 2 );
  vec3 midb = getMidpoint( mid0, mid1, clamp * 2 );
  vec3 midc = getMidpoint( mid1, mid2, clamp * 2 );
  fractalLeft( a, mid0, mid2, mida, clamp * 2, levels - 1 );
  fractalRight( mid0, b, mid1, midb, clamp * 2, levels - 1 );
  fractalTop( mid2, mid1, c, midc, clamp * 2, levels - 1 );
  fractalInner( mid1, mid2, mid0, midc, mida, midb, clamp * 2, levels - 1 );
}

void display()
{
  glClear( GL_COLOR_BUFFER_BIT );

  int i;
  for( i = 0; i < Index; i+=3 ) {
    glDrawArrays( GL_LINE_LOOP, i, 3 );
  }

  // Pass the color into the fragment shader
  glUniform4fv( fColorLoc, 1, fColor );

  glFlush();
}

void keyboard( unsigned char key, int x, int y )
{

}

void init()
{
  // Initial data setup
  seed = time( NULL );
  fractal( recursiveLevels );

  // Create a vertex array object
  GLuint vao[1];
  glGenVertexArrays( 1, vao );
  glBindVertexArray( vao[0] );

  // Create and initialize a buffer object
  GLuint buffer;
  glGenBuffers( 1, &buffer );
  glBindBuffer( GL_ARRAY_BUFFER, buffer );
  glBufferData( GL_ARRAY_BUFFER, sizeof(points), points, GL_DYNAMIC_DRAW );

  // Load shaders and use the resulting shader program
  GLuint program = InitShader( "shaders/vshader.glsl", "shaders/fshader.glsl" );
  glUseProgram( program );

  // Initialize the vertex position attribute from the vertex shader
  GLuint loc = glGetAttribLocation( program, "vPosition" );
  glEnableVertexAttribArray( loc );
  glVertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, 0,
                         BUFFER_OFFSET(0) );

  // Set fragment color
  fColorLoc = glGetUniformLocation( program, "fColor" );
  glUniform4fv( fColorLoc, 1, fColor );

  // Set our vertex projection matrix
  mat4 orthoProjection = Ortho( -2.0, 2.0, -2.0, 2.0, -1.0, 1.0 );
  loc = glGetUniformLocation( program, "projection" );
  glUniformMatrix4fv( loc, 1, GL_TRUE, orthoProjection );

  glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
}

int main( int argc, char *argv[] )
{
  // Set glut up
  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_RGBA );
  glutInitWindowSize( 500, 500 );
  glutInitWindowPosition( 100, 100 );
  mainWindowID = glutCreateWindow( "Assignment 1" );

  // Call our inits
  glewExperimental = GL_TRUE;
  glewInit();
  init();

  // glut callbacks
  glutDisplayFunc( display );
  glutKeyboardFunc( keyboard );

  // Create our GLUI object
  GLUI *glui = GLUI_Master.create_glui( "Controls" );
  glui->set_main_gfx_window( mainWindowID );

  // Create and initialize our levels text field
  GLUI_EditText *levelsText =
    glui->add_edittext( "Levels:", GLUI_EDITTEXT_INT, &recursiveLevels, -1, recursion_cb );
  levelsText->set_int_limits( 0, 5, GLUI_LIMIT_CLAMP );

  // Create and initialize our color panel
  GLUI_Panel *color_panel = glui->add_panel( "Color" );
  GLUI_RadioGroup *colorGroup =
    glui->add_radiogroup_to_panel( color_panel, &color, -1, color_cb );
  glui->add_radiobutton_to_group( colorGroup, "Red" );
  glui->add_radiobutton_to_group( colorGroup, "Green" );
  glui->add_radiobutton_to_group( colorGroup, "Blue" );

  // Add reset button
  glui->add_button( "Reset", 0, reset_cb );

  GLUI_Master.set_glutIdleFunc( myGlutIdle );
  GLUI_Master.auto_set_viewport();

  glutMainLoop();

  return 0;
}

// GLUI callbacks
void color_cb( int ID )
{
  switch( color ) {
  case 0:
    fColor = vec4( 1.0, 0.0, 0.0, 1.0 );
    break;
  case 1:
    fColor = vec4( 0.0, 1.0, 0.0, 1.0 );
    break;
  case 2:
    fColor = vec4( 0.0, 0.0, 1.0, 1.0 );
    break;
  }
}

void recursion_cb( int ID )
{
  Index = 0;

  // Recalculate the fractals at the specified level
  fractal( recursiveLevels );

  // Resend the geometry
  glBufferData( GL_ARRAY_BUFFER, sizeof(points), points, GL_DYNAMIC_DRAW );

  // Redraw
  if ( glutGetWindow() != mainWindowID )
    glutSetWindow( mainWindowID);
  glutPostRedisplay();
}

void reset_cb( int ID )
{
  seed = time( NULL );

  recursion_cb( 0 );
}

void myGlutIdle( void )
{
  if ( glutGetWindow() != mainWindowID )
    glutSetWindow( mainWindowID);
  glutPostRedisplay();
}

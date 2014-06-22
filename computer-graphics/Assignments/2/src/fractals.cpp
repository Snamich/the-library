#include "fractals.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

vec3 points[4000];
vec3 normals[4000];
int mainWindowID, PointsIndex = 0, NormalIndex = 0, seed;

// lighting variables
vec4 materialColor = vec4( 1.0, 0.0, 0.0, 1.0 );
GLuint materialColorLoc, ambientLoc, specularLoc, elevationLoc;
color4 ambient_product;
color4 specular_product;

// GLUI live variables
int recursiveLevels = 2, color, ambient = 0, specular = 1, elevation = 1, shading;

// Starting geometry
vec3 vertices[3] = {
  vec3( -1.5, -1.0, 0.0 ), vec3( 1.5, -1.0, 0.0 ), vec3( 0.0, 1.0, 0.0 )
};

void triangle( const vec3& a, const vec3& b, const vec3& c )
{
  // Add geometry
  points[PointsIndex++] = a;
  points[PointsIndex++] = b;
  points[PointsIndex++] = c;

  // Add normal
  vec3 u = a - c;
  vec3 v = b - c;
  vec3 n = cross( u, v );
  normals[NormalIndex++] = n;
  normals[NormalIndex++] = n;
  normals[NormalIndex++] = n;
}

vec3 getMidpoint( const vec3& a, const vec3& b, float clamp )
{
  float displacementAmount;
  int displacementSign;

  displacementSign = rand() & 1;
  displacementAmount = (rand() / (double)RAND_MAX) / clamp;
  displacementAmount *= displacementSign < 1 ? 1 : 1;
  vec3 midpoint = (a + b) / 2.0;
  midpoint[2] += displacementAmount;

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
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // Pass the color into the shader
  glUniform4fv( materialColorLoc, 1, materialColor );

  glDrawArrays( GL_TRIANGLES, 0, PointsIndex );

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
  GLuint vao;
  glGenVertexArrays( 1, &vao );
  glBindVertexArray( vao );

  // Create and initialize a buffer object
  GLuint buffer;
  glGenBuffers( 1, &buffer );
  glBindBuffer( GL_ARRAY_BUFFER, buffer );
  glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals), NULL, GL_STATIC_DRAW );
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(normals), normals );

  // Load shaders and use the resulting shader program
  GLuint program = InitShader( "shaders/vshader.glsl", "shaders/fshader.glsl" );
  glUseProgram( program );

  // Initialize the vertex position attribute from the vertex shader
  GLuint loc = glGetAttribLocation( program, "vPosition" );
  glEnableVertexAttribArray( loc );
  glVertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, 0,
                         BUFFER_OFFSET(0) );

  GLuint vNormal = glGetAttribLocation( program, "vNormal" );
  glEnableVertexAttribArray( vNormal );
  glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0,
                         BUFFER_OFFSET(sizeof(points)) );

  // Initialize lighting parameters
  point4 light_position( 0.0, 0.0, 1.0, 0.0 );
  color4 light_ambient( 0.2, 0.2, 0.2, 1.0 );
  color4 light_diffuse( 1.0, 1.0, 1.0, 1.0 );
  color4 light_specular( 1.0, 1.0, 1.0, 1.0 );

  // Material properties
  color4 material_ambient( 1.0, 0.0, 1.0, 1.0 );
  color4 material_specular( 1.0, 0.8, 0.0, 1.0 );
  float  material_shininess = 100.0;
  materialColorLoc = glGetUniformLocation( program, "MaterialColor" );
  glUniform4fv( materialColorLoc, 1, materialColor );

  // Calculate products
  ambient_product = light_ambient * material_ambient;
  specular_product = light_specular * material_specular;

  // Pass in light products
  ambientLoc = glGetUniformLocation( program, "AmbientProduct" );
  specularLoc = glGetUniformLocation( program, "SpecularProduct" );
  elevationLoc = glGetUniformLocation( program, "Elevation" );
  glUniform4fv( ambientLoc, 1, color4( 0.0, 0.0, 0.0, 1.0 ) );
  glUniform4fv( glGetUniformLocation( program, "Diffuse" ),
                1, light_diffuse );
  glUniform4fv( specularLoc, 1, specular_product );
  glUniform4fv( glGetUniformLocation( program, "LightPosition" ),
                1, light_position );
  glUniform1f( glGetUniformLocation( program, "Shininess" ),
               material_shininess );
  glUniform1i( elevationLoc, elevation );

  // Set our vertex projection matrix
  mat4 orthoProjection = Ortho( -3.0, 3.0, -3.0, 3.0, -1.0, 1.0 );
  loc = glGetUniformLocation( program, "Projection" );
  glUniformMatrix4fv( loc, 1, GL_TRUE, orthoProjection );

  // Set up our camera
  vec4 eye( 0.0, 1.0, 0.25, 1.0 );
  vec4 at( 0.0, 0.0, 0.0, 1.0 );
  vec4 up( 0.0, 0.0, 1.0, 0.0 );
  mat4 N = LookAt( eye, at, up );
  glUniformMatrix4fv( glGetUniformLocation( program, "ModelView" ),
                      1, GL_TRUE, N );

  glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background

  glEnable( GL_DEPTH_TEST );
}

int main( int argc, char *argv[] )
{
  // Set glut up
  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_RGBA );
  glutInitWindowSize( 500, 500 );
  glutInitWindowPosition( 100, 100 );
  mainWindowID = glutCreateWindow( "Assignment 2" );

  // Call our inits
  glewExperimental = GL_TRUE;
  glewInit();
  init();

  // glut callbacks
  glutDisplayFunc( display );
  glutKeyboardFunc( keyboard );

  // Create our GLUI object
  GLUI *glui = GLUI_Master.create_glui_subwindow( mainWindowID, GLUI_SUBWINDOW_BOTTOM );
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

  // Add lighting options
  glui->add_column( true );
  glui->add_checkbox( "Ambient", &ambient, 1, lighting_cb );
  glui->add_checkbox( "Specular", &specular, 2, lighting_cb );
  glui->add_checkbox( "Elevation Coloring", &elevation, 3, lighting_cb );
  GLUI_Panel *shading_panel = glui->add_panel( "Shading" );
  GLUI_RadioGroup *shadingGroup =
    glui->add_radiogroup_to_panel( shading_panel, &shading, -1, shading_cb );
  glui->add_radiobutton_to_group( shadingGroup, "Flat" );
  glui->add_radiobutton_to_group( shadingGroup, "Smooth" );
  glui->sync_live();


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
    materialColor = vec4( 1.0, 0.0, 0.0, 1.0 );
    break;
  case 1:
    materialColor = vec4( 0.0, 1.0, 0.0, 1.0 );
    break;
  case 2:
    materialColor = vec4( 0.0, 0.0, 1.0, 1.0 );
    break;
  }
}

void lighting_cb( int ID )
{
  switch( ID ) {
  case 1:
    if( ambient == 0 ) {
      glUniform4fv( ambientLoc, 1, color4( 0.0, 0.0, 0.0, 1.0 ) );
    } else {
      glUniform4fv( ambientLoc, 1, ambient_product );
    }
    break;
  case 2:
    if( specular == 0 ) {
      glUniform4fv( specularLoc, 1, color4( 0.0, 0.0, 0.0, 1.0 ) );
    } else {
      glUniform4fv( specularLoc, 1, specular_product );
    }
    break;
  case 3:
    if( elevation == 0 ) {
      glUniform1i( elevationLoc, 0 );
    } else {
      glUniform1i( elevationLoc, 1 );
    }
    break;
  }
}

void recursion_cb( int ID )
{
  PointsIndex = 0;
  NormalIndex = 0;

  // Recalculate the fractals at the specified level
  fractal( recursiveLevels );

  // Resend the geometry
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(normals), normals );

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

void shading_cb( int ID )
{

}

void myGlutIdle( void )
{
  if ( glutGetWindow() != mainWindowID )
    glutSetWindow( mainWindowID);
  glutPostRedisplay();
}

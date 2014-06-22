#include "fractals.h"
#include "Mesh.cpp"

typedef Angel::vec4 color4;
typedef Angel::vec4 point4;

std::vector<vec3> points;
std::vector<vec3> normals;
std::vector<Mesh> Models;
int mainWindowID, seed;
mat4 orthoProjection, N, M;
GLuint transformationLoc;

// lighting variables
vec4 materialColor = vec4( 1.0, 0.0, 0.0, 1.0 );
GLuint materialColorLoc, ambientLoc, specularLoc, elevationLoc;
color4 ambient_product;
color4 specular_product;

// GLUI live variables
int recursiveLevels = 2, color, ambient = 0, specular = 1, elevation = 1, shading, wireframe = 0;
GLfloat scale[1], translationX[1], translationY[1], translationZ[1];
GLfloat rotation[16] = { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
                         0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };

// Starting geometry
vec3 vertices[3] = {
  vec3( -1.5, -1.0, 0.0 ), vec3( 1.5, -1.0, 0.0 ), vec3( 0.0, 1.0, 0.0 )
};

void triangle( const vec3& a, const vec3& b, const vec3& c )
{
  // Add geometry
  points.push_back( a );
  points.push_back( b );
  points.push_back( c );

  // Add normal
  vec3 u = a - c;
  vec3 v = b - c;
  vec3 n = cross( u, v );
  normals.push_back( n );
  normals.push_back( n );
  normals.push_back( n );
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

void init()
{
  // Initial data setup
  seed = time( NULL );
  fractal( recursiveLevels );

  Mesh woman;
  woman.LoadMesh( "woman" );
  woman.M = Scale( 0.1, 0.1, 0.1 );
  Models.push_back( woman );
  
  Mesh terrain;
  terrain.LoadMesh( points, normals );
  Models.push_back( terrain );

  // Load shaders and use the resulting shader program
  GLuint program = InitShader( "shaders/vshader.glsl", "shaders/fshader.glsl" );
  glUseProgram( program );

  transformationLoc = glGetUniformLocation( program, "Transformation" );

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
  orthoProjection = Ortho( -5.0, 5.0, -5.0, 5.0, -5.0, 5.0 );
  GLuint loc = glGetUniformLocation( program, "Projection" );
  glUniformMatrix4fv( loc, 1, GL_TRUE, orthoProjection );

  // Set up our camera
  vec4 eye( 0.0, 1.0, 0.25, 1.0 );
  vec4 at( 0.0, 0.0, 0.0, 1.0 );
  vec4 up( 0.0, 0.0, 1.0, 0.0 );
  N = LookAt( eye, at, up );
  glUniformMatrix4fv( glGetUniformLocation( program, "ModelView" ),
                      1, GL_TRUE, N );

  glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background

  glEnable( GL_DEPTH_TEST );
}

int main( int argc, char *argv[] )
{
  // Set glut up
  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH );
  glutInitWindowSize( 500, 500 );
  glutInitWindowPosition( 100, 100 );
  mainWindowID = glutCreateWindow( "Assignment 3" );

  // Call our inits
  glewExperimental = GL_TRUE;
  glewInit();
  init();

  // glut callbacks
  glutDisplayFunc( display );
  glutKeyboardFunc( keyboard );
  glutMouseFunc( mouse_cb );

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
  glui->add_button( "Add Model", 0, addModel_cb );

  // Add lighting options
  glui->add_column( true );
  glui->add_checkbox( "Ambient", &ambient, 1, lighting_cb );
  glui->add_checkbox( "Specular", &specular, 2, lighting_cb );
  glui->add_checkbox( "Elevation Coloring", &elevation, 3, lighting_cb );
  glui->add_checkbox( "Wireframe", &wireframe, -1, lighting_cb );
  GLUI_Panel *shading_panel = glui->add_panel( "Shading" );
  GLUI_RadioGroup *shadingGroup =
    glui->add_radiogroup_to_panel( shading_panel, &shading, -1, shading_cb );
  glui->add_radiobutton_to_group( shadingGroup, "Flat" );
  glui->add_radiobutton_to_group( shadingGroup, "Smooth" );
  glui->sync_live();

  // Translation controls
  glui->add_column( true );
  glui->add_rotation( "Rotation", rotation, 0, transformation_cb );
  GLUI_Translation *glui_translation;
  glui_translation = glui->add_translation( "Scale", GLUI_TRANSLATION_Y, scale, 1, transformation_cb );
  glui_translation->set_speed( 0.1 );
  glui->add_column( true );
  glui_translation = glui->add_translation( "TranslationX", GLUI_TRANSLATION_X, translationX, 2, transformation_cb );
  glui_translation->set_speed( 0.1 );
  glui_translation = glui->add_translation( "TranslationY", GLUI_TRANSLATION_Y, translationY, 3, transformation_cb );
  glui_translation->set_speed( 0.1 );
  glui_translation = glui->add_translation( "TranslationZ", GLUI_TRANSLATION_Z, translationZ, 4, transformation_cb );
  glui_translation->set_speed( 0.1 );

  GLUI_Master.set_glutIdleFunc( myGlutIdle );
  GLUI_Master.auto_set_viewport();

  glutMainLoop();

  return 0;
}

// glut callbacks
void display()
{
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  for( int i = 0; i < Models.size(); ++i ) {
    if( Models[i].Selected ) {
      glUniform4fv( materialColorLoc, 1, color4( 0.0, 0.0, 0.0, 1.0 )  );
    } else {
      glUniform4fv( materialColorLoc, 1, Models[i].GetMaterialColor()  );
    }
    
    glUniformMatrix4fv( transformationLoc, 1, GL_TRUE, Models[i].M );
    Models[i].Render();
  }

  glFlush();
}

void keyboard( unsigned char key, int x, int y )
{

}

void mouse_cb( int button, int state, int x, int y )
{
  if( state != GLUT_DOWN )
    return;
  if( button == GLUT_RIGHT_BUTTON )
    exit( 0 );

  vec3 mousePosition = GetOGLPos( x, y );

  for( int i = 0; i < Models.size(); ++i ) {
    if( Models[i].IntersectP( mousePosition ) ) {
      Models[i].Selected = true;
      break;
    } else {
      Models[i].Selected = false;
    }
  }

  glutPostRedisplay();
}

// GLUI callbacks
void addModel_cb( int ID )
{
  Mesh newModel;
  newModel.LoadMesh( "woman" );
  newModel.M = Scale( 0.1, 0.1, 0.1 );
  Models.push_back( newModel );
}

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
  points.clear();
  normals.clear();

  // Recalculate the fractals at the specified level
  fractal( recursiveLevels );

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

void transformation_cb( int ID )
{
  float s = 1.0;
  for( int i = 0; i < Models.size(); ++i ) {
    if( Models[i].Selected ) {
      switch( ID ) {
      case 0:
        Models[i].M *= mat4( rotation[0], rotation[1], rotation[2], rotation[3],
                             rotation[4], rotation[5], rotation[6], rotation[7],
                             rotation[8], rotation[9], rotation[10], rotation[11],
                             rotation[12], rotation[13], rotation[14], rotation[15] );
        break;
      case 1: // scale
        s = fabs( scale[0] );
        if( s <= 0.001 )
          s = 1.0;
        Models[i].M *= Scale( s, s, s );
        break;
      case 2: // x
        Models[i].M *= Translate( translationX[0], 0.0, 0.0 );
        break;
      case 3: // y
        Models[i].M *= Translate( 0.0, translationY[0], 0.0 );
        break;
      case 4: // z
        Models[i].M *= Translate( 0.0, 0.0, translationZ[0] );
        break;
      }
    }
  }
}

void myGlutIdle( void )
{
  if ( glutGetWindow() != mainWindowID )
    glutSetWindow( mainWindowID);
  glutPostRedisplay();
}

// Misc functions
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

#include "common/Angel.h"
#include "Prog3.h"
#include "Fractal.cpp"
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

int mainWindowID;
mat4 orthoProjection, M;
GLuint transformationLoc;

// lighting variables
vec4 materialColor = vec4( 1.0, 0.0, 0.0, 1.0 );
GLuint materialColorLoc, ambientLoc, specularLoc, elevationLoc;
color4 ambient_product;
color4 specular_product;

// GLUI live variables
int recursiveLevels = 2, color, ambient = 0, specular = 1, elevation = 1, shading, wireframe = 0;
GLfloat scale = 1.0;
GLfloat translationX[1] = { 0 }, translationY[1] = { 0 }, translationZ[1] = { 0 };
GLfloat rotation[16] = { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
                         0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };

std::vector<Mesh> Models;
Fractal Terrain;
int Selected = -1;

//----------------------------------------------------------------------------
// Camera functions

void camera_look_around()
{
  float i = -90.0;
  struct timeval t1, t2;
  gettimeofday( &t1, NULL );
  float radius = length( eye - at );

  vec4 t = at;

  while( i < 270.0 ) {
    gettimeofday( &t2, NULL );
    float timeDelta = (t2.tv_usec - t1.tv_usec) / 1000.0;

    if( timeDelta < 0.0 ) timeDelta = 0.0;

    gettimeofday( &t1, NULL );
    float theta = i * DegreesToRadians;

    at = vec4( eye.x + radius * cos(theta), t.y, eye.z + radius * sinf(theta), 1.0 );
    mat4 N = LookAt( eye, at, up );
    glUniformMatrix4fv( ModelViewLoc, 1, GL_TRUE, N );

    display_cb();

    i += 0.1 * timeDelta;
  }
}

void camera_orbit( vec4 target )
{
  float i = 90.0;
  struct timeval t1, t2;
  gettimeofday( &t1, NULL );
  float radius = length( eye - target );

  while( i < 450.0 ) {
    gettimeofday( &t2, NULL );
    float timeDelta = (t2.tv_usec - t1.tv_usec) / 1000.0;

    if( timeDelta < 0.0 ) timeDelta = 0.0;

    gettimeofday( &t1, NULL );
    float theta = i * DegreesToRadians;

    eye = vec4( target.x + radius * cosf(theta), eye.y, target.z + radius * sinf(theta), 1.0 );
    mat4 N = LookAt( eye, target, up );
    glUniformMatrix4fv( ModelViewLoc, 1, GL_TRUE, N );

    display_cb();

    i += 0.1 * timeDelta;
  }
}

void camera_reset()
{
  eye = vec4( 0.0, 0.0, 5.0, 1.0 );
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

  for( int i = 0; i < Models.size(); ++i ) {
    if( Models[i].Selected ) {
      glUniform4fv( materialColorLoc, 1, color4( 0.0, 1.0, 1.0, 1.0 )  );
    } else if( color == 0 ) {
      glUniform4fv( materialColorLoc, 1, Models[i].GetMaterialColor()  );
    } else {
      glUniform4fv( materialColorLoc, 1, materialColor );
    }
    
    glUniformMatrix4fv( transformationLoc, 1, GL_TRUE, Models[i].M );
    Models[i].Render();

    if( wireframe ) {
      Models[i].BB.Render();
    }
  }
  
  glUniformMatrix4fv( transformationLoc, 1, GL_TRUE, mat4() );

  if( elevation ) {
    glUniform1i( elevationLoc, 1 );
    Terrain.Render();
    glUniform1i( elevationLoc, 0 );
  } else {
    Terrain.Render();
  }
  
  glFlush();
}

void glut_idle_cb()
{
  if ( glutGetWindow() != mainWindowID )
    glutSetWindow( mainWindowID );
  glutPostRedisplay();
}

void keyboard_cb( unsigned char key, int x, int y )
{
  switch ( key ) {
  case 033:
    exit( EXIT_SUCCESS );
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
    vec3 mousePosition = GetOGLPos( x, y );

    for( int i = 0; i < Models.size(); ++i ) {
      if( Models[i].IntersectP( mousePosition ) ) {
        Models[i].Selected = true;
        Models[Selected].Selected = false;
        Selected = i;
        glutPostRedisplay();
        return;
      } else {
        Models[i].Selected = false;
      }
    }

    Selected = -1;
    CAMERA_STATE = PAN;
  }
}

//----------------------------------------------------------------------------
// GLUI callbacks

void addModel_cb( int ID )
{
  Mesh newModel;
  newModel.LoadMesh( "woman" );
  newModel.M = Scale( 0.03, 0.03, 0.03 );
  Models.push_back( newModel );
}

void camera_cb( int ID )
{
  switch( ID ) {
  case 0: // look around
    camera_look_around();
    break;
  case 1: // orbit
    if( Selected >= 0 ) camera_orbit( Models[Selected].GetCenter() );
    break;
  case 2:
    camera_reset();
    break;
  }
}

void color_cb( int ID )
{
  switch( color ) {
  case 1:
    materialColor = color4( 1.0, 0.0, 0.0, 1.0 );
    break;
  case 2:
    materialColor = color4( 0.0, 1.0, 0.0, 1.0 );
    break;
  case 3:
    materialColor = color4( 0.0, 0.0, 1.0, 1.0 );
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
  }
}

void recursion_cb( int ID )
{
  // Recalculate the fractals at the specified level
  Terrain.SetLevel( recursiveLevels );

  // Redraw
  if ( glutGetWindow() != mainWindowID )
    glutSetWindow( mainWindowID);
  glutPostRedisplay();
}

void reset_cb( int ID )
{
  Terrain.Reset();
}

void shading_cb( int ID )
{

}

void transformation_cb( int ID )
{
  if( Selected >= 0 ) {
    switch( ID ) {
    case 0:
      Models[Selected].M *= mat4( rotation[0], rotation[1], rotation[2], rotation[3],
                                  rotation[4], rotation[5], rotation[6], rotation[7],
                                  rotation[8], rotation[9], rotation[10], rotation[11],
                                  rotation[12], rotation[13], rotation[14], rotation[15] );
      break;
    case 1: // scale
      Models[Selected].M *= Scale( scale, scale, scale );
      break;
    case 2: // x
      Models[Selected].M *= Translate( translationX[0], 0.0, 0.0 );
      break;
    case 3: // y
      Models[Selected].M *= Translate( 0.0, translationY[0], 0.0 );
      break;
    case 4: // z
      Models[Selected].M *= Translate( 0.0, 0.0, translationZ[0] );
      break;
    }
  }

  display_cb();
}

//----------------------------------------------------------------------------
// Main and init

int main( int argc, char **argv )
{
  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH );
  glutInitWindowSize( 500, 500 );
  glutInitWindowPosition( 100, 100 );

  mainWindowID = glutCreateWindow( "Assignment 4" );
  glewExperimental = GL_TRUE;
  glewInit();
  init();

  glutDisplayFunc( display_cb );
  glutKeyboardFunc( keyboard_cb );
  glutMotionFunc( motion_cb );
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
  glui->add_radiobutton_to_group( colorGroup, "Material" );
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

  // Camera buttons
  glui->add_statictext( "Camera Controls" );
  glui->add_button( "Look Around", 0, camera_cb );
  glui->add_button( "Orbit", 1, camera_cb );
  glui->add_button( "Default", 2, camera_cb );

  // Transformation controls
  glui->add_column( true );
  GLUI_Translation *glui_translation;
  glui_translation = glui->add_translation( "TranslationX", GLUI_TRANSLATION_X, translationX, 2, transformation_cb );
  glui_translation->set_speed( 0.1 );
  glui_translation = glui->add_translation( "TranslationY", GLUI_TRANSLATION_Y, translationY, 3, transformation_cb );
  glui_translation->set_speed( 0.1 );
  glui->add_separator();
  GLUI_Spinner *scaler = glui->add_spinner( "Scale", GLUI_SPINNER_FLOAT, &scale, 1, transformation_cb );
  scaler->set_float_limits( 0.1, 2.0, GLUI_LIMIT_CLAMP );
  scaler->set_speed( 0.1 );
  glui->add_column( false );
  glui_translation = glui->add_translation( "TranslationZ", GLUI_TRANSLATION_Z, translationZ, 4, transformation_cb );
  glui_translation->set_speed( 0.1 );
  glui->add_rotation( "Rotation", rotation, 0, transformation_cb );

  GLUI_Master.set_glutIdleFunc( glut_idle_cb );
  GLUI_Master.auto_set_viewport();

  glutMainLoop();
  return 0;
}

void init()
{
  // Read model in
  Mesh woman;
  woman.LoadMesh( "woman" );
  woman.M = Scale( 0.03, 0.03, 0.03 );
  Models.push_back( woman );

  Terrain.CalculateGeometry();

  // Load shaders and use the resulting shader program
  GLuint program = InitShader( "shaders/vshader.glsl", "shaders/fshader.glsl" );
  glUseProgram( program );

  transformationLoc = glGetUniformLocation( program, "Transformation" );

  // Initialize shader lighting parameters
  point4 light_position( 0.0, 2.0, 0.0, 0.0 );
  color4 light_ambient( 0.5, 0.5, 0.5, 1.0 );
  color4 light_diffuse( 1.0, 1.0, 1.0, 1.0 );
  color4 light_specular( 1.0, 1.0, 1.0, 1.0 );

  // Material properties
  color4 material_ambient( 1.0, 0.0, 1.0, 1.0 );
  color4 material_diffuse( 0.0, 0.0, 1.0, 1.0 );
  color4 material_specular( 1.0, 0.8, 0.0, 1.0 );
  float  material_shininess = 100.0;

  materialColorLoc = glGetUniformLocation( program, "MaterialColor" );
  glUniform4fv( materialColorLoc, 1, materialColor );

  // Calculate products
  ambient_product = light_ambient * material_ambient;

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

  // Set our vertex projection matrix
  orthoProjection = Perspective( 45.0, 1.0, 5.0, -5.0 );
  GLuint loc = glGetUniformLocation( program, "Projection" );
  glUniformMatrix4fv( loc, 1, GL_TRUE, orthoProjection );

  // Set up our camera
  eye = vec4( 0.0, 0.0, 5.0, 1.0 );
  at = vec4( 0.0, 0.0, 0.0, 1.0 );
  up = vec4( 0.0, 1.0, 0.0, 0.0 );
  mat4 N = LookAt( eye, at, up );
  ModelViewLoc = glGetUniformLocation( program, "ModelView" );
  glUniformMatrix4fv( ModelViewLoc, 1, GL_TRUE, N );

  glClearColor( 0.0, 0.0, 0.0, 1.0 ); // black background

  glEnable( GL_DEPTH_TEST );
  glDepthMask( GL_TRUE );
  glDepthFunc( GL_LESS ); 
  glDepthRange(-5.0f, 5.0f);
}

// Misc functions
vec3 GetOGLPos( int x, int y )
{
  GLint viewport[4];
  GLdouble modelview[16];
  GLdouble projection[16];
  GLfloat winX, winY, winZ;
  GLdouble posX, posY, posZ;

  mat4 N = LookAt( eye, at, up );
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

  gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ );
 
  return vec3( posX, posY, posZ );
}

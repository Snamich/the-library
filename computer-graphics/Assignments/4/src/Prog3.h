#include "common/Angel.h"

// Camera methods
void camera_look_around();
void camera_orbit( vec4& );
void camera_reset();

// GLUT callbacks
void display_cb();
void keyboard_cb( unsigned char, int, int );
void motion_cb( int, int );
void mouse_cb( int, int, int, int );

// GLUI callbacks
void addModel_cb( int );
void camera_cb( int );
void color_cb( int );
void lighting_cb( int );
void recursion_cb( int );
void reset_cb( int );
void shading_cb( int );
void transformation_cb( int );
void myGlutIdle();

// Init methods
int main( int, char** );
void init();

// Misc
vec3 GetOGLPos( int, int );

#include <stdlib.h>
#include <vector>
#include "common/Angel.h"

// Fractal methods
void triangle( const vec3&, const vec3&, const vec3& );
void fractal( int );
void fractalLeft( const vec3&, const vec3&, const vec3&,
                  const vec3&, float, int );
void fractalRight( const vec3&, const vec3&, const vec3&,
                   const vec3&, float, int );
void fractalTop( const vec3&, const vec3&, const vec3&,
                 const vec3&, float, int );
void fractalOuter( const vec3&, const vec3&, const vec3&, float, int );
void fractalInner( const vec3&, const vec3&, const vec3&,
                   const vec3&, const vec3&, const vec3&, float, int );
void fractalRecurs( const vec3&, const vec3&, const vec3&,
                    const vec3&, const vec3&, const vec3&, float, int );
vec3 getMidpoint( const vec3&, const vec3&, float );

// glut callbacks
void display();
void keyboard( unsigned char, int, int );
void mouse_cb( int, int, int, int );

// Init methods
int main( int, char** );
void init();

// GLUI callbacks
void addModel_cb( int );
void color_cb( int );
void lighting_cb( int );
void recursion_cb( int );
void reset_cb( int );
void shading_cb( int );
void transformation_cb( int );
void myGlutIdle();

// Misc
vec3 GetOGLPos( int, int );

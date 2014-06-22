#include "common/Angel.h"

//----------------------------------------------------------------------------

void
init( void )
{
    // Specify our triangle
    vec3 vertices[3] = {
      vec3( -1.0, -1.0, 0.0 ), vec3( 1.0, -1.0, 0.0 ), vec3( 0.0, 1.5, 0.0 )
    };

    // Create a vertex array object
    GLuint vao[1];
    glGenVertexArrays( 1, vao );
    glBindVertexArray( vao[0] );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );

    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "shaders/vshader.glsl", "shaders/fshader.glsl" );
    glUseProgram( program );

    // Initialize the vertex position attribute from the vertex shader
    GLuint loc = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( loc );
    glVertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );

    // Set our vertex projection matrix
    mat4 orthoProjection = Ortho( -2.0, 2.0, -2.0, 2.0, -1.0, 1.0 );
    loc = glGetUniformLocation( program, "projection" );
    glUniformMatrix4fv( loc, 1, GL_TRUE, orthoProjection );

    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
}

//----------------------------------------------------------------------------

void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT );     // clear the window
    glDrawArrays( GL_LINE_LOOP, 0, 3 );    // draw the points
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

    glutCreateWindow( "Lab 2" );
    glewExperimental = GL_TRUE;
    glewInit();
    init();

    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );

    glutMainLoop();
    return 0;
}

uniform mat4 projection;

attribute vec4 vPosition;

void
main()
{
    gl_Position = vPosition * projection;
}



attribute vec4 vPosition;
varying vec4 color;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 LightPosition;
uniform mat4 Projection;
uniform vec4 vNormal;
uniform mat4 ModelView;

void
main()
{
  // vec3 L = normalize( LightPosition.xyz );
  //   vec3 N = vNormal.xyz;
    
  //   // Compute terms in the illumination equation
  //   vec4 ambient = AmbientProduct;

  //   float Kd = max( dot( L, N ), 0.0 );
  //   vec4 diffuse = Kd * DiffuseProduct;
    
  gl_Position = vPosition * ModelView * Projection;


    color = vec4( 1.0, 0.0, 0.0, 1.0 ); //ambient + diffuse;
    color.a = 1.0;
}

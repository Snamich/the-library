attribute vec4 vPosition;
//attribute vec4 vNormal;
varying vec4 color;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct, MaterialColor;
uniform vec4 LightPosition;
uniform mat4 Projection;
uniform mat4 ModelView;
uniform vec4 vNormal;

void
main()
{
  vec3 L = normalize( LightPosition.xyz );
  vec3 N = vNormal.xyz;
    
  // Compute terms in the illumination equation
  vec4 ambient = AmbientProduct;

  float Kd = max( dot( L, N ), 0.0 );
  vec4 diffuse = Kd * DiffuseProduct;
  
  gl_Position = Projection * (ModelView * vPosition);

  color = DiffuseProduct * MaterialColor;
  color.a = 1.0;
}

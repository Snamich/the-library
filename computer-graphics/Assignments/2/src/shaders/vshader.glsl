attribute vec4 vPosition;
attribute vec3 vNormal;
varying vec4 color;

uniform vec4 AmbientProduct, Diffuse, SpecularProduct;
uniform vec4 MaterialColor;
uniform float Shininess;
uniform int Elevation;
uniform vec4 LightPosition;
uniform mat4 Projection;
uniform mat4 ModelView;

void
main()
{
    // Transform vertex  position into eye coordinates
    vec3 pos = (ModelView * vPosition).xyz;
    
    vec3 L = normalize( (ModelView*LightPosition).xyz - pos );
    vec3 E = normalize( -pos );
    vec3 H = normalize( L + E );

    // Transform vertex normal into eye coordinates
    vec3 N = normalize( ModelView*vec4(vNormal, 0.0) ).xyz;

    // Compute terms in the illumination equation
    vec4 ambient = AmbientProduct;

    vec4 DiffuseProduct;
    if( Elevation == 1 ) {
        if( vPosition.z < 0.35 ) {
            DiffuseProduct = Diffuse * vec4( 0.0, 0.392, 0.0, 1.0 );
        } else if( vPosition.z >= 0.35 && vPosition.z <= 0.5 ) {
            DiffuseProduct = Diffuse * vec4( .627, .271, .075, 1.0 );
        } else {
            DiffuseProduct = Diffuse * vec4( 1.0, .98, .98, 1.0 );
        }
    } else {
        DiffuseProduct = Diffuse * MaterialColor;
    }  

    float Kd = max( dot( L, N ), 0.0 );
    vec4 diffuse = Kd * DiffuseProduct;

    float Ks = pow( max(dot(N, H), 0.0), Shininess );
    vec4  specular = Ks * SpecularProduct;
    
    if( dot(L, N) < 0.0 ) {
        specular = vec4(0.0, 0.0, 0.0, 1.0);
    } 
    
    gl_Position = vPosition * ModelView * Projection;

    color = ambient + diffuse + specular;
    color.a = 1.0;
}
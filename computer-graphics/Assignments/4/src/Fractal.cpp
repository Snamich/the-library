#include "Fractal.h"
#include "common/vec.h"

// Starting geometry
vec3 vertices[3] = {
  vec3( -1.5, 0.0, -1.0 ), vec3( 1.5, 0.0, -1.0 ), vec3( 0.0, 0.0, 1.0 )
};

void Fractal::CalculateGeometry()
{
  srand( seed );

  FractalRecurs( vertices[0], vertices[1], vertices[2], 1.0, level );

  for( int i = 0; i < Vertices.size(); ++i ) {
    Indices.push_back( i );
  }

  Init();
}

void Fractal::Clear()
{
  Indices.clear();
  Normals.clear();
  Vertices.clear();
}

vec3 Fractal::GetMidpoint( const vec3& a, const vec3& b, float clamp )
{
  float displacementAmount;
  int displacementSign;

  displacementSign = rand() & 1;
  displacementAmount = (rand() / (double)RAND_MAX) / clamp;
  displacementAmount *= displacementSign < 1 ? 1 : 1;
  vec3 midpoint = (a + b) / 2.0;
  midpoint[1] += displacementAmount;

  return midpoint;
}

void Fractal::Init()
{
  glGenBuffers( 1, &VB );
  glBindBuffer( GL_ARRAY_BUFFER, VB );
  glBufferData( GL_ARRAY_BUFFER, sizeof(vec3) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW );

  glGenBuffers( 1, &IB );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IB );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), &Indices[0], GL_STATIC_DRAW );
}

void Fractal::Render()
{
  glEnableVertexAttribArray( 0 );

  // Vertices
  glBindBuffer( GL_ARRAY_BUFFER, VB );
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );

  // Indices
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IB );
  glDrawElements( GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0 );

  glDisableVertexAttribArray( 0 );
}

void Fractal::Reset()
{
  seed = time( NULL );

  Clear();

  CalculateGeometry();
}

void Fractal::SetLevel( int l )
{
  level = l;

  Clear();

  CalculateGeometry();
}

void Fractal::Triangle( const vec3& a, const vec3& b, const vec3& c )
{
  // Add geometry
  Vertices.push_back( a );
  Vertices.push_back( b );
  Vertices.push_back( c );

  // Add normal
  vec3 u = a - c;
  vec3 v = b - c;
  vec3 n = normalize( cross( u, v ) );
  Normals.push_back( n );
  Normals.push_back( n );
  Normals.push_back( n );
}

void Fractal::FractalRecurs( const vec3& a, const vec3& b, const vec3& c, float clamp, int level )
{
  if( level > 0 ) {
    vec3 mid0 = GetMidpoint( a, b, clamp );
    vec3 mid1 = GetMidpoint( b, c, clamp );
    vec3 mid2 = GetMidpoint( a, c, clamp );
    FractalRecurs( a, mid0, mid2, clamp * 2, level - 1 );
    FractalRecurs( mid0, b, mid1, clamp * 2, level - 1 );
    FractalRecurs( mid2, mid1, c, clamp * 2, level - 1 );
    FractalRecurs( mid1, mid2, mid0, clamp * 2, level - 1 );
  } else {
    Triangle( a, b, c );
  }
}

Fractal::Fractal()
{
  level = 2;
  seed = time( NULL );
}

Fractal::~Fractal() {}

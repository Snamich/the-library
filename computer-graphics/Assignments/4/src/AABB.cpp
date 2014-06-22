#include "AABB.h"

vec4 AABB::GetCenter()
{
  return vec4( (maxX + minX) / 2.0, (maxY + minY) / 2.0, (maxZ + minZ) / 2.0, 1.0 );
}

vec4 AABB::GetMaterialColor()
{
  return MaterialColor;
}

void AABB::Init( GLfloat x, GLfloat X, GLfloat y, GLfloat Y, GLfloat z, GLfloat Z )
{
  minX = x;
  maxX = X;
  minY = y;
  maxY = Y;
  minZ = z;
  maxZ = Z;

  Vertices.push_back( vec3( minX, minY, minZ ) );
  Vertices.push_back( vec3( maxX, minY, minZ ) );
  Vertices.push_back( vec3( maxX, maxY, minZ ) );
  Vertices.push_back( vec3( minX, maxY, minZ ) );
  Vertices.push_back( vec3( minX, minY, maxZ ) );
  Vertices.push_back( vec3( maxX, minY, maxZ ) );
  Vertices.push_back( vec3( maxX, maxY, maxZ ) );
  Vertices.push_back( vec3( minX, maxY, maxZ ) );

  Indices.push_back(0);
  Indices.push_back(1);
  Indices.push_back(2);
  Indices.push_back(3);
  Indices.push_back(4);
  Indices.push_back(5);
  Indices.push_back(6);
  Indices.push_back(7);
  Indices.push_back(0);
  Indices.push_back(4);
  Indices.push_back(1);
  Indices.push_back(5);
  Indices.push_back(2);
  Indices.push_back(6);
  Indices.push_back(3);
  Indices.push_back(7);

  glGenBuffers( 1, &VB );
  glBindBuffer( GL_ARRAY_BUFFER, VB );
  glBufferData( GL_ARRAY_BUFFER, sizeof(vec3) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW );

  glGenBuffers( 1, &IB );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IB );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), &Indices[0], GL_STATIC_DRAW );
}

bool AABB::IntersectP( vec3& p, mat4& M )
{
  if( p.x < (M * vec4( vec3( minX ), 1.0 ) ).x || 
      p.x > (M * vec4( vec3( maxX ), 1.0 ) ).x ) return false;
  if( p.y < (M * vec4( vec3( minY ), 1.0 ) ).y || 
      p.y > (M * vec4( vec3( maxY ), 1.0 ) ).y ) return false;
  if( p.z < (M * vec4( vec3( minZ ), 1.0 ) ).z || 
      p.z > (M * vec4( vec3( maxZ ), 1.0 ) ).z ) return false;

  return true;
}

void AABB::Render()
{
  glEnableVertexAttribArray( 0 );

  glBindBuffer( GL_ARRAY_BUFFER, VB );
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );

  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IB );

  glDrawElements( GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);
  glDrawElements( GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (GLvoid*)(4 * sizeof(GL_UNSIGNED_INT)) );
  glDrawElements( GL_LINES, 8, GL_UNSIGNED_INT, (GLvoid*)(8 * sizeof(GL_UNSIGNED_INT)) );

  glDisableVertexAttribArray( 0 );
}

AABB::AABB()
{
  MaterialColor = vec4( 1.0, 0.0, 0.0, 0.0 );
}

AABB::~AABB() {}

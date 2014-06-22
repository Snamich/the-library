#ifndef AABB_H
#define AABB_H

#include <vector>

class AABB
{
 public:
  AABB();

  ~AABB();

  vec4 GetCenter();
  vec4 GetMaterialColor();
  void Init( GLfloat x, GLfloat X, GLfloat y, GLfloat Y, GLfloat z, GLfloat Z );
  bool IntersectP( vec3& p );
  void Render();

 private:
  GLuint VB, IB;
  vec4 MaterialColor;
  GLfloat minX, maxX, minY, maxY, minZ, maxZ;

  std::vector<vec3> Vertices;
  std::vector<unsigned int> Indices;
};

#endif


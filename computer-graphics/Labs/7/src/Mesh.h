#ifndef MESH_H
#define	MESH_H

#include <string>
#include <vector>
#include "AABB.h"

class Mesh
{
 public:
  Mesh();

  ~Mesh();

  vec4 GetCenter();
  vec4 GetMaterialColor();
  void LoadMesh( const std::string& meshName );
  void Render();

  AABB BB;
  bool Selected;
  mat4 M;

 private:
  void InitMesh();
  void ReadCoords( const char *filename );
  void ReadPolys( const char *filename );

  GLuint VB, IB;
  vec4 MaterialColor;

  std::vector<unsigned int> Indices;
  std::vector<vec3> Vertices;
  std::vector<vec3> Normals;
};

#endif

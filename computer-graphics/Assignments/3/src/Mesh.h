#ifndef MESH_H
#define	MESH_H

#include <string>
#include <vector>

class Mesh
{
 public:
  Mesh();

  ~Mesh();

  vec4 GetMaterialColor();
  void LoadMesh( const std::string& meshName );
  void LoadMesh( const std::vector<vec3>&, const std::vector<vec3>& );
  bool IntersectP( const vec3& p );
  void Render();

  bool Selected;
  mat4 M;

 private:
  void InitMesh();
  void ReadCoords( const char *filename );
  void ReadPolys( const char *filename );

  GLuint VB, IB;
  GLint minx, maxx, miny, maxy, minz, maxz;
  vec4 MaterialColor;

  std::vector<unsigned int> Indices;
  std::vector<vec3> Vertices;
  std::vector<vec3> Normals;
};

#endif

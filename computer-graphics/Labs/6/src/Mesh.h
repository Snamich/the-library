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
 bool IntersectP( const vec3& p );
 void Render();

 private:
 void InitMesh();
 void ReadCoords( const char *filename );
 void ReadPolys( const char *filename );

 struct MeshEntry {
   MeshEntry();

   ~MeshEntry();

   std::vector<unsigned int> Indices;
   vec3 N;
 }; 

 GLuint VB, IB;
 GLfloat minx, maxx, miny, maxy, minz, maxz;
 vec4 MaterialColor;

 std::vector<MeshEntry> m_Entries;
 std::vector<unsigned int> Indices;
 std::vector<vec3> Vertices;
 std::vector<vec3> Normals;
};

#endif

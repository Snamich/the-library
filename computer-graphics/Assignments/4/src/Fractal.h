#ifndef FRACTAL_H
#define FRACTAL_H

#include <stdlib.h>
#include <vector>

class Fractal
{
 public:
  Fractal();

  ~Fractal();

  void CalculateGeometry();
  void Render();
  void Reset();
  void SetLevel( int l );

 private:
  int level, seed;
  GLuint VB, IB;
  std::vector<vec3> Vertices;
  std::vector<unsigned int> Indices;
  std::vector<vec3> Normals;

  void Clear();
  vec3 GetMidpoint( const vec3&, const vec3&, float );
  void FractalRecurs( const vec3&, const vec3&, const vec3&, float, int );
  void Init();
  void Triangle( const vec3&, const vec3&, const vec3& );
};

#endif

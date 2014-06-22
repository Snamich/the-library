#include "Mesh.h"
#include "AABB.cpp"

void Mesh::ReadCoords( const char *filename )
{
  int     i, j, numCoords;
  float   x, y, z, minX, maxX, minY, maxY, minZ, maxZ;
  FILE    *f;

  f = fopen( filename, "r" );
  if ( !f ) {
    std::cout << "Can not open the file " <<  filename << std::endl;
    exit( -1 );
  }

  if ( fscanf(f, "%d", &numCoords ) != 1 ) {
    std::cout << "Can not read the first line of  " <<  filename << std::endl;
    exit( -1 );
  }

  printf( "NUMCOORDS %d\n", numCoords );  

  minX = maxX = minY = maxY = minZ = maxZ = 0.0;

  for( i = 0; i < numCoords; i++ ) {
    if ( fscanf( f, "%d,%f,%f,%f", &j, &x, &y, &z ) != 4 ) {
      std::cout << "Can't read from " << std::endl;
      exit( -1 );
    }

    if ( (j < 0) || (j > numCoords) ) {
      std::cout << "Illegal index" << std::endl;
      exit( -1 );
    }

    this->Vertices.push_back( vec3( x, y, z ) );
 
    if( x < minX ) minX = x;
    if( x > maxX ) maxX = x;
    if( y < minY ) minY = y;
    if( y > maxY ) maxY = y;
    if( z < minZ ) minZ = y;
    if( z > maxZ ) maxZ = y;
  }

  BB.Init( minX, maxX, minY, maxY, minZ, maxZ );

  fclose( f );

  std::cout << "Vertices size = " << this->Vertices.size() << std::endl;
}

void Mesh::ReadPolys( const char *filename )
{
  int numPolygons, pnt, result, ind;
  char c;
  FILE *f;
  int indices[20];

  std::cout << "inside read poly " << std::endl;

  f = fopen( filename, "r" );
  if ( !f ) {
    std::cout << "Can't read the file " <<  filename << std::endl;
    exit( -1 );
  }

  if ( fscanf( f, "%d", &numPolygons ) != 1 ) {
    std::cout << "Can't read first line of " <<  filename << std::endl;
    exit( -1 );
  }

  std::cout << numPolygons << " num poly" << std::endl;

  for( int i = 0; i < numPolygons; ++i ) {
    do 
      c = fgetc( f );
    while( !feof( f ) && (c != ' ') );

    ind = 0;

    do {
      result = fscanf( f, "%d", &pnt );
      if ( result ) {
        indices[ind] = pnt;
        ++ind;
      }
    }
    while( !feof( f ) && result );

    int count = 2;
    while( count < ind ) {
      Indices.push_back( indices[0] - 1 );
      Indices.push_back( indices[count - 1] - 1 );
      Indices.push_back( indices[count] - 1 );

      vec3 u = Vertices[indices[0] - 1] - Vertices[indices[count - 1] -1];
      vec3 v = Vertices[indices[count] - 1] - Vertices[indices[count - 1] -1];;
      vec3 n = cross( u, v );
      Normals.push_back( n );
      Normals.push_back( n );
      Normals.push_back( n );

      ++count;
    }

  }

  fclose( f );
  std::cout <<"Number of polygons " << numPolygons << std::endl;
}

void Mesh::LoadMesh( const std::string& meshName )
{
  ReadCoords( ( "data/" + meshName + ".coor" ).c_str() );
  std::cout << "Read coordinates file successfully." << std::endl;

  ReadPolys( ( "data/" + meshName + ".poly" ).c_str() );
  std::cout << "Read polygon file successfully." << std::endl;

  InitMesh();
}

vec4 Mesh::GetMaterialColor()
{
  return MaterialColor;
}

void Mesh::Render()
{
  glEnableVertexAttribArray( 0 );

  glBindBuffer( GL_ARRAY_BUFFER, VB );
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );

  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IB );
  glDrawElements( GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0 );

  glDisableVertexAttribArray( 0 );
}

void Mesh::InitMesh()
{
  glGenBuffers( 1, &VB );
  glBindBuffer( GL_ARRAY_BUFFER, VB );
  glBufferData( GL_ARRAY_BUFFER, sizeof(vec3) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW );

  glGenBuffers( 1, &IB );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IB );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), &Indices[0], GL_STATIC_DRAW );
}

vec4 Mesh::GetCenter()
{
  return M * BB.GetCenter();
}

Mesh::Mesh() 
{
  MaterialColor = vec4( 0.0, 0.0, 1.0, 1.0 );   
  Selected = false;
  M = mat4( 1.0 );
  BB = AABB();
}

Mesh::~Mesh() 
{
  Vertices.clear();
  Indices.clear();
  Normals.clear();
}

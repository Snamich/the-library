#include <vector>

void readCoords( const char *filename, std::vector<Angel::vec3> *verts )
{
  int     i, j, numCoords;
  float   x, y, z;
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

  for( i = 0; i < numCoords; i++ ) {
    if ( fscanf( f, "%d,%f,%f,%f", &j, &x, &y, &z ) != 4 ) {
      std::cout << "Can't read from " << std::endl;
      exit( -1 );
    }

    if ( (j < 0) || (j > numCoords) ) {
      std::cout << "Illegal index" << std::endl;
      exit( -1 );
    }

    verts->push_back( vec3( x, y, z ) );
  }

  fclose( f );

  std::cout << "verts size = " << verts->size() << std::endl;
}

// read .poly files
void readPolygons( const char *filename, std::vector<Angel::vec3> *verts,
                   std::vector<Angel::vec3> *data )
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
      data->push_back( verts->at( indices[0] - 1 ) );
      data->push_back( verts->at( indices[count - 1] - 1 ) );
      data->push_back( verts->at( indices[count] - 1 ) );
      ++count;
    }

  }

  fclose( f );
  std::cout <<" Number of polygons " << numPolygons << std::endl;
}


void loadModel( const char *coordinates, const char *polygons, 
                std::vector<Angel::vec3> *verts, std::vector<Angel::vec3> *data )
{
  readCoords( coordinates, verts );
  std::cout << "Read coordinates file successfully." << std::endl;

  readPolygons( polygons, verts, data );
  std::cout << "Read polygon file successfully." << std::endl;
}

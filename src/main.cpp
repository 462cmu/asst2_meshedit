#include "CMU462/CMU462.h"

#include "collada.h"
#include "meshEdit.h"

#include <iostream>

using namespace std;
using namespace CMU462;

#define msg(s) cerr << "[Collada Viewer] " << s << endl;

int loadFile( MeshEdit* collada_viewer, const char* path ) {

  Scene* scene = new Scene();

  if( ColladaParser::load( path, scene ) < 0) {
    delete scene;
    return -1;
  }

  collada_viewer->load( scene );
  return 0;
}

int main( int argc, char** argv ) {

  // create viewer
  Viewer viewer = Viewer();

  // create collada_viewer
  MeshEdit* collada_viewer = new MeshEdit();

  // set collada_viewer as renderer
  viewer.set_renderer(collada_viewer);

  // init viewer
  viewer.init();

  // load tests
  if( argc == 2 ) {
    if (loadFile(collada_viewer, argv[1]) < 0) exit(0);
  } else {
    msg("Usage: collada_viewer <path to scene file>"); exit(0);
  }

  // start viewer
  viewer.start();

  return 0;
}

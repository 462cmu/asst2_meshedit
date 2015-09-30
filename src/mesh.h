#ifndef CMU462_MESH_H
#define CMU462_MESH_H

#include "scene.h"
#include "material.h"

namespace CMU462 {

struct Polygon {

  std::vector<size_t> vertex_indices;    ///< 0-based indices into vertex array
  std::vector<size_t> normal_indices;    ///< 0-based indices into normal array
  std::vector<size_t> texcoord_indices;  ///< 0-based indices into texcoord array

}; // struct Polygon

typedef std::vector<Polygon> PolyList;
typedef PolyList::iterator PolyListIter;

struct Polymesh : Instance {

  std::string id;
  std::string name;

  std::vector<Vector3D> vertices;   ///< polygon vertex array
  std::vector<Vector3D> normals;    ///< polygon normal array
  std::vector<Vector2D> texcoords;  ///< texture coordinate array

  PolyList  polygons;   ///< polygons

  Material* material;  ///< material of the mesh

}; // struct Polymesh

std::ostream& operator<<( std::ostream& os, const Polymesh& polymesh );

} // namespace CMU462

#endif // CMU462_MESH_H

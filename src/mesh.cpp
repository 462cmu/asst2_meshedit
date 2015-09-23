#include "mesh.h"

using namespace std;

namespace CMU462 {

std::ostream& operator<<( std::ostream& os, const Polymesh& polymesh ) {

  os << "Polymesh: " << polymesh.name << " (id:" << polymesh.id << ")";

  os << " [";

    os << " num_polygons="  << polymesh.polygons.size();
    os << " num_vertices="  << polymesh.vertices.size();
    os << " num_normals="   << polymesh.normals.size();
    os << " num_texcoords=" << polymesh.texcoords.size();

  os << " ]";

  return os;

}

} // namespace CMU462

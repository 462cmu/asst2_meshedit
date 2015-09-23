#include "camera.h"

using namespace std;

namespace CMU462 {

std::ostream& operator<<( std::ostream& os, const Camera& camera ) {

  os << "Camera: " << camera.name << " (id: " << camera.id << ")";

  os << " [";

  os << " hfov="  << camera.hfov;
  os << " vfov="  << camera.vfov;
  os << " nclip=" << camera.nclip;
  os << " fclip=" << camera.fclip;

  os << " ]";

  return os;
}


} // namespace CMU462


#include "camera.h"

using namespace std;

namespace CMU462 {

Camera::Camera() {

  hfov = 50;
  vfov = 35;

  nclip = 0.1;
  fclip = 100;

  position = Vector3D(0,  0,  0);  // COLLADA defaults (Spec v1.4 page 54)
  view_dir = Vector3D(0,  0, -1);  // COLLADA defaults (Spec v1.4 page 54)
  up_dir   = Vector3D(0,  1,  0);  // COLLADA defaults (Spec v1.4 page 54) 

}

std::ostream& operator<<( std::ostream& os, const Camera& camera ) {

  os << "Camera: " << camera.name << " (id: " << camera.id << ")";

  os << " [";

  os << " position="  << camera.position;

  os << " view_dir=" << camera.view_dir;
  os << " up_dir="   << camera.up_dir;

  os << " hfov="  << camera.hfov;
  os << " vfov="  << camera.vfov;
  os << " nclip=" << camera.nclip;
  os << " fclip=" << camera.fclip;

  os << " ]";

  return os;
}


} // namespace CMU462


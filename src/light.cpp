#include "light.h"

using namespace std;

namespace CMU462 {

std::ostream& operator<<( std::ostream& os, const Light& light ) {

  os << "Light: " << light.name << " (id:" << light.id << ")";

  os << " [";

    switch (light.light_type) {
      case AMBIENT:
        os << " type=ambient";
        break;
      case POINT:
        os << " type=point";
        os << " attenuation=" << light.attenuation;
        break;
      case DIRECTIONAL:
        os << " type=directional";
        break;
    }

    os << " r=" << light.color.r;
    os << " g=" << light.color.g;
    os << " b=" << light.color.b;
    os << " a=" << light.color.a;

  os << " ]";

  return os;
}


} // namespace CMU462


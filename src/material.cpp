#include "material.h"

using namespace std;

namespace CMU462 {

std::ostream& operator<<( std::ostream& os, const Material& material ) {

  os << "Material: " << material.name << " (id:" << material.id << ")";


  os << " [";

    os << " ambi=(" << material.ambi.r << ","
                    << material.ambi.g << ","
                    << material.ambi.b << ","
                    << material.ambi.a << ")";
    os << " diff=(" << material.diff.r << ","
                    << material.diff.g << ","
                    << material.diff.b << ","
                    << material.diff.a << ")";
    os << " spec=(" << material.spec.r << ","
                    << material.spec.g << ","
                    << material.spec.b << ","
                    << material.spec.a << ")";

    os << " shininess=" << material.shininess;

    os << " refractive_index=" << material.refractive_index;

  os << " ]";

  return os;
}

} // namespace CMU462


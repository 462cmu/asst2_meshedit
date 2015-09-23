#ifndef CMU462_LIGHT_H
#define CMU462_LIGHT_H

#include "scene.h"

namespace CMU462 {

typedef enum e_LightType {

  AMBIENT,
  POINT,
  DIRECTIONAL // FIXME : Add support for diffuse and specular lights.

} LightType;

struct Light : Instance {

  LightType light_type; ///< type of the light

  Color color;    ///< color of the light

  float attenuation;  ///< attentuation of the light

}; // struct Light

std::ostream& operator<<( std::ostream& os, const Light& light );

} // namespace CMU462

#endif // CMU462_LIGHT_H

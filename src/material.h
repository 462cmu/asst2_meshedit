#ifndef CMU462_MATERIAL_H
#define CMU462_MATERIAL_H

#include "scene.h"
#include "texture.h"

namespace CMU462 {

struct Material : Instance {

  Color emit; ///< emission color
  Color ambi; ///< ambient  color
  Color diff; ///< diffuse  color
  Color spec; ///< specular color

  float shininess; ///< shininess

  float refractive_index; ///< refractive index

  Texture* tex; ///< texture

}; // struct Material

std::ostream& operator<<( std::ostream& os, const Material& material );

} // namespace CMU462

#endif // CMU462_MATERIAL_H

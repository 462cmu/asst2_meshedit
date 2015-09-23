#ifndef CMU462_SCENE_H
#define CMU462_SCENE_H

// A Scene represents a 3D geometric scene (including geometry, lights, cameras, etc.),
// generally representing the same kind of objects as found in a COLLADA file.  This
// data structure is used by the viewer to draw and manipulate the scene.

#include <string>
#include <vector>
#include <iostream>

#include "CMU462/CMU462.h"

namespace CMU462 {

// There are a variety of different types of objects
// that can appear in a scene (lights, cameras, etc.);
// the e_InstanceType enum flags a node in the scene
// hierarchy as being one of these types.
typedef enum e_InstanceType
{
  CAMERA,
  LIGHT,
  POLYMESH,
  MATERIAL
} InstanceType;

// An instance represents a generic object in the scene,
// which can be one of several types.
struct Instance
{
  InstanceType type;  ///< type of instance

  std::string id;     ///< instance ID
  std::string name;   ///< instance name

}; // struct Instance

// A node represents both an object as well as its
// geometric transformation relative to the rest of
// the scene.
struct Node
{
  std::string id;
  std::string name;

  Instance* instance; ///< instance

  Matrix4x4 transform; ///< transformation

}; // struct Node

// A scene is a flat list of nodes, i.e., objects along
// with their transformations.  (Note that we have essentially
// "flattened" the original hierarchy that may have been found
// in an input COLLADA file.)
struct Scene {

  std::vector<Node> nodes;

}; // struct Scene

} // namespace CMU462

#endif // CMU462_SCENE_H

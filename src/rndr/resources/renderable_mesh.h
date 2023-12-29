/**
 * @file renderable_mesh.h
 * @author Jackson Wyatt Kaplan (JwyattK@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-11-25
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef RNDR_MESH_H_
#define RNDR_MESH_H_

#include "rndr/math/matrix.h"
#include "rndr/types/types.h"

#include <vector>
#include <webgpu/webgpu_cpp.h>

namespace rndr {

class RenderableMesh {
public:
  RenderableMesh(MeshData &inMeshData);

private:
  std::vector<math::vec3> vertices_;
};

} // namespace rndr

#endif
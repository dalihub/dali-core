#ifndef __DALI_INTERNAL_SCENE_GRAPH_BONE_TRANSFORMS_H__
#define __DALI_INTERNAL_SCENE_GRAPH_BONE_TRANSFORMS_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// INTERNAL INCLUDES
#include <dali/public-api/math/matrix3.h>
#include <dali/public-api/math/matrix.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{
static const size_t MAX_NUMBER_OF_BONES_PER_MESH = 12;

struct BoneTransforms
{
  std::vector<Matrix>  transforms;     // Bone transform matrices, calculated once per update
  std::vector<Matrix>  viewTransforms; // The transform matrices multiplied by current view, calculated once per render task
  std::vector<Matrix3> inverseTransforms; // inverse of the viewTransform matrix, calculated once per render task
};


} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_BONE_TRANSFORMS_H__

#ifndef _DALI_INTERNAL_SCENE_GRAPH_CULLING_ALGORITHMS_H_
#define _DALI_INTERNAL_SCENE_GRAPH_CULLING_ALGORITHMS_H_

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/rect.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

/**
 * Determine if the given bounding box is outside clip space (given by the
 * model view projection matrix).
 * @param[in] modelMatrix The world matrix of the bounding box.
 * @param[in] modelViewProjectionMatrix The clip space matrix
 * @param[in] boundingBox The bounding box of the geometry in object space
 * @return true if the bounding box is outside clip space
 */
bool Is2dBoxOutsideClipSpace(const Matrix& modelMatrix,
                             const Matrix& modelViewProjectionMatrix,
                             const Rect<float>& boundingBox );
} // SceneGraph
} // Internal
} // Dali

#endif //_DALI_INTERNAL_SCENE_GRAPH_CULLING_ALGORITHMS_H_

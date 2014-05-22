#ifndef __DALI_INTERNAL_TEXT_VERTEX_BUFFER_H__
#define __DALI_INTERNAL_TEXT_VERTEX_BUFFER_H__

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
 *
 */

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/math/vector2.h>
#include <dali/internal/common/text-vertex-2d.h>

namespace Dali
{

namespace Internal
{

/*
 * Vertex data for display text.
 * No indices are stored because, text is an array of quads.
 * each character quad is made up for 2 triangles (4 points).
 */
struct TextVertexBuffer
{
  std::vector<TextVertex2D> mVertices;    ///< List of vertices (coordinates and texture coordinates)
  unsigned int mTextureId;                ///< Texture id
  Vector2 mVertexMax;                     ///< Maximum extent of 2d vertex array
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_TEXT_VERTEX_BUFFER_H__

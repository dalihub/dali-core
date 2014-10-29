#ifndef __DALI_MESH_FACTORY_H__
#define __DALI_MESH_FACTORY_H__
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

#include <dali/public-api/math/rect.h>

namespace Dali
{
class MeshData;

namespace MeshFactory
{

/**
 * @brief Create an initialized plane aligned on the XY axis.
 *
 * @param[in] width   The width of the plane
 * @param[in] height  The height of the plane
 * @param[in] xSteps  The number of vertices along the X axis
 * @param[in] ySteps  The number of vertices along the Y axis
 * @param[in] textureCoordinates  UV coordinates.
 * @return A mesh data structure containing the plane mesh
 */
DALI_IMPORT_API Dali::MeshData NewPlane( const float width,
                                         const float height,
                                         const int xSteps,
                                         const int ySteps,
                                         const Rect<float>& textureCoordinates = Rect<float>(0.0f, 0.0f, 1.0f, 1.0f) );

} // MeshFactory
} // Dali

#endif // __DALI_MESH_FACTORY_H__

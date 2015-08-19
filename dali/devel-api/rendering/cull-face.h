#ifndef DALI_CULL_FACE_H
#define DALI_CULL_FACE_H

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

/**
 * @brief Face culling modes.
 */
enum CullFaceMode
{
  CullNone,                 ///< Face culling disabled
  CullFront,                ///< Cull front facing polygons
  CullBack,                 ///< Cull back facing polygons
  CullFrontAndBack          ///< Cull front and back facing polygons
};

class ImageActor;

/**
 * @brief Set the face-culling mode for an image-actor.
 *
 * @param[in] actor The image-actor to set the cull-face on.
 * @param[in] mode The culling mode.
 */
DALI_IMPORT_API void SetCullFace( ImageActor actor, CullFaceMode mode);

/**
 * @brief Retrieve the face-culling mode for an image-actor.
 *
 * @param[in] actor The image-actor whose cull-face is required.
 * @return mode The culling mode.
 */
DALI_IMPORT_API CullFaceMode GetCullFace( ImageActor actor );

} //namespace Dali

#endif // DALI_CULL_FACE_H

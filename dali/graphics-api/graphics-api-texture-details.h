#ifndef DALI_GRAPHICS_API_TEXTURE_DETAILS_H
#define DALI_GRAPHICS_API_TEXTURE_DETAILS_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#include <dali/graphics-api/graphics-api-types.h>

namespace Dali
{
namespace Graphics
{
namespace API
{
namespace TextureDetails
{

using AttachmentId = size_t;
using LayerId = size_t;
using LevelId = size_t;

enum class Type
{
  TEXTURE_2D,
  TEXTURE_3D,
  TEXTURE_CUBEMAP,
};

enum class Usage
{
  SAMPLE,
  COLOR_ATTACHMENT,
  DEPTH_ATTACHMENT
};

using Format = API::Format;

enum class MipMapFlag
{
  ENABLED,
  DISABLED,
};

enum class DepthStencilFlag
{
  NONE,
  DEPTH,
  STENCIL,
  DEPTH_STENCIL,
};

} // namespace TextureDetails
} // namespace API
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_API_TEXTURE_DETAILS_H

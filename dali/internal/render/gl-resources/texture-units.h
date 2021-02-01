#ifndef DALI_INTERNAL_TEXTURE_UNITS_H
#define DALI_INTERNAL_TEXTURE_UNITS_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/gl-defines.h>

namespace Dali
{
namespace Internal
{
enum TextureUnit
{
  TEXTURE_UNIT_IMAGE = 0,
  TEXTURE_UNIT_TEXT,
  TEXTURE_UNIT_MATERIAL_DIFFUSE,
  TEXTURE_UNIT_MATERIAL_OPACITY,
  TEXTURE_UNIT_MATERIAL_NORMAL_MAP,
  TEXTURE_UNIT_SHADER,
  TEXTURE_UNIT_UPLOAD,
  TEXTURE_UNIT_FRAMEBUFFER,
  TEXTURE_UNIT_LAST
};

inline unsigned int TextureUnitAsGLenum(TextureUnit unit)
{
  // GL texture units are #defines in growing order to converting that to index
  return GL_TEXTURE0 + static_cast<unsigned int>(unit);
}

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_TEXTURE_UNITS_H

#ifndef DALI_GRAPHICS_API_TEXTURE_SET_H
#define DALI_GRAPHICS_API_TEXTURE_SET_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include <dali/graphics-api/graphics-api-texture.h>

namespace Dali
{
namespace Graphics
{
namespace API
{

/**
 * @brief Interface class for TextureSet types in the graphics API.
 */
class TextureSet
{
public:
  using Handle = size_t;

  virtual void AddTexture(Texture::Handle texture) = 0;

  // not copyable
  TextureSet(const TextureSet&) = delete;
  TextureSet& operator=(const TextureSet&) = delete;

  virtual ~TextureSet() = default;

protected:
  // derived types should not be moved direcly to prevent slicing
  TextureSet(TextureSet&&) = default;
  TextureSet& operator=(TextureSet&&) = default;

  /**
   * Objects of this type should not directly.
   */
  TextureSet() = default;
};

} // namespace API
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_API_TEXTURE_SET_H

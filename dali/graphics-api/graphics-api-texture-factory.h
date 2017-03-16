#ifndef DALI_GRAPHICS_API_TEXTURE_FACTORY_H
#define DALI_GRAPHICS_API_TEXTURE_FACTORY_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#include <dali/graphics-api/graphics-api-base-factory.h>
#include <dali/graphics-api/graphics-api-texture-details.h>
#include <dali/graphics-api/graphics-api-texture.h>

namespace Dali
{
namespace Graphics
{
namespace API
{

/**
 * @brief Interface class for TextureFactory types in the graphics API.
 */
class TextureFactory : BaseFactory< Texture >
{
public:
  virtual void SetSize(TextureDetails::RectSize)        = 0;
  virtual void SetFormat(TextureDetails::Format format) = 0;

  // not copyable
  TextureFactory(const TextureFactory&) = delete;
  TextureFactory& operator=(const TextureFactory&) = delete;

  virtual ~TextureFactory() = default;

protected:
  // derived types should not be moved direcly to prevent slicing
  TextureFactory(TextureFactory&&) = default;
  TextureFactory& operator=(TextureFactory&&) = default;

  /**
   * Objects of this type should not directly.
   */
  TextureFactory() = default;
};

} // namespace API
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_API_TEXTURE_FACTORY_H

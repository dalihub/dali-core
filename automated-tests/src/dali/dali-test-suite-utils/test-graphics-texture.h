#ifndef DALI_TEST_GRAPHICS_TEXTURE_H
#define DALI_TEST_GRAPHICS_TEXTURE_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

#include <dali/graphics-api/graphics-texture-create-info.h>
#include <dali/graphics-api/graphics-texture.h>
#include <dali/graphics-api/graphics-types.h>

#include "test-graphics-sampler.h"

namespace Dali
{
class TestGraphicsTexture : public Graphics::Texture
{
public:
  TestGraphicsTexture( const Graphics::TextureCreateInfo& createInfo );

  ~TestGraphicsTexture() override;

  /**
   * Initialize the texture: allocate gpu mem, apply default samplers
   */
  void Initialize( uint32_t target );

  /**
   * Ensure native resource is created, bound and targeted.
   */
  void InitializeNativeImage( uint32_t target );

  /**
   * Get the GL target of this texture
   */
  uint32_t GetTarget();

  /**
   * Get the texture type
   */
  Graphics::TextureType GetType()
  {
    return mCreateInfo.textureType;
  }

  /**
   * Get the texture format
   */
  Graphics::Format GetFormat()
  {
    return mCreateInfo.format;
  }

  /**
   * Bind this texture, ensure Native image is initialized if necessary.
   */
  void Bind( uint32_t textureUnit );

  /**
   * Prepare ensures that the native texture is updated if necessary
   */
  void Prepare();

  /**
   * Writes actual texture data to GL.
   */
  void Update( Graphics::TextureUpdateInfo updateInfo, Graphics::TextureUpdateSourceInfo source );

  uint32_t mId{ 0 };

  Graphics::TextureCreateInfo mCreateInfo;
  bool                        mIsCompressed{ false };
  uint32_t                    mGlInternalFormat; ///< The gl internal format of the pixel data
  uint32_t                    mGlFormat;         ///< The gl format of the pixel data
  uint32_t                    mPixelDataType;    ///< The data type of the pixel data
};

} // namespace Dali

#endif //DALI_TEST_GRAPHICS_TEXTURE_H

#ifndef __DALI_INTERNAL_NATIVE_TEXTURE_H__
#define __DALI_INTERNAL_NATIVE_TEXTURE_H__

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
#include <dali/integration-api/gl-defines.h>
#include <dali/public-api/images/native-image.h>
#include <dali/internal/render/gl-resources/texture.h>

namespace Dali
{

namespace Internal
{

class Texture;

/**
 *
 * Native texture class.
 * Used for handing already loaded image data to Dali.
 *
 */
class NativeTexture : public Texture
{
public:
  /**
   * Constructor.
   * @pre nativeImg has to be initialized. (GLTexture is already created)
   * @param nativeImg         the NativeImage to be used
   * @param context           the GL context
   */
  NativeTexture(NativeImage* nativeImg, Context& context);

  /**
   * Destructor.
   */
  virtual ~NativeTexture();

  /**
   * @copydoc Texture::Bind
   */
  virtual bool Bind( GLenum target, TextureUnit textureunit );

  /**
   * @copydoc Texture::IsFullyOpaque
   */
  virtual bool IsFullyOpaque() const;

  /**
   * @copydoc Texture::HasAlphaChannel
   */
  virtual bool HasAlphaChannel() const;


  /**
   * @copydoc Texture::GetPixelFormat
   */
  virtual Pixel::Format GetPixelFormat() const;

  /**
   * @copydoc Texture::Init
   */
  virtual bool Init();

protected:
  /**
   * @copydoc Texture::CreateGlTexture
   */
  virtual bool CreateGlTexture();

  /**
   * @copydoc Texture::GlCleanup
   */
  virtual void GlCleanup();


private:

  NativeImagePtr mNativeImage; ///< reference to NativeImage the Texture was created from
};

}  //namespace Internal

}  //namespace Dali

#endif //__DALI_INTERNAL_NATIVE_TEXTURE_H__

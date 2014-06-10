#ifndef __DALI_INTERNAL_FRAME_BUFFER_NATIVE_IMAGE_TEXTURE_H__
#define __DALI_INTERNAL_FRAME_BUFFER_NATIVE_IMAGE_TEXTURE_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/internal/render/gl-resources/texture.h>
#include <dali/internal/render/gl-resources/frame-buffer-texture.h>
#include <dali/integration-api/bitmap.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Internal
{

/**
 * Frame Buffer Texture created from NativeImage.
 * Used as a frame buffer for RenderTask
 */
class NativeFrameBufferTexture : public FrameBufferTexture
{
public:
  /**
   * Creates a new texture object
   * @param[in] nativeImage The NativeImage
   * @param     context The GL context
   */
  NativeFrameBufferTexture( NativeImagePtr nativeImage, Context& context);

  /**
   * Destructor.
   */
  virtual ~NativeFrameBufferTexture();

  /**
   * @copydoc Texture::IsFullyOpaque
   */
  virtual bool IsFullyOpaque() const;

  /**
   * @copydoc Texture::HasAlphaChannel
   */
  virtual bool HasAlphaChannel() const;

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

}; // class NativeFrameBufferTexture

}  //namespace Internal

} //namespace Dali

#endif // header


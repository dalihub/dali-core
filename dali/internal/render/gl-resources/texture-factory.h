#ifndef __DALI_INTERNAL_TEXTURE_FACTORY_H__
#define __DALI_INTERNAL_TEXTURE_FACTORY_H__

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
#include <dali/public-api/object/ref-object.h>
#include <dali/devel-api/images/native-image-interface.h>
#include <dali/internal/render/gl-resources/texture-cache.h>

namespace Dali
{
class NativeImageInterface;

namespace Integration
{
class Bitmap;
}

namespace Internal
{
class Context;
class Texture;

namespace TextureFactory
{
/**
 * Creates a new texture object from a Bitmap
 * @param[in] bitmap The Bitmap
 * @param[in] context The GL context
 * @return A newly allocated texture
 */
Internal::Texture* NewBitmapTexture( Integration::Bitmap* const bitmap,
                                     Context& context,
                                     ResourcePolicy::Discardable discardPolicy );

/**
 * Creates a new bitmap texture object of the given dimensions.
 * @param[in] width       The width (pixels)
 * @param[in] height      The height (pixels)
 * @param[in] pixelFormat The pixel format
 * @param[in] clearPixels True if the pixel data should be cleared first
 * @param[in] context The GL context
 */
Internal::Texture* NewBitmapTexture( unsigned int      width,
                                     unsigned int      height,
                                     Pixel::Format     pixelFormat,
                                     bool              clearPixels,
                                     Context&          context,
                                     ResourcePolicy::Discardable discardPolicy );

/**
 * Creates a texture object from a native image (eg.: EGLImage).
 * @param[in] nativeImage The native image to load
 * @param[in] context The GL context
 * @return A newly allocated texture
 */
Internal::Texture* NewNativeImageTexture( NativeImageInterface& nativeImage, Context& context );

/**
 * @copydoc FrameBufferTexture::FrameBufferTexture
 * @return A newly allocated texture
 */
Internal::Texture* NewFrameBufferTexture( unsigned int width,
                                          unsigned int height,
                                          Pixel::Format pixelFormat,
                                          Context& context );

/**
 * @copydoc FrameBufferTexture::FrameBufferTexture
 * @return A newly allocated texture
 */
Internal::Texture* NewFrameBufferTexture( NativeImageInterfacePtr nativeImage, Context& context );

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_TEXTURE_FACTORY_H__

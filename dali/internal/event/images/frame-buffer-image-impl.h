#ifndef __DALI_INTERNAL_FRAME_BUFFER_IMAGE_H__
#define __DALI_INTERNAL_FRAME_BUFFER_IMAGE_H__

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
#include <dali/internal/event/images/image-impl.h>
#include <dali/public-api/images/frame-buffer-image.h>

namespace Dali
{

namespace Internal
{

class FrameBufferImage;
typedef IntrusivePtr<FrameBufferImage> FrameBufferImagePtr;

/**
 * @copydoc Dali::FrameBufferImage
 */
class FrameBufferImage : public Image
{
public:
  /// @copydoc Dali::FrameBufferImage::FrameBufferImage
  FrameBufferImage(unsigned int width, unsigned int height, Pixel::Format pixelFormat);

  /// @copydoc Dali::FrameBufferImage::FrameBufferImage
  FrameBufferImage(unsigned int width, unsigned int height, Pixel::Format pixelFormat, ReleasePolicy releasePolicy);

  /// @copydoc Dali::FrameBufferImage::FrameBufferImage
  FrameBufferImage(NativeImage& image);

  /// @copydoc Dali::FrameBufferImage::FrameBufferImage
  FrameBufferImage(NativeImage& image, ReleasePolicy releasePolicy);

public: // From Image
  /**
   * @copydoc Dali::Internal::Image::Connect()
   */
  virtual void Connect();

  /**
   * @copydoc Dali::Internal::Image::Disconnect()
   */
  virtual void Disconnect();

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~FrameBufferImage();

private:
  // cached values for the size / pixel format we were created with. Needed to recreate us when we Connect() to stage and mTicket was reset from a previous call to Disconnect().
  NativeImagePtr mNativeImage;
  Pixel::Format mPixelFormat;
}; // class FrameBufferImage

} // namespace Internal

/**
 * Helper methods for public API.
 */
inline Internal::FrameBufferImage& GetImplementation(Dali::FrameBufferImage& image)
{
  DALI_ASSERT_ALWAYS( image && "FrameBufferImage handle is empty" );

  BaseObject& handle = image.GetBaseObject();

  return static_cast<Internal::FrameBufferImage&>(handle);
}

inline const Internal::FrameBufferImage& GetImplementation(const Dali::FrameBufferImage& image)
{
  DALI_ASSERT_ALWAYS( image && "FrameBufferImage handle is empty" );

  const BaseObject& handle = image.GetBaseObject();

  return static_cast<const Internal::FrameBufferImage&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_FRAME_BUFFER_IMAGE_H__

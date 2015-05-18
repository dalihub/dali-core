#ifndef __DALI_INTERNAL_NATIVE_IMAGE_H__
#define __DALI_INTERNAL_NATIVE_IMAGE_H__

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
#include <dali/public-api/images/native-image.h>
#include <dali/internal/event/images/image-impl.h>

namespace Dali
{

namespace Internal
{

class NativeImage;
typedef IntrusivePtr<NativeImage> NativeImagePtr;

/**
 * @copydoc Dali::NativeImage
 */
class NativeImage : public Image
{
public:

  /**
   * Creates object by using native resources
   * the maximum size of the image is limited by GL_MAX_TEXTURE_SIZE
   * @param [in] nativeImageInterface An reference to the object of the interface implementation.
   * @return a pointer to a newly created object.
   */
  static NativeImagePtr New( NativeImageInterface& nativeImageInterface );

  /**
   * @copydoc Dali::NativeImage::CreateGlTexture
   */
  void CreateGlTexture();

protected:

  /**
   * Constructor
   */
  NativeImage();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~NativeImage();
};

} // namespace Internal

/**
 * Helper methods for public API.
 */
inline Internal::NativeImage& GetImplementation(Dali::NativeImage& image)
{
  DALI_ASSERT_ALWAYS( image && "Image handle is empty" );

  BaseObject& handle = image.GetBaseObject();

  return static_cast<Internal::NativeImage&>(handle);
}

inline const Internal::NativeImage& GetImplementation(const Dali::NativeImage& image)
{
  DALI_ASSERT_ALWAYS( image && "Image handle is empty" );

  const BaseObject& handle = image.GetBaseObject();

  return static_cast<const Internal::NativeImage&>(handle);
}

} // namespace Dali
#endif // __DALI_INTERNAL_NATIVE_IMAGE_H__

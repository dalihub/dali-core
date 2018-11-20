#ifndef __DALI_INTERNAL_IMAGE_H__
#define __DALI_INTERNAL_IMAGE_H__

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/images/image.h>
#include <dali/public-api/object/base-object.h>
#include <dali/internal/event/rendering/texture-impl.h>

namespace Dali
{

namespace Internal
{

class Image;
class ImageFactory;
typedef IntrusivePtr<Image> ImagePtr;

/**
 * Image represents an image resource that can be added to actors etc.
 * When the Image object is created, resource loading will be attempted.
 * Provided this is successful, the resource will exist until the Image is destroyed.
 */
class Image : public BaseObject
{
public:

  /**
   * @copydoc Dali::Image::UploadedSignal()
   */
  Dali::Image::ImageSignalType& UploadedSignal() { return mUploaded; }

  /**
   * Get the width of the image.
   * Only to be used after the image has finished loading.
   * (Ticket's LoadingSucceeded callback was called)
   * The returned value will reflect the true image dimensions once the asynchronous loading has finished.
   * @pre image should be loaded
   */
  virtual unsigned int GetWidth() const;

  /**
   * Get the height of the image.
   * Only to be used after the image has finished loading.
   * (Ticket's LoadingSucceeded callback was called)
   * The returned value will reflect the true image dimensions once the asynchronous loading has finished.
   * @pre image should be loaded
   */
  virtual unsigned int GetHeight() const;

  /**
   * Return the natural size of the image.
   * This is the size that the loaded image will take
   */
  virtual Vector2 GetNaturalSize() const;

  /**
   * Returns a pointer to the internal texture used by the image
   */
  Texture* GetTexture() const
  {
    return mTexture.Get();
  }

public:

  /**
   * Indicates that the image is used.
   */
  virtual void Connect() {}

  /**
   * Indicates that the image is not used anymore.
   */
  virtual void Disconnect() {}

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Image();

  /**
   * Constructor, with default parameters
   */
  Image();

  /**
   * Second stage initialization
   */
  void Initialize();

protected:

  TexturePtr mTexture;  ///< smart pointer to the texture used by the image

  unsigned int mWidth;     ///< natural width of the image
  unsigned int mHeight;    ///< natural height of the image

  unsigned int mConnectionCount; ///< number of on-stage objects using this image

private:

  Dali::Image::ImageSignalType mUploaded;
};

} // namespace Internal

/**
 * Helper methods for public API.
 */
inline Internal::Image& GetImplementation(Dali::Image& image)
{
  DALI_ASSERT_ALWAYS( image && "Image handle is empty" );

  BaseObject& handle = image.GetBaseObject();

  return static_cast<Internal::Image&>(handle);
}

inline const Internal::Image& GetImplementation(const Dali::Image& image)
{
  DALI_ASSERT_ALWAYS( image && "Image handle is empty" );

  const BaseObject& handle = image.GetBaseObject();

  return static_cast<const Internal::Image&>(handle);
}

} // namespace Dali
#endif // __DALI_INTERNAL_IMAGE_H__

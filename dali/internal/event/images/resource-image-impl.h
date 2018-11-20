#ifndef __DALI_INTERNAL_RESOURCE_IMAGE_H__
#define __DALI_INTERNAL_RESOURCE_IMAGE_H__

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
#include <dali/public-api/images/resource-image.h>
#include <dali/internal/event/images/image-impl.h>
#include <dali/internal/common/image-attributes.h>
#include <dali/integration-api/debug.h> // For DALI_LOG_OBJECT_STRING_DECLARATION

namespace Dali
{

namespace Internal
{

class ResourceImage;
typedef IntrusivePtr<ResourceImage> ResourceImagePtr;

/**
 * ResourceImage is an image loaded using a URL, it is an image resource that can be added to actors etc.
 */
class ResourceImage : public Image
{
public:

  /**
   * Creates a pointer to an uninitialized Image object.
   * @return a pointer to a newly created object.
   */
  static ResourceImagePtr New();

  /**
   * Creates object and loads image from filesystem
   * the maximum size of the image is limited by GL_MAX_TEXTURE_SIZE
   * @param [in] url The URL of the image file.
   * @param [in] attributes requested parameters for loading (size, scaling etc.)
   *                        if width or height is specified as 0, the natural size will be used.
   * @return a pointer to a newly created object.
   */
  static ResourceImagePtr New( const std::string& url,
                          const ImageAttributes& attributes );

  /**
   * @copydoc Dali::ResourceImage::GetLoadingState()
   */
  Dali::LoadingState GetLoadingState() const { return mLoadingState; }

  /**
   * @copydoc Dali::ResourceImage::LoadingFinishedSignal()
   */
  Dali::ResourceImage::ResourceImageSignal& LoadingFinishedSignal() { return mLoadingFinished; }

  /**
   * Get the attributes of the image.
   * Only to be used after the image has finished loading.
   * (Ticket's LoadingSucceeded callback was called)
   * Reflects the last cached values after a LoadComplete.
   * If requested width or height was 0, they are replaced by concrete dimensions.
   * @return a copy of the attributes
   */
  const ImageAttributes& GetAttributes() const;

  /**
   * @copydoc Dali::ResourceImage::GetUrl()
   * virtual so deriving class can override it
   */
  virtual const std::string& GetUrl() const;

  /**
   * @copydoc Dali::ResourceImage::Reload()
   */
  void Reload();

  /**
   * @copydoc Dali::Image::GetWidth()
   */
  virtual unsigned int GetWidth() const;

  /**
   * @copydoc Dali::Image::GetWidth()
   */
  virtual unsigned int GetHeight() const;

  /**
   * @copydoc Dali::Internal::Image::GetNaturalSize()
   */
  virtual Vector2 GetNaturalSize() const;


protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ResourceImage();

  /**
   * Constructor, with default parameters
   */
  ResourceImage();

  /**
   * Constructor, with url and attributes
   */
  ResourceImage( const std::string& url, const ImageAttributes& attributes);

private:
  Dali::ResourceImage::ResourceImageSignal mLoadingFinished;
  ImageAttributes mAttributes;
  std::string mUrl;
  Dali::LoadingState mLoadingState;

  // Changes scope, should be at end of class
  DALI_LOG_OBJECT_STRING_DECLARATION;
};

} // namespace Internal

/**
 * Helper methods for public API.
 */
inline Internal::ResourceImage& GetImplementation(Dali::ResourceImage& image)
{
  DALI_ASSERT_ALWAYS( image && "Image handle is empty" );

  BaseObject& handle = image.GetBaseObject();

  return static_cast<Internal::ResourceImage&>(handle);
}

inline const Internal::ResourceImage& GetImplementation(const Dali::ResourceImage& image)
{
  DALI_ASSERT_ALWAYS( image && "Image handle is empty" );

  const BaseObject& handle = image.GetBaseObject();

  return static_cast<const Internal::ResourceImage&>(handle);
}

} // namespace Dali
#endif // __DALI_INTERNAL_RESOURCE_IMAGE_H__

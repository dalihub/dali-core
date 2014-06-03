#ifndef __DALI_INTERNAL_IMAGE_H__
#define __DALI_INTERNAL_IMAGE_H__

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/images/image.h>
#include <dali/public-api/images/native-image.h>
#include <dali/public-api/object/base-object.h>
#include <dali/internal/render/gl-resources/texture.h>
#include <dali/internal/event/images/image-factory-cache.h>
#include <dali/internal/event/resources/image-ticket.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/internal/event/resources/resource-ticket-observer.h>

namespace Dali
{

class NativeImage;

namespace Internal
{

typedef Dali::Image::LoadPolicy    LoadPolicy;
typedef Dali::Image::ReleasePolicy ReleasePolicy;

class Image;
class ImageFactory;
typedef IntrusivePtr<Image> ImagePtr;

const LoadPolicy    ImageLoadPolicyDefault    = Dali::Image::Immediate;
const ReleasePolicy ImageReleasePolicyDefault = Dali::Image::Never;

/**
 * Image represents an image resource that can be added to actors etc.
 * When the Image object is created, resource loading will be attempted.
 * Provided this is successful, the resource will exist until the Image is destroyed.
 */
class Image : public BaseObject, public ResourceTicketObserver
{
protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Image();

  /**
   * Constructor, with default parameters
   */
  Image(LoadPolicy loadPol=ImageLoadPolicyDefault, ReleasePolicy releasePol=ImageReleasePolicyDefault);

public:
  /**
   * Creates a pointer to an uninitialized Image object.
   * @return a pointer to a newly created object.
   */
  static ImagePtr New();

  /**
   * Creates object and loads image from filesystem
   * the maximum size of the image is limited by GL_MAX_TEXTURE_SIZE
   * @param [in] filename   the path of the image on the filesystem
   * @param [in] attributes requested parameters for loading (size, cropping etc.)
   *                        if width or height is specified as 0, the natural size will be used.
   * @param [in] loadPol controls time of loading a resource from the filesystem (default: load when Image is created).
   * @param [in] releasePol optionally relase memory when image is not visible on screen (default: keep image data until Image object is alive).
   * @return a pointer to a newly created object.
   */
  static ImagePtr New( const std::string& filename,
                       const Dali::ImageAttributes& attributes=Dali::ImageAttributes::DEFAULT_ATTRIBUTES,
                       LoadPolicy loadPol=ImageLoadPolicyDefault,
                       ReleasePolicy releasePol=ImageReleasePolicyDefault );

  /**
   * Creates object with already loaded NativeImage
   * the maximum size of the image is limited by GL_MAX_TEXTURE_SIZE
   * @pre nativeImg should be initialised
   * @param [in] nativeImg already initialised NativeImage
   * @param [in] loadPol controls time of loading a resource from the filesystem (default: load when Image is created).
   * @param [in] releasePol optionally relase memory when image is not visible on screen (default: keep image data until Image object is alive).
   * @return a pointer to a newly created object.
   */
  static ImagePtr New( NativeImage& nativeImg,
                       LoadPolicy loadPol=ImageLoadPolicyDefault,
                       ReleasePolicy releasePol=ImageReleasePolicyDefault );

  /**
   * @copydoc Dali::Image::GetLoadingState()
   */
  Dali::LoadingState GetLoadingState() const { return mTicket ? mTicket->GetLoadingState() : ResourceLoading; }

  /**
   * @copydoc Dali::Image::GetLoadPolicy()
   */
  LoadPolicy GetLoadPolicy () const { return mLoadPolicy; }

  /**
   * @copydoc Dali::Image::GetReleasePolicy()
   */
  ReleasePolicy GetReleasePolicy () const { return mReleasePolicy; }

  /**
   * @copydoc Dali::Image::LoadingFinishedSignal()
   */
  Dali::Image::ImageSignalV2& LoadingFinishedSignal() { return mLoadingFinishedV2; }

  /**
   * @copydoc Dali::Image::UploadedSignal()
   */
  Dali::Image::ImageSignalV2& UploadedSignal() { return mUploadedV2; }

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

  /**
   * returns the Id used for lookups
   * @note if LoadPolicy::OnDemand is used and Image is off Stage, this will return 0.
   * @return the unique ID of the image data resource. This is actually also the same as Dali Texture id.
   */
  ResourceId GetResourceId() const;

  /**
   * Get the attributes of the image.
   * Only to be used after the image has finished loading.
   * (Ticket's LoadingSucceeded callback was called)
   * Reflects the last cached values after a LoadComplete.
   * If requested width or height was 0, they are replaced by concrete dimensions.
   * @return a copy of the attributes
   */
  const Dali::ImageAttributes& GetAttributes() const;

  /**
   * Get the width of the image.
   * Only to be used after the image has finished loading.
   * (Ticket's LoadingSucceeded callback was called)
   * The returned value will reflect the true image dimensions once the asynchronous loading has finished.
   * Connect to SignalLoadingFinished or use GetLoadingState to make sure this value is actual.
   * @pre image should be loaded
   */
  unsigned int GetWidth() const;

  /**
   * Get the height of the image.
   * Only to be used after the image has finished loading.
   * (Ticket's LoadingSucceeded callback was called)
   * The returned value will reflect the true image dimensions once the asynchronous loading has finished.
   * Connect to SignalLoadingFinished or use GetLoadingState to make sure this value is actual.
   * @pre image should be loaded
   */
  unsigned int GetHeight() const;

  /**
   * Return the natural size of the image.
   * This is the size that the loaded image will take
   */
  Vector2 GetNaturalSize() const;

  /**
   * @copydoc Dali::Image::GetFilename()
   */
  const std::string& GetFilename() const;

  /**
   * @copydoc Dali::Image::Reload()
   */
  void Reload();

public: // From ResourceTicketObserver

  /**
   * @copydoc Dali::Internal::ResourceTicketObserver::ResourceLoadingFailed()
   */
  virtual void ResourceLoadingFailed(const ResourceTicket& ticket);

  /**
   * @copydoc Dali::Internal::ResourceTicketObserver::ResourceLoadingSucceeded()
   */
  virtual void ResourceLoadingSucceeded(const ResourceTicket& ticket);

  /**
   * @copydoc Dali::Internal::ResourceTicketObserver::ResourceUploaded()
   */
  virtual void ResourceUploaded(const ResourceTicket& ticket);

  /**
   * @copydoc Dali::Internal::ResourceTicketObserver::ResourceSavingSucceeded()
   */
  virtual void ResourceSavingSucceeded( const ResourceTicket& ticket );

  /**
   * @copydoc Dali::Internal::ResourceTicketObserver::ResourceSavingFailed()
   */
  virtual void ResourceSavingFailed( const ResourceTicket& ticket );

public:

  /**
   * Indicates that the image is used.
   */
  virtual void Connect();

  /**
   * Indicates that the image is not used anymore.
   */
  virtual void Disconnect();

protected:
  /**
   * Second stage initialization
   */
  void Initialize();

private:

  /**
   * Helper method to set new resource ticket. Stops observing current ticket if any, and starts observing
   * the new one or just resets the intrusive pointer.
   * @param[in] ticket pointer to new resource Ticket or NULL.
   */
  void SetTicket( ResourceTicket* ticket );

  /**
   * Helper method to determine if the filename indicates that the image has a 9 patch border.
   * @param[in] filename The filename to check
   * @return true if it is a 9 patch image
   */
  static bool IsNinePatchFileName( std::string filename );


protected:
  unsigned int mWidth;
  unsigned int mHeight;

  ResourceTicketPtr mTicket;
  ImageFactoryCache::RequestPtr mRequest;         ///< contains the initially requested attributes for image. Request is reissued when memory was released.
  LoadPolicy     mLoadPolicy;
  ReleasePolicy  mReleasePolicy;

  unsigned int   mConnectionCount; ///< number of on-stage objects using this image
  ImageFactory&  mImageFactory;

private:
  Dali::Image::ImageSignalV2 mLoadingFinishedV2;
  Dali::Image::ImageSignalV2 mUploadedV2;

  // Changes scope, should be at end of class
  DALI_LOG_OBJECT_STRING_DECLARATION;
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

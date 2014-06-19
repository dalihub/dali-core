#ifndef __DALI_IMAGE_H__
#define __DALI_IMAGE_H__

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

/**
 * @addtogroup CAPI_DALI_IMAGES_MODULE
 * @{
 */

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/common/loading-state.h>
#include <dali/public-api/images/pixel.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal-v2.h>

namespace Dali DALI_IMPORT_API
{
struct Vector2;

namespace Internal DALI_INTERNAL
{
class Image;
}

class NativeImage;
class ImageAttributes;


/**
 * @brief An Image object represents an image resource that can be added to ImageActors.
 *
 * Image objects can also handle custom requests for image loading and are responsible for
 * the underlying resource's lifetime.
 *
 * <h3>Image Loading</h3>
 *
 * When the Image object is created, resource loading will be attempted unless
 * the Image object is created with OnDemand loading policy or a compatible resource is found in cache.
 * In case of loading images on demand, resource loading will only be attempted if the associated ImageActor
 * is put on Stage.
 * Custom loading requests can be made by providing an ImageAttributes object to Image::New().
 *
 * <i>LoadPolicies</i>
 * - Immediate: acquire image resource when creating Image object.
 * - OnDemand: only load in case the associated ImageActor is put on Stage
 *
 * <i>ReleasePolicies</i>
 * - Unused: release resource once ImageActor using it is taken off stage.
 * - Never: keep resource alive until Image object is thrown away.
 *
 * <i>Resolution of conflicting policies</i>
 * If the same image is created more than once with conflicting policies, LoadPolicy "Immediate" overrides "OnDemand"
 * and ReleasePolicy "Never" overrides "Unused".
 *
 * <i>Custom load requests</i>
 * Size, scaling mode, orientation compensation can be set when requesting an image resource.
 * See ImageAttributes for more details.
 *
 * <i>Compatible resources</i>
 *
 * Before loading a new image the internal image resource cache is checked by dali.
 * If there is an Image already loaded in memory and is deemed "compatible" with the requested Image,
 * that resource is reused.
 * This happens for example if a loaded image exists with the same filename, and the difference between both
 * of the dimensions is less than 50%.
 *
 * <i>Reloading Images</i>
 *
 * The same request used on creating the Image is re-issued when reloading Images.
 * If the file changed since the last load operation, this might result in a different resource.
 * Reload only takes effect if both of these conditions apply:
 * - The Image has already finished loading
 * - The Image is either on Stage or using Immediate load policy
 *
 * <h3>Image resource lifetime</h3>
 *
 * Image objects can be shared between ImageActors. This is practical if you have a visual element on screen
 * which is repeatedly used. An example would be a button background image.
 * The image resource is discarded when all ImageActors using the Image object are discarded or in case they
 * were created with ReleasePolicy::Unused, taken off stage.
 * Note: if a resource was shared between Image objects it exists until its last reference is gone.
 *
 */
class Image : public BaseHandle
{
public:
  /**
   * @brief Resource management options.
   */

  /**
   * @brief LoadPolicy controls the way images are loaded into memory.
   */
  enum LoadPolicy
  {
    Immediate, ///< load image once it is created (default)
    OnDemand   ///< delay loading until the image is being used (a related actor is added to Stage)
  };

  /**
   * @brief ReleasePolicy controls the way images are deleted from memory.
   */
  enum ReleasePolicy
  {
    Unused, ///< release resource once image is not in use anymore (eg. all actors using it become offstage). Reload when resource is required again.
    Never   ///< keep image data for the lifetime of the object. (default)
  };

  /**
   * @brief Type of signal for LoadingFinished and Uploaded.
   */
  typedef SignalV2< void (Image) > ImageSignalV2;

  // Signal Names
  static const char* const SIGNAL_IMAGE_LOADING_FINISHED; ///< Name of LoadingFinished signal
  static const char* const SIGNAL_IMAGE_UPLOADED; ///< Name of Uploaded signal

public:

  /**
   * @brief Get the size of an image from disk.
   *
   * This function will read the header info from file on disk and is
   * synchronous, so it should not be used repeatedly or in tight
   * loops.
   *
   * @param [in] filename of the image file to use.
   * @return The width and height in pixels of the image.
   */
  static Vector2 GetImageSize(const std::string filename);

  /**
   * @brief Constructor which creates an empty Image object.
   *
   * Use Image::New(...) to create an initialised object.
   */
  Image();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Image();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

  /**
   * @brief Create an initialised image object.
   *
   * @param [in] filename The filename of the image file to use.
   * @return A handle to a newly allocated object
   */
  static Image New(const std::string& filename);

  /**
   * @brief Create an initialised image object.
   *
   * @param [in] filename The filename of the image file to use.
   * @param [in] loadPol    The LoadPolicy to apply when loading the image resource.
   * @param [in] releasePol The ReleasePolicy to apply to Image.
   * @return A handle to a newly allocated object
   */
  static Image New(const std::string& filename, LoadPolicy loadPol, ReleasePolicy releasePol);

  /**
   * @brief Create an initialised image object.
   *
   * @param [in] filename   The filename of the image file to use.
   * @param [in] attributes Requested parameters for loading (size, scaling etc.).
   * @return A handle to a newly allocated object
   */
  static Image New(const std::string& filename, const ImageAttributes& attributes);

  /**
   * @brief Create an initialised image object.
   *
   * @param [in] filename   The filename of the image file to use.
   * @param [in] attributes Requested parameters for loading (size, scaling etc.).
   * @param [in] loadPol    The LoadPolicy to apply when loading the image resource.
   * @param [in] releasePol The ReleasePolicy to apply to Image.
   * @return A handle to a newly allocated object
   */
  static Image New(const std::string& filename, const ImageAttributes& attributes, LoadPolicy loadPol, ReleasePolicy releasePol);

  /**
   * @brief Create an initialised image object.
   *
   * @param [in] filename The filename of the image file to use.
   * @return A handle to a newly allocated object
   */
  static Image NewDistanceField(const std::string& filename);

  /**
   * @brief Create an initialised image object.
   *
   * @param [in] filename   The filename of the image file to use.
   * @param [in] loadPol    The LoadPolicy to apply when loading the image resource.
   * @param [in] releasePol The ReleasePolicy to apply to Image.
   * @return A handle to a newly allocated object
   */
  static Image NewDistanceField(const std::string& filename, LoadPolicy loadPol, ReleasePolicy releasePol);

  /**
   * @brief Create an initialised image object.
   *
   * @param [in] filename   The filename of the image file to use.
   * @param [in] attributes The minimum search radius to check for differing pixels
   * @return A handle to a newly allocated object
   */
  static Image NewDistanceField(const std::string& filename, const ImageAttributes& attributes );

  /**
   * @brief Create an initialised image object.
   *
   * @param [in] filename   The filename of the image file to use.
   * @param [in] attributes The minimum search radius to check for differing pixels
   * @param [in] loadPol    The LoadPolicy to apply when loading the image resource.
   * @param [in] releasePol The ReleasePolicy to apply to Image.
   * @return A handle to a newly allocated object
   */
  static Image NewDistanceField(const std::string& filename, const ImageAttributes& attributes, LoadPolicy loadPol, ReleasePolicy releasePol);

  /**
   * @brief Creates object with already loaded NativeImage.
   *
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE
   * @pre nativeImg should be initialised
   * @param [in] nativeImg already initialised NativeImage
   * @return A handle to a newly allocated object
   */
  static Image New(NativeImage& nativeImg);

  /**
   * @brief Creates object with already loaded NativeImage.
   *
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE
   * @pre nativeImg should be initialised
   * @param [in] nativeImg already initialised NativeImage
   * @param [in] loadPol    The LoadPolicy to apply when allocating the GL resource.
   * @param [in] releasePol The ReleasePolicy to apply to Image.
   * @return A handle to a newly allocated object
   */
  static Image New(NativeImage& nativeImg, LoadPolicy loadPol, ReleasePolicy releasePol);

  /**
   * @brief Downcast an Object handle to Image handle.
   *
   * If handle points to a Image object the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a Image object or an uninitialized handle
   */
  static Image DownCast( BaseHandle handle );

  /**
   * @brief Query whether the image data has loaded.
   *
   * The asynchronous loading begins when the Image object is created.
   * After the Image object is discarded, the image data will be released from memory.
   * @return The loading state, either Loading, Success or Failed.
   */
  LoadingState GetLoadingState() const;

  /**
   * @brief Returns the filename of the image if the image is created from a file.
   *
   * @return the image filename or empty string
   */
  std::string GetFilename() const;

  /**
   * @brief Return load policy.
   *
   * @return resource load policy
   */
  LoadPolicy GetLoadPolicy () const;

  /**
   * @brief Return resource release policy.
   *
   * @return resource release policy
   */
  ReleasePolicy GetReleasePolicy () const;

  /**
   * @brief Reload image from filesystem.
   *
   * The set ImageAttributes are used when requesting the image again.
   * @note if Image is offstage and OnDemand policy is set, reload request is ignored.
   */
  void Reload();

  /**
   * @brief Returns the width of the image.
   *
   * Only to be used after the image has finished loading.
   * (Ticket's LoadingSucceeded callback was called)
   * The returned value will reflect the true image dimensions once the asynchronous loading has finished.
   * Connect to SignalLoadingFinished or use GetLoadingState to make sure this value is actual.
   *
   * @return width of the image in pixels.
   */
  unsigned int GetWidth() const;

  /**
   * @brief Returns the height of the image.
   *
   * Only to be used after the image has finished loading.
   * (Ticket's LoadingSucceeded callback was called)
   * The returned value will reflect the true image dimensions once the asynchronous loading has finished.
   * Connect to SignalLoadingFinished or use GetLoadingState to make sure this value is actual.
   *
   * @return height of the image in pixels.
   */
  unsigned int GetHeight() const;

  /**
   * @brief Get the attributes of an image.
   *
   * Only to be used after the image has finished loading.
   * (Ticket's LoadingSucceeded callback was called)
   * The returned value will reflect the true image dimensions once the asynchronous loading has finished.
   * Connect to SignalLoadingFinished or use GetLoadingState to make sure this value is actual.
   * @pre image should be loaded
   * @return a copy of the attributes
   */
  ImageAttributes GetAttributes() const;

public: // Signals

  /**
   * @brief Emitted when the image data loads successfully, or when the loading fails.
   *
   * @return A signal object to Connect() with.
   */
  ImageSignalV2& LoadingFinishedSignal();

  /**
   * @brief This signal is emitted when the image data gets uploaded to GL.
   *
   * It Will be sent after an actor using the image is added to
   * the stage, when such a staged image is reloaded, or when a staged
   * BitmapImage calls Update().
   * @return A signal object to Connect() with.
   */
  ImageSignalV2& UploadedSignal();

public: // Not intended for application developers

  explicit DALI_INTERNAL Image(Internal::Image*);
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_IMAGE_H__

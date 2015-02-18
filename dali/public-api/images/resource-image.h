#ifndef __DALI_RESOURCE_IMAGE_H__
#define __DALI_RESOURCE_IMAGE_H__

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/common/loading-state.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
struct Vector2;
class ImageAttributes;

namespace Internal DALI_INTERNAL
{
class ResourceImage;
}

/**
 * @brief ResourceImage is an image loaded using a URL
 *
 * <h3>ResourceImage Loading</h3>
 *
 * When the ResourceImage is created, resource loading will be attempted unless
 * the ResourceImage is created with IMMEDIATE loading policy or a compatible resource is found in cache.
 * In case of loading images ON_DEMAND, resource loading will only be attempted if the associated ImageActor
 * is put on Stage.
 * Custom loading requests can be made by providing an ImageAttributes object to ResourceImage::New().
 *
 * <i>LoadPolicies</i>
 * - IMMEDIATE: acquire image resource when creating ResourceImage.
 * - ON_DEMAND: only load in case the associated ImageActor is put on Stage
 *
 * <i>Resolution of conflicting policies</i>
 * If the same image is created more than once with conflicting policies, LoadPolicy "IMMEDIATE" overrides "ON_DEMAND".
 *
 * <i>Custom load requests</i>
 * Size, scaling mode, orientation compensation can be set when requesting an image.
 * See ImageAttributes for more details.
 *
 * <i>Compatible resources</i>
 *
 * Before loading a new ResourceImage the internal image resource cache is checked by dali.
 * If there is an image already loaded in memory and is deemed "compatible" with the requested image,
 * that resource is reused.
 * This happens for example if a loaded image exists with the same URL, and the difference between both
 * of the dimensions is less than 50%.
 *
 * <i>Reloading images</i>
 *
 * The same request used on creating the ResourceImage is re-issued when reloading images.
 * If the file changed since the last load operation, this might result in a different resource.
 * Reload only takes effect if both of these conditions apply:
 * - The ResourceImage has already finished loading
 * - The ResourceImage is either on Stage or using IMMEDIATE load policy
 */
class DALI_IMPORT_API ResourceImage : public Image
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
    IMMEDIATE, ///< load image once it is created (default)
    ON_DEMAND  ///< delay loading until the image is being used (a related actor is added to Stage)
  };

  /**
   * @brief Type of signal for LoadingFinished and Uploaded.
   */
  typedef Signal< void (ResourceImage) > ResourceImageSignal;

  // Signal Names
  static const char* const SIGNAL_IMAGE_LOADING_FINISHED; ///< Name of LoadingFinished signal

public:

  /**
   * @brief Get the size of an image from disk.
   *
   * This function will read the header info from file on disk and is
   * synchronous, so it should not be used repeatedly or in tight
   * loops.
   *
   * @param [in] url The URL of the image file.
   * @return The width and height in pixels of the image.
   */
  static Vector2 GetImageSize( const std::string& url );

  /**
   * @brief Constructor which creates an empty ResourceImage object.
   *
   * Use ResourceImage::New(...) to create an initialised object.
   */
  ResourceImage();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ResourceImage();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  ResourceImage( const ResourceImage& handle );

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  ResourceImage& operator=( const ResourceImage& rhs );

  /**
   * @brief Create an initialised ResourceImage object.
   *
   * @param [in] url The URL of the image file to use.
   * @return A handle to a newly allocated object
   */
  static ResourceImage New( const std::string& url );

  /**
   * @brief Create an initialised ResourceImage object.
   *
   * @param [in] url The URL of the image file to use.
   * @param [in] loadPol    The LoadPolicy to apply when loading the image resource.
   * @param [in] releasePol The ReleasePolicy to apply to Image.
   * @return A handle to a newly allocated object
   */
  static ResourceImage New( const std::string& url, LoadPolicy loadPol, ReleasePolicy releasePol );

  /**
   * @brief Create an initialised ResourceImage object.
   *
   * @param [in] url The URL of the image file to use.
   * @param [in] attributes Requested parameters for loading (size, scaling etc.).
   * @return A handle to a newly allocated object
   */
  static ResourceImage New( const std::string& url, const ImageAttributes& attributes );

  /**
   * @brief Create an initialised ResourceImage object.
   *
   * @param [in] url The URL of the image file to use.
   * @param [in] attributes Requested parameters for loading (size, scaling etc.).
   * @param [in] loadPol    The LoadPolicy to apply when loading the image resource.
   * @param [in] releasePol The ReleasePolicy to apply to Image.
   * @return A handle to a newly allocated object
   */
  static ResourceImage New( const std::string& url, const ImageAttributes& attributes, LoadPolicy loadPol, ReleasePolicy releasePol );

  /**
   * @brief Downcast an Object handle to ResourceImage handle.
   *
   * If handle points to a ResourceImage object the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a Image object or an uninitialized handle
   */
  static ResourceImage DownCast( BaseHandle handle );

  /**
   * @brief Return load policy.
   *
   * @return resource load policy
   */
  LoadPolicy GetLoadPolicy() const;

  /**
   * @brief Query whether the image data has loaded.
   *
   * The asynchronous loading begins when the Image object is created.
   * After the Image object is discarded, the image data will be released from memory.
   * @return The loading state, either Loading, Success or Failed.
   */
  LoadingState GetLoadingState() const;

  /**
   * @brief Returns the URL of the image.
   *
   * @return The URL of the image file.
   */
  std::string GetUrl() const;

  /**
   * @brief Reload image from filesystem.
   *
   * The set ImageAttributes are used when requesting the image again.
   * @note if Image is offstage and OnDemand policy is set, reload request is ignored.
   */
  void Reload();

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
  ResourceImageSignal& LoadingFinishedSignal();

public: // Not intended for application developers

  explicit DALI_INTERNAL ResourceImage( Internal::ResourceImage* );
};

} // namespace Dali

#endif // __DALI_RESOURCE_IMAGE_H__

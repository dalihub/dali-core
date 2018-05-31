#ifndef __DALI_RESOURCE_IMAGE_H__
#define __DALI_RESOURCE_IMAGE_H__

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
#include <dali/public-api/common/loading-state.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/signals/dali-signal.h>
#include <dali/public-api/images/image-operations.h>

namespace Dali
{
/**
 * @addtogroup dali_core_images
 * @{
 */

namespace Internal DALI_INTERNAL
{
class ResourceImage;
}

/**
 * @DEPRECATED_1_2.41
 *
 * @brief ResourceImage is an image loaded using a URL.
 *
 * <i>Customizing load requests</i>
 *
 * Size, scaling mode, filter mode, and orientation compensation can be set when requesting an image.
 *
 * <i>Reloading images</i>
 *
 * The same request used on creating the ResourceImage is re-issued when reloading images.
 * If the file changed since the last load operation, this might result in a different resource.
 * Reload only takes effect if the ResourceImage has already finished loading.
 *
 * Signals
 * | %Signal Name         | Method                       |
 * |----------------------|------------------------------|
 * | imageLoadingFinished | @ref LoadingFinishedSignal() |
 * @SINCE_1_0.0
 */
class DALI_CORE_API ResourceImage : public Image
{
public:

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief Type of signal for LoadingFinished and Uploaded.
   * @SINCE_1_0.0
   */
  typedef Signal< void (ResourceImage) > ResourceImageSignal;

public:

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief Gets the size of an image from disk.
   *
   * This function will read the header info from file on disk and is
   * synchronous, so it should not be used repeatedly or in tight
   * loops.
   *
   * @SINCE_1_0.0
   * @REMARK_STORAGE
   * @param[in] url The URL of the image file
   * @return The width and height in pixels of the image
   */
  static ImageDimensions GetImageSize( const std::string& url ) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief Constructor which creates an empty ResourceImage object.
   *
   * Use ResourceImage::New(...) to create an initialised object.
   * @SINCE_1_0.0
   */
  ResourceImage() DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~ResourceImage() DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] handle A reference to the copied handle
   */
  ResourceImage( const ResourceImage& handle ) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  ResourceImage& operator=( const ResourceImage& rhs ) DALI_DEPRECATED_API;

  /**
   * @name ResourceImageFactoryFunctions
   * Creates ResourceImage object instances using these functions.
   */
  ///@{

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief Creates an initialized ResourceImage object.
   *
   * Uses defaults for all options.
   *
   * @sa Dali::FittingMode::Type Dali::SamplingMode::Type
   * @SINCE_1_0.0
   * @REMARK_INTERNET
   * @REMARK_STORAGE
   * @param[in] url The URL of the image file to use
   * @param[in] orientationCorrection Reorient the image to respect any orientation metadata in its header
   * @return A handle to a newly allocated object
   */
  static ResourceImage New( const std::string& url, bool orientationCorrection = true ) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief Creates an initialized ResourceImage object.
   *
   * @SINCE_1_0.0
   * @REMARK_INTERNET
   * @REMARK_STORAGE
   * @param[in] url The URL of the image file to use
   * @param[in] size The width and height to fit the loaded image to
   * @param[in] fittingMode The method used to fit the shape of the image before loading to the shape defined by the size parameter
   * @param[in] samplingMode The filtering method used when sampling pixels from the input image while fitting it to desired size
   * @param[in] orientationCorrection Reorient the image to respect any orientation metadata in its header
   * @return A handle to a newly allocated object
   */
  static ResourceImage New( const std::string& url,
                            ImageDimensions size,
                            FittingMode::Type fittingMode = FittingMode::DEFAULT,
                            SamplingMode::Type samplingMode = SamplingMode::DEFAULT,
                            bool orientationCorrection = true ) DALI_DEPRECATED_API;

  ///@}

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief Downcasts a handle to ResourceImage handle.
   *
   * If handle points to a ResourceImage object, the
   * downcast produces valid handle. If not, the returned handle is left uninitialized.
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return Handle to a Image object or an uninitialized handle
   */
  static ResourceImage DownCast( BaseHandle handle ) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief Queries whether the image data has loaded.
   *
   * The asynchronous loading begins when the Image object is created.
   * After the Image object is discarded, the image data will be released from memory.
   * @SINCE_1_0.0
   * @return The loading state, either Loading, Success or Failed
   */
  LoadingState GetLoadingState() const DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief Returns the URL of the image.
   *
   * @SINCE_1_0.0
   * @return The URL of the image file
   */
  std::string GetUrl() const DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief Reloads image from filesystem.
   *
   * The original set of image loading attributes (requested dimensions, scaling
   * mode and filter mode) are used when requesting the image again.
   * @SINCE_1_0.0
   * @note If image is offstage and OnDemand policy is set, the reload request is ignored.
   */
  void Reload() DALI_DEPRECATED_API;

public: // Signals

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief Emitted when the image data loads successfully, or when the loading fails.
   *
   * @SINCE_1_0.0
   * @return A signal object to Connect() with
   */
  ResourceImageSignal& LoadingFinishedSignal() DALI_DEPRECATED_API;

public: // Not intended for application developers

  explicit DALI_INTERNAL ResourceImage( Internal::ResourceImage* );
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_RESOURCE_IMAGE_H__

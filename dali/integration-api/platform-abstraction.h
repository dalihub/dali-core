#ifndef __DALI_INTEGRATION_PLATFORM_ABSTRACTION_H__
#define __DALI_INTEGRATION_PLATFORM_ABSTRACTION_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include <dali/integration-api/bitmap.h>
#include <dali/integration-api/resource-types.h>
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{

namespace Integration
{
typedef unsigned int ResourceId;
typedef IntrusivePtr<Dali::RefObject> ResourcePointer;

/**
 * PlatformAbstraction is an abstract interface, used by Dali to access platform specific services.
 * A concrete implementation must be created for each platform, and provided when creating the
 * Dali::Integration::Core object.
 */
class PlatformAbstraction
{
public:

  /**
   * Virtual destructor.
   */
  virtual ~PlatformAbstraction() {}

  // Resource Loading

  /**
   * @brief Determine the size of an image the resource loaders will provide when
   * given the same image loading parameters.
   *
   * This is a synchronous request.
   * This function is used to determine the size of an image before it has loaded.
   * @param[in] filename name of the image.
   * @param[in] size The requested size for the image.
   * @param[in] fittingMode The method to use to map the source image to the desired
   * dimensions.
   * @param[in] samplingMode The image filter to use if the image needs to be
   * downsampled to the requested size.
   * @param[in] orientationCorrection Whether to use image metadata to rotate or
   * flip the image, e.g., from portrait to landscape.
   * @return dimensions that image will have if it is loaded with given parameters.
   */
  virtual ImageDimensions GetClosestImageSize( const std::string& filename,
                                               ImageDimensions size = ImageDimensions( 0, 0 ),
                                               FittingMode::Type fittingMode = FittingMode::SHRINK_TO_FIT,
                                               SamplingMode::Type samplingMode = SamplingMode::BOX,
                                               bool orientationCorrection = true) = 0;

  /**
   * @brief Determine the size of an image the resource loaders will provide when
   * given the same image loading parameters.
   *
   * This is a synchronous request.
   * This function is used to determine the size of an image before it has loaded.
   * @param[in] filename name of the image.
   * @param[in] size The requested size for the image.
   * @param[in] fittingMode The method to use to map the source image to the desired
   * dimensions.
   * @param[in] samplingMode The image filter to use if the image needs to be
   * downsampled to the requested size.
   * @param[in] orientationCorrection Whether to use image metadata to rotate or
   * flip the image, e.g., from portrait to landscape.
   * @return dimensions that image will have if it is loaded with given parameters.
   */
  virtual ImageDimensions GetClosestImageSize( ResourcePointer resourceBuffer,
                                               ImageDimensions size = ImageDimensions( 0, 0 ),
                                               FittingMode::Type fittingMode = FittingMode::SHRINK_TO_FIT,
                                               SamplingMode::Type samplingMode = SamplingMode::BOX,
                                               bool orientationCorrection = true) = 0;

  /**
   * Request a resource from the native filesystem. This is a synchronous request, i.e.
   * it will block the main loop whilst executing. It should therefore be used sparingly.
   *
   * Multi-threading note: this method will be called from the main thread only i.e. not
   * from within the Core::Render() method.
   * @param[in] resourceType The type of resource to load
   * @param[in] resourcePath The path to the resource
   * @return A pointer to a ref-counted resource
   */
  virtual ResourcePointer LoadResourceSynchronously( const ResourceType& resourceType, const std::string& resourcePath ) = 0;

  /**
   * Decode a buffer of data synchronously.
   * @param[in] resourceType The type of resource to load
   * @param[in] buffer The decoded data
   * @param[in] bufferSize The size of the buffer used by the decoded data.
   *
   * @return A pointer to the decoded buffer.
   */
  virtual BitmapPtr DecodeBuffer( const ResourceType& resourceType, uint8_t * buffer, size_t bufferSize ) = 0;

  /**
   * Load a shader binary file into a buffer
   * @param[in] filename The shader binary filename to load
   * @param[out] buffer  A buffer to receive the file.
   * @result             true if the file is loaded.
   */
  virtual bool LoadShaderBinaryFile( const std::string& filename, Dali::Vector< unsigned char >& buffer ) const = 0;

  /**
   * Save a shader binary file to the resource file system.
   * @param[in] filename The shader binary filename to save to.
   * @param[in] buffer  A buffer to write the file from.
   * @param[in] numbytes Size of the buffer.
   * @result             true if the file is saved, else false.
   */
  virtual bool SaveShaderBinaryFile( const std::string& filename, const unsigned char * buffer, unsigned int numBytes ) const = 0;

}; // class PlatformAbstraction

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_PLATFORM_ABSTRACTION_H__

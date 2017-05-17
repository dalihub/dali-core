#ifndef __DALI_INTEGRATION_RESOURCE_TYPES_H__
#define __DALI_INTEGRATION_RESOURCE_TYPES_H__

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <stdint.h>
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/math/uint-16-pair.h>
#include <dali/public-api/math/vector2.h>

namespace Dali
{

typedef Uint16Pair ImageDimensions;

namespace Integration
{

// Resource Types

/**
 * Extendable set of resource types
 */
enum ResourceTypeId
{
  ResourceBitmap
};

/**
 * The abstract base class for resource types.
 */
struct ResourceType
{
  /**
   * Constructor.
   * @param[in] typeId resource type id
   */
  ResourceType(ResourceTypeId typeId)
  : id(typeId) {}

  /**
   * Destructor.
   */
  virtual ~ResourceType() {}

  /**
   * Create a copy of the resource type with the same attributes.
   * @return pointer to the new ResourceType.
   */
  virtual ResourceType* Clone() const = 0;

  const ResourceTypeId id;

private:

  // Undefined copy constructor.
  ResourceType(const ResourceType& typePath);

  // Undefined assignment operator.
  ResourceType& operator=(const ResourceType& rhs);
};

/**
 * BitmapResourceType describes a bitmap resource, which can be requested
 * from ResourceLoader::LoadResource() or AllocateBitmapImage.
 */
struct BitmapResourceType : public ResourceType
{
  /**
   * Constructor.
   * @param[in] size The requested size for the bitmap.
   * @param[in] scalingMode The method to use to map the source bitmap to the desired
   * dimensions.
   * @param[in] samplingMode The filter to use if the bitmap needs to be downsampled
   * to the requested size.
   * @param[in] orientationCorrection Whether to use bitmap metadata to rotate or
   * flip the bitmap, e.g., from portrait to landscape.
   */
  BitmapResourceType( ImageDimensions size = ImageDimensions( 0, 0 ),
                      FittingMode::Type scalingMode = FittingMode::DEFAULT,
                      SamplingMode::Type samplingMode = SamplingMode::DEFAULT,
                      bool orientationCorrection = true )
  : ResourceType(ResourceBitmap),
    size(size), scalingMode(scalingMode), samplingMode(samplingMode), orientationCorrection(orientationCorrection) {}

  /**
   * Destructor.
   */
  virtual ~BitmapResourceType() {}

  /**
   * @copydoc ResourceType::Clone
   */
  virtual ResourceType* Clone() const
  {
    return new BitmapResourceType( size, scalingMode, samplingMode, orientationCorrection );
  }

  /**
   * Attributes are copied from the request.
   */
  ImageDimensions size;
  FittingMode::Type scalingMode;
  SamplingMode::Type samplingMode;
  bool orientationCorrection;

private:

  // Undefined copy constructor.
  BitmapResourceType(const BitmapResourceType& typePath);

  // Undefined assignment operator.
  BitmapResourceType& operator=(const BitmapResourceType& rhs);
};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_RESOURCE_TYPES_H__

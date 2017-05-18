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

// INTERNAL INCLUDES
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/math/uint-16-pair.h>

namespace Dali
{

typedef Uint16Pair ImageDimensions;

namespace Integration
{

/**
 * BitmapResourceType describes a bitmap resource.
 */
struct BitmapResourceType
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
  : size( size ),
    scalingMode( scalingMode ),
    samplingMode( samplingMode ),
    orientationCorrection( orientationCorrection )
  {}

  /**
   * Destructor.
   */
  ~BitmapResourceType()
  {}

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

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

// CLASS HEADER
#include <dali/internal/event/resources/resource-type-path.h>

using namespace Dali::Integration;

namespace Dali
{

namespace Internal
{

namespace
{

/**
 * @brief Compare two sets of image loading parameters for equality.
 */
inline bool AttributesEqual( ImageDimensions aDims, FittingMode::Type aScaling, SamplingMode::Type aSampling, bool aOrient, ImageDimensions bDims, FittingMode::Type bScaling, SamplingMode::Type bSampling, bool bOrient )
{
  return aDims     == bDims &&
         aScaling  == bScaling &&
         aSampling == bSampling &&
         aOrient   == bOrient;
}

/**
 * @brief Compare two sets of image loading parameters
 * @pre The two sets are not identical.
 */
inline bool AttributesLessAssumingNotEqual( ImageDimensions aDims, FittingMode::Type aScaling, SamplingMode::Type aSampling, bool aOrient, ImageDimensions bDims, FittingMode::Type bScaling, SamplingMode::Type bSampling, bool bOrient )
{
  return aDims     < bDims &&
         aScaling  < bScaling &&
         aSampling < bSampling &&
         aOrient   < bOrient;
}

/**
 * Compare two resource types.
 * @return zero if the types are equal,
 *         less than zero if lhs < rhs or
 *         greater than zero if lhs > rhs
 */
int ResourceTypeCompare(const ResourceType& lhs, const ResourceType& rhs)
{
  int result = 0;

  if (lhs.id != rhs.id)
  {
    result = lhs.id > rhs.id ? 1 : -1;
  }
  else // lhs.id == rhs.id
  {
    // Cast to a derived resource type and compare...

    switch (lhs.id)
    {
      case ResourceBitmap:
      {
        // compare bitmap widths & heights
        const BitmapResourceType& lhsBitmap = static_cast<const BitmapResourceType&>(lhs);
        const BitmapResourceType& rhsBitmap = static_cast<const BitmapResourceType&>(rhs);

        if( ! AttributesEqual( lhsBitmap.size, lhsBitmap.scalingMode, lhsBitmap.samplingMode, lhsBitmap.orientationCorrection,
                               rhsBitmap.size, rhsBitmap.scalingMode, rhsBitmap.samplingMode, rhsBitmap.orientationCorrection ) )
        {
          result = AttributesLessAssumingNotEqual( lhsBitmap.size, lhsBitmap.scalingMode, lhsBitmap.samplingMode, lhsBitmap.orientationCorrection,
                                                   rhsBitmap.size, rhsBitmap.scalingMode, rhsBitmap.samplingMode, rhsBitmap.orientationCorrection );
        }
        // else result = 0
        break;
      }

      case ResourceNativeImage:
      {
        // compare native image widths & heights

        const NativeImageResourceType& lhsNativeImage = static_cast<const NativeImageResourceType&>(lhs);
        const NativeImageResourceType& rhsNativeImage = static_cast<const NativeImageResourceType&>(rhs);

        if (lhsNativeImage.imageDimensions != rhsNativeImage.imageDimensions)
        {
          result = lhsNativeImage.imageDimensions < rhsNativeImage.imageDimensions ? -1 : 1;
        }
        // else result = 0
        break;
      }

      case ResourceTargetImage:
      {
        // compare bitmap widths & heights
        const RenderTargetResourceType& lhsImage = static_cast<const RenderTargetResourceType&>(lhs);
        const RenderTargetResourceType& rhsImage = static_cast<const RenderTargetResourceType&>(rhs);

        if (lhsImage.imageDimensions != rhsImage.imageDimensions)
        {
          result = lhsImage.imageDimensions < rhsImage.imageDimensions ? -1 : 1;
        }
        // else result = 0
        break;
      }

      case ResourceShader:
      {
        // compare shader source hashes
        const ShaderResourceType& lhsShader = static_cast<const ShaderResourceType&>(lhs);
        const ShaderResourceType& rhsShader = static_cast<const ShaderResourceType&>(rhs);

        if (lhsShader.hash != rhsShader.hash)
        {
          result = lhsShader.hash < rhsShader.hash ? -1 : 1;
        }
        // else result = 0
        break;
      }



    }
  }
  return result;
}
}

bool ResourceTypePath::operator<(const ResourceTypePath& rhs) const
{
  int typeComparison = ResourceTypeCompare(*type, *(rhs.type));

  if (typeComparison != 0)
  {
    return (typeComparison < 0);
  }
  else
  {
    return path < rhs.path;
  }
}

} // namespace Internal

} // namespace Dali

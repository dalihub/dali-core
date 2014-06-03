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
#include <dali/public-api/images/image-attributes.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/math-utils.h>

namespace Dali
{

const ImageAttributes ImageAttributes::DEFAULT_ATTRIBUTES;

struct ImageAttributes::ImageAttributesImpl
{
  ImageAttributesImpl()
  :  width(0),
     height(0),
     scaling(ShrinkToFit),
     pixelformat(Pixel::RGBA8888),
     crop(0.0f,0.0f,1.0f,1.0f),
     mOrientationCorrection(false),
     isDistanceField(false),
     fieldRadius(4.0f),
     fieldBorder(4)
  {
  }

  ~ImageAttributesImpl()
  {
  }

  ImageAttributesImpl(const ImageAttributesImpl& rhs)
  : width( rhs.width ),
    height( rhs.height ),
    scaling( rhs.scaling ),
    pixelformat( rhs.pixelformat ),
    crop( rhs.crop ),
    mOrientationCorrection( rhs.mOrientationCorrection ),
    isDistanceField( rhs.isDistanceField ),
    fieldRadius( rhs.fieldRadius ),
    fieldBorder( rhs.fieldBorder )
  {
  }

  ImageAttributesImpl& operator=(const ImageAttributesImpl& rhs)
  {
    if (this != &rhs)
    {
      width = rhs.width;
      height = rhs.height;
      scaling = rhs.scaling;
      pixelformat = rhs.pixelformat;
      crop = rhs.crop;
      mOrientationCorrection = rhs.mOrientationCorrection;
      isDistanceField = rhs.isDistanceField;
      fieldRadius = rhs.fieldRadius;
      fieldBorder = rhs.fieldBorder;
    }

    return *this;
  }

  unsigned int  width;            ///< image width in pixels
  unsigned int  height;           ///< image height in pixels
  ScalingMode   scaling;          ///< scaling option, ShrinkToFit is default
  Pixel::Format pixelformat;      ///< pixel format, default is RGBA8888

  /**
   * Cropping options, relative to image actual size.
   * (0.0, 0.0) is top left corner, (1.0, 1.0) is the full width and height
   * defaults are (0,0,1,1) so that whole image is loaded
   * (0.25, 0.25, 0.5, 0.5) would mean that 50% of the image is loaded from the middle
   */
  Rect<float>   crop;

  bool          mOrientationCorrection; ///< If true, image pixels are reordered according to orientation metadata on load.

  // For distance fields :
  bool          isDistanceField;  ///< true, if the image is a distancefield. Default is false.
  float         fieldRadius;      ///< The minimum search radius to check for differing pixels
  int           fieldBorder;      ///< The amount of distancefield cells to add around the data (for glow/shadow effects)
};


ImageAttributes::ImageAttributes()
: impl( new ImageAttributesImpl() )
{
}

ImageAttributes::ImageAttributes(const ImageAttributes& rhs)
: impl( new ImageAttributesImpl(*rhs.impl) )
{
}

ImageAttributes& ImageAttributes::operator=(const ImageAttributes& rhs)
{
  *impl = *rhs.impl;

  return *this;
}

ImageAttributes::~ImageAttributes()
{
  delete impl;
}

void ImageAttributes::SetSize(unsigned int width, unsigned int height)
{
  impl->width = width;
  impl->height = height;
}

void ImageAttributes::SetSize( const Size& size )
{
  impl->width = size.width;
  impl->height = size.height;
}

void ImageAttributes::SetPixelFormat(Pixel::Format format)
{
  impl->pixelformat = format;
}

void ImageAttributes::SetScalingMode(ScalingMode scale)
{
  impl->scaling = scale;
}

void ImageAttributes::SetCrop(const Rect<float>& cropRect)
{
  impl->crop = cropRect;
}

void ImageAttributes::SetOrientationCorrection(const bool enabled)
{
  impl->mOrientationCorrection = enabled;
}

unsigned int ImageAttributes::GetWidth() const
{
  return impl->width;
}

unsigned int ImageAttributes::GetHeight() const
{
  return impl->height;
}

Size ImageAttributes::GetSize() const
{
  return Size(impl->width, impl->height);
}

Pixel::Format ImageAttributes::GetPixelFormat() const
{
  return impl->pixelformat;
}

ImageAttributes::ScalingMode ImageAttributes::GetScalingMode() const
{
  return impl->scaling;
}

bool ImageAttributes::IsDistanceField() const
{
  return impl->isDistanceField;
}

int ImageAttributes::GetFieldBorder() const
{
  return impl->fieldBorder;
}

float ImageAttributes::GetFieldRadius() const
{
  return impl->fieldRadius;
}

const Rect<float>& ImageAttributes::GetCrop() const
{
  return impl->crop;
}

bool ImageAttributes::GetOrientationCorrection() const
{
  return impl->mOrientationCorrection;
}

ImageAttributes ImageAttributes::New()
{
  return ImageAttributes();
}

ImageAttributes ImageAttributes::New(unsigned int imageWidth, unsigned int imageHeight, Pixel::Format format)
{
  ImageAttributes attributes;
  attributes.impl->width = imageWidth;
  attributes.impl->height = imageHeight;
  attributes.impl->pixelformat = format;
  return attributes;
}

ImageAttributes ImageAttributes::NewDistanceField()
{
  ImageAttributes attributes;
  attributes.impl->isDistanceField = true;
  attributes.impl->fieldRadius = 4.0f;
  attributes.impl->fieldBorder = 4;
  return attributes;
}

ImageAttributes ImageAttributes::NewDistanceField(float fieldRadius, int fieldBorder)
{
  ImageAttributes attributes;
  attributes.impl->isDistanceField = true;
  attributes.impl->fieldRadius = fieldRadius;
  attributes.impl->fieldBorder = fieldBorder;
  return attributes;
}

/**
 * Less then comparison operator.
 * @param [in] a parameter tested
 * @param [in] b parameter tested
 */
bool operator<(const ImageAttributes& a, const ImageAttributes& b)
{
  // Bail out if one is distance field and the other is not.
  if (a.impl->isDistanceField != b.impl->isDistanceField)
  {
    return a.impl->isDistanceField < b.impl->isDistanceField;
  }

  if (a.impl->width != b.impl->width)
  {
    return a.impl->width < b.impl->width;
  }

  if (a.impl->height != b.impl->height)
  {
    return a.impl->height < b.impl->height;
  }

  float a_area = a.impl->crop.Area();
  float b_area = b.impl->crop.Area();
  if (fabsf(a_area - b_area) > GetRangedEpsilon(a_area, b_area))
  {
    return a_area < b_area;
  }

  if (a.impl->mOrientationCorrection != b.impl->mOrientationCorrection)
  {
    return a.impl->mOrientationCorrection < b.impl->mOrientationCorrection;
  }

  if (a.impl->pixelformat != b.impl->pixelformat)
  {
    return a.impl->pixelformat < b.impl->pixelformat;
  }

  if (a.impl->scaling != b.impl->scaling)
  {
    return a.impl->scaling < b.impl->scaling;
  }

  if (a.impl->isDistanceField && b.impl->isDistanceField)
  {
    if (fabs(a.impl->fieldRadius - b.impl->fieldRadius) > Math::MACHINE_EPSILON_0)
    {
      return a.impl->fieldRadius < b.impl->fieldRadius;
    }

    if (a.impl->fieldBorder != b.impl->fieldBorder)
    {
      return a.impl->fieldBorder < b.impl->fieldBorder;
    }
  }

  // they are equal
  return false;
}

/**
 * Equal to comparison operator.
 * @param [in] a parameter tested for equality
 * @param [in] b parameter tested for equality
 */
bool operator==(const ImageAttributes& a, const ImageAttributes& b)
{
  return a.impl->width                  == b.impl->width       &&
         a.impl->height                 == b.impl->height      &&
         a.impl->crop                   == b.impl->crop        &&
         a.impl->mOrientationCorrection == b.impl->mOrientationCorrection &&
         a.impl->pixelformat            == b.impl->pixelformat &&
         a.impl->scaling                == b.impl->scaling     &&
         a.impl->isDistanceField  == b.impl->isDistanceField &&
         fabs(a.impl->fieldRadius -  b.impl->fieldRadius) < Math::MACHINE_EPSILON_0 &&
         a.impl->fieldBorder      == b.impl->fieldBorder;
}

/**
 * Not equal to comparison operator.
 * @param [in] a parameter tested for equality
 * @param [in] b parameter tested for equality
 */
bool operator!=(const ImageAttributes& a, const ImageAttributes& b)
{
  return !(a == b);
}

} // namespace Dali

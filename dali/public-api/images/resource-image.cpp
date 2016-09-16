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

// CLASS HEADER
#include <dali/public-api/images/resource-image.h>

// INTERNAL INCLUDES
#include <dali/internal/common/image-attributes.h>
#include <dali/internal/event/images/resource-image-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/integration-api/platform-abstraction.h>

namespace Dali
{

ImageDimensions ResourceImage::GetImageSize( const std::string& url )
{
  const Internal::ImageAttributes& attribs = Internal::ImageAttributes::DEFAULT_ATTRIBUTES;
  const ImageDimensions desiredSize = ImageDimensions( attribs.GetWidth(), attribs.GetHeight() );
  const ImageDimensions size = Internal::ThreadLocalStorage::Get().GetPlatformAbstraction().GetClosestImageSize( url, desiredSize, attribs.GetScalingMode(), attribs.GetFilterMode(), attribs.GetOrientationCorrection() );
  return size;
}

ResourceImage::ResourceImage()
{
}

ResourceImage::ResourceImage(Internal::ResourceImage* internal) : Image(internal)
{
}

ResourceImage::~ResourceImage()
{
}

ResourceImage::ResourceImage( const ResourceImage& handle )
: Image(handle)
{
}

ResourceImage& ResourceImage::operator=( const ResourceImage& rhs )
{
  BaseHandle::operator=(rhs);
  return *this;
}

ResourceImage ResourceImage::New( const std::string& url, bool orientationCorrection )
{
  Internal::ImageAttributes attributes = Internal::ImageAttributes::DEFAULT_ATTRIBUTES;
  attributes.SetOrientationCorrection( orientationCorrection );
  return ResourceImage( Internal::ResourceImage::New( url, attributes ).Get() );
}

ResourceImage ResourceImage::New( const std::string& url, ImageDimensions size, FittingMode::Type scalingMode, SamplingMode::Type samplingMode, bool orientationCorrection )
{
  Internal::ImageAttributes attributes = Internal::ImageAttributes::DEFAULT_ATTRIBUTES;
  attributes.SetSize( Size( size.GetWidth(), size.GetHeight() ) );
  attributes.SetScalingMode( scalingMode );
  attributes.SetFilterMode( samplingMode );
  attributes.SetOrientationCorrection( orientationCorrection );
  return ResourceImage( Internal::ResourceImage::New( url, attributes ).Get() );
}

ResourceImage ResourceImage::DownCast( BaseHandle handle )
{
  return ResourceImage( dynamic_cast<Dali::Internal::ResourceImage*>(handle.GetObjectPtr()) );
}

LoadingState ResourceImage::GetLoadingState() const
{
  return GetImplementation(*this).GetLoadingState();
}

std::string ResourceImage::GetUrl() const
{
  return GetImplementation(*this).GetUrl();
}

void ResourceImage::Reload()
{
  GetImplementation(*this).Reload();
}

ResourceImage::ResourceImageSignal& ResourceImage::LoadingFinishedSignal()
{
  return GetImplementation(*this).LoadingFinishedSignal();
}

} // namespace Dali

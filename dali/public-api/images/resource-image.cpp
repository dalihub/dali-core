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
#include <dali/public-api/images/image-attributes.h>
#include <dali/public-api/math/vector2.h>
#include <dali/internal/event/images/resource-image-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/integration-api/platform-abstraction.h>

namespace Dali
{

const char* const ResourceImage::SIGNAL_IMAGE_LOADING_FINISHED = "image-loading-finished";

Vector2 ResourceImage::GetImageSize( const std::string& url )
{
  Vector2 size;
  Internal::ThreadLocalStorage::Get().GetPlatformAbstraction().GetClosestImageSize( url, ImageAttributes::DEFAULT_ATTRIBUTES, size );
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

ResourceImage ResourceImage::New( const std::string& url )
{
  Internal::ResourceImagePtr internal = Internal::ResourceImage::New( url,
                                                         Dali::ImageAttributes::DEFAULT_ATTRIBUTES );
  return ResourceImage(internal.Get());
}

ResourceImage ResourceImage::New( const std::string& url, LoadPolicy loadPol, ReleasePolicy releasePol )
{
  Internal::ResourceImagePtr internal = Internal::ResourceImage::New( url,
                                                         Dali::ImageAttributes::DEFAULT_ATTRIBUTES,
                                                         loadPol, releasePol );
  return ResourceImage(internal.Get());
}

ResourceImage ResourceImage::New( const std::string& url, const ImageAttributes& attributes )
{
  Internal::ResourceImagePtr internal = Internal::ResourceImage::New( url, attributes );
  return ResourceImage(internal.Get());
}

ResourceImage ResourceImage::New( const std::string& url, const ImageAttributes& attributes, LoadPolicy loadPol, ReleasePolicy releasePol )
{
  Internal::ResourceImagePtr internal = Internal::ResourceImage::New( url, attributes, loadPol, releasePol );
  return ResourceImage(internal.Get());
}

ResourceImage ResourceImage::DownCast( BaseHandle handle )
{
  return ResourceImage( dynamic_cast<Dali::Internal::ResourceImage*>(handle.GetObjectPtr()) );
}

ResourceImage::LoadPolicy ResourceImage::GetLoadPolicy() const
{
  return GetImplementation(*this).GetLoadPolicy();
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

ImageAttributes ResourceImage::GetAttributes() const
{
  return GetImplementation(*this).GetAttributes();
}

ResourceImage::ResourceImageSignal& ResourceImage::LoadingFinishedSignal()
{
  return GetImplementation(*this).LoadingFinishedSignal();
}

} // namespace Dali

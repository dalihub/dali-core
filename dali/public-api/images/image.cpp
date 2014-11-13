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
#include <dali/public-api/images/image.h>

// INTERNAL INCLUDES
#include <dali/public-api/images/image-attributes.h>
#include <dali/public-api/math/vector2.h>
#include <dali/internal/event/images/image-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/integration-api/platform-abstraction.h>

namespace Dali
{

const char* const Image::SIGNAL_IMAGE_LOADING_FINISHED = "image-loading-finished";
const char* const Image::SIGNAL_IMAGE_UPLOADED = "uploaded";

Vector2 Image::GetImageSize(const std::string fileName)
{
  Vector2 size;
  Internal::ThreadLocalStorage::Get().GetPlatformAbstraction().GetClosestImageSize(fileName, ImageAttributes::DEFAULT_ATTRIBUTES, size);
  return size;
}

Image::Image()
{
}

Image::Image(Internal::Image* internal) : BaseHandle(internal)
{
}

Image::~Image()
{
}

Image::Image(const Image& handle)
: BaseHandle(handle)
{
}

Image& Image::operator=(const Image& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

Image Image::New(const std::string& filename)
{
  Internal::ImagePtr internal = Internal::Image::New( filename,
                                                      Dali::ImageAttributes::DEFAULT_ATTRIBUTES );
  return Image(internal.Get());
}

Image Image::New(const std::string& filename, LoadPolicy loadPol, ReleasePolicy releasePol)
{
  Internal::ImagePtr internal = Internal::Image::New( filename,
                                                      Dali::ImageAttributes::DEFAULT_ATTRIBUTES,
                                                      loadPol, releasePol );
  return Image(internal.Get());
}

Image Image::New(const std::string& filename, const ImageAttributes& attributes)
{
  Internal::ImagePtr internal = Internal::Image::New(filename, attributes);
  return Image(internal.Get());
}

Image Image::New(const std::string& filename, const ImageAttributes& attributes, LoadPolicy loadPol, ReleasePolicy releasePol)
{
  Internal::ImagePtr internal = Internal::Image::New(filename, attributes, loadPol, releasePol);
  return Image(internal.Get());
}

Image Image::NewDistanceField(const std::string& filename)
{
  ImageAttributes attributes = ImageAttributes::NewDistanceField();
  Internal::ImagePtr internal = Internal::Image::New(filename, attributes);
  return Image(internal.Get());
}

Image Image::NewDistanceField(const std::string& filename, LoadPolicy loadPol, ReleasePolicy releasePol)
{
  ImageAttributes attributes = ImageAttributes::NewDistanceField();
  Internal::ImagePtr internal = Internal::Image::New(filename, attributes, loadPol, releasePol);
  return Image(internal.Get());
}

Image Image::NewDistanceField(const std::string& filename, const ImageAttributes& attributes)
{
  DALI_ASSERT_DEBUG(attributes.IsDistanceField());

  Internal::ImagePtr internal = Internal::Image::New(filename, attributes);
  return Image(internal.Get());
}

Image Image::NewDistanceField(const std::string& filename, const ImageAttributes& attributes, LoadPolicy loadPol, ReleasePolicy releasePol)
{
  DALI_ASSERT_DEBUG(attributes.IsDistanceField());

  Internal::ImagePtr internal = Internal::Image::New(filename, attributes, loadPol, releasePol);
  return Image(internal.Get());
}

Image Image::New(NativeImage& nativeImg)
{
  Internal::ImagePtr internal = Internal::Image::New(nativeImg);
  return Image(internal.Get());
}

Image Image::DownCast( BaseHandle handle )
{
  return Image( dynamic_cast<Dali::Internal::Image*>(handle.GetObjectPtr()) );
}

LoadingState Image::GetLoadingState() const
{
  return GetImplementation(*this).GetLoadingState();
}

ImageAttributes Image::GetAttributes() const
{
  return GetImplementation(*this).GetAttributes();
}

std::string Image::GetFilename() const
{
  return GetImplementation(*this).GetFilename();
}

Image::LoadPolicy Image::GetLoadPolicy() const
{
  return GetImplementation(*this).GetLoadPolicy();
}

Image::ReleasePolicy Image::GetReleasePolicy() const
{
  return GetImplementation(*this).GetReleasePolicy();
}

unsigned int Image::GetWidth() const
{
  return GetImplementation(*this).GetWidth();
}

unsigned int Image::GetHeight() const
{
  return GetImplementation(*this).GetHeight();
}

void Image::Reload()
{
  GetImplementation(*this).Reload();
}

Image::ImageSignalV2& Image::LoadingFinishedSignal()
{
  return GetImplementation(*this).LoadingFinishedSignal();
}

Image::ImageSignalV2& Image::UploadedSignal()
{
  return GetImplementation(*this).UploadedSignal();
}

} // namespace Dali

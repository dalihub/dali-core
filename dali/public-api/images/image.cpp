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
#include <dali/public-api/images/image.h>

// INTERNAL INCLUDES
#include <dali/internal/event/images/image-impl.h>


namespace Dali
{

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

Image Image::DownCast( BaseHandle handle )
{
  return Image( dynamic_cast<Dali::Internal::Image*>(handle.GetObjectPtr()) );
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

Image::ImageSignalType& Image::UploadedSignal()
{
  return GetImplementation(*this).UploadedSignal();
}

} // namespace Dali

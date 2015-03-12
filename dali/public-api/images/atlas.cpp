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
#include <dali/public-api/images/atlas.h>

// INTERNAL INCLUDES
#include <dali/internal/event/images/atlas-impl.h>
#include <dali/internal/event/images/buffer-image-impl.h>

namespace Dali
{

Atlas Atlas::New( SizeType width,
                  SizeType height,
                  Pixel::Format pixelFormat,
                  bool recoverContext )
{
  DALI_ASSERT_ALWAYS( 0u != width  && "Invalid Atlas width requested" );
  DALI_ASSERT_ALWAYS( 0u != height && "Invalid Atlas height requested" );

  return Atlas( Internal::Atlas::New( width, height, pixelFormat, recoverContext ) );
}

Atlas::Atlas()
{
}

void Atlas::Clear( const Vector4& color  )
{
  GetImplementation( *this ).Clear( color );
}

bool Atlas::Upload( BufferImage bufferImage,
                    SizeType xOffset,
                    SizeType yOffset )
{
  return GetImplementation(*this).Upload( GetImplementation(bufferImage), xOffset, yOffset );
}

bool Atlas::Upload( const std::string& url,
                    SizeType xOffset,
                    SizeType yOffset )
{
  return GetImplementation(*this).Upload( url, xOffset, yOffset );
}

Atlas Atlas::DownCast( BaseHandle handle )
{
  return Atlas( dynamic_cast<Dali::Internal::Atlas*>(handle.GetObjectPtr()) );
}

Atlas::~Atlas()
{
}

Atlas::Atlas( const Atlas& handle )
: Image( handle )
{
}

Atlas& Atlas::operator=( const Atlas& rhs )
{
  BaseHandle::operator=(rhs);
  return *this;
}

Atlas::Atlas( Internal::Atlas* internal )
: Image( internal )
{
}

} // namespace Dali

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
#include <dali/devel-api/object/property-buffer.h> // Dali::PropertyBuffer

// INTERNAL INCLUDES
#include <dali/public-api/object/property-map.h> // Dali::Property::Map
#include <dali/internal/event/common/property-buffer-impl.h>  // Dali::Internal::PropertyBuffer

namespace Dali
{

PropertyBuffer PropertyBuffer::New( Dali::Property::Map& bufferFormat )
{
  Internal::PropertyBufferPtr propertyBuffer = Internal::PropertyBuffer::New( bufferFormat );

  return PropertyBuffer( propertyBuffer.Get() );
}

PropertyBuffer::PropertyBuffer()
{
}

PropertyBuffer::~PropertyBuffer()
{
}

PropertyBuffer::PropertyBuffer( const PropertyBuffer& handle )
: BaseHandle( handle )
{
}

PropertyBuffer PropertyBuffer::DownCast( BaseHandle handle )
{
  return PropertyBuffer( dynamic_cast<Dali::Internal::PropertyBuffer*>(handle.GetObjectPtr()));
}

PropertyBuffer& PropertyBuffer::operator=( const PropertyBuffer& handle )
{
  BaseHandle::operator=( handle );
  return *this;
}

void PropertyBuffer::SetData( const void* data, std::size_t size )
{
  GetImplementation(*this).SetData( data, size );
}

std::size_t PropertyBuffer::GetSize() const
{
  return  GetImplementation(*this).GetSize();
}


PropertyBuffer::PropertyBuffer( Internal::PropertyBuffer* pointer )
: BaseHandle( pointer )
{
}

} // namespace Dali

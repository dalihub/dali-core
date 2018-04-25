/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#include <dali/internal/update/rendering/render-property-buffer.h>
#include <dali/internal/event/common/property-buffer-impl.h>  // Dali::Internal::PropertyBuffer
#include <dali/graphics-api/graphics-api-controller.h>
#include <dali/graphics-api/graphics-api-buffer.h>


namespace Dali
{
namespace Internal
{
namespace Render
{

PropertyBuffer::PropertyBuffer()
:mFormat(NULL),
 mData(NULL),
 mSize(0),
 mDataChanged(true),
 mGfxBuffer{ nullptr },
 mGfxBufferUsage{ Graphics::API::Buffer::UsageHint::ATTRIBUTES }
{
}

PropertyBuffer::~PropertyBuffer()
{
}

void PropertyBuffer::SetUsage( Graphics::API::Buffer::UsageHint usage )
{
  mGfxBufferUsage = usage;
}

void PropertyBuffer::SetFormat( PropertyBuffer::Format* format )
{
  mFormat = format;
  mDataChanged = true;
}

void PropertyBuffer::SetData( Dali::Vector<char>* data, size_t size )
{
  mData = data;
  mSize = size;
  mDataChanged = true;
}

bool PropertyBuffer::Update( Dali::Graphics::API::Controller& controller )
{
  if( !mData || !mFormat || !mSize )
  {
    return false;
  }

  if( mDataChanged || !mGfxBuffer )
  {
    if( !mGfxBuffer )
    {
      mGfxBuffer = controller.CreateBuffer( controller.GetBufferFactory()
                                                      .SetUsage( mGfxBufferUsage )
                                                      .SetSize( GetDataSize() ) );
    }

    // schedule deferred write
    mGfxBuffer.Get().Write( mData.Get()->begin(), GetDataSize(), 0u );

    mDataChanged = false;
  }

  return true;
}

} //Render
} //Internal
} //Dali

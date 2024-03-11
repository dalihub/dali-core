/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/rendering/scene-graph-property-buffer.h>

// INTERNAL HEADERS
#include <dali/internal/event/common/property-buffer-impl.h>  // Dali::Internal::PropertyBuffer
#include <dali/graphics-api/graphics-controller.h>
#include <dali/graphics-api/graphics-buffer.h>


namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

PropertyBuffer::PropertyBuffer()
: mGraphicsController( nullptr ),
  mFormat( nullptr ),
  mData( nullptr ),
  mGpuBuffer(nullptr),
  mSize(0),
  mDataChanged(true)
{
}

PropertyBuffer::~PropertyBuffer()
{
}

void PropertyBuffer::Initialize( Graphics::Controller& graphicsController )
{
  mGraphicsController = &graphicsController;
}

void PropertyBuffer::SetFormat( OwnerPointer< PropertyBuffer::Format >& format )
{
  mFormat.Swap( format );
  mDataChanged = true;
}

void PropertyBuffer::SetData( OwnerPointer< Dali::Vector<uint8_t> >& data, uint32_t size )
{
  mData.Swap( data );
  mSize = size;
  mDataChanged = true;
}

bool PropertyBuffer::Update( Dali::Graphics::Controller& graphicsController )
{
  if( !mData || !mFormat || !mSize )
  {
    return false;
  }

  if(!mGpuBuffer || mDataChanged)
  {
    if(!mGpuBuffer)
    {
      mGpuBuffer = new GpuBuffer(0 | Graphics::BufferUsage::VERTEX_BUFFER, GpuBuffer::WritePolicy::DISCARD);
    }

    // Update the GpuBuffer
    if(mGpuBuffer && mData)
    {
      DALI_ASSERT_DEBUG(mSize && "No data in the property buffer!");
      mGpuBuffer->UpdateDataBuffer(graphicsController, GetDataSize(), &((*mData)[0]));
    }
    mDataChanged = false;
  }

  return true;
}


} //Render
} //Internal
} //Dali

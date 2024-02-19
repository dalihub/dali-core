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
 */

#include "test-graphics-buffer.h"
#include "dali-test-suite-utils.h"
#include <cstring>

namespace Test
{

GraphicsBuffer::GraphicsBuffer( GraphicsController& controller, Dali::Graphics::BufferUsageFlags usage, uint32_t size )
: mController( controller )
{
  if( size > 0 )
  {
    mBuffer.resize(size);
  }
}

GraphicsBuffer::~GraphicsBuffer()
{
}

void* GraphicsBuffer::Map()
{
  mController.mBufferTrace.PushCall("Map");
  return &mBuffer[0];
}

void GraphicsBuffer::Unmap()
{
  mController.mBufferTrace.PushCall("Unmap");
}

void GraphicsBuffer::Write( void* src, uint32_t srcSize, uint32_t dstOffset )
{
  Test::TraceCallStack::NamedParams namedParams;
  namedParams["src"] = Dali::Any( src );
  namedParams["srcSize"] = Dali::Any( srcSize );
  namedParams["dstOffset"] = Dali::Any( dstOffset );
  mController.mShaderTrace.PushCall("Write", namedParams);

  if( dstOffset + srcSize > mBuffer.size() )
  {
    // Only write up to maximum of current buffer size. Notify user
    srcSize = uint32_t(mBuffer.size()) - dstOffset;
    fprintf(stderr, "Attempting to write past end of buffer.");
  }

  memcpy( &mBuffer[dstOffset], src, srcSize);
}

void GraphicsBuffer::Flush()
{
  mController.mBufferTrace.PushCall("Flush");
}

void GraphicsBuffer::DestroyNow()
{
  mController.mBufferTrace.PushCall("DestroyNow");
}

} // Test

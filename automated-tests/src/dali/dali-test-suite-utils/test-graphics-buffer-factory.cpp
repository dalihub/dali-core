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

#include "test-graphics-buffer-factory.h"
#include "test-graphics-buffer.h"

namespace Test
{

GraphicsBufferFactory::GraphicsBufferFactory(GraphicsController* controller)
: mController(*controller)
{
}

GraphicsBufferFactory::~GraphicsBufferFactory()=default;


Dali::Graphics::BufferFactory& GraphicsBufferFactory::SetUsageFlags( Dali::Graphics::BufferUsageFlags usage )
{
  mUsageFlags = usage;
  return *this;
}

Dali::Graphics::BufferFactory& GraphicsBufferFactory::SetSize( uint32_t size )
{
  mSize = size;
  return *this;
}

Dali::Graphics::BufferFactory::PointerType GraphicsBufferFactory::Create() const
{
  auto buffer = std::make_unique< GraphicsBuffer >( mController, mUsageFlags, mSize );
  return buffer;
}

} // Test

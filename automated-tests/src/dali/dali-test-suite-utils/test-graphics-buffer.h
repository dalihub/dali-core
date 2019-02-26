#ifndef TEST_GRAPHICS_BUFFER_H
#define TEST_GRAPHICS_BUFFER_H

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

#include <dali/graphics-api/graphics-api-buffer.h>
#include <dali/graphics-api/graphics-api-types.h>
#include <vector>

namespace Test
{
class GraphicsController;

class GraphicsBuffer : public Dali::Graphics::Buffer
{
public:
  GraphicsBuffer( GraphicsController& controller, Dali::Graphics::BufferUsageFlags usage, uint32_t size );
  ~GraphicsBuffer();

  void* Map() override;

  void Unmap() override;

  void Write( void* src, uint32_t srcSize, uint32_t dstOffset ) override;

  void Flush() override;

  void DestroyNow() override;

public:
  GraphicsController& mController;
  std::vector<uint8_t> mBuffer;
};

} // Test

#endif //TEST_GRAPHICS_BUFFER_H

#ifndef DALI_GRAPHICS_API_BUFFER_FACTORY_H
#define DALI_GRAPHICS_API_BUFFER_FACTORY_H

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

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-api-base-factory.h>
#include <dali/graphics-api/graphics-api-buffer.h>

namespace Dali
{
namespace Graphics
{
namespace API
{
class BufferFactory : public API::BaseFactory<API::Buffer>
{
public:

  // not copyable
  BufferFactory(const BufferFactory&) = delete;
  BufferFactory& operator=(const BufferFactory&) = delete;

  virtual ~BufferFactory() = default;

  /**
   * Sets usage of the buffer
   * @param usage
   * @return
   */
  virtual BufferFactory& SetUsage( Buffer::UsageHint usage ) = 0;

  /**
   * Sets size of the buffer
   * @param size
   * @return
   */
  virtual BufferFactory& SetSize( uint32_t size ) = 0;

protected:

  /// @brief default constructor
  BufferFactory() = default;

  // derived types should not be moved directly to prevent slicing
  BufferFactory(BufferFactory&&) = default;
  BufferFactory& operator=(BufferFactory&&) = default;
};

}
}
}

#endif //DALI_GRAPHICS_API_BUFFER_FACTORY_H

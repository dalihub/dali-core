#ifndef DALI_GRAPHICS_BUFFER_H
#define DALI_GRAPHICS_BUFFER_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

namespace Dali
{
namespace Graphics
{
/**
 * Buffer class represents a GPU buffer object. It may represent
 * vertex buffer, index buffer, pixel buffer, uniform buffer and
 * any other.
 */
class Buffer
{
public:
  Buffer()          = default;
  virtual ~Buffer() = default;

  // not copyable
  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;

protected:
  Buffer(Buffer&&) = default;
  Buffer& operator=(Buffer&&) = default;
};

} // Namespace Graphics
} // Namespace Dali

#endif
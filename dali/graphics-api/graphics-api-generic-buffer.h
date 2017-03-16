#ifndef DALI_GRAPHICS_API_GENERIC_BUFFER_H
#define DALI_GRAPHICS_API_GENERIC_BUFFER_H
/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include <vector>

namespace Dali
{
namespace Graphics
{
namespace API
{

/**
 * @brief Interface class for GenericBuffer types in the graphics API.
 */
template< typename Base, typename Structure >
class GenericBuffer final : public Base
{
public:
  GenericBuffer(size_t size) = default;

  // not copyable
  GenericBuffer(const GenericBuffer&) = delete;
  GenericBuffer& operator=(const GenericBuffer&) = delete;

  virtual ~GenericBuffer() = default;

protected:
  // derived types should not be moved direcly to prevent slicing
  GenericBuffer(GenericBuffer&&) = default;
  GenericBuffer& operator=(GenericBuffer&&) = default;

private:

  std::vector< Structure > mData;
};

} // namespace API
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_API_GENERIC_BUFFER_H

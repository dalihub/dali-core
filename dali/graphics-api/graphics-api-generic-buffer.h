#ifndef DALI_GRAPHICS_API_GENERIC_BUFFER_H
#define DALI_GRAPHICS_API_GENERIC_BUFFER_H
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

#include <cstddef>
#include <memory>

namespace Dali
{
namespace Graphics
{
namespace API
{
class GenericBufferBase
{
public:
  enum class UsageHint
  {
    FRAME_CONSTANTS,
    PRIMITIVE_UNIFORMS,
    ATTRIBUTES,
    INSTANCE,
    INDEX_BUFFER,
  };

  GenericBufferBase( size_t size, std::unique_ptr<char>&& data ) : mSize{size}, mData( std::move( data ) )
  {
  }

  size_t GetSize() const
  {
    return mSize;
  }

  virtual ~GenericBufferBase() = default;

protected:
  char* GetDataBase()
  {
    return mData.get();
  }

  // derived types should not be moved direcly to prevent slicing
  GenericBufferBase( GenericBufferBase&& ) = default;
  GenericBufferBase& operator=( GenericBufferBase&& ) = default;

  // not copyable
  GenericBufferBase( const GenericBufferBase& ) = delete;
  GenericBufferBase& operator=( const GenericBufferBase& ) = delete;

private:
  size_t                mSize;
  std::unique_ptr<char> mData;
};

/**
 * @brief Interface class for GenericBuffer types in the graphics API.
 */
template<typename T>
class GenericBuffer final : public GenericBufferBase
{
public:
  GenericBuffer( size_t numberOfElements, std::unique_ptr<char>&& data )
  : GenericBufferBase( numberOfElements * sizeof( T ), std::move( data ) ){};

  virtual ~GenericBuffer() = default;

  GenericBuffer( GenericBuffer&& ) = default;
  GenericBuffer& operator=( GenericBuffer&& ) = default;

  // not copyable
  GenericBuffer( const GenericBuffer& ) = delete;
  GenericBuffer& operator=( const GenericBuffer& ) = delete;

  T* GetData()
  {
    return reinterpret_cast<T*>( GenericBufferBase::GetDataBase() );
  }
};

} // namespace API
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_API_GENERIC_BUFFER_H

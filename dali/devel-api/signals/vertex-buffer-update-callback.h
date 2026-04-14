#ifndef DALI_VERTEX_BUFFER_UPDATE_CALLBACK_H
#define DALI_VERTEX_BUFFER_UPDATE_CALLBACK_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/signals/callback.h>

// EXTERNAL INCLUDES
#include <cstdint> ///< for uint32_t, size_t

namespace Dali
{
/**
 * @class VertexBufferUpdateCallback
 *
 * The class defines a callback that VertexBuffer object may call
 * to obtain new data. The callback runs before draw call is issued
 * and it will run on update/render thread (developers must synchronize
 * explicitly).
 *
 * Callback returns number of bytes written. This will limit next draw call
 * to number of elements that have been written by the callback.
 *
 * Using callback invalidates current vertex buffer data. Unchanged data
 * stays undefined.
 */
class DALI_CORE_API VertexBufferUpdateCallback
{
public:
  /**
   * @brief Destructor
   */
  ~VertexBufferUpdateCallback();

  /**
   * @brief Callback functor signature:
   *
   * uint32_t T::*(void* pointer, size_t dataSizeInBytes)
   */
  template<class T>
  using FuncType = uint32_t (T::*)(void*, size_t);

  /**
   * @brief Creates a new instance of VertexBufferUpdateCallback
   *
   * @tparam T class the functor is a member of
   * @param[in] object Object associated with callback
   * @param[in] functor Member function to be executed
   *
   * @return Returns valid VertexBufferUpdateCallback object
   */
  template<class T>
  static UniquePtr<VertexBufferUpdateCallback> New(T* object, FuncType<T> functor)
  {
    auto callback = Dali::MakeCallback(object, functor);
    return New(callback);
  }

  /**
   * @brief Invokes callback directly
   * @param[in] data pointer to write into
   * @param[in] size size of region in bytes
   *
   * @return Number of bytes written
   */
  uint32_t Invoke(void* data, size_t size);

private:
  static UniquePtr<VertexBufferUpdateCallback> New(CallbackBase* callbackBase);

  struct Impl;
  explicit VertexBufferUpdateCallback(UniquePtr<VertexBufferUpdateCallback::Impl>&& impl);
  UniquePtr<Impl> mImpl;
};
} // namespace Dali

#endif // DALI_VERTEX_BUFFER_UPDATE_CALLBACK_H

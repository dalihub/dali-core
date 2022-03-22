#ifndef DALI_RENDER_CALLBACK_H
#define DALI_RENDER_CALLBACK_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/signals/callback.h>

// EXTERNAL INCLUDES
#include <memory>

namespace Dali
{
/**
 * @class RenderCallbackInput
 *
 * This structure contains data to be passed into the RenderCallback
 * functor.
 *
 * RenderCallbackInput inherits from Graphics::NativeDrawInput
 *
 * @SINCE_2_1.14
 */
struct RenderCallbackInput
{
  Matrix mvp;
  Matrix projection;
  Size   size;
};

/**
 * @class DrawableCallback
 *
 * The class wraps CallbackBase object ensuring its type-safe assignment
 *
 * @SINCE_2_1.14
 */
class RenderCallback
{
public:
  /**
   * Templated member function type
   */
  template<class T>
  using FuncType = bool (T::*)(const RenderCallbackInput&);

  /**
   * @brief Constructor of RenderCallback
   *
   * @param[in] object Object to invoke
   * @param[in] func Member function to invoke
   *
   * @SINCE_2_1.14
   */
  template<class T>
  RenderCallback(T* object, FuncType<T> func)
  : mCallback(MakeCallback(object, func))
  {
  }

  /**
   * @brief Creates new instance of RenderCallback
   *
   * @SINCE_2_1.14
   * @param[in] object Object to invoke
   * @param[in] func Member function to invoke
   * @return Unique pointer to the RenderCallback instance
   */
  template<class T>
  static std::unique_ptr<RenderCallback> New(T* object, FuncType<T> func)
  {
    return std::make_unique<RenderCallback>(object, func);
  }

  /**
   * @brief Explicit cast operator
   *
   * @SINCE_2_1.14
   * @return casts RenderCallback to CallbackBase object
   */
  explicit operator CallbackBase*()
  {
    return mCallback.get();
  }

  /**
   * @brief Explicit cast operator
   *
   * @SINCE_2_1.14
   * @return casts RenderCallback to CallbackBase object
   */
  explicit operator CallbackBase&()
  {
    return *mCallback;
  }

private:
  std::unique_ptr<CallbackBase> mCallback; //< Callback base object
};
} // namespace Dali

#endif // DALI_RENDER_CALLBACK_H

#ifndef DALI_RENDER_CALLBACK_H
#define DALI_RENDER_CALLBACK_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/rendering/texture.h>
#include <dali/public-api/signals/callback.h>

// EXTERNAL INCLUDES
#include <any>
#include <memory>
#include <utility>
#include <vector>

namespace Dali
{
/**
 * @class RenderCallbackInput
 *
 * This structure contains data to be passed into the RenderCallback
 * functor.
 *
 * @SINCE_2_1.30
 */
struct DALI_CORE_API RenderCallbackInput
{
  Matrix                mvp;
  Matrix                projection;
  Size                  size;
  Rect<int32_t>         clippingBox; ///< in screen coordinates
  std::vector<uint32_t> textureBindings;

  std::any eglContext;         ///< Storage for EGL Context
  bool     usingOwnEglContext; ///< Uses own EGL context (owns GL state), custom code should be aware of it

  Matrix  view; // Added at end to avoid abi break.
  Vector4 worldColor;
};

/**
 * @class RenderCallback
 *
 * RenderCallback is used by the direct rendering feature and allows injecting
 * custom render code (using native API such as GL directly) into the DALi
 * rendering stream. The RenderCallback will be executed within own context (unless
 * native API is context-less) to maintain state separation from DALi render state.
 *
 * The class wraps CallbackBase object ensuring its type-safe assignment
 *
 * @SINCE_2_1.14
 */
class DALI_CORE_API RenderCallback
{
public:
  /**
   * @brief Mode of execution of custom rendering code into the pipeline
   *
   * ISOLATED mode will try to isolate custom rendering so it will start with
   * clean state of graphics API and won't affect DALi rendering pipeline. This
   * mode is considered as safe.
   *
   * UNSAFE mode will inject custom rendering code into DALi rendering
   * pipeline as is and won't isolate graphics native API state. This mode
   * should be used with caution and is considered unsafe.
   *
   * The default mode is ISOLATED.
   *
   * @SINCE_2_3.12
   */
  enum class ExecutionMode
  {
    /**
     * @brief Native rendering commands will be isolated from DALi graphics pipline state
     * @details This mode is default and provides safest way of executing custom graphics API commands.
     * @SINCE_2_3.12
     */
    ISOLATED,

    /**
     * @brief Native rendering will be injected directly into DALi graphics pipeline
     * @details This mode is considered unsafe as it's directly injected into DALi rendering pipeline.
     * It inherits current graphics API state and may alter it.
     * @SINCE_2_3.12
     */
    UNSAFE,

    /**
     * @brief Default mode is ISOLATED
     * @SINCE_2_3.12
     */
    DEFAULT = ISOLATED
  };

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
   * @param[in] executionMode execution mode of custom code
   *
   * @SINCE_2_3.12
   */
  template<class T>
  RenderCallback(T* object, FuncType<T> func, ExecutionMode executionMode)
  : mCallback(MakeCallback(object, func)),
    mExecutionMode(executionMode)
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
    return std::make_unique<RenderCallback>(object, func, ExecutionMode::DEFAULT);
  }

  /**
   * @brief Creates new instance of RenderCallback
   *
   * @SINCE_2_3.12
   * @param[in] object Object to invoke
   * @param[in] func Member function to invoke
   * @param[in] executionMode Execution mode of custom code
   * @return Unique pointer to the RenderCallback instance
   */
  template<class T>
  static std::unique_ptr<RenderCallback> New(T* object, FuncType<T> func, ExecutionMode executionMode)
  {
    return std::make_unique<RenderCallback>(object, func, executionMode);
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
   * @brief Binds DALi textures to the callback
   *
   * The textures that are bound to the callback will be passed upon
   * callback execution providing native handles (like GL name) so they
   * can be used alongside with custom GL code.
   *
   * Binding texture does not affect lifecycle and it's up to the client-side
   * to make sure the resource is alive when used inside the callback.
   *
   * @param[in] textures List of DALi textures to be bound to the callback
   * @SINCE_2_1.30
   */
  void BindTextureResources(std::vector<Dali::Texture> textures)
  {
    mTextureResources = std::move(textures);
  }

  /**
   * @brief Returns list of DALi textures bound to the callback
   *
   * @return list of textures
   */
  [[nodiscard]] const std::vector<Dali::Texture>& GetTextureResources() const
  {
    return mTextureResources;
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

  /**
   * @brief Returns input data passed to the callback upon execution
   *
   * The input data will be filled by the DALi and Graphics backend
   * providing DALi rendering related data (such as clipping box) as well as
   * Graphics native API related data (like EGLContext for GL).
   *
   * @SINCE_2_1.30
   * @return Valid RenderCallbackInput structure
   */
  RenderCallbackInput& GetRenderCallbackInput()
  {
    return mRenderCallbackInput;
  }

  /**
   * @brief Returns execution mode of the callback
   *
   * @SINCE_2_3.12
   * @return Valid execution mode
   */
  [[nodiscard]] ExecutionMode GetExecutionMode() const
  {
    return mExecutionMode;
  }

private:
  std::unique_ptr<CallbackBase> mCallback; //< Callback base object
  RenderCallbackInput           mRenderCallbackInput;
  ExecutionMode                 mExecutionMode{ExecutionMode::DEFAULT};
  std::vector<Dali::Texture>    mTextureResources{};
};
} // namespace Dali

#endif // DALI_RENDER_CALLBACK_H

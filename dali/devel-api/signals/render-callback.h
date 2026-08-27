#ifndef DALI_RENDER_CALLBACK_H
#define DALI_RENDER_CALLBACK_H

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
#include <dali/devel-api/threading/mutex.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/object/any.h>
#include <dali/public-api/rendering/texture.h>
#include <dali/public-api/signals/callback.h>

// EXTERNAL INCLUDES
#include <utility>

namespace Dali
{
/**
 * @class RenderCallbackInput
 *
 * This structure contains data to be passed into the RenderCallback
 * functor.
 */
struct DALI_CORE_API RenderCallbackInput
{
  Dali::Matrix mvp;
  Dali::Matrix view;
  Dali::Matrix projection;

  Dali::BoundsInteger clippingBox; ///< in screen coordinates
  Dali::Vector4       worldColor;
  Dali::Size          size;

  Dali::Any eglContext; ///< Storage for EGL Context

  /**
   * @brief Native handles of the textures bound with RenderCallback::BindTextureResources().
   *
   * Entries are in the same order as the list passed to BindTextureResources(), so the
   * index is what associates an entry with the texture the client bound.
   *
   * An entry is 0 until DALi has created the GL texture behind it, and keeps its position
   * meanwhile. Check for 0 before using an entry.
   */
  Dali::Vector<uint32_t> textureBindings;

  bool usingOwnEglContext; ///< Uses own EGL context (owns GL state), custom code should be aware of it
  bool isTerminated;       ///< Whether this callback is for terminate case, or not.
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
   */
  enum class ExecutionMode
  {
    /**
     * @brief Native rendering commands will be isolated from DALi graphics pipline state
     * @details This mode is default and provides safest way of executing custom graphics API commands.
     */
    ISOLATED,

    /**
     * @brief Native rendering will be injected directly into DALi graphics pipeline
     * @details This mode is considered unsafe as it's directly injected into DALi rendering pipeline.
     * It inherits current graphics API state and may alter it.
     */
    UNSAFE,

    /**
     * @brief Default mode is ISOLATED
     */
    DEFAULT = ISOLATED
  };

  /**
   * Templated member function type
   */
  template<class T>
  using FuncType = bool (T::*)(const Dali::RenderCallbackInput&);

  /**
   * @brief Constructor of RenderCallback
   *
   * @param[in] object Object to invoke
   * @param[in] func Member function to invoke
   * @param[in] executionMode execution mode of custom code
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
   * @param[in] object Object to invoke
   * @param[in] func Member function to invoke
   * @return Unique pointer to the RenderCallback instance
   */
  template<class T>
  static UniquePtr<Dali::RenderCallback> New(T* object, FuncType<T> func)
  {
    return MakeUnique<Dali::RenderCallback>(object, func, ExecutionMode::DEFAULT);
  }

  /**
   * @brief Creates new instance of RenderCallback
   *
   * @param[in] object Object to invoke
   * @param[in] func Member function to invoke
   * @param[in] executionMode Execution mode of custom code
   * @return Unique pointer to the RenderCallback instance
   */
  template<class T>
  static UniquePtr<Dali::RenderCallback> New(T* object, FuncType<T> func, ExecutionMode executionMode)
  {
    return MakeUnique<Dali::RenderCallback>(object, func, executionMode);
  }

  /**
   * @brief Explicit cast operator
   *
   * @return casts RenderCallback to CallbackBase object
   */
  explicit operator Dali::CallbackBase*()
  {
    return mCallback.Get();
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
   *
   * @note A texture may be bound before DALi has created the GL texture behind it, in
   *       which case RenderCallbackInput::textureBindings reports 0 for it.
   * @note Safe to call while the render thread is reading the previously bound list.
   */
  void BindTextureResources(Dali::Vector<Dali::Texture> textures)
  {
    Dali::Mutex::ScopedLock lock(mTextureResourcesMutex);
    mTextureResources = std::move(textures);
  }

  /**
   * @brief Scoped read access to the list of DALi textures bound to the callback
   *
   * BindTextureResources() replaces the list from the event thread, which releases the
   * storage the previous one was using, so this accessor must stay alive for as long as
   * the reference it hands out is in use.
   *
   * Scoped rather than copied on purpose: the render thread must not allocate, nor touch
   * the reference count of an event thread object.
   */
  class TextureResourcesAccessor
  {
  public:
    TextureResourcesAccessor(Dali::Mutex& mutex, const Dali::Vector<Dali::Texture>& resources)
    : mLock(mutex),
      mResources(resources)
    {
    }

    /**
     * @brief Returns the bound textures, valid for the lifetime of this accessor
     *
     * @return list of textures
     */
    [[nodiscard]] const Dali::Vector<Dali::Texture>& Get() const
    {
      return mResources;
    }

  private:
    Dali::Mutex::ScopedLock            mLock;
    const Dali::Vector<Dali::Texture>& mResources;
  };

  /**
   * @brief Takes scoped read access to the textures bound to the callback
   *
   * @return An accessor that must outlive any use of the list it exposes
   */
  [[nodiscard]] TextureResourcesAccessor AccessTextureResources() const
  {
    return TextureResourcesAccessor(mTextureResourcesMutex, mTextureResources);
  }
  /**
   * @brief Explicit cast operator
   *
   * @return casts RenderCallback to CallbackBase object
   */
  explicit operator Dali::CallbackBase&()
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
   * @return Valid RenderCallbackInput structure
   */
  Dali::RenderCallbackInput& GetRenderCallbackInput()
  {
    return mRenderCallbackInput;
  }

  /**
   * @brief Returns execution mode of the callback
   *
   * @return Valid execution mode
   */
  [[nodiscard]] ExecutionMode GetExecutionMode() const
  {
    return mExecutionMode;
  }

private:
  UniquePtr<Dali::CallbackBase> mCallback; //< Callback base object
  Dali::RenderCallbackInput     mRenderCallbackInput;
  ExecutionMode                 mExecutionMode{ExecutionMode::DEFAULT};
  Dali::Vector<Dali::Texture>   mTextureResources{};
  mutable Dali::Mutex           mTextureResourcesMutex{}; ///< Guards mTextureResources across the event and render threads
};
} // namespace Dali

#endif // DALI_RENDER_CALLBACK_H

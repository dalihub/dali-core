#ifndef DALI_INTERNAL_RENDER_TERMINATED_NATIVE_DRAW_MANAGER_H
#define DALI_INTERNAL_RENDER_TERMINATED_NATIVE_DRAW_MANAGER_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/signals/render-callback.h>
#include <unordered_map>

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-command-buffer.h>                  ///< for Graphics::DrawNativeInfo
#include <dali/internal/render/common/render-target-graphics-objects.h> ///< For RenderTargetGraphicsObjects::LifecycleObserver

namespace Dali
{
namespace Graphics
{
class Controller;
} // namespace Graphics

namespace Internal::Render
{
/**
 * @brief The class for collects detail implements for native draw callbacks, and control the render callback terminate cases.
 * Native draw has dependency with each RenderTargetGraphicsObjects. So we must ensure the render target and render pass
 * be used even for native draw callback terminated cases.
 */
class TerminatedNativeDrawManager : public SceneGraph::RenderTargetGraphicsObjects::LifecycleObserver
{
public:
  /**
   * @brief Constructor
   * @param[in] graphicsController The graphics controller. It will be used when we create new command buffer.
   */
  TerminatedNativeDrawManager(Graphics::Controller& graphicsController);

  /**
   * @brief Destructor
   */
  ~TerminatedNativeDrawManager() = default;

public: // For control terminated callback
  /**
   * @brief Gets if their any terminated render callback registered.
   * @return True if any terminated render callback registered.
   */
  bool AnyTerminatedCallbackExist() const;

  /**
   * @brief Gets if their terminated render callback registered for given render target graphics objects.
   * @param[in] renderTarget The render target graphics objects that want to check terminated callback exist or not.
   * @return True if terminated render callback registered.
   */
  bool TerminatedCallbackExist(const SceneGraph::RenderTargetGraphicsObjects& renderTarget) const;

  /**
   * @brief Register terminated render callback.
   *        It will own the reference of RenderCallbackInput. Will be removed after submit command.
   * @param[in] renderTarget The render target graphics objects that want to register terminated callback.
   * @param[in] renderCallback The render callback. (now owned)
   * @param[in] renderCallbackInput The input parameter of render callback.
   */
  void RegisterTerminatedRenderCallback(const SceneGraph::RenderTargetGraphicsObjects& renderTarget, RenderCallback* renderCallback, std::unique_ptr<Dali::RenderCallbackInput> renderCallbackInput);

  /**
   * @brief Submit all terminated render callbacks to graphics controller.
   */
  void SubmitAllTerminatedRenderCallback();

  /**
   * @brief Submit terminated render callbacks for given render target graphics objects to graphics controller.
   * @param[in] renderTarget The render target graphics objects that want to submit terminated callback.
   */
  void SubmitTerminatedRenderCallback(const SceneGraph::RenderTargetGraphicsObjects& renderTarget);

public: // From SceneGraph::RenderTargetGraphicsObjects::LifecycleObserver
  /**
   * @copydoc Dali::Internal::SceneGraph::RenderTargetGraphicsObjects::LifecycleObserver::RenderTargetGraphicsObjectsDestroyed()
   */
  void RenderTargetGraphicsObjectsDestroyed(const SceneGraph::RenderTargetGraphicsObjects* renderTargetGraphicsObjects);

private:
  struct TerminateRenderCallbackInfo
  {
    TerminateRenderCallbackInfo(RenderCallback* callback, Graphics::DrawNativeInfo&& drawInfo, std::unique_ptr<Dali::RenderCallbackInput> callbackInput)
    : renderCallback(callback),
      info(std::move(drawInfo)),
      renderCallbackInput(std::move(callbackInput))
    {
    }

    TerminateRenderCallbackInfo(TerminateRenderCallbackInfo&&) noexcept            = default;
    TerminateRenderCallbackInfo& operator=(TerminateRenderCallbackInfo&&) noexcept = default;

    // Deleted copy constructor and copy assignment operator
    TerminateRenderCallbackInfo(const TerminateRenderCallbackInfo&)            = delete;
    TerminateRenderCallbackInfo& operator=(const TerminateRenderCallbackInfo&) = delete;

    RenderCallback*                            renderCallback;
    Graphics::DrawNativeInfo                   info;
    std::unique_ptr<Dali::RenderCallbackInput> renderCallbackInput;
  };
  using TerminateRenderCallbackContainer = std::vector<TerminateRenderCallbackInfo>;
  using TerminatedRenderTargetsContainer = std::unordered_map<const SceneGraph::RenderTargetGraphicsObjects*, TerminateRenderCallbackContainer>;

  Graphics::Controller*            mGraphicsController{nullptr};
  TerminatedRenderTargetsContainer mTerminatedRenderTargets;
};
} // namespace Internal::Render
} // namespace Dali

#endif // DALI_INTERNAL_RENDER_TERMINATED_NATIVE_DRAW_MANAGER_H

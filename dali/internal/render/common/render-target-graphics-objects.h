#ifndef DALI_INTERNAL_RENDER_TARGET_GRAPHICS_OBJECTS_H
#define DALI_INTERNAL_RENDER_TARGET_GRAPHICS_OBJECTS_H

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
#include <cstdint>
#include <unordered_map>

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-controller.h> ///< for Graphics relative structs.
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali::Internal::SceneGraph
{
/**
 * An interface to hold Graphics::RenderTarget, Graphics::RenderPass (with and w.o. clear)
 * and list of Graphics::ClearValue data.
 * This interface is used by SceneGraph::Scene and Render::FrameBuffer, who could be render target.
 */
class RenderTargetGraphicsObjects
{
public:
  /**
   * Observer to determine when the render target pass holder is no longer present
   */
  class LifecycleObserver
  {
  public:
    /**
     * Called shortly before the render target pass holder is destroyed.
     */
    virtual void RenderTargetGraphicsObjectsDestroyed(const RenderTargetGraphicsObjects* renderTargetGraphicsObjects) = 0;

  protected:
    /**
     * Virtual destructor, no deletion through this interface
     */
    virtual ~LifecycleObserver() = default;
  };

public:
  /**
   * Constructor. Nothing to do as a pure interface.
   */
  RenderTargetGraphicsObjects() = default;

  /**
   * Initialize and register graphics controller.
   */
  void Initialize(Graphics::Controller& graphicsController)
  {
    mGraphicsController = &graphicsController;
  }

  [[nodiscard]] Graphics::RenderTarget* GetGraphicsRenderTarget() const
  {
    return mRenderTarget.get();
  }

  [[nodiscard]] Graphics::RenderPass* GetGraphicsRenderPass(Graphics::AttachmentLoadOp  colorLoadOp,
                                                            Graphics::AttachmentStoreOp colorStoreOp) const
  {
    // clear only when requested
    if(colorLoadOp == Graphics::AttachmentLoadOp::CLEAR)
    {
      return mRenderPass.get();
    }
    else
    {
      return mRenderPassNoClear.get();
    }
  }

  /**
   * The function returns initialized array of clear values
   * which then can be modified and passed to BeginRenderPass()
   * command.
   */
  [[nodiscard]] auto& GetGraphicsRenderPassClearValues()
  {
    return mClearValues;
  }

  [[nodiscard]] bool IsCompatible(RenderTargetGraphicsObjects* rhs)
  {
    // Query the backend to ensure that the render targets and the render passes are compatible
    // for the purpose of defining pipelines.
    if(this != rhs && rhs != nullptr)
    {
      return mGraphicsController->IsCompatible(*mRenderTarget.get(), *rhs->mRenderTarget.get(), *mRenderPass.get(), *rhs->mRenderPass.get());
    }
    return true;
  }

public:
  void CreateRenderTarget(Graphics::Controller& graphicsController, const Graphics::RenderTargetCreateInfo& rtInfo)
  {
    // Notify to obsevers that old render target invalidated.
    NotifyRenderTargetDestroyed();

    mRenderTarget = graphicsController.CreateRenderTarget(rtInfo, std::move(mRenderTarget));
  }

  void CreateRenderPass(Graphics::Controller& graphicsController, const Graphics::RenderPassCreateInfo& rpInfo)
  {
    mRenderPass = graphicsController.CreateRenderPass(rpInfo, std::move(mRenderPass));
  }

  void CreateRenderPassNoClear(Graphics::Controller& graphicsController, const Graphics::RenderPassCreateInfo& rpInfo)
  {
    mRenderPassNoClear = graphicsController.CreateRenderPass(rpInfo, std::move(mRenderPassNoClear));
  }

protected:
  /**
   * Virtual destructor
   */
  virtual ~RenderTargetGraphicsObjects()
  {
    NotifyRenderTargetDestroyed();

    // Note : We don't need to restore mObserverNotifying to false as we are in delete the object.
    // If someone call AddObserver / RemoveObserver after this, assert.
    mObserverNotifying = true;
  }

  /**
   * Notify to observers and clear.
   */
  void NotifyRenderTargetDestroyed()
  {
    if(mRenderTarget.get())
    {
      mObserverNotifying = true;
      for(auto&& iter : mLifecycleObservers)
      {
        auto* observer = iter.first;
        observer->RenderTargetGraphicsObjectsDestroyed(this);
      }
      mLifecycleObservers.clear();
      mObserverNotifying = false;
    }
  }

public:
  /**
   * Allows RenderTarget to track the life-cycle of this object.
   * Note that we allow to observe lifecycle multiple times.
   * But RenderTargetDestroyed callback will be called only one time.
   * @param[in] observer The observer to add.
   */
  void AddLifecycleObserver(LifecycleObserver& observer)
  {
    DALI_ASSERT_ALWAYS(!mObserverNotifying && "Cannot add observer while notifying RenderTargetGraphicsObjects::LifecycleObservers");

    auto iter = mLifecycleObservers.find(&observer);
    if(iter != mLifecycleObservers.end())
    {
      // Increase the number of observer count
      ++(iter->second);
    }
    else
    {
      mLifecycleObservers.insert({&observer, 1u});
    }
  }

  /**
   * The RenderTarget no longer needs to track the life-cycle of this object.
   * @param[in] observer The observer that to remove.
   */
  void RemoveLifecycleObserver(LifecycleObserver& observer)
  {
    DALI_ASSERT_ALWAYS(!mObserverNotifying && "Cannot remove observer while notifying RenderTargetGraphicsObjects::LifecycleObservers");

    auto iter = mLifecycleObservers.find(&observer);
    DALI_ASSERT_ALWAYS(iter != mLifecycleObservers.end());

    if(--(iter->second) == 0u)
    {
      mLifecycleObservers.erase(iter);
    }
  }

protected: /// TODO : Make it as private if possible
  Graphics::Controller* mGraphicsController{nullptr};

  Graphics::UniquePtr<Graphics::RenderTarget> mRenderTarget{nullptr};

  /**
   * Render pass is created on fly depending on Load and Store operations
   * The default render pass (most likely to be used) is the load = CLEAR
   * and store = STORE for color attachment.
   */
  Graphics::UniquePtr<Graphics::RenderPass> mRenderPass{nullptr};        ///< The render pass created
  Graphics::UniquePtr<Graphics::RenderPass> mRenderPassNoClear{nullptr}; ///< The render pass created without clearing color

  std::vector<Graphics::ClearValue> mClearValues{}; ///< Clear colors

private:
  using LifecycleObserverContainer = std::unordered_map<LifecycleObserver*, uint32_t>; ///< Lifecycle observers container. We allow to add same observer multiple times.
                                                                                       ///< Key is a pointer to observer, value is the number of observer added.
  LifecycleObserverContainer mLifecycleObservers{};

  bool mObserverNotifying{false}; ///< Safety guard flag to ensure that the LifecycleObserver not be added or deleted while observing.
};
} // namespace Dali::Internal::SceneGraph

#endif // DALI_INTERNAL_RENDER_TARGET_GRAPHICS_OBJECTS_H

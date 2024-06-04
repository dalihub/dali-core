#ifndef DALI_INTERNAL_SCENEGRAPH_RENDERER_RESETTER_H
#define DALI_INTERNAL_SCENEGRAPH_RENDERER_RESETTER_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
 */

// EXTERNAL INCLDUES
#include <cstdint> // int8_t

#include <dali/internal/update/animation/property-accessor.h>
#include <dali/internal/update/animation/property-component-accessor.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/rendering/scene-graph-renderer.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
/**
 * Class to reset the renderer's properties to their base values. Used by UpdateManager
 * to reset the renderer properties after the renderer is created.
 */
class RendererResetter : public PropertyOwner::Observer
{
public:
  /**
   * New method.
   * @param[in] renderer The renderer
   * @return the new renderer resetter
   */
  static RendererResetter* New(const Renderer& renderer)
  {
    return new RendererResetter(const_cast<Renderer*>(&renderer));
  }

  /**
   * Virtual Destructor
   */
  ~RendererResetter() override
  {
    if(mRenderer != nullptr)
    {
      mRenderer->RemoveObserver(*this);
    }
  }

  /**
   * Initialize.
   *
   * Watches the renderer to track if it's destroyed or not.
   */
  void Initialize()
  {
    mRenderer->AddObserver(*this);
  }

  /**
   * Reset the renderer properties to their base values if the renderer is still alive and on stage
   * @param[in] updateBufferIndex the current buffer index
   */
  void ResetToBaseValue(BufferIndex updateBufferIndex)
  {
    if(mRenderer != nullptr && mActive)
    {
      // Start aging the renderer properties.
      // We need to reset the renderer properties for only one frames to ensure
      // initialized case.
      --mActive;

      mRenderer->ResetToBaseValues(updateBufferIndex);
    }
  };

  /**
   * Called when the renderer is connected to the scene graph.
   *
   * Note, SceneGraph::Renderer don't call this API.
   *
   * @param[in] owner The property owner
   */
  void PropertyOwnerConnected(PropertyOwner& owner) override
  {
  }

  /**
   * Called when mPropertyOwner is disconnected from the scene graph.
   *
   * Note, SceneGraph::Renderer don't call this API.
   *
   * @param[in] bufferIndex the current buffer index
   * @param[in] owner The property owner
   */
  NotifyReturnType PropertyOwnerDisconnected(BufferIndex bufferIndex, PropertyOwner& owner) override
  {
    return NotifyReturnType::KEEP_OBSERVING;
  }

  /**
   * Called shortly before the propertyOwner is destroyed
   * @param[in] owner The property owner
   */
  void PropertyOwnerDestroyed(PropertyOwner& owner) override
  {
    mRenderer = nullptr;

    // Don't need to wait another frame as the properties are being destroyed
    mActive = STOPPED;
  }

  /**
   * Determine if the renderer resetter has finished.
   *
   * @return true if the renderer resetter has finished.
   */
  virtual bool IsFinished()
  {
    return mActive <= STOPPED;
  }

protected:
  enum
  {
    STOPPED = 0,
    AGING   = 1,
    ACTIVE  = 2,
  };

  /**
   * Constructor
   *
   * @param[in] renderer The renderer storing the base properties
   */
  RendererResetter(Renderer* renderer)
  : mRenderer(renderer),
    mActive(AGING) // Since we make this resetter only initialize case now.
  {
  }

  Renderer* mRenderer; ///< The renderer that owns the properties
  int8_t    mActive;   ///< 2 if active, 1 if aging, 0 if stopped
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif //DALI_INTERNAL_SCENEGRAPH_RENDERER_RESETTER_H

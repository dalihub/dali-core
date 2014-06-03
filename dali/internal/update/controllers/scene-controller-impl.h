#ifndef __DALI_INTERNAL_SCENE_GRAPH_SCENE_CONTROLLER_IMPL_H__
#define __DALI_INTERNAL_SCENE_GRAPH_SCENE_CONTROLLER_IMPL_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/controllers/scene-controller.h>
#include <dali/internal/update/controllers/light-controller.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

/**
 * concrete implementation of the scene controller interface
 */
class SceneControllerImpl: public SceneController
{

public:

  /**
   * Constructor
   * @param[in] context  dali context
   * @param[in] rendererDispatcher Used for passing ownership of renderers to the render-thread.
   * @param[in] renderQueue  The renderQueue
   * @param[in] discardQueue The discardQueue
   * @param[in] completeStatusTracker The resource complete status tracker
   */
  SceneControllerImpl( RenderMessageDispatcher& renderMessageDispatcher, RenderQueue& renderQueue, DiscardQueue& discardQueue, TextureCache& textureCache, CompleteStatusManager& completeStatusManager );

  /**
   * Destructor
   */
  virtual ~SceneControllerImpl();

public:  // for scene controller interface

  /**
   * Get the light  controller
   * @return  reference to a light controller
   */
  virtual LightController& GetLightController() { return *mLightController; }

  /**
   * Return the renderer dispatcher
   * @return A reference to the renderer dispatcher
   */
  virtual RenderMessageDispatcher& GetRenderMessageDispatcher() { return mRenderMessageDispatcher; }

  /**
   * Return the render queue
   * @return A reference to the render queue
   */
  virtual RenderQueue& GetRenderQueue() { return mRenderQueue; }

  /**
   * Return the discard queue
   * @return A reference to the discard queue
   */
  virtual DiscardQueue& GetDiscardQueue() { return mDiscardQueue; }

  /**
   * Return the texture cache
   * @note USE ONLY IN RENDER THREAD OBJECTS
   * @return The texture cache
   */
  virtual TextureCache& GetTextureCache() { return mTextureCache; }

  /**
   * @return the complete status tracker
   */
  virtual CompleteStatusManager& GetCompleteStatusManager() { return mCompleteStatusManager;  }

private:

  // Undefined copy constructor.
  SceneControllerImpl( const SceneControllerImpl& );

  // Undefined assignment operator.
  SceneControllerImpl& operator=( const SceneControllerImpl& );

private:

  LightController*       mLightController;       ///< light controller
  RenderMessageDispatcher& mRenderMessageDispatcher;    ///< Used for passing messages to the render-thread
  RenderQueue&           mRenderQueue;           ///< render queue
  DiscardQueue&          mDiscardQueue;          ///< discard queue
  TextureCache&          mTextureCache;          ///< texture cache
  CompleteStatusManager& mCompleteStatusManager; ///< Complete Status manager
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_SCENE_CONTROLLER_IMPL_H__

#ifndef __DALI_INTERNAL_SCENE_GRAPH_SCENE_CONTROLLER_IMPL_H__
#define __DALI_INTERNAL_SCENE_GRAPH_SCENE_CONTROLLER_IMPL_H__

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

// INTERNAL INCLUDES
#include <dali/internal/update/controllers/scene-controller.h>

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
   * @param[in] discardQueue The discardQueue
   */
  SceneControllerImpl( DiscardQueue& discardQueue );

  /**
   * Destructor
   */
  virtual ~SceneControllerImpl();

public:  // from SceneController

  /**
   * @copydoc SceneController::GetDiscardQueue()
   */
  virtual DiscardQueue& GetDiscardQueue() { return mDiscardQueue; }

private:
  SceneControllerImpl( const SceneControllerImpl& ) = delete;
  SceneControllerImpl& operator=( const SceneControllerImpl& ) = delete;

private:
  DiscardQueue&            mDiscardQueue;          ///< discard queue
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_SCENE_CONTROLLER_IMPL_H__

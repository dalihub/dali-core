#ifndef __DALI_INTERNAL_SCENE_GRAPH_SCENE_CONTROLLER_H__
#define __DALI_INTERNAL_SCENE_GRAPH_SCENE_CONTROLLER_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

class RenderMessageDispatcher;
class RenderQueue;
class DiscardQueue;

/**
 * Abstract interface for the scene controller
 */
class SceneController
{
public:

  /**
   * Constructor
   */
  SceneController()
  {
  }

  /**
   * Destructor
   */
  virtual ~SceneController()
  {
  }

  /**
   * Return the render message dispatcher
   * @return A reference to the render message dispatcher
   */
  virtual RenderMessageDispatcher& GetRenderMessageDispatcher() = 0;

  /**
   * Return the render queue
   * @return A reference to the render queue
   */
  virtual RenderQueue& GetRenderQueue() = 0;

  /**
   * Return the discard queue
   * @return A reference to the discard queue
   */
  virtual DiscardQueue& GetDiscardQueue() = 0;

private:

  // Undefined copy constructor.
  SceneController( const SceneController& );

  // Undefined assignment operator.
  SceneController& operator=( const SceneController& );
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_SCENE_CONTROLLER_H__

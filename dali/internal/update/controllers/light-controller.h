#ifndef __DALI_INTERNAL_SCENE_GRAPH_LIGHT_CONTROLLER_H__
#define __DALI_INTERNAL_SCENE_GRAPH_LIGHT_CONTROLLER_H__

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

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

class Node;

/**
 * Abstract interface for an object which controls lights in the scene graph.
 */
class LightController
{

public:

  /**
   * Virtual destructor
   */
  virtual ~LightController(){}

  /**
   * Add a light to the active lights list.
   * Only one copy of the light will be inserted into the list.
   * @param[in] lightNode The light
   */
  virtual void AddLight( Node& lightNode ) = 0;

  /**
   * Remove a light from the active lights list.
   * @param[in] lightNode The light
   */
  virtual void RemoveLight( Node& lightNode ) = 0;

  /**
   * Get an active light by index
   * @param[in] lightIndex The index to the active light
   * @return a NodePtr to a light or false
   */
  virtual Node& GetLight( const unsigned int lightIndex ) const = 0;

  /**
   * Get number of active lights
   * @return number of lights
   */
  virtual unsigned int GetNumberOfLights() const = 0;

};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_LIGHT_CONTROLLER_H__

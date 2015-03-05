#ifndef DALI_INTERNAL_SCENE_GRAPH_UNIFORM_MAP_H
#define DALI_INTERNAL_SCENE_GRAPH_UNIFORM_MAP_H

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

/**
 * The uniform map is generated on stage change, e.g when renderers, materials, geometry,
 * actors etc are added/removed from the stage.
 *
 * It computes a list of uniform strings to property value addresses that is needed
 * for the Renderer to match once against Program uniforms; subsequent render calls do
 * not need to re-generate the match, instead using the uniform index.
 */
class UniformMap
{
public:
  /**
   * Constructor
   */
  UniformMap();

  /**
   * Destructor
   */
  ~UniformMap();

private:
  // data
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_UNIFORM_MAP_H

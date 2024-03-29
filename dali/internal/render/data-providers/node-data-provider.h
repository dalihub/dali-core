#ifndef DALI_INTERNAL_SCENE_GRAPH_NODE_DATA_PROVIDER_H
#define DALI_INTERNAL_SCENE_GRAPH_NODE_DATA_PROVIDER_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

#include <dali/internal/common/buffer-index.h>
#include <dali/internal/update/common/uniform-map.h>
#include <dali/public-api/math/vector4.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
/**
 * An interface to provide data for a Renderer
 */
class NodeDataProvider
{
public:
  /**
   * Constructor. Nothing to do as a pure interface.
   */
  NodeDataProvider() = default;

  /**
   * @param bufferIndex to use
   * @return a reference to the color
   */
  virtual const Vector4& GetRenderColor(BufferIndex bufferIndex) const = 0;

  /**
   * Get the map of uniforms to property value addresses (for node only)
   */
  virtual const UniformMap& GetNodeUniformMap() const = 0;

protected:
  /**
   * Virtual destructor, this is an interface, no deletion through this interface
   */
  ~NodeDataProvider() = default;
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_NODE_DATA_PROVIDER_H

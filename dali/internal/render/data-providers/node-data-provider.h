#ifndef DALI_INTERNAL_SCENE_GRAPH_NODE_DATA_PROVIDER_H
#define DALI_INTERNAL_SCENE_GRAPH_NODE_DATA_PROVIDER_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

#include <dali/internal/render/data-providers/uniform-map-data-provider.h>

namespace Dali
{
struct Vector4;
class Matrix;

namespace Internal
{
namespace SceneGraph
{

/**
 * An interface to provide data for a Renderer
 */
class NodeDataProvider : UniformMapDataProvider
{
public:

  /**
   * Constructor. Nothing to do as a pure interface.
   */
  NodeDataProvider() = default;

  /**
   * @param bufferIndex to use
   * @return a reference to the model matrix
   */
  virtual const Matrix& GetModelMatrix( BufferIndex bufferIndex ) const = 0;

  /**
   * @param bufferIndex to use
   * @return a reference to the color
   */
  virtual const Vector4& GetRenderColor( BufferIndex bufferIndex ) const = 0;

  /**
   * @copydoc Dali::Internal::SceneGraph::UniformMapDataProvider::GetUniformMapChanged()
   */
  bool GetUniformMapChanged( BufferIndex bufferIndex ) const override = 0;

  /**
   * @copydoc Dali::Internal::SceneGraph::UniformMapDataProvider::GetUniformMap()
   */
  const CollectedUniformMap& GetUniformMap( BufferIndex bufferIndex ) const override = 0;

protected:
  /**
   * Virtual destructor, this is an interface, no deletion through this interface
   */
  ~NodeDataProvider() override = default;
};

} // SceneGraph
} // Internal
} // Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_NODE_DATA_PROVIDER_H

#ifndef DALI_INTERNAL_SCENE_GRAPH_UPDATE_ALGORITHMS_H
#define DALI_INTERNAL_SCENE_GRAPH_UPDATE_ALGORITHMS_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/update/nodes/node-declarations.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class Layer;
class PropertyOwner;
class RenderQueue;

using PropertyOwnerContainer = Dali::Vector<PropertyOwner*>;

/**
 * Constrain the local properties of the PropertyOwner.
 * @param[in] propertyOwner The PropertyOwner to constrain
 * @param[in] updateBufferIndex The current update buffer index.
 * @param[in] isPreConstraint True if the constraint is performed before transform.
 */
void ConstrainPropertyOwner(PropertyOwner& propertyOwner, BufferIndex updateBufferIndex, bool isPreConstraint = true);

/**
 * Update a tree of nodes
 * The inherited properties of each node are recalculated if necessary.
 * @param[in] rootNode The root of a tree of nodes.
 * @param[in] updateBufferIndex The current update buffer index.
 * @param[in] renderQueue Used to query messages for the next Render.
 * @param[out] postPropertyOwner property owners those have post constraint.
 * @return The cumulative (ORed) dirty flags for the updated nodes
 */
NodePropertyFlags UpdateNodeTree(Layer&                  rootNode,
                                 BufferIndex             updateBufferIndex,
                                 RenderQueue&            renderQueue,
                                 PropertyOwnerContainer& postPropertyOwners);
/**
 * This updates all the sub-layer's reusability flags without affecting
 * the root layer.
 *
 * @param layer The root layer
 * @param updateBufferIndex The current buffer index
 */
void UpdateLayerTree(Layer& layer, BufferIndex updateBufferIndex);

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_UPDATE_ALGORITHMS_H

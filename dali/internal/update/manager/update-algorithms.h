#ifndef __DALI_INTERNAL_SCENE_GRAPH_UPDATE_ALGORITHMS_H__
#define __DALI_INTERNAL_SCENE_GRAPH_UPDATE_ALGORITHMS_H__

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
#include <dali/internal/common/buffer-index.h>

namespace Dali
{

namespace Internal
{

class ResourceManager;

namespace SceneGraph
{

class Layer;
class Node;
class PropertyOwner;
class RenderQueue;

/**
 * Recursively apply the constraints on the nodes.
 * @param[in] node to constraint.
 * @param[in] updateBufferIndex The current update buffer index.
 * @return number of active constraints.
 */
unsigned int ConstrainNodes( Node& node, BufferIndex updateBufferIndex );

/**
 * Constrain the local properties of the PropertyOwner.
 * @param[in] propertyOwner The PropertyOwner to constrain
 * @param[in] updateBufferIndex The current update buffer index.
 * @return The number of active constraints.
 */
unsigned int ConstrainPropertyOwner( PropertyOwner& propertyOwner, BufferIndex updateBufferIndex );

/**
 * Update a tree of nodes, and attached objects.
 * The inherited properties of each node are recalculated if necessary.
 * When a renderable attachment is ready to render, PrepareResources() is called and
 * it is added to the list for its Layer.
 * @param[in] rootNode The root of a tree of nodes.
 * @param[in] updateBufferIndex The current update buffer index.
 * @param[in] resourceManager The resource manager.
 * @param[in] renderQueue Used to query messages for the next Render.
 * @return The cumulative (ORed) dirty flags for the updated nodes
 */
int UpdateNodesAndAttachments( Layer& rootNode,
                               BufferIndex updateBufferIndex,
                               ResourceManager& resourceManager,
                               RenderQueue& renderQueue );

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_UPDATE_ALGORITHMS_H__


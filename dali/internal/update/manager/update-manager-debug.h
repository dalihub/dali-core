#ifndef __DALI_INTERNAL_SCENE_GRAPH_UPDATE_MANAGER_DEBUG_H__
#define __DALI_INTERNAL_SCENE_GRAPH_UPDATE_MANAGER_DEBUG_H__

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/internal/update/nodes/node.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

void PrintNodes( const Node& node, BufferIndex updateBufferIndex, int level);

void PrintNodeTree( const Node& node, BufferIndex bufferIndex, std::string indentation="" );

} // SceneGraph

} // Internal

} // Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_UPDATE_MANAGER_DEBUG_H__

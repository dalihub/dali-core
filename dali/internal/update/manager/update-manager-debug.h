#ifndef DALI_INTERNAL_SCENE_GRAPH_UPDATE_MANAGER_DEBUG_H
#define DALI_INTERNAL_SCENE_GRAPH_UPDATE_MANAGER_DEBUG_H

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/internal/update/nodes/node.h>
#include <dali/integration-api/debug.h>

// Define this to access debug functions for the update manager.
#undef DALI_PRINT_UPDATE_INFO

#ifdef DALI_PRINT_UPDATE_INFO

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

#endif

#endif // DALI_INTERNAL_SCENE_GRAPH_UPDATE_MANAGER_DEBUG_H

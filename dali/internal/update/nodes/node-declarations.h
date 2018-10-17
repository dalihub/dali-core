#ifndef __DALI_INTERNAL_SCENE_GRAPH_NODE_DECLARATIONS_H__
#define __DALI_INTERNAL_SCENE_GRAPH_NODE_DECLARATIONS_H__

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
#include <dali/public-api/common/dali-vector.h>
#include <dali/devel-api/common/bitwise-enum.h>
#include <dali/devel-api/common/owner-container.h>
#include <dali/internal/common/owner-pointer.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

class Node;

typedef Dali::Vector< Node* > NodeContainer;
typedef NodeContainer::Iterator NodeIter;
typedef NodeContainer::ConstIterator NodeConstIter;

/**
 * Flag whether property has changed, during the Update phase.
 */
enum class NodePropertyFlags : uint8_t
// 8 bits is enough for 4 flags (compiler will check it)
{
  NOTHING          = 0x000,
  TRANSFORM        = 0x001,
  VISIBLE          = 0x002,
  COLOR            = 0x004,
  CHILD_DELETED    = 0x008,
  ALL = ( CHILD_DELETED << 1 ) - 1 // all the flags
};

} // namespace SceneGraph

} // namespace Internal

// specialization has to be done in the same namespace
template<> struct EnableBitMaskOperators< Internal::SceneGraph::NodePropertyFlags > { static const bool ENABLE = true; };

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_NODE_DECLARATIONS_H__

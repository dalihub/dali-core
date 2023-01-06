#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDER_ITEM_KEY_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDER_ITEM_KEY_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

#include <dali/internal/common/memory-pool-key.h>
#include <dali/internal/common/type-abstraction-enums.h>

namespace Dali
{
namespace Internal::SceneGraph
{
struct RenderItem;

using RenderItemKey = MemoryPoolKey<RenderItem>;
} // namespace Internal::SceneGraph

// Ensure RenderItemKey can be used in Dali::Vector
template<>
struct TypeTraits<Internal::SceneGraph::RenderItemKey> : public BasicTypes<Internal::SceneGraph::RenderItemKey>
{
  enum
  {
    IS_TRIVIAL_TYPE = true
  };
};

} // namespace Dali

#endif //  DALI_INTERNAL_SCENE_GRAPH_RENDER_ITEM_KEY_H

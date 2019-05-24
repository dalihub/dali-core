#ifndef DALI_INTERNAL_SCENE_GRAPH_SORTED_LAYERS_H
#define DALI_INTERNAL_SCENE_GRAPH_SORTED_LAYERS_H

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

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

class Layer;

/**
 * A container of Layer pointers sorted by depth
 */
typedef std::vector<Layer*> SortedLayerPointers;

typedef SortedLayerPointers::iterator       SortedLayersIter;
typedef SortedLayerPointers::const_iterator SortedLayersConstIter;

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_SORTED_LAYERS_H

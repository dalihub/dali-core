#ifndef DALI_INTERNAL_SCENE_GRAPH_COLLECTED_UNIFORM_MAP_H
#define DALI_INTERNAL_SCENE_GRAPH_COLLECTED_UNIFORM_MAP_H

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
 */
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/update/common/uniform-map.h>
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class UniformMap;
class UniformPropertyMapping;

/**
 * Class to collect uniform mappings together into a new map.
 * Distinct from UniformMap, as it doesn't need the observation/lifecycle
 * overhead.
 *
 * It has a counter that increments each time it's changed, which
 * allows a client to know if it's been modified since the last time
 * the client checked.
 */
struct CollectedUniformMap
{
  /**
   * Constructor
   */
  CollectedUniformMap() = default;

  /**
   * Destructor
   */
  ~CollectedUniformMap() = default;

  /**
   * @brief Add mappings from a UniformMap.
   * @param[in] uniformMap to copy
   */
  void AddMappings(const UniformMap& uniformMap);

  /**
   * Clear the mappings
   */
  inline void Clear()
  {
    mUniformMap.Clear();
    UpdateChangeCounter();
  }

  /**
   * Reserve space for mappings
   * @param[in] size The number of mappings to reserve
   */
  inline void Reserve(std::size_t size)
  {
    mUniformMap.Reserve(size);
  }

  /**
   * @return the count of the number of mappings
   */
  inline std::size_t Count() const
  {
    return mUniformMap.Size();
  }

  /**
   * @brief Update the change counter if the map changes.
   */
  void UpdateChangeCounter()
  {
    ++mChangeCounter;
  }

  /**
   * Return the currently calculated hash.
   */
  inline std::size_t GetChangeCounter() const
  {
    return mChangeCounter;
  }

  Dali::Vector<const UniformPropertyMapping*> mUniformMap;        ///< The mappings
  std::size_t                          mChangeCounter{0u}; ///< The change counter
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_COLLECTED_UNIFORM_MAP_H

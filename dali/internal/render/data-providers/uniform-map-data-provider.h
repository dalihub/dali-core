#ifndef DALI_INTERNAL_SCENE_GRAPH_UNIFORM_MAP_DATA_PROVIDER_H
#define DALI_INTERNAL_SCENE_GRAPH_UNIFORM_MAP_DATA_PROVIDER_H

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
 */
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/update/common/collected-uniform-map.h>
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
/**
 * This class maps uniform names to property value pointers.
 */
class UniformMapDataProvider
{
public:
  /**
   * Constructor
   */
  UniformMapDataProvider() = default;

  /**
   * Get the collected map of uniforms to property value addresses
   *
   * @return the uniform map
   */
  [[nodiscard]] virtual const CollectedUniformMap& GetCollectedUniformMap() const = 0;

protected:
  /**
   * No deletion through this interface
   */
  virtual ~UniformMapDataProvider() = default;
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_UNIFORM_MAP_DATA_PROVIDER_H

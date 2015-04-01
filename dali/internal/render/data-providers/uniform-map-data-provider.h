#ifndef DALI_INTERNAL_SCENE_GRAPH_UNIFORM_MAP_DATA_PROVIDER_H
#define DALI_INTERNAL_SCENE_GRAPH_UNIFORM_MAP_DATA_PROVIDER_H

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-vector.h>
#include <dali/integration-api/resource-declarations.h>
#include <dali/internal/common/buffer-index.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class UniformMap;
class UniformPropertyMapping;

typedef Dali::Vector< const UniformPropertyMapping* > CollectedUniformMap;

/**
 * This class maps uniform names to property value pointers.
 */
class UniformMapDataProvider
{
public:
  /**
   * Constructor
   */
  UniformMapDataProvider()
  {
  }

  /**
   * Return true if the uniform map has been changed this frame
   * Note, this only informs if the uniform mappings have changed,
   * not if any actual property value has changed.
   *
   * @param[in] bufferIndex The buffer index
   * @return true if the uniform map has changed
   */
  virtual bool GetUniformMapChanged( BufferIndex bufferIndex ) const = 0;

  /**
   * Get the complete map of uniforms to property value addresses
   * (The map is double buffered - it can be retrieved through this interface)
   *
   * @param[in] bufferIndex The bufferIndex
   * @return the uniform map
   */
  virtual const CollectedUniformMap& GetUniformMap( BufferIndex bufferIndex ) const = 0;

protected:
  /**
   * No deletion through this interface
   */
  virtual ~UniformMapDataProvider()
  {
  }
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_UNIFORM_MAP_DATA_PROVIDER_H

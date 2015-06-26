#ifndef DALI_INTERNAL_SCENE_GRAPH_SAMPLER_DATA_PROVIDER_H
#define DALI_INTERNAL_SCENE_GRAPH_SAMPLER_DATA_PROVIDER_H

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

#include <dali/devel-api/rendering/sampler.h>
#include <dali/integration-api/resource-declarations.h>
#include <dali/internal/common/buffer-index.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

class SamplerDataProvider
{
public:
  typedef Dali::Sampler::FilterMode FilterMode;
  typedef Dali::Sampler::WrapMode   WrapMode;
  typedef Dali::Integration::ResourceId ResourceId;

  /**
   * Constructor
   */
  SamplerDataProvider()
  {
  }

  /**
   * Get the sampler's texture unit uniform name
   * @return The texture unit uniform name
   */
  virtual const std::string& GetTextureUnitUniformName() const = 0;

  /**
   * Get the texture identity associated with the sampler
   * @return The texture identity
   */
  virtual ResourceId GetTextureId( BufferIndex bufferIndex ) const = 0;

  /**
   * Get the filter mode
   * @param[in] bufferIndex The buffer index to use
   * @return The minify filter mode
   */
  virtual FilterMode GetMinifyFilterMode( BufferIndex bufferIndex ) const = 0;

  /**
   * Get the filter mode
   * @param[in] bufferIndex The buffer index to use
   * @return The magnify filter mode
   */
  virtual FilterMode GetMagnifyFilterMode( BufferIndex bufferIndex ) const = 0;

  /**
   * Get the horizontal wrap mode
   * @param[in] bufferIndex The buffer index to use
   * @return The horizontal wrap mode
   */
  virtual WrapMode GetUWrapMode( BufferIndex bufferIndex ) const = 0;

  /**
   * Get the vertical wrap mode
   * @param[in] bufferIndex The buffer index to use
   * @return The vertical wrap mode
   */
  virtual WrapMode GetVWrapMode( BufferIndex bufferIndex ) const = 0;

protected:
  /**
   * No deletion through this interface
   */
  virtual ~SamplerDataProvider()
  {
  }
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_SAMPLER_DATA_PROVIDER_H

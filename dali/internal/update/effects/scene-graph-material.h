#ifndef DALI_INTERNAL_SCENE_GRAPH_MATERIAL_H
#define DALI_INTERNAL_SCENE_GRAPH_MATERIAL_H

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

#include <dali/public-api/actors/renderable-actor.h> // For CullFaceMode
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/blending-options.h>
#include <dali/internal/update/common/double-buffered.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/render/renderers/material-data-provider.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class Sampler;
class Shader;

class Material : public PropertyOwner, public MaterialDataProvider
{
public:
  typedef Dali::Vector< Sampler* > Samplers;

  /**
   * Constructor
   */
  Material();

  /**
   * Destructor
   */
  virtual ~Material();

  /**
   * Set the shader effect for this material
   * @param[in] shader The shader effect to use
   */
  void SetShader( Shader* shader );

  /**
   * Get the shader effect of this material
   * @return the shader effect;
   */
  Shader* GetShader();

  /**
   * Add a sampler (image + sampler modes) to the material
   * @param[in] sampler A sampler to add
   */
  void AddSampler( Sampler* sampler );

  /**
   * Remove a sampler (image + sampler modes) from the material
   * @param[in] sampler A sampler to remove
   */
  void RemoveSampler( Sampler* sampler );

  /**
   * Get the samplers this material uses.
   * @return the samplers
   */
  const Samplers& GetSamplers() const;

  /**
   * Set the face culling mode for geometry used by this material
   * @param[in] mode The face culling mode to use
   */
  void SetFaceCulling( BufferIndex updateBufferIndex, Dali::CullFaceMode mode );

  /**
   * Get the face culling mode
   * @return the face culling mdoe
   */
  CullFaceMode GetFaceCulling() const ;

  // @note Blending mode is per actor, not per material (requires actor color)

  /**
   * Set the blending options. This should only be called from the update-thread.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] options A bitmask of blending options.
   */
  void SetBlendingOptions( BufferIndex updateBufferIndex, unsigned int options );

  /**
   * Set the blend-color. This should only be called from the update-thread.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] color The new blend-color.
   */
  void SetBlendColor( BufferIndex updateBufferIndex, const Vector4& color );

private:
  Shader* mShader;
  Samplers mSamplers; // Not owned (though who does?)
  Dali::CullFaceMode mCullFaceMode:3;
  BlendingOptions mBlendingOptions;
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif //  DALI_INTERNAL_SCENE_GRAPH_MATERIAL_H

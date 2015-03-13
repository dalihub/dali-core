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
#include <dali/internal/common/event-to-update.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/double-buffered.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/render/data-providers/material-data-provider.h>

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
  void SetShader( const Shader* shader );

  /**
   * Get the shader effect of this material
   * @return the shader effect;
   */
  virtual Shader* GetShader() const;

  /**
   * Add a sampler (image + sampler modes) to the material
   * @param[in] sampler A sampler to add
   */
  void AddSampler( const Sampler* sampler );

  /**
   * Remove a sampler (image + sampler modes) from the material
   * @param[in] sampler A sampler to remove
   */
  void RemoveSampler( const Sampler* sampler );

  /**
   * Get the samplers this material uses.
   * @return the samplers
   */
  virtual const Samplers& GetSamplers() const;

private:
  const Shader* mShader;
  Samplers mSamplers; // Not owned

  AnimatableProperty<Vector4> mColor;

  // @todo MESH_REWORK add property values for cull face mode, blending options, blend color
  // Add getters/setters?
};

inline void SetShaderMessage( EventToUpdate& eventToUpdate, const Material& material, const Shader& shader )
{
  typedef MessageValue1< Material, const Shader* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &material, &Material::SetShader, &shader );
}

inline void AddSamplerMessage( EventToUpdate& eventToUpdate, const Material& material, const Sampler& sampler )
{
  typedef MessageValue1< Material, const Sampler* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &material, &Material::AddSampler, &sampler );
}

inline void RemoveSamplerMessage( EventToUpdate& eventToUpdate, const Material& material, const Sampler& sampler )
{
  typedef MessageValue1< Material, const Sampler* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &material, &Material::RemoveSampler, &sampler );
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif //  DALI_INTERNAL_SCENE_GRAPH_MATERIAL_H

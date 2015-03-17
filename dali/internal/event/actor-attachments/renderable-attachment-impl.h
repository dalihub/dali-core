#ifndef __DALI_INTERNAL_RENDERABLE_ATTACHMENT_H__
#define __DALI_INTERNAL_RENDERABLE_ATTACHMENT_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/renderable-actor.h>
#include <dali/public-api/actors/blending.h>
#include <dali/internal/common/blending-options.h>
#include <dali/internal/common/image-sampler.h>
#include <dali/internal/event/actor-attachments/actor-attachment-impl.h>
#include <dali/internal/event/effects/shader-declarations.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{
class RenderableAttachment;
}

/**
 * An base class for renderable actor attachments
 */
class RenderableAttachment : public ActorAttachment
{
public:

  /**
   * Set the depth-sort modifier for the renderable.
   * This modifies the back-to-front distance calculation, when rendering with transparency.
   * This is useful for ordering transparent objects that are positioned close to each other.
   * @param[in] modifier The depth-sort modifier.
   */
  void SetSortModifier(float modifier);

  /**
   * From Renderable; Retrieve the depth-sort modifier for the renderable.
   * @return The depth-sort modifier.
   */
  float GetSortModifier() const;

  /**
   * Set the face-culling mode for this attachment.
   * @param[in] mode The culling mode.
   */
  void SetCullFace(CullFaceMode mode);

  /**
   * Retrieve the face-culling mode for this attachment.
   * @return mode The culling mode.
   */
  CullFaceMode GetCullFace() const;

  /**
   * @copydoc Dali::RenderableActor::SetBlendMode()
   */
  void SetBlendMode( BlendingMode::Type mode );

  /**
   * @copydoc Dali::RenderableActor::GetBlendMode()
   */
  BlendingMode::Type GetBlendMode() const;

  /**
   * @copydoc Dali::RenderableActor::SetBlendFunc()
   */
  void SetBlendFunc( BlendingFactor::Type srcFactorRgb,   BlendingFactor::Type destFactorRgb,
                     BlendingFactor::Type srcFactorAlpha, BlendingFactor::Type destFactorAlpha );

  /**
   * @copydoc Dali::RenderableActor::GetBlendFunc()
   */
  void GetBlendFunc( BlendingFactor::Type& srcFactorRgb,   BlendingFactor::Type& destFactorRgb,
                     BlendingFactor::Type& srcFactorAlpha, BlendingFactor::Type& destFactorAlpha ) const;

  /**
   * @copydoc Dali::RenderableActor::SetBlendEquation()
   */
  void SetBlendEquation( BlendingEquation::Type equationRgb, BlendingEquation::Type equationAlpha );

  /**
   * @copydoc Dali::RenderableActor::GetBlendEquation()
   */
  void GetBlendEquation( BlendingEquation::Type& equationRgb, BlendingEquation::Type& equationAlpha ) const;

  /**
   * @copydoc Dali::RenderableActor::SetBlendColor()
   */
  void SetBlendColor( const Vector4& color );

  /**
   * @copydoc Dali::RenderableActor::GetBlendColor()
   */
  const Vector4& GetBlendColor() const;

  /**
   * @copydoc Dali::RenderableActor::SetFilterMode()
   */
  void SetFilterMode( FilterMode::Type minFilter, FilterMode::Type magFilter );

  /**
   * @copydoc Dali::RenderableActor::GetFilterMode()
   */
  void GetFilterMode( FilterMode::Type& minFilter, FilterMode::Type& magFilter ) const;

  /**
   * @copydoc Dali::RenderableActor::SetShaderEffect
   */
  void SetShaderEffect(ShaderEffect& effect);

  /**
   * @copydoc Dali::RenderableActor::GetShaderEffect
   */
  ShaderEffectPtr GetShaderEffect() const;

  /**
   * @copydoc Dali::RenderableActor::RemoveShaderEffect
   */
  void RemoveShaderEffect();

protected:

  /**
   * Protected constructor; only base classes are instantiatable.
   * @param[in] eventThreadServices Used for messaging to and reading from scene-graph.
   */
  RenderableAttachment( EventThreadServices& eventThreadServices );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~RenderableAttachment();

private:

  // Undefined
  RenderableAttachment(const RenderableAttachment&);

  // Undefined
  RenderableAttachment& operator=(const RenderableAttachment& rhs);

  /**
   * Helper for getting the scene-graph renderable attachment.
   * @return The scene object.
   */
  const SceneGraph::RenderableAttachment& GetSceneAttachment() const;

  /**
   * @copydoc Dali::Internal::ActorAttachment::OnStageConnection()
   */
  virtual void OnStageConnection();

  /**
   * @copydoc Dali::Internal::ActorAttachment::OnStageDisconnection()
   */
  virtual void OnStageDisconnection();

  /**
   * For derived classes, chained from OnStageConnection()
   */
  virtual void OnStageConnection2() = 0;

  /**
   * For derived classes, chained from OnStageDisconnection()
   */
  virtual void OnStageDisconnection2() = 0;

  /**
   * For derived classes to provide a corresponding scene-graph object
   * @return The scene-object.
   */
  virtual const SceneGraph::RenderableAttachment& GetSceneObject() const = 0;

private: // Data, cached for actor-thread getters

  ShaderEffectPtr    mShaderEffect;    ///< Optional referenced shader effect
  BlendingOptions    mBlendingOptions;
  unsigned int       mSamplerBitfield;
  float              mSortModifier;
  CullFaceMode       mCullFaceMode:3;  ///< cullface mode, 3 bits enough for 4 values
  BlendingMode::Type mBlendingMode:2;  ///< blending mode, 2 bits enough for 3 values


};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_RENDERABLE_ATTACHMENT_H__

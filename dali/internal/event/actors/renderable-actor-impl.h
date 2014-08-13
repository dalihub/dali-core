#ifndef __DALI_INTERNAL_RENDERABLE_ACTOR_H__
#define __DALI_INTERNAL_RENDERABLE_ACTOR_H__

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
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/event/actors/actor-impl.h>

namespace Dali
{

namespace Internal
{

class RenderableAttachment;

/**
 * An abstract base class for renderable actors
 */
class RenderableActor : public Actor
{
public:

  /**
   * @copydoc Dali::RenderableActor::SetSortModifier()
   */
  void SetSortModifier(float modifier);

  /**
   * @copydoc Dali::RenderableActor::GetSortModifier()
   */
  float GetSortModifier() const;

  /**
   * @copydoc Dali::RenderableActor::SetCullFace()
   */
  void SetCullFace(CullFaceMode mode);

  /**
   * @copydoc Dali::RenderableActor::GetCullFace()
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
  void SetBlendFunc( BlendingFactor::Type srcFactorRgba,   BlendingFactor::Type destFactorRgba );

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
  void SetBlendEquation( BlendingEquation::Type equationRgba );

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

protected:

  /**
   * Protected constructor; only derived classes are instantiatable.
   */
  RenderableActor();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~RenderableActor();

public: // from Actor, in future not virtual. Accessible also from RenderableActor

  /**
   * @copydoc Actor::SetShaderEffect
   */
  virtual void SetShaderEffect(ShaderEffect& effect);

  /**
   * @copydoc Actor::GetShaderEffect
   */
  virtual ShaderEffectPtr GetShaderEffect() const;

  /**
   * @copydoc Actor::RemoveShaderEffect
   */
  virtual void RemoveShaderEffect();

private:

  /**
   * @return a reference to the renderable attachment
   */
  virtual RenderableAttachment& GetRenderableAttachment() const = 0;

  // Undefined
  RenderableActor(const RenderableActor&);
  // Undefined
  RenderableActor& operator=(const RenderableActor& rhs);

};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::RenderableActor& GetImplementation(Dali::RenderableActor& renderable)
{
  DALI_ASSERT_ALWAYS( renderable && "RenderableActor handle is empty" );

  BaseObject& handle = renderable.GetBaseObject();

  return static_cast<Internal::RenderableActor&>(handle);
}

inline const Internal::RenderableActor& GetImplementation(const Dali::RenderableActor& renderable)
{
  DALI_ASSERT_ALWAYS(renderable && "RenderableActor handle is empty" );

  const BaseObject& handle = renderable.GetBaseObject();

  return static_cast<const Internal::RenderableActor&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_RENDERABLE_ACTOR_H__

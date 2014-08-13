#ifndef __DALI_RENDERABLE_ACTOR_H__
#define __DALI_RENDERABLE_ACTOR_H__

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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/blending.h>
#include <dali/public-api/actors/sampling.h>

namespace Dali DALI_IMPORT_API
{
namespace Internal DALI_INTERNAL
{
class RenderableActor;
}

/**
 * @brief Face culling modes.
 */
enum CullFaceMode
{
  CullNone,                 ///< Face culling disabled
  CullFront,                ///< Cull front facing polygons
  CullBack,                 ///< Cull back facing polygons
  CullFrontAndBack          ///< Cull front and back facing polygons
};


/**
 * @brief A base class for renderable actors.
 */
class DALI_IMPORT_API RenderableActor : public Actor
{
public:

  static const BlendingMode::Type DEFAULT_BLENDING_MODE; ///< default value is BlendingMode::AUTO

  /**
   * @brief Create an uninitialized actor.
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  RenderableActor();

  /**
   * @brief Downcast an Object handle to RenderableActor.
   *
   * If handle points to a RenderableActor the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a RenderableActor or an uninitialized handle
   */
  static RenderableActor DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~RenderableActor();

  /**
   * @brief Copy constructor
   *
   * @param [in] copy The actor to copy.
   */
  RenderableActor(const RenderableActor& copy);

  /**
   * @brief Assignment operator
   *
   * @param [in] rhs The actor to copy.
   */
  RenderableActor& operator=(const RenderableActor& rhs);

  /**
   * @brief This method is defined to allow assignment of the NULL value,
   * and will throw an exception if passed any other value.
   *
   * Assigning to NULL is an alias for Reset().
   * @param [in] rhs  A NULL pointer
   * @return A reference to this handle
   */
  RenderableActor& operator=(BaseHandle::NullType* rhs);

  /**
   * @brief Allows modification of an actors position in the depth sort algorithm.
   *
   * The offset can be altered for each coplanar actor hence allowing an order of painting.
   * @pre The Actor has been initialized.
   * @param [in] depthOffset the offset to be given to the actor. Positive values pushing it further back.
   */
  void SetSortModifier(float depthOffset);

  /**
   * @brief Retrieves the offset used to modify an actors position in the depth sort algorithm.
   *
   * The offset can be altered for each coplanar actor hence allowing an order of painting.
   * @pre The Actor has been initialized.
   * @return  the offset that has been given to the actor. Positive values pushing it further back.
   */
  float GetSortModifier() const;

  /**
   * @brief Set the face-culling mode for this actor.
   *
   * @param[in] mode The culling mode.
   */
  void SetCullFace(CullFaceMode mode);

  /**
   * @brief Retrieve the face-culling mode for this actor.
   *
   * @return mode The culling mode.
   */
  CullFaceMode GetCullFace() const;

  /**
   * @brief Sets the blending mode.
   *
   * Possible values are: BlendingMode::OFF, BlendingMode::AUTO and BlendingMode::ON. Default is BlendingMode::AUTO.
   *
   * If blending is disabled (BlendingMode::OFF) fade in and fade out animations do not work.
   *
   * <ul>
   *   <li> \e OFF Blending is disabled.
   *   <li> \e AUTO Blending is enabled only if the renderable actor has alpha channel.
   *   <li> \e ON Blending is enabled.
   * </ul>
   *
   * @param[in] mode The blending mode.
   */
  void SetBlendMode( BlendingMode::Type mode );

  /**
   * @brief Retrieves the blending mode.
   *
   * @return The blending mode, one of BlendingMode::OFF, BlendingMode::AUTO or BlendingMode::ON.
   */
  BlendingMode::Type GetBlendMode() const;

  /**
   * @brief Specify the pixel arithmetic used when the actor is blended.
   *
   * @param[in] srcFactorRgba Specifies how the red, green, blue, and alpha source blending factors are computed.
   * The options are BlendingFactor::ZERO, ONE, SRC_COLOR, ONE_MINUS_SRC_COLOR, DST_COLOR, ONE_MINUS_DST_COLOR,
   * SRC_ALPHA, ONE_MINUS_SRC_ALPHA, DST_ALPHA, ONE_MINUS_DST_ALPHA, CONSTANT_COLOR, ONE_MINUS_CONSTANT_COLOR,
   * GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA, and GL_SRC_ALPHA_SATURATE.
   *
   * @param[in] destFactorRgba Specifies how the red, green, blue, and alpha destination blending factors are computed.
   * The options are BlendingFactor::ZERO, ONE, SRC_COLOR, ONE_MINUS_SRC_COLOR, DST_COLOR, ONE_MINUS_DST_COLOR,
   * SRC_ALPHA, ONE_MINUS_SRC_ALPHA, DST_ALPHA, ONE_MINUS_DST_ALPHA, CONSTANT_COLOR, ONE_MINUS_CONSTANT_COLOR,
   * GL_CONSTANT_ALPHA, and GL_ONE_MINUS_CONSTANT_ALPHA.
   */
  void SetBlendFunc( BlendingFactor::Type srcFactorRgba, BlendingFactor::Type destFactorRgba );

  /**
   * @brief Specify the pixel arithmetic used when the actor is blended.
   *
   * @param[in] srcFactorRgb Specifies how the red, green, and blue source blending factors are computed.
   * The options are BlendingFactor::ZERO, ONE, SRC_COLOR, ONE_MINUS_SRC_COLOR, DST_COLOR, ONE_MINUS_DST_COLOR,
   * SRC_ALPHA, ONE_MINUS_SRC_ALPHA, DST_ALPHA, ONE_MINUS_DST_ALPHA, CONSTANT_COLOR, ONE_MINUS_CONSTANT_COLOR,
   * GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA, and GL_SRC_ALPHA_SATURATE.
   *
   * @param[in] destFactorRgb Specifies how the red, green, blue, and alpha destination blending factors are computed.
   * The options are BlendingFactor::ZERO, ONE, SRC_COLOR, ONE_MINUS_SRC_COLOR, DST_COLOR, ONE_MINUS_DST_COLOR,
   * SRC_ALPHA, ONE_MINUS_SRC_ALPHA, DST_ALPHA, ONE_MINUS_DST_ALPHA, CONSTANT_COLOR, ONE_MINUS_CONSTANT_COLOR,
   * GL_CONSTANT_ALPHA, and GL_ONE_MINUS_CONSTANT_ALPHA.
   *
   * @param[in] srcFactorAlpha Specifies how the alpha source blending factor is computed.
   * The options are the same as for srcFactorRgb.
   *
   * @param[in] destFactorAlpha Specifies how the alpha source blending factor is computed.
   * The options are the same as for destFactorRgb.
   */
  void SetBlendFunc( BlendingFactor::Type srcFactorRgb,   BlendingFactor::Type destFactorRgb,
                     BlendingFactor::Type srcFactorAlpha, BlendingFactor::Type destFactorAlpha );

  /**
   * @brief Query the pixel arithmetic used when the actor is blended.
   *
   * @param[out] srcFactorRgb Specifies how the red, green, blue, and alpha source blending factors are computed.
   * @param[out] destFactorRgb Specifies how the red, green, blue, and alpha destination blending factors are computed.
   * @param[out] srcFactorAlpha Specifies how the red, green, blue, and alpha source blending factors are computed.
   * @param[out] destFactorAlpha Specifies how the red, green, blue, and alpha destination blending factors are computed.
   */
  void GetBlendFunc( BlendingFactor::Type& srcFactorRgb,   BlendingFactor::Type& destFactorRgb,
                     BlendingFactor::Type& srcFactorAlpha, BlendingFactor::Type& destFactorAlpha ) const;

  /**
   * @brief Specify the equation used when the actor is blended.
   *
   * The options are BlendingEquation::ADD, SUBTRACT, or REVERSE_SUBTRACT.
   * @param[in] equationRgba The equation used for combining red, green, blue, and alpha components.
   */
  void SetBlendEquation( BlendingEquation::Type equationRgba );

  /**
   * @brief Specify the equation used when the actor is blended.
   *
   * @param[in] equationRgb The equation used for combining red, green, and blue components.
   * @param[in] equationAlpha The equation used for combining the alpha component.
   * The options are BlendingEquation::ADD, SUBTRACT, or REVERSE_SUBTRACT.
   */
  void SetBlendEquation( BlendingEquation::Type equationRgb, BlendingEquation::Type equationAlpha );

  /**
   * @brief Query the equation used when the actor is blended.
   *
   * @param[out] equationRgb The equation used for combining red, green, and blue components.
   * @param[out] equationAlpha The equation used for combining the alpha component.
   */
  void GetBlendEquation( BlendingEquation::Type& equationRgb, BlendingEquation::Type& equationAlpha ) const;

  /**
   * @brief Specify the color used when the actor is blended; the default is Vector4::ZERO.
   *
   * @param[in] color The blend color.
   */
  void SetBlendColor( const Vector4& color );

  /**
   * @brief Query the color used when the actor is blended.
   *
   * @return The blend color.
   */
  const Vector4& GetBlendColor() const;

  /**
   * @brief Sets the filtering mode.
   *
   * Possible values are: FilterMode::NEAREST and FilterMode::LINEAR. Default is FilterMode::LINEAR.
   *
   * <ul>
   *   <li> \e NEAREST Use nearest filtering
   *   <li> \e LINEAR Use linear filtering
   * </ul>
   *
   * @param[in] minFilter The minification filtering mode.
   * @param[in] magFilter The magnification filtering mode.
   */
  void SetFilterMode( FilterMode::Type minFilter, FilterMode::Type magFilter );

  /**
   * @brief Retrieves the filtering mode.
   *
   * @param[out] minFilter The return minification value
   * @param[out] magFilter The return magnification value
   */
  void GetFilterMode( FilterMode::Type& minFilter, FilterMode::Type& magFilter) const;

  /**
   * @brief Sets the shader effect for the RenderableActor.
   *
   * Shader effects provide special effects like ripple and bend.
   * Setting a shader effect removes any shader effect previously set by SetShaderEffect.
   * @pre The actor has been initialized.
   * @pre effect has been initialized.
   * @param [in] effect The shader effect.
   */
  void SetShaderEffect( ShaderEffect effect );

  /**
   * @brief Retrieve the custom shader effect for the RenderableActor.
   * If default shader is used an empty handle is returned.
   *
   * @pre The Actor has been initialized.
   * @return The shader effect
   */
  ShaderEffect GetShaderEffect() const;

  /**
   * @brief Removes the current shader effect.
   *
   * @pre The Actor has been initialized.
   */
  void RemoveShaderEffect();

public: // Not intended for application developers

  /**
   * @brief This constructor is used by Dali New() methods
   *
   * @param [in] actor A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL RenderableActor(Internal::RenderableActor* actor);
};

/**
 * @brief Sets the shader effect for all RenderableActors in a tree of Actors.
 *
 * @see RenderableActor::SetShaderEffect
 *
 * @param [in] actor root of a tree of actors.
 * @param [in] effect The shader effect.
 */
void SetShaderEffectRecursively( Actor actor, ShaderEffect effect );

/**
 * @brief Removes the shader effect from all RenderableActors in a tree of Actors.
 *
 * @see RenderableActor::RemoveShaderEffect
 *
 * @param [in] actor root of a tree of actors.
 */
void RemoveShaderEffectRecursively( Actor actor );

} // namespace Dali

#endif // __DALI_RENDERABLE_ACTOR_H__

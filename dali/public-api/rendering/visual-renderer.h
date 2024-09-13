#ifndef DALI_VISUAL_RENDERER_H
#define DALI_VISUAL_RENDERER_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/rendering/renderer.h>

namespace Dali
{
/**
 * @addtogroup dali_core_rendering_effects
 * @{
 */

namespace Internal DALI_INTERNAL
{
class VisualRenderer;
}

/**
 * @brief VisualRenderer is a handle to a Renderer with extra properties for Toolkit::Visuals
 *
 * @SINCE_2_1.13
 */
class DALI_CORE_API VisualRenderer : public Renderer
{
public:
  /**
   * @brief Policies used by the transform for the offset or size.
   * @SINCE_2_1.13
   */
  struct TransformPolicy
  {
    /**
     * @brief Enumeration for the type of Transform Policy.
     * @SINCE_2_1.13
     */
    enum Type
    {
      RELATIVE = 0, ///< Relative to the control (percentage [0.0f to 1.0f] of the control). @SINCE_2_1.13
      ABSOLUTE = 1  ///< Absolute value in world units. @SINCE_2_1.13
    };
  };

  /**
   * @brief Enumeration for instances of properties belonging to the VisualRenderer class.
   * @SINCE_2_1.13
   */
  struct Property
  {
    static constexpr Dali::Property::Index DEFAULT_VISUAL_RENDERER_PROPERTY_START_INDEX = DEFAULT_RENDERER_PROPERTY_START_INDEX + DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION;

    /**
     * @brief Enumeration for instances of properties belonging to the VisualRenderer class.
     * @SINCE_2_1.13
     */
    enum
    {
      /**
       * @brief Name "transformOffset", type Property::Vector2, animatable.
       * @SINCE_2_1.13
       * @note The default value is (0,0).
       */
      TRANSFORM_OFFSET = DEFAULT_VISUAL_RENDERER_PROPERTY_START_INDEX,

      /**
       * @brief Size of the visual, which can be either relative (percentage [0.0f to 1.0f] of the parent) or absolute (in world units).
       * @details Name "transformSize", type Property::VECTOR2, animatable.
       * @SINCE_2_1.13
       * @see TRANSFORM_SIZE_POLICY
       */
      TRANSFORM_SIZE,

      /**
       * @brief The origin of the visual renderer within its control area.
       * @details Name "transformOrigin", type Property::VECTOR2
       * @note This is relative to the size of the actor, where the origin is the center of the actor,
       * and the range is -0.5 to +0.5 vertically and horizontally
       * @note Toolkit may impose additional layout directions
       * @SINCE_2_1.13
       * @note The default is top left
       */
      TRANSFORM_ORIGIN,

      /**
       * @brief The anchor-point of the visual renderer
       * @details Name "transformAnchorPoint", type Property::VECTOR2
       * @note This is relative to the size of the actor; where the origin is the center of the actor,
       * and the range is -0.5 to +0.5 vertically and horizontally
       * @note Toolkit may impose additional layout directions
       * @SINCE_2_1.13
       * @note The default is top left.
       */
      TRANSFORM_ANCHOR_POINT,

      /**
       * @brief Whether the x or y OFFSET/SIZE values are relative (percentage [0.0f to 1.0f] of the control) or absolute (in world units).
       * @details Name "transformOffsetSizeMode", type Property::VECTOR4
       *
       * @code
       * visualRenderer.SetProperty( VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE,
       *                             Vector4( Policy::ABSOLUTE, Policy::RELATIVE, Policy::ABSOLUTE, Policy::RELATIVE ));
       * @endcode
       * @see Policy::Type
       * @SINCE_2_1.13
       * @note By default, both the x and the y offset/size is RELATIVE.
       */
      TRANSFORM_OFFSET_SIZE_MODE,

      /**
       * @brief Any extra size the shader needs for drawing into.
       * @details Name "extraSize", type Property::VECTOR2, animatable
       * @SINCE_2_1.13
       * @note the default is (0, 0)
       */
      EXTRA_SIZE,

      /**
       * @brief Mix color is a generic color for any visual.
       * @details Name "visualMixColor", type Property::VECTOR3
       * @SINCE_2_1.13
       * @DEPRECATED_2_3.41 Use Renderer::Property::MIX_COLOR instead.
       */
      VISUAL_MIX_COLOR,

      /**
       * @brief PremultipliedAlpha is a float representing a bool, and is either 0.0f, or 1.0f
       * @details Name "visualPreMultipliedAlpha", type Property::FLOAT
       * @SINCE_2_1.13
       * @DEPRECATED_2_3.41 Not be added as uniform anymore. Use Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA instead.
       */
      VISUAL_PRE_MULTIPLIED_ALPHA,
    };
  };

  /**
   * @brief Creates a new Renderer object.
   *
   * @SINCE_2_1.13
   * @param[in] geometry Geometry to be used by this renderer
   * @param[in] shader Shader to be used by this renderer
   * @return A handle to the Renderer
   */
  static VisualRenderer New(Geometry& geometry, Shader& shader);

  /**
   * @brief Default constructor, creates an empty handle
   *
   * @SINCE_2_1.13
   */
  VisualRenderer();

  /**
   * @brief Destructor.
   *
   * @SINCE_2_1.13
   */
  ~VisualRenderer();

  /**
   * @brief Copy constructor, creates a new handle to the same object.
   *
   * @SINCE_2_1.13
   * @param[in] handle Handle to an object
   */
  VisualRenderer(const VisualRenderer& handle);

  /**
   * @brief Downcasts to a visual renderer handle.
   * If not, a renderer the returned visual renderer handle is left uninitialized.
   *
   * @SINCE_2_1.13
   * @param[in] handle Handle to an object
   * @return Renderer handle or an uninitialized handle
   */
  static VisualRenderer DownCast(BaseHandle handle);

  /**
   * @brief Assignment operator, changes this handle to point at the same object.
   *
   * @SINCE_2_1.13
   * @param[in] handle Handle to an object
   * @return Reference to the assigned object
   */
  VisualRenderer& operator=(const VisualRenderer& handle);

  /**
   * @brief Move constructor.
   *
   * @SINCE_2_1.13
   * @param[in] rhs A reference to the moved handle
   */
  VisualRenderer(VisualRenderer&& rhs) noexcept;

  /**
   * @brief Move assignment operator.
   *
   * @SINCE_2_1.13
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this handle
   */
  VisualRenderer& operator=(VisualRenderer&& rhs) noexcept;

public:
  /// @cond internal
  /**
   * @brief The constructor.
   * @note  Not intended for application developers.
   * @SINCE_2_1.13
   * @param[in] pointer A pointer to a newly allocated VisualRenderer
   */
  explicit DALI_INTERNAL VisualRenderer(Internal::VisualRenderer* pointer);
  /// @endcond
};

/**
 * @}
 */
} //namespace Dali

#endif // DALI_VISUAL_RENDERER_H

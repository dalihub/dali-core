#ifndef DALI_VISUAL_RENDERER_H
#define DALI_VISUAL_RENDERER_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
       * @brief Name "transformOffsetX", type Property::FLOAT, animatable.
       * @SINCE_2_5.22
       */
      TRANSFORM_OFFSET_X,

      /**
       * @brief Name "transformOffsetY", type Property::FLOAT, animatable.
       * @SINCE_2_5.22
       */
      TRANSFORM_OFFSET_Y,

      /**
       * @brief Size of the visual, which can be either relative (percentage [0.0f to 1.0f] of the parent) or absolute (in world units).
       * @details Name "transformSize", type Property::VECTOR2, animatable.
       * @SINCE_2_1.13
       * @see TRANSFORM_SIZE_POLICY
       */
      TRANSFORM_SIZE,

      /**
       * @brief Width of the visual, which can be either relative (percentage [0.0f to 1.0f] of the parent) or absolute (in world units).
       * @details Name "transformSizeWidth", type Property::FLOAT, animatable.
       * @SINCE_2_5.22
       */
      TRANSFORM_SIZE_WIDTH,

      /**
       * @brief Height of the visual, which can be either relative (percentage [0.0f to 1.0f] of the parent) or absolute (in world units).
       * @details Name "transformSizeHeight", type Property::FLOAT, animatable.
       * @SINCE_2_5.22
       */
      TRANSFORM_SIZE_HEIGHT,

      /**
       * @brief Any extra size the shader needs for drawing into.
       * @details Name "extraSize", type Property::VECTOR2, animatable
       * @SINCE_2_1.13
       * @note the default is (0, 0)
       */
      EXTRA_SIZE,

      /**
       * @brief Any extra width the shader needs for drawing into.
       * @details Name "extraSizeWidth", type Property::FLOAT, animatable
       * @SINCE_2_5.22
       */
      EXTRA_SIZE_WIDTH,

      /**
       * @brief Any extra height the shader needs for drawing into.
       * @details Name "extraSizeHeight", type Property::FLOAT, animatable
       * @SINCE_2_5.22
       */
      EXTRA_SIZE_HEIGHT,

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
       * @brief The x origin of the visual renderer within its control area.
       * @details Name "transformOriginX", type Property::FLOAT.
       * @SINCE_2_5.22
       */
      TRANSFORM_ORIGIN_X,

      /**
       * @brief The y origin of the visual renderer within its control area.
       * @details Name "transformOriginY", type Property::FLOAT.
       * @SINCE_2_5.22
       */
      TRANSFORM_ORIGIN_Y,

      /**
       * @brief The pivot of the visual renderer
       * @details Name "transformPivot", type Property::VECTOR2
       * @note This is relative to the size of the actor; where the origin is the center of the actor,
       * and the range is -0.5 to +0.5 vertically and horizontally
       * @note Toolkit may impose additional layout directions
       * @SINCE_2_1.13
       * @note The default is top left.
       */
      TRANSFORM_PIVOT,

      /**
       * @brief The x pivot of the visual renderer
       * @details Name "transformPivotX", type Property::FLOAT.
       * @SINCE_2_5.22
       */
      TRANSFORM_PIVOT_X,

      /**
       * @brief The y pivot of the visual renderer
       * @details Name "transformPivotY", type Property::FLOAT.
       * @SINCE_2_5.22
       */
      TRANSFORM_PIVOT_Y,

      /**
       * @brief Whether the x or y OFFSET/SIZE values are relative (percentage [0.0f to 1.0f] of the control) or absolute (in world units).
       * @details Name "transformOffsetSizeMode", type Property::VECTOR4
       *
       * @code
       * visualRenderer.SetProperty( VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE,
       *                             Vector4( TransformPolicy::ABSOLUTE, TransformPolicy::RELATIVE, TransformPolicy::ABSOLUTE, TransformPolicy::RELATIVE ));
       * @endcode
       * @see TransformPolicy::Type
       * @SINCE_2_1.13
       * @note By default, both the x and the y offset/size is RELATIVE.
       */
      TRANSFORM_OFFSET_SIZE_MODE,

      /**
       * @brief The x offset transform policy
       * @details Name "transformOffsetXPolicy", type Property::FLOAT.
       * @see TransformPolicy::Type
       * @SINCE_2_5.22
       */
      TRANSFORM_OFFSET_X_POLICY,

      /**
       * @brief The y offset transform policy
       * @details Name "transformOffsetYPolicy", type Property::FLOAT.
       * @see TransformPolicy::Type
       * @SINCE_2_5.22
       */
      TRANSFORM_OFFSET_Y_POLICY,

      /**
       * @brief The width transform policy
       * @details Name "transformSizeWidthPolicy", type Property::FLOAT.
       * @see TransformPolicy::Type
       * @SINCE_2_5.22
       */
      TRANSFORM_SIZE_WIDTH_POLICY,

      /**
       * @brief The height transform policy
       * @details Name "transformSizeHeightPolicy", type Property::FLOAT.
       * @see TransformPolicy::Type
       * @SINCE_2_5.22
       */
      TRANSFORM_SIZE_HEIGHT_POLICY,
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

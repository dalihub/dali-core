#ifndef DALI_DECOREATED_VISUAL_RENDERER_H
#define DALI_DECOREATED_VISUAL_RENDERER_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/rendering/visual-renderer.h>

namespace Dali
{
/**
 * @addtogroup dali_core_rendering_effects
 * @{
 */

namespace Internal DALI_INTERNAL
{
class DecoratedVisualRenderer;
}

/**
 * @brief DecoratedVisualRenderer is a handle to a Renderer with extra properties for Toolkit::Visuals with some decoration
 *
 * @SINCE_2_1.21
 */
class DALI_CORE_API DecoratedVisualRenderer : public VisualRenderer
{
public:
  /**
   * @brief Enumeration for instances of properties belonging to the DecoratedVisualRenderer class.
   * @SINCE_2_1.21
   */
  struct Property
  {
    static constexpr Dali::Property::Index DEFAULT_DECORATED_VISUAL_RENDERER_PROPERTY_START_INDEX = VisualRenderer::Property::DEFAULT_VISUAL_RENDERER_PROPERTY_START_INDEX + DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION;

    /**
     * @brief Enumeration for instances of properties belonging to the DecoratedVisualRenderer class.
     * @SINCE_2_1.21
     */
    enum
    {
      /**
       * @brief The radius for the rounded corners of the visual.
       * @details Name "cornerRadius", type Property::Vector4, animatable.
       *
       * @see Dali::Toolkit::DevelVisual::Property::CORNER_RADIUS
       * @SINCE_2_1.21
       * @note The default value is (0, 0, 0, 0).
       */
      CORNER_RADIUS = DEFAULT_DECORATED_VISUAL_RENDERER_PROPERTY_START_INDEX,

      /**
       * @brief Whether the CORNER_RADIUS values are relative (percentage [0.0f to 0.5f] of the control) or absolute (in world units).
       * @details Name "cornerRadiusPolicy", type Property::FLOAT
       *
       * @code
       * decoratedVisualRenderer.SetProperty( DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY, Policy::ABSOLUTE );
       * @endcode
       *
       * @see Policy::Type
       * @see Dali::Toolkit::DevelVisual::Property::CORNER_RADIUS_POLICY
       * @SINCE_2_1.21
       * @note The default value is Policy::RELATIVE.
       */
      CORNER_RADIUS_POLICY,

      /**
       * @brief The width for the borderline of the visual.
       * @details Name "borderlineWidth", type Property::FLOAT, animatable.
       *
       * @see Dali::Toolkit::DevelVisual::Property::BORDERLINE_WIDTH
       * @SINCE_2_1.21
       * @note The default value is 0.0.
       */
      BORDERLINE_WIDTH,

      /**
       * @brief The color for the borderline of the visual.
       * @details Name "borderlineColor", type Property::Vector4, animatable.
       *
       * @see Dali::Toolkit::DevelVisual::Property::BORDERLINE_COLOR
       * @SINCE_2_1.21
       * @note The default value is Color::BLACK.
       */
      BORDERLINE_COLOR,

      /**
       * @brief The offset from the visual borderline (recommend [-1.0f to 1.0f]).
       * @details Name "borderlineOffset", type Property::FLOAT, animatable.
       *
       * @see Dali::Toolkit::DevelVisual::Property::BORDERLINE_OFFSET
       * @SINCE_2_1.21
       * @note The default value is 0.0f.
       */
      BORDERLINE_OFFSET,

      /**
       * @brief The blur radius of the visual.
       * @details Name "blurRadius", type Property::FLOAT, animatable.
       *          If the value is 0, the edge is sharp. Otherwise, the larger the value, the more the edge is blurred.
       *
       * @see Dali::Toolkit::DevelColorVisual::Property::BLUR_RADIUS
       * @SINCE_2_1.21
       * @note The default value is 0.0f.
       */
      BLUR_RADIUS,
    };
  };

  /**
   * @brief Creates a new DecoratedVisualRenderer object.
   *
   * @SINCE_2_1.21
   * @param[in] geometry Geometry to be used by this renderer
   * @param[in] shader Shader to be used by this renderer
   * @return A handle to the Renderer
   */
  static DecoratedVisualRenderer New(Geometry& geometry, Shader& shader);

  /**
   * @brief Default constructor, creates an empty handle
   *
   * @SINCE_2_1.21
   */
  DecoratedVisualRenderer();

  /**
   * @brief Destructor.
   *
   * @SINCE_2_1.21
   */
  ~DecoratedVisualRenderer();

  /**
   * @brief Copy constructor, creates a new handle to the same object.
   *
   * @SINCE_2_1.21
   * @param[in] handle Handle to an object
   */
  DecoratedVisualRenderer(const DecoratedVisualRenderer& handle);

  /**
   * @brief Downcasts to a decorated visual renderer handle.
   * If not, a renderer the returned decorated visual renderer handle is left uninitialized.
   *
   * @SINCE_2_1.21
   * @param[in] handle Handle to an object
   * @return Renderer handle or an uninitialized handle
   */
  static DecoratedVisualRenderer DownCast(BaseHandle handle);

  /**
   * @brief Assignment operator, changes this handle to point at the same object.
   *
   * @SINCE_2_1.21
   * @param[in] handle Handle to an object
   * @return Reference to the assigned object
   */
  DecoratedVisualRenderer& operator=(const DecoratedVisualRenderer& handle);

  /**
   * @brief Register relate with corner radius uniforms so we can use it as uniform properties.
   *
   * @SINCE_2_1.21
   */
  void RegisterCornerRadiusUniform();

  /**
   * @brief Register relate with borderline uniforms so we can use it as uniform properties.
   *
   * @SINCE_2_1.21
   */
  void RegisterBorderlineUniform();

  /**
   * @brief Register relate with blur radius uniforms so we can use it as uniform properties.
   *
   * @SINCE_2_1.21
   */
  void RegisterBlurRadiusUniform();

  /**
   * @brief Move constructor.
   *
   * @SINCE_2_1.21
   * @param[in] rhs A reference to the moved handle
   */
  DecoratedVisualRenderer(DecoratedVisualRenderer&& rhs);

  /**
   * @brief Move assignment operator.
   *
   * @SINCE_2_1.21
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this handle
   */
  DecoratedVisualRenderer& operator=(DecoratedVisualRenderer&& rhs);

public:
  /// @cond internal
  /**
   * @brief The constructor.
   * @note  Not intended for application developers.
   * @SINCE_2_1.21
   * @param[in] pointer A pointer to a newly allocated DecoratedVisualRenderer
   */
  explicit DALI_INTERNAL DecoratedVisualRenderer(Internal::DecoratedVisualRenderer* pointer);
  /// @endcond
};

/**
 * @}
 */
} //namespace Dali

#endif // DALI_DECOREATED_VISUAL_RENDERER_H

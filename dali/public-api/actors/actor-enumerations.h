#ifndef DALI_ACTOR_ENUMERATIONS_H
#define DALI_ACTOR_ENUMERATIONS_H

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

#include <dali/public-api/common/dali-common.h>

namespace Dali
{
/**
 * @addtogroup dali_core_actors
 * @{
 */

/**
 * @brief Enumeration for Actor color mode.
 *
 * Determines how an actor's own color (Actor::Property::COLOR) combines with its parent's color to
 * produce the actor's world color (Actor::Property::WORLD_COLOR), which is the color actually used
 * when the actor is drawn.
 *
 * @SINCE_1_0.0
 * @see Actor::SetColorMode()
 * @see Actor::Property::COLOR_MODE
 */
enum ColorMode
{
  USE_OWN_COLOR,                 ///< The actor uses its own color and ignores the parent's color entirely. @SINCE_1_0.0
  USE_PARENT_COLOR,              ///< The actor uses its parent's world color and ignores its own color. @SINCE_1_0.0
  USE_OWN_MULTIPLY_PARENT_COLOR, ///< The actor multiplies its own color (all four channels) with its parent's world color. @SINCE_1_0.0
  USE_OWN_MULTIPLY_PARENT_ALPHA  ///< The actor uses its own RGB, but multiplies its alpha by the parent's alpha, so it fades in and out together with the parent. This is the default. @SINCE_1_0.0
};

/**
 * @brief DrawMode to determine how the Actor and its children will be drawn.
 *
 * @SINCE_1_0.0
 */
namespace DrawMode
{
/**
 * @brief Enumeration for the instance of how the actor and it's children will be drawn.
 *
 * @SINCE_1_0.0
 * @see Dali::Actor::SetDrawMode()
 * @see Actor::Property::DRAW_MODE
 */
enum Type
{
  NORMAL     = 0, ///< The actor and its children are drawn in the normal depth order. This is the default. @SINCE_1_0.0
  OVERLAY_2D = 1  ///< The actor and its children are drawn on top of all NORMAL siblings, ignoring depth testing. Within the overlay, children draw over parents and later siblings over earlier ones. Cannot be combined with ClippingMode::CLIP_TO_BOUNDING_BOX (the clipping is then ignored). @SINCE_1_0.0
};

} // namespace DrawMode

/**
 * @brief Enumeration for layout Dimensions.
 * @SINCE_1_0.0
 */
namespace Dimension
{
/**
 * @brief Enumeration for Dimension types.
 * @SINCE_1_0.0
 */
enum Type
{
  WIDTH  = 0x1, ///< Width dimension @SINCE_1_0.0
  HEIGHT = 0x2, ///< Height dimension @SINCE_1_0.0

  ALL_DIMENSIONS = 0x3 ///< Mask to cover all flags @SINCE_1_0.0
};

/**
 * @brief Enumeration for Dimension number.
 * @SINCE_1_0.0
 */
enum Meta
{
  DIMENSION_COUNT = 2 ///< Number of dimensions - update this if adding new dimension @SINCE_1_0.0
};

} // namespace Dimension

/**
 * @brief Enumeration for size negotiation resize policies.
 * @SINCE_1_0.0
 */
namespace ResizePolicy
{
/**
 * @brief Enumeration for ResizePolicy types.
 * @SINCE_1_0.0
 */
enum Type
{
  FIXED,                         ///< Size is fixed as set by SetSize @SINCE_1_0.0
  USE_NATURAL_SIZE,              ///< Size is to use the actor's natural size @SINCE_1_0.0 @see Actor::GetNaturalSize()
  FILL_TO_PARENT,                ///< Size is to fill up to the actor's parent's bounds. Aspect ratio is not maintained. @SINCE_1_0.0
  SIZE_RELATIVE_TO_PARENT,       ///< The actor's size will be ( ParentSize * SizeModeFactor ). @SINCE_1_0.0
  SIZE_FIXED_OFFSET_FROM_PARENT, ///< The actor's size will be ( ParentSize + SizeModeFactor ). @SINCE_1_0.0
  FIT_TO_CHILDREN,               ///< Size will adjust to wrap around all children @SINCE_1_0.0
  DIMENSION_DEPENDENCY,          ///< One dimension is dependent on the other @SINCE_1_0.0
  USE_ASSIGNED_SIZE              ///< The size will be assigned to the actor @SINCE_1_0.0
};
const Type DEFAULT = USE_NATURAL_SIZE; ///< Default resize policy

} // namespace ResizePolicy

/**
 * @brief Enumeration for policies to determine how an actor should resize itself when having its size set in size negotiation.
 * @SINCE_1_0.0
 */
namespace SizeScalePolicy
{
/**
 * @brief Enumeration for SizeScalePolicy types.
 * @SINCE_1_0.0
 */
enum Type
{
  USE_SIZE_SET,          ///< Use the size that was set @SINCE_1_0.0
  FIT_WITH_ASPECT_RATIO, ///< Fit within the size set maintaining natural size aspect ratio @SINCE_1_0.0
  FILL_WITH_ASPECT_RATIO ///< Fill up the size set maintaining natural size aspect ratio. May exceed size bounds in one dimension. @SINCE_1_0.0
};

} // namespace SizeScalePolicy

/**
 * @brief Enumeration for horizontal alignment types.
 * @SINCE_1_0.0
 */
namespace HorizontalAlignment
{
/**
 * @brief Enumeration for HorizontalAlignment types.
 * @SINCE_1_0.0
 */
enum Type
{
  LEFT,   ///< Align horizontally left @SINCE_1_0.0
  CENTER, ///< Align horizontally center @SINCE_1_0.0
  RIGHT   ///< Align horizontally right @SINCE_1_0.0
};

} // namespace HorizontalAlignment

/**
 * @brief Enumeration for vertical alignment types.
 * @SINCE_1_0.0
 */
namespace VerticalAlignment
{
/**
 * @brief Enumeration for VerticalAlignment types.
 * @SINCE_1_0.0
 */
enum Type
{
  TOP,    ///< Align vertically top @SINCE_1_0.0
  CENTER, ///< Align vertically center @SINCE_1_0.0
  BOTTOM  ///< Align vertically bottom @SINCE_1_0.0
};

} // namespace VerticalAlignment

/**
 * @brief Enumeration for ClippingMode describing how this Actor's children will be clipped against it.
 * @SINCE_1_2_5
 */
namespace ClippingMode
{
/**
 * @brief Enumeration for ClippingMode types.
 * @SINCE_1_2_5
 * @see Dali::Actor::SetClippingMode()
 * @see Actor::Property::CLIPPING_MODE
 */
enum Type
{
  DISABLED,            ///< This Actor will not clip its children. This is the default. @SINCE_1_2_5
  CLIP_CHILDREN,       ///< This Actor clips itself and all its children to the pixel areas of this actor's own renderers, so the shape drawn by the renderers defines the clip region. @SINCE_1_2_5
  CLIP_TO_BOUNDING_BOX ///< This Actor clips itself and all its children to a screen-aligned rectangle (an axis-aligned bounding box) enclosing its bounds, which is cheaper than CLIP_CHILDREN but ignores rotation. Cannot be combined with DrawMode::OVERLAY_2D (the clipping is then ignored). @SINCE_1_2.61
};

} // namespace ClippingMode

/**
 * @brief Enumeration for the direction of the layout.
 * @SINCE_1_2.60
 */
namespace LayoutDirection
{
/**
 * @brief Enumeration for the LayoutDirection types.
 * @SINCE_1_2.60
 */
enum Type
{
  LEFT_TO_RIGHT, ///< Layout direction is from Left to Right direction. @SINCE_1_2.60
  RIGHT_TO_LEFT, ///< Layout direction is from Right to Left direction. @SINCE_1_2.60
  INHERIT        ///< Layout direction is inherited from the parent. @SINCE_2_5.32
};

} // namespace LayoutDirection

/**
 * @brief Enumeration describing whether an Actor owns off-screen render tasks that must be reordered
 *        relative to the actor, and in which direction they draw.
 *
 * Off-screen renderables are render tasks that render into an off-screen buffer whose output is then
 * composited with the scene. The value indicates whether such tasks exist and whether they draw content
 * in front of (FORWARD) or behind (BACKWARD) this actor, so the rendering order can be resolved correctly.
 *
 * @SINCE_2_3.43
 */
namespace OffScreenRenderable
{
/**
 * @brief Enumeration for the OffScreenRenderable types.
 *
 * @note The values are bit flags, so FORWARD and BACKWARD can be combined (see BOTH). The provided
 *       operator& and operator! allow testing which directions are present.
 * @SINCE_2_3.43
 */
enum class Type
{
  NONE     = 0,                  ///< The Actor has no off-screen renderables. @SINCE_2_3.43
  FORWARD  = 1,                  ///< The Actor has render tasks that need reordering and that draw actors positioned in front of this Actor. @SINCE_2_3.43
  BACKWARD = 2,                  ///< The Actor has render tasks that need reordering and that draw actors positioned behind this Actor. @SINCE_2_3.43
  BOTH     = FORWARD | BACKWARD, ///< The Actor has render tasks for both the FORWARD and BACKWARD directions. @SINCE_2_3.43
};

/**
 * @brief Bitwise AND operator, used to test which direction flags are set in a value.
 * @param[in] lhs The left-hand operand
 * @param[in] rhs The right-hand operand
 * @return The bitwise AND of the two operands
 * @SINCE_2_3.43
 */
inline Type operator&(Type lhs, Type rhs)
{
  return static_cast<Type>(static_cast<int>(lhs) & static_cast<int>(rhs));
}

/**
 * @brief Equality operator.
 * @param[in] lhs The left-hand operand
 * @param[in] rhs The right-hand operand
 * @return True if both operands have the same value
 * @SINCE_2_3.43
 */
inline bool operator==(Type lhs, Type rhs)
{
  return static_cast<int>(lhs) == static_cast<int>(rhs);
}

/**
 * @brief Logical NOT operator, used to test whether a value is NONE (no flags set).
 * @param[in] t The operand to test
 * @return True if the value is NONE, false if any direction flag is set
 * @SINCE_2_3.43
 */
inline bool operator!(Type t)
{
  return !static_cast<int>(t);
}
} //namespace OffScreenRenderable

/**
 * @brief Enumeration identifying whose Property::VISIBLE change triggered a VisibilityChangedSignal.
 *
 * Passed as the third argument to the Actor::VisibilityChangedSignal() callback to distinguish a change
 * of the actor's own visibility from a change inherited from one of its parents.
 *
 * @SINCE_2_5.29
 * @see Actor::VisibilityChangedSignal()
 */
enum class VisibilityChangeType
{
  SELF,  ///< This actor's own Property::VISIBLE value changed. @SINCE_2_5.29
  PARENT ///< A parent's Property::VISIBLE value changed, affecting this actor's shown state. @SINCE_2_5.29
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_ACTOR_ENUMERATIONS_H

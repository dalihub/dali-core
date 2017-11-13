#ifndef DALI_ACTOR_ENUMERATIONS_H
#define DALI_ACTOR_ENUMERATIONS_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
 * @SINCE_1_0.0
 */
enum ColorMode
{
  USE_OWN_COLOR,                 ///< Actor will use its own color @SINCE_1_0.0
  USE_PARENT_COLOR,              ///< Actor will use its parent color @SINCE_1_0.0
  USE_OWN_MULTIPLY_PARENT_COLOR, ///< Actor will blend its color with its parents color. @SINCE_1_0.0
  USE_OWN_MULTIPLY_PARENT_ALPHA  ///< Actor will blend its alpha with its parents alpha. This means when parent fades in or out child does as well. This is the default. @SINCE_1_0.0
};

/**
 * @DEPRECATED_1_1.24 Use SetInheritPosition instead
 * @brief Enumeration for Actor position inheritance mode.
 * @SINCE_1_0.0
 */
enum PositionInheritanceMode
{
  INHERIT_PARENT_POSITION,                 ///< Actor will inherit its parent position. This is the default @SINCE_1_0.0
  USE_PARENT_POSITION,                     ///< Actor will copy its parent position. This is useful if many actors are stacked together in the same place. This option ignores parent origin and anchor point. @SINCE_1_0.0
  USE_PARENT_POSITION_PLUS_LOCAL_POSITION, ///< Actor will copy its parent position and add local position. This is useful if many actors are stacked together in the same place with an offset.  This option ignores parent origin and anchor point. @SINCE_1_0.0
  DONT_INHERIT_POSITION                    ///< Actor will not inherit position. Local position is treated as world position. This is useful if a constraint is used to override local position or if an actor is positioned globally. This option ignores parent origin, anchor point and local position. @SINCE_1_0.0
};

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
  WIDTH  = 0x1,       ///< Width dimension @SINCE_1_0.0
  HEIGHT = 0x2,       ///< Height dimension @SINCE_1_0.0

  ALL_DIMENSIONS = 0x3  ///< Mask to cover all flags @SINCE_1_0.0
};

/**
 * @brief Enumeration for Dimension number.
 * @SINCE_1_0.0
 */
enum Meta
{
  DIMENSION_COUNT = 2  ///< Number of dimensions - update this if adding new dimension @SINCE_1_0.0
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
  FIXED,                 ///< Size is fixed as set by SetSize @SINCE_1_0.0
  USE_NATURAL_SIZE,      ///< Size is to use the actor's natural size @SINCE_1_0.0 @see Actor::GetNaturalSize()
  FILL_TO_PARENT,        ///< Size is to fill up to the actor's parent's bounds. Aspect ratio is not maintained. @SINCE_1_0.0
  SIZE_RELATIVE_TO_PARENT,        ///< The actors size will be ( ParentSize * SizeRelativeToParentFactor ). @SINCE_1_0.0
  SIZE_FIXED_OFFSET_FROM_PARENT,  ///< The actors size will be ( ParentSize + SizeRelativeToParentFactor ). @SINCE_1_0.0
  FIT_TO_CHILDREN,       ///< Size will adjust to wrap around all children @SINCE_1_0.0
  DIMENSION_DEPENDENCY,  ///< One dimension is dependent on the other @SINCE_1_0.0
  USE_ASSIGNED_SIZE      ///< The size will be assigned to the actor @SINCE_1_0.0
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
  USE_SIZE_SET,                ///< Use the size that was set @SINCE_1_0.0
  FIT_WITH_ASPECT_RATIO,       ///< Fit within the size set maintaining natural size aspect ratio @SINCE_1_0.0
  FILL_WITH_ASPECT_RATIO       ///< Fill up the size set maintaining natural size aspect ratio. May exceed size bounds in one dimension. @SINCE_1_0.0
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
  LEFT,         ///< Align horizontally left @SINCE_1_0.0
  CENTER,       ///< Align horizontally center @SINCE_1_0.0
  RIGHT         ///< Align horizontally right @SINCE_1_0.0
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
  TOP,          ///< Align vertically top @SINCE_1_0.0
  CENTER,       ///< Align vertically center @SINCE_1_0.0
  BOTTOM        ///< Align vertically bottom @SINCE_1_0.0
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
 */
enum Type
{
  DISABLED,                     ///< This Actor will not clip its children. @SINCE_1_2_5
  CLIP_CHILDREN,                ///< This Actor will clip itself and all children to within the pixel areas of this actors renderers. @SINCE_1_2_5
  CLIP_TO_BOUNDING_BOX          ///< This Actor will clip itself and all children to within a screen-aligned rectangle encompassing its boundaries. @SINCE_1_2.61
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
  LEFT_TO_RIGHT,   ///< Layout direction is from Left to Right direction. @SINCE_1_2.60
  RIGHT_TO_LEFT    ///< Layout direction is from Right to Left direction. @SINCE_1_2.60
};

} // namespace LayoutDirection

/**
 * @}
 */
} // namespace Dali

#endif // DALI_ACTOR_ENUMERATIONS_H

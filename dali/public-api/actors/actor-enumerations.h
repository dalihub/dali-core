#ifndef __DALI_ACTOR_ENUMERATIONS_H__
#define __DALI_ACTOR_ENUMERATIONS_H__

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

#include <dali/public-api/common/dali-common.h>

namespace Dali
{

/**
 * @brief Actor color mode.
 *
 */
enum ColorMode
{
  USE_OWN_COLOR,                 ///< Actor will use its own color
  USE_PARENT_COLOR,              ///< Actor will use its parent color
  USE_OWN_MULTIPLY_PARENT_COLOR, ///< Actor will blend its color with its parents color.
  USE_OWN_MULTIPLY_PARENT_ALPHA  ///< Actor will blend its alpha with its parents alpha. This means when parent fades in or out child does as well. This is the default.
};

/**
 * @brief Actor position inheritance mode.
 */
enum PositionInheritanceMode
{
  INHERIT_PARENT_POSITION,                 ///< Actor will inherit its parent position. This is the default
  USE_PARENT_POSITION,                     ///< Actor will copy its parent position. This is useful if many actors are stacked together in the same place. This option ignores parent origin and anchor point.
  USE_PARENT_POSITION_PLUS_LOCAL_POSITION, ///< Actor will copy its parent position and add local position. This is useful if many actors are stacked together in the same place with an offset.  This option ignores parent origin and anchor point.
  DONT_INHERIT_POSITION                    ///< Actor will not inherit position. Local position is treated as world position. This is useful if a constraint is used to override local position or if an actor is positioned globally. This option ignores parent origin, anchor point and local position.
};

/**
 * @brief Dimensions for layout
 */
enum Dimension
{
  WIDTH  = 0x1,       ///< Width dimension
  HEIGHT = 0x2,       ///< Height dimension

  DIMENSION_COUNT = 2,  ///< Number of dimensions - update this if adding new dimension
  ALL_DIMENSIONS = 0x3  ///< Mask to cover all flags
};

/**
 * @brief Size negotiation resize policies
 */
enum ResizePolicy
{
  FIXED,                 ///< Size is fixed as set by SetSize
  USE_NATURAL_SIZE,      ///< Size is to use the actor's natural size
  FILL_TO_PARENT,        ///< Size is to fill up to the actor's parent's bounds. Aspect ratio not maintained.
  SIZE_RELATIVE_TO_PARENT,        ///< The actors size will be ( ParentSize * SizeRelativeToParentFactor ).
  SIZE_FIXED_OFFSET_FROM_PARENT,  ///< The actors size will be ( ParentSize + SizeRelativeToParentFactor ).
  FIT_TO_CHILDREN,       ///< Size will adjust to wrap around all children
  DIMENSION_DEPENDENCY,  ///< One dimension is dependent on the other
  USE_ASSIGNED_SIZE      ///< The size will be assigned to the actor
};

/**
 * @brief Policies to determine how an actor should resize itself when having its size set in size negotiation
 */
enum SizeScalePolicy
{
  USE_SIZE_SET,                ///< Use the size that was set
  FIT_WITH_ASPECT_RATIO,       ///< Fit within the size set maintaining natural size aspect ratio
  FILL_WITH_ASPECT_RATIO       ///< Fill up the size set maintaining natural size aspect ratio. May exceed size bounds in one dimension.
};

/**
 * @brief Different types of alignment.
 */
namespace HorizontalAlignment
{
enum Type
{
  LEFT,         ///< Align horizontally left
  CENTER,       ///< Align horizontally center
  RIGHT         ///< Align horiztonally right
};
}

namespace VerticalAlignment
{
enum Type
{
  TOP,          ///< Align vertically top
  CENTER,       ///< Align vertically center
  BOTTOM        ///< Align vertically bottom
};
}

} // namespace Dali

#endif // __DALI_ACTOR_ENUMERATIONS_H__

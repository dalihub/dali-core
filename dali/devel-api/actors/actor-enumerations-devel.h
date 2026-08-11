#ifndef DALI_ACTOR_ENUMERATIONS_DEVEL_H
#define DALI_ACTOR_ENUMERATIONS_DEVEL_H

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
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
/**
 * @brief Enumeration for layout Dimensions.
 */
namespace Dimension
{
/**
 * @brief Enumeration for Dimension types.
 */
enum Type
{
  WIDTH  = 0x1, ///< Width dimension
  HEIGHT = 0x2, ///< Height dimension

  ALL_DIMENSIONS = 0x3 ///< Mask to cover all flags
};

/**
 * @brief Enumeration for Dimension number.
 */
enum Meta
{
  DIMENSION_COUNT = 2 ///< Number of dimensions - update this if adding new dimension
};

} // namespace Dimension

/**
 * @brief Enumeration for size negotiation resize policies.
 */
namespace ResizePolicy
{
/**
 * @brief Enumeration for ResizePolicy types.
 */
enum Type
{
  FIXED,                         ///< Size is fixed as set by SetSize
  USE_NATURAL_SIZE,              ///< Size is to use the actor's natural size @see Actor::GetNaturalSize()
  FILL_TO_PARENT,                ///< Size is to fill up to the actor's parent's bounds. Aspect ratio is not maintained.
  SIZE_RELATIVE_TO_PARENT,       ///< The actor's size will be ( ParentSize * SizeModeFactor ).
  SIZE_FIXED_OFFSET_FROM_PARENT, ///< The actor's size will be ( ParentSize + SizeModeFactor ).
  FIT_TO_CHILDREN,               ///< Size will adjust to wrap around all children
  DIMENSION_DEPENDENCY,          ///< One dimension is dependent on the other
  USE_ASSIGNED_SIZE              ///< The size will be assigned to the actor
};
const Type DEFAULT = USE_NATURAL_SIZE; ///< Default resize policy

} // namespace ResizePolicy

/**
 * @brief Enumeration for policies to determine how an actor should resize itself when having its size set in size negotiation.
 */
namespace SizeScalePolicy
{
/**
 * @brief Enumeration for SizeScalePolicy types.
 */
enum Type
{
  USE_SIZE_SET,          ///< Use the size that was set
  FIT_WITH_ASPECT_RATIO, ///< Fit within the size set maintaining natural size aspect ratio
  FILL_WITH_ASPECT_RATIO ///< Fill up the size set maintaining natural size aspect ratio. May exceed size bounds in one dimension.
};

} // namespace SizeScalePolicy

/**
 * @brief Enumeration for horizontal alignment types.
 */
namespace HorizontalAlignment
{
/**
 * @brief Enumeration for HorizontalAlignment types.
 */
enum Type
{
  LEFT,   ///< Align horizontally left
  CENTER, ///< Align horizontally center
  RIGHT   ///< Align horizontally right
};

} // namespace HorizontalAlignment

/**
 * @brief Enumeration for vertical alignment types.
 */
namespace VerticalAlignment
{
/**
 * @brief Enumeration for VerticalAlignment types.
 */
enum Type
{
  TOP,    ///< Align vertically top
  CENTER, ///< Align vertically center
  BOTTOM  ///< Align vertically bottom
};

} // namespace VerticalAlignment

} // namespace Dali

#endif // DALI_ACTOR_ENUMERATIONS_DEVEL_H

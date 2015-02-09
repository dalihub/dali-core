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
 * @brief Actor size relative to parent mode.
 */
enum SizeMode
{
  USE_OWN_SIZE,                            ///< The mode is bypassed. The size Vector3 will be used as normal.
  SIZE_EQUAL_TO_PARENT,                    ///< The actor will have the exact same size as the parent.
  SIZE_RELATIVE_TO_PARENT,                 ///< The actors size will be ( ParentSize * SizeRelativeToParentFactor ).
  SIZE_FIXED_OFFSET_FROM_PARENT            ///< The actors size will be ( ParentSize + SizeRelativeToParentFactor ).
};

} // namespace Dali

#endif // __DALI_ACTOR_ENUMERATIONS_H__

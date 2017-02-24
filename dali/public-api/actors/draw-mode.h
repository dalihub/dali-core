#ifndef __DALI_DRAW_MODE_H__
#define __DALI_DRAW_MODE_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

namespace DrawMode
{

/**
 * @brief Enumeration for the instance of how the actor and it's children will be drawn.
 *
 * @SINCE_1_0.0
 * @see Dali::Actor::SetDrawMode()
 */
enum Type
{
  NORMAL     = 0, ///< @brief binary 00. The default draw-mode @SINCE_1_0.0
  OVERLAY_2D = 1, ///< @brief binary 01. Draw the actor and its children as an overlay @SINCE_1_0.0
  STENCIL    = 3  ///< @DEPRECATED_1_1.31 Will be replaced by separate ClippingMode enum @brief binary 11. Draw the actor and its children into the stencil buffer @SINCE_1_0.0
};

} // namespace DrawMode

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_DRAW_MODE_H__

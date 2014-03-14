#ifndef __DALI_DRAW_MODE_H__
#define __DALI_DRAW_MODE_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/**
 * @addtogroup CAPI_DALI_ACTORS_MODULE
 * @{
 */

#include <dali/public-api/common/dali-common.h>

namespace Dali DALI_IMPORT_API
{

namespace DrawMode
{

/**
 * @brief How the actor and it's children will be drawn.
 *
 * @see Dali::Actor::SetDrawMode()
 */
enum Type
{
  NORMAL  = 0, ///< binary 00. The default draw-mode
  OVERLAY = 1, ///< binary 01. Draw the actor and its children as an overlay
  STENCIL = 3  ///< binary 11. Draw the actor and its children into the stencil buffer
};

} // namespace DrawMode

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_DRAW_MODE_H__

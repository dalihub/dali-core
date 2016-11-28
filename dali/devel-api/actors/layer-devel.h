#ifndef DALI_LAYER_DEVEL_H
#define DALI_LAYER_DEVEL_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/layer.h>

namespace Dali
{

namespace DevelLayer
{

  /**
   * @brief TREE_DEPTH_MULTIPLIER is used by the rendering sorting algorithm to decide which actors to render first.
   * @SINCE_1_0.0
   */
  enum TreeDepthMultiplier
  {
    TREE_DEPTH_MULTIPLIER = 1000000,
  };

  /**
  * @brief SIBLING_ORDER_MULTIPLIER is used by the rendering sorting algorithm to decide which actors to render first.
  * @SINCE_1_2.16
  */
 enum SiblingOrderMultiplier
 {
   SIBLING_ORDER_MULTIPLIER = 1000,
 };

}

}

#endif  //DALI_LAYER_DEVEL_H

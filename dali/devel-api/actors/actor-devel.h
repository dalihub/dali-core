#ifndef DALI_ACTOR_DEVEL_H
#define DALI_ACTOR_DEVEL_H

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
#include <dali/public-api/actors/actor.h>

namespace Dali
{

namespace DevelActor
{

namespace Property
{

enum Type
{
  PARENT_ORIGIN        = Dali::Actor::Property::PARENT_ORIGIN,
  PARENT_ORIGIN_X      = Dali::Actor::Property::PARENT_ORIGIN_X,
  PARENT_ORIGIN_Y      = Dali::Actor::Property::PARENT_ORIGIN_Y,
  PARENT_ORIGIN_Z      = Dali::Actor::Property::PARENT_ORIGIN_Z,
  ANCHOR_POINT         = Dali::Actor::Property::ANCHOR_POINT,
  ANCHOR_POINT_X       = Dali::Actor::Property::ANCHOR_POINT_X,
  ANCHOR_POINT_Y       = Dali::Actor::Property::ANCHOR_POINT_Y,
  ANCHOR_POINT_Z       = Dali::Actor::Property::ANCHOR_POINT_Z,
  SIZE                 = Dali::Actor::Property::SIZE,
  SIZE_WIDTH           = Dali::Actor::Property::SIZE_WIDTH,
  SIZE_HEIGHT          = Dali::Actor::Property::SIZE_HEIGHT,
  SIZE_DEPTH           = Dali::Actor::Property::SIZE_DEPTH,
  POSITION             = Dali::Actor::Property::POSITION,
  POSITION_X           = Dali::Actor::Property::POSITION_X,
  POSITION_Y           = Dali::Actor::Property::POSITION_Y,
  POSITION_Z           = Dali::Actor::Property::POSITION_Z,
  WORLD_POSITION       = Dali::Actor::Property::WORLD_POSITION,
  WORLD_POSITION_X     = Dali::Actor::Property::WORLD_POSITION_X,
  WORLD_POSITION_Y     = Dali::Actor::Property::WORLD_POSITION_Y,
  WORLD_POSITION_Z     = Dali::Actor::Property::WORLD_POSITION_Z,
  ORIENTATION          = Dali::Actor::Property::ORIENTATION,
  WORLD_ORIENTATION    = Dali::Actor::Property::WORLD_ORIENTATION,
  SCALE                = Dali::Actor::Property::SCALE,
  SCALE_X              = Dali::Actor::Property::SCALE_X,
  SCALE_Y              = Dali::Actor::Property::SCALE_Y,
  SCALE_Z              = Dali::Actor::Property::SCALE_Z,
  WORLD_SCALE          = Dali::Actor::Property::WORLD_SCALE,
  VISIBLE              = Dali::Actor::Property::VISIBLE,
  COLOR                = Dali::Actor::Property::COLOR,
  COLOR_RED            = Dali::Actor::Property::COLOR_RED,
  COLOR_GREEN          = Dali::Actor::Property::COLOR_GREEN,
  COLOR_BLUE           = Dali::Actor::Property::COLOR_BLUE,
  COLOR_ALPHA          = Dali::Actor::Property::COLOR_ALPHA,
  WORLD_COLOR          = Dali::Actor::Property::WORLD_COLOR,
  WORLD_MATRIX         = Dali::Actor::Property::WORLD_MATRIX,
  NAME                 = Dali::Actor::Property::NAME,
  SENSITIVE            = Dali::Actor::Property::SENSITIVE,
  LEAVE_REQUIRED       = Dali::Actor::Property::LEAVE_REQUIRED,
  INHERIT_ORIENTATION  = Dali::Actor::Property::INHERIT_ORIENTATION,
  INHERIT_SCALE        = Dali::Actor::Property::INHERIT_SCALE,
  COLOR_MODE           = Dali::Actor::Property::COLOR_MODE,
  POSITION_INHERITANCE = Dali::Actor::Property::POSITION_INHERITANCE,
  DRAW_MODE            = Dali::Actor::Property::DRAW_MODE,
  SIZE_MODE_FACTOR     = Dali::Actor::Property::SIZE_MODE_FACTOR,
  WIDTH_RESIZE_POLICY  = Dali::Actor::Property::WIDTH_RESIZE_POLICY,
  HEIGHT_RESIZE_POLICY = Dali::Actor::Property::HEIGHT_RESIZE_POLICY,
  SIZE_SCALE_POLICY    = Dali::Actor::Property::SIZE_SCALE_POLICY,
  WIDTH_FOR_HEIGHT     = Dali::Actor::Property::WIDTH_FOR_HEIGHT,
  HEIGHT_FOR_WIDTH     = Dali::Actor::Property::HEIGHT_FOR_WIDTH,
  PADDING              = Dali::Actor::Property::PADDING,
  MINIMUM_SIZE         = Dali::Actor::Property::MINIMUM_SIZE,
  MAXIMUM_SIZE         = Dali::Actor::Property::MAXIMUM_SIZE,
  INHERIT_POSITION     = Dali::Actor::Property::INHERIT_POSITION,
  CLIPPING_MODE        = Dali::Actor::Property::CLIPPING_MODE,

    /**
     * @brief name "batchParent", type BOOLEAN
     * @see Batching
     * @note The default value is 'false'
     */
  BATCH_PARENT           = CLIPPING_MODE + 1,
  SIBLING_ORDER          = CLIPPING_MODE + 2,
};

} // namespace Property

} // namespace DevelActor

} // namespace Dali

#endif // DALI_ACTOR_DEVEL_H

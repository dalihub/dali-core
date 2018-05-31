#ifndef DALI_ACTOR_DEVEL_H
#define DALI_ACTOR_DEVEL_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
  PARENT_ORIGIN               = Dali::Actor::Property::PARENT_ORIGIN,
  PARENT_ORIGIN_X             = Dali::Actor::Property::PARENT_ORIGIN_X,
  PARENT_ORIGIN_Y             = Dali::Actor::Property::PARENT_ORIGIN_Y,
  PARENT_ORIGIN_Z             = Dali::Actor::Property::PARENT_ORIGIN_Z,
  ANCHOR_POINT                = Dali::Actor::Property::ANCHOR_POINT,
  ANCHOR_POINT_X              = Dali::Actor::Property::ANCHOR_POINT_X,
  ANCHOR_POINT_Y              = Dali::Actor::Property::ANCHOR_POINT_Y,
  ANCHOR_POINT_Z              = Dali::Actor::Property::ANCHOR_POINT_Z,
  SIZE                        = Dali::Actor::Property::SIZE,
  SIZE_WIDTH                  = Dali::Actor::Property::SIZE_WIDTH,
  SIZE_HEIGHT                 = Dali::Actor::Property::SIZE_HEIGHT,
  SIZE_DEPTH                  = Dali::Actor::Property::SIZE_DEPTH,
  POSITION                    = Dali::Actor::Property::POSITION,
  POSITION_X                  = Dali::Actor::Property::POSITION_X,
  POSITION_Y                  = Dali::Actor::Property::POSITION_Y,
  POSITION_Z                  = Dali::Actor::Property::POSITION_Z,
  WORLD_POSITION              = Dali::Actor::Property::WORLD_POSITION,
  WORLD_POSITION_X            = Dali::Actor::Property::WORLD_POSITION_X,
  WORLD_POSITION_Y            = Dali::Actor::Property::WORLD_POSITION_Y,
  WORLD_POSITION_Z            = Dali::Actor::Property::WORLD_POSITION_Z,
  ORIENTATION                 = Dali::Actor::Property::ORIENTATION,
  WORLD_ORIENTATION           = Dali::Actor::Property::WORLD_ORIENTATION,
  SCALE                       = Dali::Actor::Property::SCALE,
  SCALE_X                     = Dali::Actor::Property::SCALE_X,
  SCALE_Y                     = Dali::Actor::Property::SCALE_Y,
  SCALE_Z                     = Dali::Actor::Property::SCALE_Z,
  WORLD_SCALE                 = Dali::Actor::Property::WORLD_SCALE,
  VISIBLE                     = Dali::Actor::Property::VISIBLE,
  COLOR                       = Dali::Actor::Property::COLOR,
  COLOR_RED                   = Dali::Actor::Property::COLOR_RED,
  COLOR_GREEN                 = Dali::Actor::Property::COLOR_GREEN,
  COLOR_BLUE                  = Dali::Actor::Property::COLOR_BLUE,
  COLOR_ALPHA                 = Dali::Actor::Property::COLOR_ALPHA,
  WORLD_COLOR                 = Dali::Actor::Property::WORLD_COLOR,
  WORLD_MATRIX                = Dali::Actor::Property::WORLD_MATRIX,
  NAME                        = Dali::Actor::Property::NAME,
  SENSITIVE                   = Dali::Actor::Property::SENSITIVE,
  LEAVE_REQUIRED              = Dali::Actor::Property::LEAVE_REQUIRED,
  INHERIT_ORIENTATION         = Dali::Actor::Property::INHERIT_ORIENTATION,
  INHERIT_SCALE               = Dali::Actor::Property::INHERIT_SCALE,
  COLOR_MODE                  = Dali::Actor::Property::COLOR_MODE,
  POSITION_INHERITANCE        = Dali::Actor::Property::POSITION_INHERITANCE,
  DRAW_MODE                   = Dali::Actor::Property::DRAW_MODE,
  SIZE_MODE_FACTOR            = Dali::Actor::Property::SIZE_MODE_FACTOR,
  WIDTH_RESIZE_POLICY         = Dali::Actor::Property::WIDTH_RESIZE_POLICY,
  HEIGHT_RESIZE_POLICY        = Dali::Actor::Property::HEIGHT_RESIZE_POLICY,
  SIZE_SCALE_POLICY           = Dali::Actor::Property::SIZE_SCALE_POLICY,
  WIDTH_FOR_HEIGHT            = Dali::Actor::Property::WIDTH_FOR_HEIGHT,
  HEIGHT_FOR_WIDTH            = Dali::Actor::Property::HEIGHT_FOR_WIDTH,
  PADDING                     = Dali::Actor::Property::PADDING,
  MINIMUM_SIZE                = Dali::Actor::Property::MINIMUM_SIZE,
  MAXIMUM_SIZE                = Dali::Actor::Property::MAXIMUM_SIZE,
  INHERIT_POSITION            = Dali::Actor::Property::INHERIT_POSITION,
  CLIPPING_MODE               = Dali::Actor::Property::CLIPPING_MODE,
  LAYOUT_DIRECTION            = Dali::Actor::Property::LAYOUT_DIRECTION,
  INHERIT_LAYOUT_DIRECTION    = Dali::Actor::Property::INHERIT_LAYOUT_DIRECTION,

  /**
   * @brief Sets the sibling order of the actor so depth position can be defined within the same parent.
   * @details Name "siblingOrder", type Property::INTEGER.
   * @note The initial value is 0.
   * @note Raise, Lower, RaiseToTop, LowerToBottom, RaiseAbove and LowerBelow will override the
   * sibling order. The values set by this Property will likely change.
   */
  SIBLING_ORDER = INHERIT_LAYOUT_DIRECTION + 1,

  /**
   * @brief The opacity of the actor.
   * @details Name "opacity", type Property::FLOAT.
   */
  OPACITY = INHERIT_LAYOUT_DIRECTION + 2,

  /**
   * @brief Returns the screen position of the Actor
   * @details Name "screenPosition", type Property::VECTOR2. Read-only
   * @note This assumes default camera and default render-task and the Z position is ZERO.
   */
  SCREEN_POSITION = INHERIT_LAYOUT_DIRECTION + 3,

  /**
   * @brief Determines whether the anchor point should be used to determine the position of the actor.
   * @details Name "positionUsesAnchorPoint", type Property::BOOLEAN.
   * @note This is true by default.
   * @note If false, then the top-left of the actor is used for the position.
   * @note Setting this to false will allow scaling or rotation around the anchor-point without affecting the actor's position.
   */
  POSITION_USES_ANCHOR_POINT = INHERIT_LAYOUT_DIRECTION + 4,
};

} // namespace Property

namespace VisibilityChange
{

enum Type
{
  SELF,   ///< The visibility of the actor itself has changed.
  PARENT  ///< The visibility of a parent has changed.
};

} // namespace VisibilityChange

typedef Signal< void ( Actor, bool, VisibilityChange::Type ) > VisibilityChangedSignalType; ///< Signal type of VisibilityChangedSignalType

/**
 * @brief This signal is emitted when the visible property of this or a parent actor is changed.
 *
 * A callback of the following type may be connected:
 * @code
 *   void YourCallbackName( Actor actor, bool visible, VisibilityChange::Type& type );
 * @endcode
 * actor: The actor, or child of actor, whose visibility has changed
 * visible: Whether the actor is now visible or not
 * type: Whether the actor's visible property has changed or a parent's.
 * @return The signal to connect to
 * @pre The Actor has been initialized.
 * @note This signal is NOT emitted if the actor becomes transparent (or the reverse), it's only linked with Actor::Property::VISIBLE.
 */
DALI_CORE_API VisibilityChangedSignalType& VisibilityChangedSignal( Actor actor );


typedef Signal< void (Actor) > ChildChangedSignalType; ///< Called when the actor has a child added or removed

/**
 * @brief This signal is emitted when a child is added to this actor.
 *
 * A callback of the following type may be connected:
 * @code
 *   void MyCallbackName( Actor child );
 * @endcode
 * child: The child that has been added.
 *
 * @note Use this signal with caution. Changing the parent of the actor
 * within this callback is possible, but DALi will prevent further signals
 * being sent.
 *
 * @return The signal to connect to
 * @pre The Actor has been initialized
 */
DALI_CORE_API ChildChangedSignalType& ChildAddedSignal( Actor actor );

/**
 * @brief This signal is emitted when a child is removed from this actor.
 *
 * A callback of the following type may be connected:
 * @code
 *   void MyCallbackName( Actor child );
 * @endcode
 * child: The child that has been removed.
 *
 * @note Use this signal with caution. Changing the parent of the actor
 * within this callback is possible, but DALi will prevent further signals
 * being sent.
 *
 * @note If the child actor is moved from one actor to another, then
 * this signal will be emitted followed immediately by an
 * ChildAddedSignal() on the new parent.
 *
 * @return The signal to connect to
 * @pre The Actor has been initialized
 */
DALI_CORE_API ChildChangedSignalType& ChildRemovedSignal( Actor actor );


} // namespace DevelActor

} // namespace Dali

#endif // DALI_ACTOR_DEVEL_H

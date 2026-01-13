#ifndef DALI_INTERNAL_ACTOR_PROPERTY_HANDLER_H
#define DALI_INTERNAL_ACTOR_PROPERTY_HANDLER_H

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
#include <dali/internal/event/actors/actor-impl.h>

namespace Dali
{
namespace Internal
{
/**
 * Extracts out the property related methods from Actor
 */
struct Actor::PropertyHandler
{
  using PropertyBase = Dali::Internal::SceneGraph::PropertyBase;

  /// @copydoc Actor::SetDefaultProperty
  static void SetDefaultProperty(
    Internal::Actor&       actor,
    Property::Index        index,
    const Property::Value& property);

  /// @copydoc Actor::OnNotifyDefaultPropertyAnimation
  static void OnNotifyDefaultPropertyAnimation(
    Internal::Actor&       actor,
    Animation&             animation,
    Property::Index        index,
    const Property::Value& value,
    Animation::Type        animationType);

  /// @copydoc Actor::GetSceneObjectAnimatableProperty
  static const PropertyBase* GetSceneObjectAnimatableProperty(
    Property::Index         index,
    const SceneGraph::Node& node);

  /// @copydoc Actor::GetSceneObjectInputProperty
  static const PropertyInputImpl* GetSceneObjectInputProperty(
    Property::Index         index,
    const SceneGraph::Node& node);

  /// @copydoc Actor::GetPropertyComponentIndex
  static int32_t GetPropertyComponentIndex(Property::Index index);

  /// @copydoc Actor::GetCachedPropertyValue
  static bool GetCachedPropertyValue(
    const Internal::Actor& actor,
    Property::Index        index,
    Property::Value&       value);

  /// @copydoc Actor::GetCurrentPropertyValue
  static bool GetCurrentPropertyValue(
    const Internal::Actor& actor,
    Property::Index        index,
    Property::Value&       value);
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_ACTOR_PROPERTY_HANDLER_H

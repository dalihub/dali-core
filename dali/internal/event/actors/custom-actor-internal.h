#ifndef DALI_INTERNAL_CUSTOM_ACTOR_H
#define DALI_INTERNAL_CUSTOM_ACTOR_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/actors/actor-declarations.h>
#include <dali/public-api/actors/custom-actor.h>
#include <dali/public-api/animation/animation.h>

namespace Dali
{

namespace Internal
{

class CustomActor : public Actor
{
public:

  /**
   * Create a new custom actor.
   * @return A smart-pointer to the newly allocated Actor.
   */
  static CustomActorPtr New(CustomActorImpl& extension);

  /**
   * Retrieve the custom actor implementation.
   * @return The implementation, or an invalid pointer if no implementation was set.
   */
  CustomActorImpl& GetImplementation()
  {
    return *mImpl;
  }

  /**
   * Retrieve the custom actor implementation.
   * @return The implementation, or an invalid pointer if no implementation was set.
   */
  const CustomActorImpl& GetImplementation() const
  {
    return *mImpl;
  }

  /**
   * Get the type info associated with this object.
   *
   * @return The type info
   */
  Dali::TypeInfo GetTypeInfo();

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~CustomActor() override;

private:

  /**
   * @copydoc Internal::Actor::OnSceneConnectionExternal
   */
  void OnSceneConnectionExternal( int32_t depth ) override
  {
    mImpl->OnSceneConnection( depth );
  }

  /**
   * @copydoc Internal::Actor::OnSceneDisconnectionExternal
   */
  void OnSceneDisconnectionExternal() override
  {
    mImpl->OnSceneDisconnection();
  }

  /**
   * @copydoc Internal::Actor::OnChildAdd
   */
  void OnChildAdd(Actor& child) override
  {
    Dali::Actor handle(&child);
    mImpl->OnChildAdd(handle);
  }

  /**
   * @copydoc Internal::Actor::OnChildRemove
   */
  void OnChildRemove(Actor& child) override
  {
    Dali::Actor handle(&child);
    mImpl->OnChildRemove(handle);
  }

  /**
   * @copydoc Internal::Actor::OnPropertySet
   */
  void OnPropertySet( Property::Index index, const Property::Value& propertyValue ) override
  {
    mImpl->OnPropertySet(index, propertyValue);
  }

  /**
   * @copydoc Internal::Actor::OnSizeSet
   */
  void OnSizeSet(const Vector3& targetSize) override
  {
    mImpl->OnSizeSet(targetSize);
  }

  /**
   * @copydoc Internal::Actor::OnSizeAnimation
   */
  void OnSizeAnimation(Animation& animation, const Vector3& targetSize) override
  {
    Dali::Animation animationHandle(&animation);
    mImpl->OnSizeAnimation(animationHandle, targetSize);
  }

  /**
   * @copydoc Internal::Actor::OnRelayout
   */
  void OnRelayout( const Vector2& size, RelayoutContainer& container ) override
  {
    mImpl->OnRelayout( size, container );
  }

  /**
   * @copydoc Internal::Actor::OnSetResizePolicy
   */
  void OnSetResizePolicy( ResizePolicy::Type policy, Dimension::Type dimension ) override
  {
    mImpl->OnSetResizePolicy( policy, dimension );
  }

  /**
   * @copydoc Internal::Actor::GetNaturalSize
   */
  Vector3 GetNaturalSize() const override
  {
    return mImpl->GetNaturalSize();
  }

  /**
   * @copydoc Internal::Actor::CalculateChildSize
   */
  float CalculateChildSize( const Dali::Actor& child, Dimension::Type dimension ) override
  {
    return mImpl->CalculateChildSize( child, dimension );
  }

  /**
   * @copydoc Internal::Actor::GetHeightForWidth
   */
  float GetHeightForWidth( float width ) override
  {
    return mImpl->GetHeightForWidth( width );
  }

  /**
   * @copydoc Internal::Actor::GetWidthForHeight
   */
  float GetWidthForHeight( float height ) override
  {
    return mImpl->GetWidthForHeight( height );
  }

  /**
   * @copydoc Internal::Actor::RelayoutDependentOnChildren
   */
  bool RelayoutDependentOnChildren( Dimension::Type dimension = Dimension::ALL_DIMENSIONS ) override
  {
    return mImpl->RelayoutDependentOnChildren( dimension );
  }

  /**
   * @copydoc Internal::Actor::OnCalculateRelayoutSize
   */
  void OnCalculateRelayoutSize( Dimension::Type dimension ) override
  {
    return mImpl->OnCalculateRelayoutSize( dimension );
  }

  /**
   * @copydoc Internal::Actor::OnLayoutNegotiated
   */
  void OnLayoutNegotiated( float size, Dimension::Type dimension ) override
  {
    return mImpl->OnLayoutNegotiated( size, dimension );
  }

  /**
   * Private constructor; see also CustomActor::New()
   */
  CustomActor( const SceneGraph::Node& node, CustomActorImpl& extension );

  // no default or copy constructor or assignment
  CustomActor() = delete;
  CustomActor( const CustomActor& ) = delete;
  CustomActor& operator=( const CustomActor& rhs ) = delete;

protected:

  CustomActorImplPtr mImpl;

};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::CustomActor& GetImpl(Dali::CustomActor& actor)
{
  DALI_ASSERT_ALWAYS( actor && "CustomActor handle is empty" );

  BaseObject& handle = actor.GetBaseObject();

  return static_cast<Internal::CustomActor&>(handle);
}

inline const Internal::CustomActor& GetImpl(const Dali::CustomActor& actor)
{
  DALI_ASSERT_ALWAYS( actor && "CustomActor handle is empty" );

  const BaseObject& handle = actor.GetBaseObject();

  return static_cast<const Internal::CustomActor&>(handle);
}

} // namespace Dali

#endif // DALI_INTERNAL_CUSTOM_ACTOR_H

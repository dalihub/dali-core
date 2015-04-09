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

// CLASS HEADER
#include <dali/public-api/actors/custom-actor-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/event/actors/custom-actor-internal.h>

namespace Dali
{

CustomActorImpl::~CustomActorImpl()
{
}

CustomActor CustomActorImpl::Self() const
{
  return CustomActor(mOwner);
}

void CustomActorImpl::OnPropertySet( Property::Index index, Property::Value propertyValue )
{
}

CustomActorImpl::CustomActorImpl(bool requiresTouchEvents)
: mOwner(NULL),
  mRequiresTouchEvents(requiresTouchEvents),
  mRequiresHoverEvents(false),
  mRequiresMouseWheelEvents(false)
{
}

void CustomActorImpl::Initialize(Internal::CustomActor& owner)
{
  DALI_ASSERT_DEBUG(mOwner == NULL); // should not already be owned

  mOwner = &owner;
}

Internal::CustomActor* CustomActorImpl::GetOwner() const
{
  return mOwner;
}

bool CustomActorImpl::RequiresTouchEvents() const
{
  return mRequiresTouchEvents;
}

bool CustomActorImpl::RequiresHoverEvents() const
{
  return mRequiresHoverEvents;
}

void CustomActorImpl::SetRequiresHoverEvents(bool requiresHoverEvents)
{
  mRequiresHoverEvents = requiresHoverEvents;
}

bool CustomActorImpl::RequiresMouseWheelEvents() const
{
  return mRequiresMouseWheelEvents;
}

void CustomActorImpl::SetRequiresMouseWheelEvents(bool requiresMouseWheelEvents)
{
  mRequiresMouseWheelEvents = requiresMouseWheelEvents;
}

void CustomActorImpl::RelayoutRequest()
{
  mOwner->RelayoutRequest();
}

float CustomActorImpl::CalculateChildSizeBase( const Dali::Actor& child, Dimension::Type dimension )
{
  return mOwner->CalculateChildSizeBase( child, dimension );
}

bool CustomActorImpl::RelayoutDependentOnChildrenBase( Dimension::Type dimension )
{
  return mOwner->RelayoutDependentOnChildrenBase( dimension );
}

} // namespace Dali

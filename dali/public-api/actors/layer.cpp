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

// CLASS HEADER
#include <dali/public-api/actors/layer.h>

// INTERNAL INCLUDES
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/images/frame-buffer-image-impl.h>

namespace Dali
{

using Dali::Layer;

Layer::Layer()
{
}

Layer Layer::New()
{
  Internal::LayerPtr internal = Internal::Layer::New();

  return Layer(internal.Get());
}

Layer Layer::DownCast( BaseHandle handle )
{
  return Layer( dynamic_cast<Dali::Internal::Layer*>(handle.GetObjectPtr()) );
}

Layer::~Layer()
{
}

Layer::Layer(const Layer& copy)
: Actor(copy)
{
}

Layer& Layer::operator=(const Layer& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

uint32_t Layer::GetDepth() const
{
  return GetImplementation(*this).GetDepth();
}

void Layer::Raise()
{
  GetImplementation(*this).Raise();
}

void Layer::Lower()
{
  GetImplementation(*this).Lower();
}

void Layer::RaiseAbove( Layer target )
{
  GetImplementation(*this).RaiseAbove( GetImplementation( target ) );
}

void Layer::LowerBelow( Layer target )
{
  GetImplementation(*this).LowerBelow( GetImplementation( target ) );
}

void Layer::RaiseToTop()
{
  GetImplementation(*this).RaiseToTop();
}

void Layer::LowerToBottom()
{
  GetImplementation(*this).LowerToBottom();
}

void Layer::MoveAbove( Layer target )
{
  GetImplementation(*this).MoveAbove( GetImplementation( target ) );
}

void Layer::MoveBelow( Layer target )
{
  GetImplementation(*this).MoveBelow( GetImplementation( target ) );
}

void Layer::SetBehavior( Behavior behavior )
{
  GetImplementation(*this).SetBehavior( behavior );
}

Layer::Behavior Layer::GetBehavior() const
{
  return GetImplementation(*this).GetBehavior();
}

void Layer::SetClipping(bool enabled)
{
  GetImplementation(*this).SetClipping(enabled);
}

bool Layer::IsClipping() const
{
  return GetImplementation(*this).IsClipping();
}

void Layer::SetClippingBox(int32_t x, int32_t y, int32_t width, int32_t height)
{
  GetImplementation(*this).SetClippingBox(x, y, width, height);
}

void Layer::SetClippingBox(ClippingBox box)
{
  GetImplementation(*this).SetClippingBox(box.x, box.y, box.width, box.height);
}

ClippingBox Layer::GetClippingBox() const
{
  return GetImplementation(*this).GetClippingBox();
}

void Layer::SetDepthTestDisabled( bool disable )
{
  GetImplementation(*this).SetDepthTestDisabled( disable );
}

bool Layer::IsDepthTestDisabled() const
{
  return GetImplementation(*this).IsDepthTestDisabled();
}

void Layer::SetSortFunction(SortFunctionType function)
{
  GetImplementation(*this).SetSortFunction(function);
}

void Layer::SetTouchConsumed( bool consume )
{
  GetImplementation( *this ).SetTouchConsumed( consume );
}

bool Layer::IsTouchConsumed() const
{
  return GetImplementation( *this ).IsTouchConsumed();
}

void Layer::SetHoverConsumed( bool consume )
{
  GetImplementation( *this ).SetHoverConsumed( consume );
}

bool Layer::IsHoverConsumed() const
{
  return GetImplementation( *this ).IsHoverConsumed();
}

Layer::Layer(Internal::Layer* internal)
: Actor(internal)
{
}

} // namespace Dali

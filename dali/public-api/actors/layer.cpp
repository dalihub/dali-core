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

// CLASS HEADER
#include <dali/public-api/actors/layer.h>

// INTERNAL INCLUDES
#include <dali/internal/event/actors/layer-impl.h>

namespace Dali
{
using Dali::Layer;

Layer::Layer() = default;

Layer Layer::New()
{
  Internal::LayerPtr internal = Internal::Layer::New();

  return Layer(internal.Get());
}

Layer Layer::DownCast(BaseHandle handle)
{
  return Layer(dynamic_cast<Dali::Internal::Layer*>(handle.GetObjectPtr()));
}

Layer::~Layer() = default;

Layer::Layer(const Layer& copy) = default;

Layer& Layer::operator=(const Layer& rhs) = default;

Layer::Layer(Layer&& rhs) = default;

Layer& Layer::operator=(Layer&& rhs) = default;

void Layer::Raise()
{
  GetImplementation(*this).Raise();
}

void Layer::Lower()
{
  GetImplementation(*this).Lower();
}

void Layer::RaiseAbove(Layer target)
{
  GetImplementation(*this).RaiseAbove(GetImplementation(target));
}

void Layer::LowerBelow(Layer target)
{
  GetImplementation(*this).LowerBelow(GetImplementation(target));
}

void Layer::RaiseToTop()
{
  GetImplementation(*this).RaiseToTop();
}

void Layer::LowerToBottom()
{
  GetImplementation(*this).LowerToBottom();
}

void Layer::MoveAbove(Layer target)
{
  GetImplementation(*this).MoveAbove(GetImplementation(target));
}

void Layer::MoveBelow(Layer target)
{
  GetImplementation(*this).MoveBelow(GetImplementation(target));
}

void Layer::SetSortFunction(SortFunctionType function)
{
  GetImplementation(*this).SetSortFunction(function);
}

Layer::Layer(Internal::Layer* internal)
: Actor(internal)
{
}

} // namespace Dali

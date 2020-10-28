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
#include <dali/public-api/events/gesture-detector.h>

// INTERNAL INCLUDES
#include <dali/internal/event/events/gesture-detector-impl.h>

namespace Dali
{
GestureDetector::GestureDetector(Internal::GestureDetector* internal)
: Handle(internal)
{
}

GestureDetector::GestureDetector() = default;

GestureDetector GestureDetector::DownCast(BaseHandle handle)
{
  return GestureDetector(dynamic_cast<Dali::Internal::GestureDetector*>(handle.GetObjectPtr()));
}

GestureDetector::~GestureDetector() = default;

GestureDetector::GestureDetector(const GestureDetector& handle) = default;

GestureDetector& GestureDetector::operator=(const GestureDetector& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

void GestureDetector::Attach(Actor actor)
{
  GetImplementation(*this).Attach(GetImplementation(actor));
}

void GestureDetector::Detach(Actor actor)
{
  GetImplementation(*this).Detach(GetImplementation(actor));
}

void GestureDetector::DetachAll()
{
  GetImplementation(*this).DetachAll();
}

size_t GestureDetector::GetAttachedActorCount() const
{
  return GetImplementation(*this).GetAttachedActorCount();
}

Actor GestureDetector::GetAttachedActor(size_t index) const
{
  return GetImplementation(*this).GetAttachedActor(index);
}

} // namespace Dali

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
#include <dali/public-api/events/gesture.h>

// INTERNAL INCLUDES
#include <dali/internal/event/events/gesture-impl.h>

namespace Dali
{
Gesture::Gesture() = default;

Gesture::Gesture(Internal::Gesture* internal)
: BaseHandle(internal)
{
}

Gesture::Gesture(const Gesture& rhs) = default;

Gesture::Gesture(Gesture&& rhs) = default;

Gesture& Gesture::operator=(const Gesture& rhs) = default;

Gesture& Gesture::operator=(Gesture&& rhs) = default;

Gesture::~Gesture() = default;

GestureType::Value Gesture::GetType() const
{
  return GetImplementation(*this).GetType();
}

GestureState Gesture::GetState() const
{
  return GetImplementation(*this).GetState();
}

uint32_t Gesture::GetTime() const
{
  return GetImplementation(*this).GetTime();
}

} // namespace Dali

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
#include <dali/public-api/events/pan-gesture.h>

// INTERNAL INCLUDES
#include <dali/internal/event/events/pan-gesture/pan-gesture-impl.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
PanGesture::PanGesture(Internal::PanGesture* internal)
: Gesture(internal)
{
}

PanGesture::PanGesture() = default;

PanGesture::PanGesture(const PanGesture& rhs) = default;

PanGesture::PanGesture(PanGesture&& rhs) noexcept = default;

PanGesture& PanGesture::operator=(const PanGesture& rhs) = default;

PanGesture& PanGesture::operator=(PanGesture&& rhs) noexcept = default;

PanGesture::~PanGesture() = default;

const Vector2& PanGesture::GetVelocity() const
{
  return GetImplementation(*this).GetVelocity();
}

const Vector2& PanGesture::GetDisplacement() const
{
  return GetImplementation(*this).GetDisplacement();
}

const Vector2& PanGesture::GetPosition() const
{
  return GetImplementation(*this).GetPosition();
}

const Vector2& PanGesture::GetScreenVelocity() const
{
  return GetImplementation(*this).GetScreenVelocity();
}

const Vector2& PanGesture::GetScreenDisplacement() const
{
  return GetImplementation(*this).GetScreenDisplacement();
}

const Vector2& PanGesture::GetScreenPosition() const
{
  return GetImplementation(*this).GetScreenPosition();
}

uint32_t PanGesture::GetNumberOfTouches() const
{
  return GetImplementation(*this).GetNumberOfTouches();
}

float PanGesture::GetSpeed() const
{
  return GetImplementation(*this).GetSpeed();
}

float PanGesture::GetDistance() const
{
  return GetImplementation(*this).GetDistance();
}

float PanGesture::GetScreenSpeed() const
{
  return GetImplementation(*this).GetScreenSpeed();
}

float PanGesture::GetScreenDistance() const
{
  return GetImplementation(*this).GetScreenDistance();
}

} // namespace Dali

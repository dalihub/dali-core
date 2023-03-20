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
#include <dali/public-api/events/pinch-gesture.h>

// INTERNAL INCLUDES
#include <dali/internal/event/events/pinch-gesture/pinch-gesture-impl.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
PinchGesture::PinchGesture(Internal::PinchGesture* internal)
: Gesture(internal)
{
}

PinchGesture::PinchGesture() = default;

PinchGesture::PinchGesture(const PinchGesture& rhs) = default;

PinchGesture::PinchGesture(PinchGesture&& rhs) noexcept = default;

PinchGesture& PinchGesture::operator=(const PinchGesture& rhs) = default;

PinchGesture& PinchGesture::operator=(PinchGesture&& rhs) noexcept = default;

PinchGesture::~PinchGesture() = default;

float PinchGesture::GetScale() const
{
  return GetImplementation(*this).GetScale();
}

float PinchGesture::GetSpeed() const
{
  return GetImplementation(*this).GetSpeed();
}

const Vector2& PinchGesture::GetScreenCenterPoint() const
{
  return GetImplementation(*this).GetScreenCenterPoint();
}

const Vector2& PinchGesture::GetLocalCenterPoint() const
{
  return GetImplementation(*this).GetLocalCenterPoint();
}

} // namespace Dali

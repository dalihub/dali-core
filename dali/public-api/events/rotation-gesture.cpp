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
#include <dali/public-api/events/rotation-gesture.h>

// INTERNAL INCLUDES
#include <dali/internal/event/events/rotation-gesture/rotation-gesture-impl.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
RotationGesture::RotationGesture(Internal::RotationGesture* internal)
: Gesture(internal)
{
}

RotationGesture::RotationGesture() = default;

RotationGesture::RotationGesture(const RotationGesture& rhs) = default;

RotationGesture::RotationGesture(RotationGesture&& rhs) noexcept = default;

RotationGesture& RotationGesture::operator=(const RotationGesture& rhs) = default;

RotationGesture& RotationGesture::operator=(RotationGesture&& rhs) noexcept = default;

RotationGesture::~RotationGesture() = default;

const Radian& RotationGesture::GetRotation() const
{
  return GetImplementation(*this).GetRotation();
}

const Vector2& RotationGesture::GetScreenCenterPoint() const
{
  return GetImplementation(*this).GetScreenCenterPoint();
}

const Vector2& RotationGesture::GetLocalCenterPoint() const
{
  return GetImplementation(*this).GetLocalCenterPoint();
}

} // namespace Dali

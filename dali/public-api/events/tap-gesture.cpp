/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/events/tap-gesture.h>

// INTERNAL INCLUDES
#include <dali/internal/event/events/tap-gesture/tap-gesture-impl.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
TapGesture::TapGesture(Internal::TapGesture* internal)
: Gesture(internal)
{
}

TapGesture::TapGesture() = default;

TapGesture::TapGesture(const TapGesture& rhs) = default;

TapGesture::TapGesture(TapGesture&& rhs) noexcept = default;

TapGesture& TapGesture::operator=(const TapGesture& rhs) = default;

TapGesture& TapGesture::operator=(TapGesture&& rhs) noexcept = default;

TapGesture::~TapGesture() = default;

uint32_t TapGesture::GetNumberOfTaps() const
{
  return GetImplementation(*this).GetNumberOfTaps();
}

uint32_t TapGesture::GetNumberOfTouches() const
{
  return GetImplementation(*this).GetNumberOfTouches();
}

const Vector2& TapGesture::GetScreenPoint() const
{
  return GetImplementation(*this).GetScreenPoint();
}

const Vector2& TapGesture::GetLocalPoint() const
{
  return GetImplementation(*this).GetLocalPoint();
}

} // namespace Dali

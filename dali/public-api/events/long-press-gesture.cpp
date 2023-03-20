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
#include <dali/public-api/events/long-press-gesture.h>

// INTERNAL INCLUDES
#include <dali/internal/event/events/long-press-gesture/long-press-gesture-impl.h>

namespace Dali
{
LongPressGesture::LongPressGesture(Internal::LongPressGesture* internal)
: Gesture(internal)
{
}

LongPressGesture::LongPressGesture() = default;

LongPressGesture::LongPressGesture(const LongPressGesture& rhs) = default;

LongPressGesture::LongPressGesture(LongPressGesture&& rhs) noexcept = default;

LongPressGesture& LongPressGesture::operator=(const LongPressGesture& rhs) = default;

LongPressGesture& LongPressGesture::operator=(LongPressGesture&& rhs) noexcept = default;

LongPressGesture::~LongPressGesture() = default;

uint32_t LongPressGesture::GetNumberOfTouches() const
{
  return GetImplementation(*this).GetNumberOfTouches();
}

const Vector2& LongPressGesture::GetScreenPoint() const
{
  return GetImplementation(*this).GetScreenPoint();
}

const Vector2& LongPressGesture::GetLocalPoint() const
{
  return GetImplementation(*this).GetLocalPoint();
}

} // namespace Dali

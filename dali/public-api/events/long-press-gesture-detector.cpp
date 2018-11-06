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
#include <dali/public-api/events/long-press-gesture-detector.h>

// INTERNAL INCLUDES
#include <dali/internal/event/events/long-press-gesture-detector-impl.h>

namespace Dali
{

LongPressGestureDetector::LongPressGestureDetector(Internal::LongPressGestureDetector* internal)
: GestureDetector(internal)
{
}

LongPressGestureDetector::LongPressGestureDetector()
{
}

LongPressGestureDetector LongPressGestureDetector::New()
{
  Internal::LongPressGestureDetectorPtr internal = Internal::LongPressGestureDetector::New();

  return LongPressGestureDetector(internal.Get());
}

LongPressGestureDetector LongPressGestureDetector::New(uint32_t touchesRequired)
{
  Internal::LongPressGestureDetectorPtr internal = Internal::LongPressGestureDetector::New(touchesRequired);

  return LongPressGestureDetector(internal.Get());
}

LongPressGestureDetector LongPressGestureDetector::New(uint32_t minTouches, uint32_t maxTouches)
{
  Internal::LongPressGestureDetectorPtr internal = Internal::LongPressGestureDetector::New(minTouches, maxTouches);

  return LongPressGestureDetector(internal.Get());
}

LongPressGestureDetector LongPressGestureDetector::DownCast( BaseHandle handle )
{
  return LongPressGestureDetector( dynamic_cast<Dali::Internal::LongPressGestureDetector*>(handle.GetObjectPtr()) );
}

LongPressGestureDetector::~LongPressGestureDetector()
{
}

LongPressGestureDetector::LongPressGestureDetector(const LongPressGestureDetector& handle)
: GestureDetector(handle)
{
}

LongPressGestureDetector& LongPressGestureDetector::operator=(const LongPressGestureDetector& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

void LongPressGestureDetector::SetTouchesRequired(uint32_t touches)
{
  GetImplementation(*this).SetTouchesRequired(touches);
}

void LongPressGestureDetector::SetTouchesRequired(uint32_t minTouches, uint32_t maxTouches)
{
  GetImplementation(*this).SetTouchesRequired(minTouches, maxTouches);
}

uint32_t LongPressGestureDetector::GetMinimumTouchesRequired() const
{
  return GetImplementation(*this).GetMinimumTouchesRequired();
}

uint32_t LongPressGestureDetector::GetMaximumTouchesRequired() const
{
  return GetImplementation(*this).GetMaximumTouchesRequired();
}

LongPressGestureDetector::DetectedSignalType& LongPressGestureDetector::DetectedSignal()
{
  return GetImplementation(*this).DetectedSignal();
}

} // namespace Dali

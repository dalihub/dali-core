/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

const char* const LongPressGestureDetector::SIGNAL_LONG_PRESS_DETECTED = "long-press-detected";

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

LongPressGestureDetector LongPressGestureDetector::New(unsigned int touchesRequired)
{
  Internal::LongPressGestureDetectorPtr internal = Internal::LongPressGestureDetector::New(touchesRequired);

  return LongPressGestureDetector(internal.Get());
}

LongPressGestureDetector LongPressGestureDetector::New(unsigned int minTouches, unsigned int maxTouches)
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

void LongPressGestureDetector::SetTouchesRequired(unsigned int touches)
{
  GetImplementation(*this).SetTouchesRequired(touches);
}

void LongPressGestureDetector::SetTouchesRequired(unsigned int minTouches, unsigned int maxTouches)
{
  GetImplementation(*this).SetTouchesRequired(minTouches, maxTouches);
}

unsigned int LongPressGestureDetector::GetMinimumTouchesRequired() const
{
  return GetImplementation(*this).GetMinimumTouchesRequired();
}

unsigned int LongPressGestureDetector::GetMaximumTouchesRequired() const
{
  return GetImplementation(*this).GetMaximumTouchesRequired();
}

LongPressGestureDetector::DetectedSignalV2& LongPressGestureDetector::DetectedSignal()
{
  return GetImplementation(*this).DetectedSignal();
}

} // namespace Dali

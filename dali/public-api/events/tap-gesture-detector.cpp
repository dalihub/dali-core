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
#include <dali/public-api/events/tap-gesture-detector.h>

// INTERNAL INCLUDES
#include <dali/internal/event/events/tap-gesture-detector-impl.h>

namespace Dali
{

const char* const TapGestureDetector::SIGNAL_TAP_DETECTED = "tap-detected";

TapGestureDetector::TapGestureDetector(Internal::TapGestureDetector* internal)
: GestureDetector(internal)
{
}

TapGestureDetector::TapGestureDetector()
{
}

TapGestureDetector TapGestureDetector::New()
{
  Internal::TapGestureDetectorPtr internal = Internal::TapGestureDetector::New();

  return TapGestureDetector(internal.Get());
}

TapGestureDetector TapGestureDetector::New(unsigned int tapsRequired, unsigned int touchesRequired)
{
  Internal::TapGestureDetectorPtr internal = Internal::TapGestureDetector::New(tapsRequired, touchesRequired);

  return TapGestureDetector(internal.Get());
}

TapGestureDetector TapGestureDetector::DownCast( BaseHandle handle )
{
  return TapGestureDetector( dynamic_cast<Dali::Internal::TapGestureDetector*>(handle.GetObjectPtr()) );
}

TapGestureDetector::~TapGestureDetector()
{
}

TapGestureDetector::TapGestureDetector(const TapGestureDetector& handle)
: GestureDetector(handle)
{
}

TapGestureDetector& TapGestureDetector::operator=(const TapGestureDetector& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

void TapGestureDetector::SetTapsRequired(unsigned int taps)
{
  GetImplementation(*this).SetTapsRequired(taps);
}

void TapGestureDetector::SetTouchesRequired(unsigned int touches)
{
  GetImplementation(*this).SetTouchesRequired(touches);
}

unsigned int TapGestureDetector::GetTapsRequired() const
{
  return GetImplementation(*this).GetTapsRequired();
}

unsigned int TapGestureDetector::GetTouchesRequired() const
{
  return GetImplementation(*this).GetTouchesRequired();
}

TapGestureDetector::DetectedSignalType& TapGestureDetector::DetectedSignal()
{
  return GetImplementation(*this).DetectedSignal();
}

} // namespace Dali

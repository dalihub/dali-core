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

TapGestureDetector TapGestureDetector::New(unsigned int tapsRequired )
{
  Internal::TapGestureDetectorPtr internal = Internal::TapGestureDetector::New( tapsRequired );

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

void TapGestureDetector::SetMinimumTapsRequired(unsigned int taps)
{
  GetImplementation(*this).SetMinimumTapsRequired(taps);
}

void TapGestureDetector::SetMaximumTapsRequired(unsigned int taps)
{
  GetImplementation(*this).SetMaximumTapsRequired(taps);
}

unsigned int TapGestureDetector::GetMinimumTapsRequired() const
{
  return GetImplementation(*this).GetMinimumTapsRequired();
}

unsigned int TapGestureDetector::GetMaximumTapsRequired() const
{
  return GetImplementation(*this).GetMaximumTapsRequired();
}

TapGestureDetector::DetectedSignalType& TapGestureDetector::DetectedSignal()
{
  return GetImplementation(*this).DetectedSignal();
}

} // namespace Dali

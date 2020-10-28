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
#include <dali/public-api/events/pinch-gesture-detector.h>

// INTERNAL INCLUDES
#include <dali/internal/event/events/pinch-gesture/pinch-gesture-detector-impl.h>

namespace Dali
{
PinchGestureDetector::PinchGestureDetector(Internal::PinchGestureDetector* internal)
: GestureDetector(internal)
{
}

PinchGestureDetector::PinchGestureDetector() = default;

PinchGestureDetector PinchGestureDetector::New()
{
  Internal::PinchGestureDetectorPtr internal = Internal::PinchGestureDetector::New();

  return PinchGestureDetector(internal.Get());
}

PinchGestureDetector PinchGestureDetector::DownCast(BaseHandle handle)
{
  return PinchGestureDetector(dynamic_cast<Dali::Internal::PinchGestureDetector*>(handle.GetObjectPtr()));
}

PinchGestureDetector::~PinchGestureDetector() = default;

PinchGestureDetector::PinchGestureDetector(const PinchGestureDetector& handle) = default;

PinchGestureDetector& PinchGestureDetector::operator=(const PinchGestureDetector& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

PinchGestureDetector::DetectedSignalType& PinchGestureDetector::DetectedSignal()
{
  return GetImplementation(*this).DetectedSignal();
}

} // namespace Dali

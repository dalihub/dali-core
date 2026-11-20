/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/events/hover-event-integ.h>

// EXTERNAL INCLUDES
#include <utility>

// INTERNAL INCLUDES
#include <dali/internal/event/events/hover-event-impl.h>

namespace DALI_NAMESPACE
{
namespace Integration
{
HoverEvent::HoverEvent()
: MultiPointEvent(Hover)
{
}

HoverEvent::HoverEvent(uint32_t initTime)
: MultiPointEvent(Hover, initTime)
{
}

HoverEvent::HoverEvent(const Dali::HoverEvent& hoverEvent)
: MultiPointEvent(Hover, GetImplementation(hoverEvent).GetTime())
{
  const Internal::HoverEvent& impl       = GetImplementation(hoverEvent);
  const uint32_t              pointCount = impl.GetPointCount();
  points.reserve(pointCount);

  for(uint32_t i = 0; i < pointCount; ++i)
  {
    AddPoint(impl.GetPoint(i));
  }
}

HoverEvent::HoverEvent(const HoverEvent& rhs)
: MultiPointEvent(Hover, rhs)
{
}

HoverEvent::HoverEvent(HoverEvent&& rhs) noexcept
: MultiPointEvent(Hover, std::move(rhs))
{
}

HoverEvent& HoverEvent::operator=(const HoverEvent& rhs)
{
  MultiPointEvent::operator=(rhs);
  return *this;
}

HoverEvent& HoverEvent::operator=(HoverEvent&& rhs) noexcept
{
  MultiPointEvent::operator=(std::move(rhs));
  return *this;
}

HoverEvent::~HoverEvent() = default;

} // namespace Integration

} //namespace DALI_NAMESPACE

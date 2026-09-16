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
#include <dali/internal/event/events/pan-gesture/pan-gesture-profile.h>

// EXTERNAL INCLUDES
#include <cmath>

// INTERNAL INCLUDES
#include <dali/public-api/math/math-utils.h>

namespace DALI_NAMESPACE
{
namespace Internal
{
namespace
{
/**
 * Returns the angle going in the opposite direction to that specified by angle.
 */
float GetOppositeAngle(float angle)
{
  // Calculate the opposite angle so that we cover both directions.
  if(angle <= 0.0f)
  {
    angle += Math::PI;
  }
  else
  {
    angle -= Math::PI;
  }

  return angle;
}

} // unnamed namespace

void PanGestureProfile::AddAngle(Radian angle, Radian threshold)
{
  threshold = fabsf(threshold); // Ensure the threshold is positive.

  // If the threshold is greater than PI, then just use PI
  // This means that any panned angle will invoke the pan gesture. We should still add this angle as
  // an angle may have been added previously with a small threshold.
  if(threshold > Math::PI)
  {
    threshold = Math::PI;
  }

  angle = WrapInDomain(angle, -Math::PI, Math::PI);

  angles.push_back(AngleThresholdPair(angle, threshold));
}

void PanGestureProfile::AddDirection(Radian direction, Radian threshold)
{
  AddAngle(direction, threshold);

  // Calculate the opposite angle so that we cover the entire direction.
  AddAngle(Radian(GetOppositeAngle(direction)), threshold);
}

void PanGestureProfile::RemoveAngle(Radian angle)
{
  angle = WrapInDomain(angle, -Math::PI, Math::PI);

  for(AngleContainer::iterator iter = angles.begin(), endIter = angles.end(); iter != endIter; ++iter)
  {
    if(iter->first == angle)
    {
      angles.erase(iter);
      break;
    }
  }
}

void PanGestureProfile::RemoveDirection(Radian direction)
{
  RemoveAngle(direction);

  // Calculate the opposite angle so that we cover the entire direction.
  RemoveAngle(Radian(GetOppositeAngle(direction)));
}

void PanGestureProfile::ClearAngles()
{
  angles.clear();
}

uint32_t PanGestureProfile::GetAngleCount() const
{
  return static_cast<uint32_t>(angles.size());
}

PanGestureProfile::AngleThresholdPair PanGestureProfile::GetAngle(uint32_t index) const
{
  AngleThresholdPair ret(Radian(0), Radian(0));

  if(index < angles.size())
  {
    ret = angles[index];
  }

  return ret;
}

bool PanGestureProfile::RequiresDirectionalPan() const
{
  // If no directional angles have been added then we do not require directional panning
  return !angles.empty();
}

bool PanGestureProfile::IsAngleAllowed(Radian angle) const
{
  if(angles.empty())
  {
    return true;
  }

  for(const AngleThresholdPair& pair : angles)
  {
    const float angleAllowed(pair.first);
    const float threshold(pair.second);

    const float relativeAngle(fabsf(WrapInDomain(angle - angleAllowed, -Math::PI, Math::PI)));
    if(relativeAngle <= threshold)
    {
      return true;
    }
  }

  return false;
}

bool PanGestureProfile::IsValid() const
{
  return minimumTouches > 0u && maximumTouches > 0u && minimumTouches <= maximumTouches;
}

} // namespace Internal

} // namespace DALI_NAMESPACE

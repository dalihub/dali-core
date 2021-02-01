#ifndef DALI_INTERNAL_PAN_GESTURE_PROFILING_H
#define DALI_INTERNAL_PAN_GESTURE_PROFILING_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/events/gesture-enumerations.h>
#include <dali/public-api/math/vector2.h>

namespace Dali
{
namespace Internal
{
struct PanGestureProfiling
{
  struct Position
  {
    Position(unsigned int time, Vector2 position, Vector2 displacement, Vector2 velocity, GestureState state)
    : time(time),
      position(position),
      displacement(displacement),
      velocity(velocity),
      state(state)
    {
    }

    unsigned int time;
    Vector2      position;
    Vector2      displacement;
    Vector2      velocity;
    GestureState state;
  };

  using PanPositionContainer = std::vector<PanGestureProfiling::Position>;

  void PrintData() const;

  void PrintData(const PanPositionContainer& dataContainer, const char* const prefix) const;

  void ClearData();

  PanPositionContainer mRawData;
  PanPositionContainer mLatestData;
  PanPositionContainer mAveragedData;
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_PAN_GESTURE_PROFILING_H

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

#include "test-gesture-generator.h"

// INTERNAL INCLUDES
#include <dali/integration-api/events/touch-event-integ.h>

namespace
{
const uint32_t RENDER_FRAME_INTERVAL = 16; ///< Duration of each frame in ms. (at approx 60FPS)

Integration::TouchEvent GenerateSingleTouch(PointState::Type state, const Vector2& screenPosition, uint32_t time)
{
  Integration::TouchEvent touchEvent;
  Integration::Point      point;
  point.SetState(state);
  point.SetDeviceId(4);
  point.SetScreenPosition(screenPosition);
  point.SetDeviceClass(Device::Class::TOUCH);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  touchEvent.points.push_back(point);
  touchEvent.time = time;
  return touchEvent;
}

Integration::TouchEvent GenerateDoubleTouch(PointState::Type stateA, const Vector2& screenPositionA, PointState::Type stateB, const Vector2& screenPositionB, uint32_t time)
{
  Integration::TouchEvent touchEvent;
  Integration::Point      point;
  point.SetState(stateA);
  point.SetDeviceId(4);
  point.SetScreenPosition(screenPositionA);
  point.SetDeviceClass(Device::Class::TOUCH);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  touchEvent.points.push_back(point);
  point.SetScreenPosition(screenPositionB);
  point.SetState(stateB);
  point.SetDeviceId(7);
  touchEvent.points.push_back(point);
  touchEvent.time = time;
  return touchEvent;
}
} // namespace

namespace Dali
{
uint32_t TestGetFrameInterval()
{
  return RENDER_FRAME_INTERVAL;
}

void TestStartLongPress(TestApplication& application, float x, float y, uint32_t time)
{
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(x, y), time));
}

void TestTriggerLongPress(TestApplication& application)
{
  application.GetPlatform().TriggerTimer();
}

void TestGenerateLongPress(TestApplication& application, float x, float y, uint32_t time)
{
  TestStartLongPress(application, x, y, time);
  TestTriggerLongPress(application);
}

void TestEndLongPress(TestApplication& application, float x, float y, uint32_t time)
{
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(x, y), time));
}

void TestGeneratePinch(TestApplication& application)
{
  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), PointState::DOWN, Vector2(20.0f, 90.0f), 150));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 28.0f), PointState::MOTION, Vector2(20.0f, 82.0f), 160));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 37.0f), PointState::MOTION, Vector2(20.0f, 74.0f), 170));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 46.0f), PointState::MOTION, Vector2(20.0f, 66.0f), 180));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 55.0f), PointState::MOTION, Vector2(20.0f, 58.0f), 190));
  application.ProcessEvent(GenerateDoubleTouch(PointState::UP, Vector2(20.0f, 55.0f), PointState::UP, Vector2(20.0f, 58.0f), 200));
}

void TestStartPinch(TestApplication& application, Vector2 a1, Vector2 b1, Vector2 a2, Vector2 b2, uint32_t time)
{
  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, a1, PointState::DOWN, b1, time));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, a2, PointState::MOTION, b2, time + 50));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, a2, PointState::MOTION, b2, time + 100));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, a2, PointState::MOTION, b2, time + 150));
}

void TestContinuePinch(TestApplication& application, Vector2 a1, Vector2 b1, Vector2 a2, Vector2 b2, uint32_t time)
{
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, a1, PointState::MOTION, b1, time));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, a1, PointState::MOTION, b1, time + 50));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, a2, PointState::MOTION, b2, time + 100));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, a2, PointState::MOTION, b2, time + 150));
}

void TestEndPinch(TestApplication& application, Vector2 a1, Vector2 b1, Vector2 a2, Vector2 b2, uint32_t time)
{
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, a1, PointState::MOTION, b1, time));
  application.ProcessEvent(GenerateDoubleTouch(PointState::UP, a2, PointState::UP, b2, time + 50));
}

void TestGenerateMiniPan(TestApplication& application)
{
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 250));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 40.0f), 251));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 40.0f), 255));
}

void TestStartPan(TestApplication& application, Vector2 start, Vector2 end, uint32_t& time)
{
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, start, time));

  time += RENDER_FRAME_INTERVAL;

  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, end, time));

  time += RENDER_FRAME_INTERVAL;

  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, end, time));

  time += RENDER_FRAME_INTERVAL;
}

void TestMovePan(TestApplication& application, Vector2 pos, uint32_t time)
{
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, pos, time));
}

void TestEndPan(TestApplication& application, Vector2 pos, uint32_t time)
{
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, pos, time));
}

void TestTriggerTap(TestApplication& application)
{
  application.GetPlatform().TriggerTimer();
}

void TestGenerateTap(TestApplication& application, float x, float y, uint32_t time_down)
{
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(x, y), time_down));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(x, y), time_down + 20));
  TestTriggerTap(application);
}

void TestGenerateTwoPointTap(TestApplication& application, float x1, float y1, float x2, float y2, uint32_t time_down)
{
  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(x1, y1), PointState::DOWN, Vector2(x2, y2), time_down));
  application.ProcessEvent(GenerateDoubleTouch(PointState::UP, Vector2(x1, y1), PointState::UP, Vector2(x2, y2), time_down + 20));
  TestTriggerTap(application);
}

void TestGenerateRotation(TestApplication& application)
{
  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), PointState::DOWN, Vector2(20.0f, 90.0f), 150));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 20.0f), PointState::MOTION, Vector2(25.0f, 95.0f), 160));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 20.0f), PointState::MOTION, Vector2(30.0f, 100.0f), 170));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 20.0f), PointState::MOTION, Vector2(35.0f, 105.0f), 180));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 20.0f), PointState::MOTION, Vector2(40.0f, 110.0f), 190));
  application.ProcessEvent(GenerateDoubleTouch(PointState::UP, Vector2(20.0f, 20.0f), PointState::UP, Vector2(45.0f, 115.0f), 200));
}

void TestStartRotation(TestApplication& application, Vector2 a1, Vector2 b1, Vector2 a2, Vector2 b2, uint32_t time)
{
  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, a1, PointState::DOWN, b1, time));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, a2, PointState::MOTION, b2, time + 50));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, a2, PointState::MOTION, b2, time + 100));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, a2, PointState::MOTION, b2, time + 150));
}

void TestContinueRotation(TestApplication& application, Vector2 a1, Vector2 b1, Vector2 a2, Vector2 b2, uint32_t time)
{
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, a1, PointState::MOTION, b1, time));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, a1, PointState::MOTION, b1, time + 50));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, a2, PointState::MOTION, b2, time + 100));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, a2, PointState::MOTION, b2, time + 150));
}

void TestEndRotation(TestApplication& application, Vector2 a1, Vector2 b1, Vector2 a2, Vector2 b2, uint32_t time)
{
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, a1, PointState::MOTION, b1, time));
  application.ProcessEvent(GenerateDoubleTouch(PointState::UP, a2, PointState::UP, b2, time + 50));
}

} // namespace Dali

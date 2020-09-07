#ifndef DALI_TEST_GESTURE_GENERATOR_H
#define DALI_TEST_GESTURE_GENERATOR_H

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

#include "test-application.h"

namespace Dali
{
/**
 * These functions use touch events to trigger a gesture, assuming the default gesture parameters are used
 */

/**
 * Returns the frame interval used in ms
 */
uint32_t TestGetFrameInterval();

/**
 * Produces the initial touch of a long press
 */
void TestStartLongPress(TestApplication& application, float x = 20.0f, float y = 20.0f, uint32_t time = 450);

/**
 * Triggers the timer to begin a long press gesture
 */
void TestTriggerLongPress(TestApplication& application);

/**
 * Produces the initial press and triggers the timer to begin a long press gesture
 */
void TestGenerateLongPress(TestApplication& application, float x = 20.0f, float y = 20.0f, uint32_t time = 450);

/**
 * End a long press by lifting the touch
 */
void TestEndLongPress(TestApplication& application, float x = 20.0f, float y = 20.0f, uint32_t time = 450);

/**
 * Produces a vertical pinch gesture between (20,20) and (20,90)
 */
void TestGeneratePinch(TestApplication& application);

/**
 * Produces the gesture started event of a pinch, using 4 touches, 50ms apart, starting with 1, ending at 2
 */
void TestStartPinch(TestApplication& application, Vector2 a1, Vector2 b1, Vector2 a2, Vector2 b2, uint32_t time);

/**
 * Produces a gesture continuing event of a pinch, using 4 touches, 50ms apart, starting with 1, ending at 2
 */
void TestContinuePinch(TestApplication& application, Vector2 a1, Vector2 b1, Vector2 a2, Vector2 b2, uint32_t time);

/**
 * Produces a gesture finished event of a pinch, using 2 touches, 50ms apart
 */
void TestEndPinch(TestApplication& application, Vector2 a1, Vector2 b1, Vector2 a2, Vector2 b2, uint32_t time);

/**
 * Produces a pan gesture from (20,20) to (20,40)
 */
void TestGenerateMiniPan(TestApplication& application);

/**
 * Produces the start event of a pan gesture, assuming minimum distance moved between start and end is greater than 15
 * in either direction or 11 in both (x&y). Time will be incremented using the standard frame interval per touch movement
 */
void TestStartPan(TestApplication& application, Vector2 start, Vector2 end, uint32_t& time);

/**
 * Continues a pan event by creating a single touch at pos.
 * N.B This does not increment the time
 */
void TestMovePan(TestApplication& application, Vector2 pos, uint32_t time = 400);

/**
 * End a pan gesture at position pos
 */
void TestEndPan(TestApplication& application, Vector2 pos, uint32_t time = 500);

/**
 * Produces a single point tap gesture with a 20ms interval
 */
void TestGenerateTap(TestApplication& application, float x = 20.0f, float y = 20.0f, uint32_t time_down = 100);

/**
 * Produce a tap gesture with two touch points and a 20ms interval
 */
void TestGenerateTwoPointTap(TestApplication& application, float x1, float y1, float x2, float y2, uint32_t time_down);

/**
 * Produces a rotation gesture.
 */
void TestGenerateRotation(TestApplication& application);

/**
 * Produces the gesture started event of a rotation, using 4 touches, 50ms apart, starting with 1, ending at 2
 */
void TestStartRotation(TestApplication& application, Vector2 a1, Vector2 b1, Vector2 a2, Vector2 b2, uint32_t time);

/**
 * Produces a gesture continuing event of a rotation, using 4 touches, 50ms apart, starting with 1, ending at 2
 */
void TestContinueRotation(TestApplication& application, Vector2 a1, Vector2 b1, Vector2 a2, Vector2 b2, uint32_t time);

/**
 * Produces a gesture finished event of a rotation, using 2 touches, 50ms apart
 */
void TestEndRotation(TestApplication& application, Vector2 a1, Vector2 b1, Vector2 a2, Vector2 b2, uint32_t time);

} // namespace Dali

#endif // DALI_TEST_GESTURE_GENERATOR_H

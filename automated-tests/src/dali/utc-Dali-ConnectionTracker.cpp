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

// EXTERNAL INCLUDES
#include <stdlib.h>

#include <iostream>

// INTERNAL INCLUDES
#include <dali-test-suite-utils.h>
#include <dali/public-api/dali-core.h>
#include <signal-helper.h>

using namespace Dali;

void utc_dali_connection_tracker_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_connection_tracker_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
} // namespace

/*******************************************
 *
 * Start of Utc test cases.
 * Test cases performed in order of API listed in dali-signal.h
 * UtcDaliSignal + FunctionName + P=positive test, N = Negative test
 *
 */

int UtcConnectionTrackerConstructorP(void)
{
  TestApplication application; // Create core for debug logging

  ConnectionTracker tracker;

  DALI_TEST_CHECK(tracker.GetConnectionCount() == 0);

  END_TEST;
}

int UtcConnectionTrackerDestructorP(void)
{
  TestApplication application; // Create core for debug logging
  // make sure the ConnectionTracker disconnects form a signal when it
  // gets deleted.
  TestButton* button = new TestButton(1);
  {
    TestApp testApp;
    button->DownSignal().Connect(&testApp, &TestApp::OnButtonPress);
    DALI_TEST_CHECK(testApp.GetConnectionCount() == 1);
    DALI_TEST_CHECK(button->DownSignal().GetConnectionCount() == 1);
  }
  // testApp out of scope it should have been disconnected
  DALI_TEST_CHECK(button->DownSignal().GetConnectionCount() == 0);

  delete button;

  END_TEST;
}

int UtcConnectionTrackerDisconnectAllP(void)
{
  TestApplication application; // Create core for debug logging

  TestButton* button = new TestButton(1);
  TestApp     testApp;
  button->DownSignal().Connect(&testApp, &TestApp::OnButtonPress);

  DALI_TEST_CHECK(button->DownSignal().GetConnectionCount() == 1);

  testApp.DisconnectAll();

  DALI_TEST_CHECK(button->DownSignal().GetConnectionCount() == 0);

  delete button;

  END_TEST;
}

int UtcConnectionTrackerDisconnectAllN(void)
{
  TestApplication application; // Create core for debug logging
  TestApp         testApp;
  TestButton*     button = new TestButton(1);

  DALI_TEST_CHECK(button->DownSignal().GetConnectionCount() == 0);
  testApp.DisconnectAll();
  DALI_TEST_CHECK(button->DownSignal().GetConnectionCount() == 0);

  delete button;

  END_TEST;
}

int UtcConnectionTrackerSignalConnectedP(void)
{
  TestApplication application; // Create core for debug logging

  TestButton* button = new TestButton(1);
  TestApp     testApp;
  button->DownSignal().Connect(&testApp, &TestApp::OnButtonPress);

  DALI_TEST_CHECK(button->DownSignal().GetConnectionCount() == 1);

  delete button;

  END_TEST;
}
int UtcConnectionTrackerSignalConnectedN(void)
{
  TestApplication application; // Create core for debug logging

  TestButton* button = new TestButton(1);
  TestApp*    testApp(NULL);

  try
  {
    // connect to a null connection tracker
    button->DownSignal().Connect(testApp, &TestApp::OnButtonPress);
  }
  catch(Dali::DaliException& e)
  {
    // Tests that a negative test of an assertion succeeds
    DALI_TEST_PRINT_ASSERT(e);
    tet_result(TET_PASS);
  }

  delete button;

  END_TEST;
}

int UtcConnectionTrackerSignalDisconnectP(void)
{
  TestApplication application; // Create core for debug logging

  TestButton* button = new TestButton(1);
  TestApp     testApp;
  button->DownSignal().Connect(&testApp, &TestApp::OnButtonPress);

  DALI_TEST_CHECK(button->DownSignal().GetConnectionCount() == 1);

  button->DownSignal().Disconnect(&testApp, &TestApp::OnButtonPress);
  DALI_TEST_CHECK(button->DownSignal().GetConnectionCount() == 0);

  delete button;

  END_TEST;
}

int UtcConnectionTrackerSignalDisconnectN(void)
{
  TestApplication application; // Create core for debug logging

  TestButton* button = new TestButton(1);
  TestApp     testApp;
  button->DownSignal().Connect(&testApp, &TestApp::OnButtonPress);

  DALI_TEST_CHECK(button->DownSignal().GetConnectionCount() == 1);

  try
  {
    application.SignalDisconnected(NULL, NULL);
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    tet_result(TET_PASS);
  }

  delete button;

  END_TEST;
}

int UtcConnectionTrackerGetConnectionCountP(void)
{
  TestApplication application; // Create core for debug logging

  TestButton* button = new TestButton(1);
  TestApp     testApp;
  button->DownSignal().Connect(&testApp, &TestApp::OnButtonPress);
  DALI_TEST_CHECK(button->DownSignal().GetConnectionCount() == 1);

  delete button;

  END_TEST;
}

int UtcConnectionTrackerGetConnectionCountN(void)
{
  TestApplication application; // Create core for debug logging

  TestButton* button = new TestButton(1);
  DALI_TEST_CHECK(button->DownSignal().GetConnectionCount() == 0);

  delete button;

  END_TEST;
}

int UtcConnectionTrackerInlineToMapHandover(void)
{
  tet_infoline("Test that connecting >8 signals triggers inline-to-map handover");
  TestApplication application;

  const int   count = 12; // Exceeds inline capacity of 8
  TestButton* buttons[count];
  TestApp     testApp;

  for(int i = 0; i < count; ++i)
  {
    buttons[i] = new TestButton(i);
    buttons[i]->DownSignal().Connect(&testApp, &TestApp::OnButtonPress);
  }

  DALI_TEST_EQUALS(testApp.GetConnectionCount(), static_cast<std::size_t>(count), TEST_LOCATION);

  // All signals should still be connected
  for(int i = 0; i < count; ++i)
  {
    DALI_TEST_EQUALS(buttons[i]->DownSignal().GetConnectionCount(), 1u, TEST_LOCATION);
  }

  // Emit each — all should fire
  for(int i = 0; i < count; ++i)
  {
    testApp.mButtonPressed = false;
    buttons[i]->Press();
    DALI_TEST_EQUALS(testApp.mButtonPressed, true, TEST_LOCATION);
    DALI_TEST_EQUALS(testApp.GetButtonPressedId(), i, TEST_LOCATION);
  }

  for(int i = 0; i < count; ++i)
  {
    delete buttons[i];
  }

  END_TEST;
}

int UtcConnectionTrackerDisconnectAfterHandover(void)
{
  tet_infoline("Test disconnecting signals after inline-to-map handover");
  TestApplication application;

  const int   count = 12;
  TestButton* buttons[count];
  TestApp     testApp;

  for(int i = 0; i < count; ++i)
  {
    buttons[i] = new TestButton(i);
    buttons[i]->DownSignal().Connect(&testApp, &TestApp::OnButtonPress);
  }

  DALI_TEST_EQUALS(testApp.GetConnectionCount(), static_cast<std::size_t>(count), TEST_LOCATION);

  // Disconnect half
  for(int i = 0; i < count; i += 2)
  {
    buttons[i]->DownSignal().Disconnect(&testApp, &TestApp::OnButtonPress);
  }

  DALI_TEST_EQUALS(testApp.GetConnectionCount(), static_cast<std::size_t>(count / 2), TEST_LOCATION);

  // Even-indexed buttons should be disconnected
  for(int i = 0; i < count; i += 2)
  {
    DALI_TEST_EQUALS(buttons[i]->DownSignal().GetConnectionCount(), 0u, TEST_LOCATION);
  }

  // Odd-indexed buttons should still work
  for(int i = 1; i < count; i += 2)
  {
    DALI_TEST_EQUALS(buttons[i]->DownSignal().GetConnectionCount(), 1u, TEST_LOCATION);
    testApp.mButtonPressed = false;
    buttons[i]->Press();
    DALI_TEST_EQUALS(testApp.mButtonPressed, true, TEST_LOCATION);
  }

  for(int i = 0; i < count; ++i)
  {
    delete buttons[i];
  }

  END_TEST;
}

int UtcConnectionTrackerDisconnectAllAfterHandover(void)
{
  tet_infoline("Test DisconnectAll resets to inline mode after map handover");
  TestApplication application;

  const int   count = 12;
  TestButton* buttons[count];
  TestApp     testApp;

  for(int i = 0; i < count; ++i)
  {
    buttons[i] = new TestButton(i);
    buttons[i]->DownSignal().Connect(&testApp, &TestApp::OnButtonPress);
  }

  DALI_TEST_EQUALS(testApp.GetConnectionCount(), static_cast<std::size_t>(count), TEST_LOCATION);

  testApp.DisconnectAll();

  DALI_TEST_EQUALS(testApp.GetConnectionCount(), static_cast<std::size_t>(0), TEST_LOCATION);

  // All signals should be disconnected
  for(int i = 0; i < count; ++i)
  {
    DALI_TEST_EQUALS(buttons[i]->DownSignal().GetConnectionCount(), 0u, TEST_LOCATION);
  }

  // Reconnect a few (should work in inline mode again after reset)
  buttons[0]->DownSignal().Connect(&testApp, &TestApp::OnButtonPress);
  buttons[1]->DownSignal().Connect(&testApp, &TestApp::OnButtonPress);

  DALI_TEST_EQUALS(testApp.GetConnectionCount(), static_cast<std::size_t>(2), TEST_LOCATION);

  testApp.mButtonPressed = false;
  buttons[0]->Press();
  DALI_TEST_EQUALS(testApp.mButtonPressed, true, TEST_LOCATION);

  for(int i = 0; i < count; ++i)
  {
    delete buttons[i];
  }

  END_TEST;
}

int UtcConnectionTrackerDestroyAfterHandover(void)
{
  tet_infoline("Test tracker destruction after map handover disconnects all signals");
  TestApplication application;

  const int   count = 12;
  TestButton* buttons[count];

  for(int i = 0; i < count; ++i)
  {
    buttons[i] = new TestButton(i);
  }

  {
    TestApp testApp;
    for(int i = 0; i < count; ++i)
    {
      buttons[i]->DownSignal().Connect(&testApp, &TestApp::OnButtonPress);
    }
    DALI_TEST_EQUALS(testApp.GetConnectionCount(), static_cast<std::size_t>(count), TEST_LOCATION);
    // testApp goes out of scope here
  }

  // All signals should be disconnected after tracker destruction
  for(int i = 0; i < count; ++i)
  {
    DALI_TEST_EQUALS(buttons[i]->DownSignal().GetConnectionCount(), 0u, TEST_LOCATION);
  }

  for(int i = 0; i < count; ++i)
  {
    delete buttons[i];
  }

  END_TEST;
}

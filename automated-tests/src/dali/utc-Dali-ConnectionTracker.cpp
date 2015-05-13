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

// EXTERNAL INCLUDES
#include <iostream>
#include <stdlib.h>

// INTERNAL INCLUDES
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>
#include <signal-helper.h>


using namespace Dali;

void utc_dali_conenction_tracker_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_conenction_tracker_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace {


} // anon namespace


/*******************************************
 *
 * Start of Utc test cases.
 * Test cases performed in order of API listed in dali-signal.h
 * UtcDaliSignal + FunctionName + P=positive test, N = Negative test
 *
 */

int UtcConnectionTrackerConstructorP(void)
{
  TestApplication app; // Create core for debug logging

  ConnectionTracker tracker;

  DALI_TEST_CHECK( tracker.GetConnectionCount() == 0 );

  END_TEST;
}

int UtcConnectionTrackerDestructorP(void)
{
  TestApplication app; // Create core for debug logging
  // make sure the ConnectionTracker disconnects form a signal when it
  // gets deleted.
  TestButton* button = new TestButton(1);
  {
    TestApp testApp;
    button->DownSignal().Connect(&testApp,&TestApp::OnButtonPress);
    DALI_TEST_CHECK( testApp.GetConnectionCount() == 1 );
    DALI_TEST_CHECK( button->DownSignal().GetConnectionCount( )== 1 );
  }
  // testApp out of scope it should have been disconnected
  DALI_TEST_CHECK( button->DownSignal().GetConnectionCount( ) == 0 );

  END_TEST;
}

int UtcConnectionTrackerDisconnectAllP(void)
{
  TestApplication app; // Create core for debug logging

  TestButton* button = new TestButton(1);
  TestApp testApp;
  button->DownSignal().Connect(&testApp,&TestApp::OnButtonPress);

  DALI_TEST_CHECK( button->DownSignal().GetConnectionCount( ) == 1 );

  testApp.DisconnectAll();

  DALI_TEST_CHECK( button->DownSignal().GetConnectionCount( ) == 0 );

  END_TEST;
}

int UtcConnectionTrackerDisconnectAllN(void)
{
  TestApplication app; // Create core for debug logging
  TestApp testApp;
  TestButton* button = new TestButton(1);

  DALI_TEST_CHECK( button->DownSignal().GetConnectionCount( ) == 0 );
  testApp.DisconnectAll();
  DALI_TEST_CHECK( button->DownSignal().GetConnectionCount( ) == 0 );

  END_TEST;
}

int UtcConnectionTrackerSignalConnectedP(void)
{
  TestApplication app; // Create core for debug logging

  TestButton* button = new TestButton(1);
  TestApp testApp;
  button->DownSignal().Connect(&testApp,&TestApp::OnButtonPress);

  DALI_TEST_CHECK( button->DownSignal().GetConnectionCount( ) == 1 );

  END_TEST;

}
int UtcConnectionTrackerSignalConnectedN(void)
{
  TestApplication app; // Create core for debug logging

  TestButton* button = new TestButton(1);
  TestApp* testApp( NULL );

  try
  {
    // connect to a null connection tracker
    button->DownSignal().Connect( testApp, &TestApp::OnButtonPress);
  }
  catch (Dali::DaliException& e)
  {
    // Tests that a negative test of an assertion succeeds
    DALI_TEST_PRINT_ASSERT( e );
    tet_result(TET_PASS);
  }

  END_TEST;
}


int UtcConnectionTrackerSignalDisconnectP(void)
{
  TestApplication app; // Create core for debug logging

  TestButton* button = new TestButton(1);
  TestApp testApp;
  button->DownSignal().Connect(&testApp,&TestApp::OnButtonPress);

  DALI_TEST_CHECK( button->DownSignal().GetConnectionCount( ) == 1 );

  button->DownSignal().Disconnect(&testApp,&TestApp::OnButtonPress);
  DALI_TEST_CHECK( button->DownSignal().GetConnectionCount( ) == 0 );

  END_TEST;

}

int UtcConnectionTrackerGetConnectionCountP(void)
{
  TestApplication app; // Create core for debug logging

  TestButton* button = new TestButton(1);
  TestApp testApp;
  button->DownSignal().Connect(&testApp,&TestApp::OnButtonPress);
  DALI_TEST_CHECK( button->DownSignal().GetConnectionCount( ) == 1 );
  END_TEST;
}

int UtcConnectionTrackerGetConnectionCountN(void)
{
  TestApplication app; // Create core for debug logging

  TestButton* button = new TestButton(1);
  DALI_TEST_CHECK( button->DownSignal().GetConnectionCount( ) == 0 );
  END_TEST;
}

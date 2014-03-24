//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <iostream>

#include <stdlib.h>
#include <dali/dali.h>
#include <dali/integration-api/events/event.h>
#include <dali/integration-api/events/gesture-event.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

void utc_dali_event_processing_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_event_processing_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

struct InvalidEvent : public Integration::Event
{
  InvalidEvent() : Event( Event::Type(-1000) ) {}
  ~InvalidEvent() {}
};

struct InvalidGesture : public Integration::GestureEvent
{
  InvalidGesture() : GestureEvent( Gesture::Type(-1000), Gesture::Clear ) {}
  ~InvalidGesture() {}
};

} // anon namespace

int UtcDaliInvalidEvent(void)
{
  TestApplication application;

  try
  {
    InvalidEvent event;
    application.ProcessEvent( event );
    tet_result( TET_FAIL );
  }
  catch ( Dali::DaliException& e )
  {
    DALI_TEST_ASSERT( e, "false", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliInvalidGesture(void)
{
  TestApplication application;

  try
  {
    InvalidGesture event;
    application.ProcessEvent( event );
    tet_result( TET_FAIL );
  }
  catch ( Dali::DaliException& e )
  {
    DALI_TEST_ASSERT( e, "false", TEST_LOCATION );
  }
  END_TEST;
}

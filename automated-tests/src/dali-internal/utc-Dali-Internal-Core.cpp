/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#include <iostream>

#include <stdlib.h>
#include <dali/public-api/dali-core.h>

#include <dali-test-suite-utils.h>

// Internal headers are allowed here


using namespace Dali;

void utc_dali_internal_core_startup()
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_core_cleanup()
{
  test_return_value = TET_PASS;
}

namespace
{

class RelayoutSignalHandler : public Dali::ConnectionTracker
{
public:

  RelayoutSignalHandler( TestApplication& application )
  : mApplication( application ),
    mSignalCalled( false )
  {
  }

  // callback to be connected to RelayoutSignal
  void RelayoutCallback( Actor actor  )
  {
    tet_infoline("RelayoutCallback is called");

    mSignalCalled = true;

    mApplication.SendNotification();
  }

  TestApplication& mApplication;
  bool   mSignalCalled;
};

} // anonymous namespace

int UtcDaliCoreTopMargin(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Core::SetTopMargin");

  Stage stage = Stage::GetCurrent();

  // Test Stage size without top-margin

  const unsigned int initialWidth(  stage.GetSize().width );
  const unsigned int initialHeight( stage.GetSize().height );

  DALI_TEST_EQUALS( TestApplication::DEFAULT_SURFACE_WIDTH,  initialWidth,  TEST_LOCATION );
  DALI_TEST_EQUALS( TestApplication::DEFAULT_SURFACE_HEIGHT, initialHeight, TEST_LOCATION );

  // Retest with top-margin

  unsigned int margin( 10 );
  application.SetTopMargin( margin );

  const unsigned int newWidth(  stage.GetSize().width );
  const unsigned int newHeight( stage.GetSize().height );

  DALI_TEST_EQUALS( TestApplication::DEFAULT_SURFACE_WIDTH,             newWidth,  TEST_LOCATION );
  DALI_TEST_EQUALS( (TestApplication::DEFAULT_SURFACE_HEIGHT - margin), newHeight, TEST_LOCATION );

  END_TEST;
}

int UtcDaliCoreProcessEvents(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Core::ProcessEvents");

  Vector3 size( 100.0f, 100.0f, 0.0f );
  Vector3 position( 100.0f, 100.0f, 0.0f );

  Actor actor = Actor::New();
  actor.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
  actor.SetSize( size );
  actor.SetPosition( position );
  Stage::GetCurrent().Add( actor );

  RelayoutSignalHandler relayoutSignal( application );
  actor.OnRelayoutSignal().Connect( &relayoutSignal, &RelayoutSignalHandler::RelayoutCallback );

  application.SendNotification();

  DALI_TEST_EQUALS( relayoutSignal.mSignalCalled, true, TEST_LOCATION );

  DALI_TEST_EQUALS( actor.GetProperty( Actor::Property::SIZE ).Get< Vector3 >(), size, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty( Actor::Property::POSITION ).Get< Vector3 >(), position, TEST_LOCATION );

  END_TEST;
}

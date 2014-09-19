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

#include <iostream>

#include <stdlib.h>
#include <dali/public-api/dali-core.h>

#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/hover-event-integ.h>
#include <dali/integration-api/events/mouse-wheel-event-integ.h>
#include <dali/integration-api/events/key-event-integ.h>

#include <dali-test-suite-utils.h>

using namespace Dali;

namespace
{
std::vector< std::string > MasterCallStack;
}

// TypeRegistry needs custom actor Implementations to have the same name (namespaces are ignored so we use one here)
namespace Impl
{

struct TestCustomActor : public CustomActorImpl
{
  /**
   * Constructor
   */
  TestCustomActor()
  : CustomActorImpl( true ), // requires touch
    mDaliProperty( Property::INVALID_INDEX ),
    mSizeSet( Vector3::ZERO ),
    mTargetSize( Vector3::ZERO )
  {
    SetRequiresMouseWheelEvents(true);
  }

  /**
   * Destructor
   */
  virtual ~TestCustomActor()
  {
  }

  void Initialize( const char* name = NULL )
  {
    mDaliProperty = Self().RegisterProperty( "Dali", std::string("no"), Property::READ_WRITE);

    OnInitialize( name );
  }

  virtual void OnInitialize( const char* name ) {}

  /**
   * Resets the call stack
   */
  void ResetCallStack()
  {
    mSizeSet = Vector3();
    mTargetSize = Vector3();
    mMethodsCalled.clear();
  }

  void AddToCallStacks( const char* method )
  {
    mMethodsCalled.push_back( method );

    // Combine Actor name with method string
    std::string nameAndMethod( Self().GetName() );
    if ( 0 == nameAndMethod.size() )
    {
      nameAndMethod = "Unknown: ";
    }
    else
    {
      nameAndMethod += ": ";
    }
    nameAndMethod += method;

    MasterCallStack.push_back( nameAndMethod );
  }

  // From CustomActorImpl
  virtual void OnStageConnection()
  {
    AddToCallStacks("OnStageConnection");
  }
  virtual void OnStageDisconnection()
  {
    AddToCallStacks("OnStageDisconnection");
  }
  virtual void OnChildAdd(Actor& child)
  {
    AddToCallStacks("OnChildAdd");
  }
  virtual void OnChildRemove(Actor& child)
  {
    AddToCallStacks("OnChildRemove");
  }
  virtual void OnPropertySet( Property::Index index, Property::Value propertyValue )
  {
    AddToCallStacks("OnPropertySet");
  }
  virtual void OnSizeSet(const Vector3& targetSize)
  {
    mSizeSet = targetSize;
    AddToCallStacks("OnSizeSet");
  }
  virtual void OnSizeAnimation(Animation& animation, const Vector3& targetSize)
  {
    mTargetSize = targetSize;
    AddToCallStacks("OnSizeAnimation");
  }
  virtual bool OnTouchEvent(const TouchEvent& event)
  {
    AddToCallStacks("OnTouchEvent");
    return true;
  }
  virtual bool OnHoverEvent(const HoverEvent& event)
  {
    AddToCallStacks("OnHoverEvent");
    return true;
  }
  virtual bool OnMouseWheelEvent(const MouseWheelEvent& event)
  {
    AddToCallStacks("OnMouseWheelEvent");
    return true;
  }
  virtual bool OnKeyEvent(const KeyEvent& event)
  {
    AddToCallStacks("OnKeyEvent");
    return true;
  }
  virtual void OnKeyInputFocusGained()
  {
    AddToCallStacks("OnKeyInputFocusGained");
  }
  virtual void OnKeyInputFocusLost()
  {
    AddToCallStacks("OnKeyInputFocusLost");
  }
  virtual Actor GetChildByAlias(const std::string& actorAlias)
  {
    AddToCallStacks("GetChildByAlias");

    if ("found" == actorAlias)
    {
      return Actor::New();
    }
    else
    {
      return Actor();
    }
  }

  virtual Vector3 GetNaturalSize()
  {
    return Vector3( 0.0f, 0.0f, 0.0f );
  }

  void SetDaliProperty(std::string s)
  {
    Self().SetProperty(mDaliProperty, s) ;
  }

  Property::Index mDaliProperty;
  std::vector< std::string > mMethodsCalled;
  Vector3 mSizeSet;
  Vector3 mTargetSize;
};
} // Namespace Impl


namespace
{
/**
 * Test custom actor handle
 */
class TestCustomActor : public CustomActor
{
public:

  static TestCustomActor New()
  {
    Impl::TestCustomActor* impl = new Impl::TestCustomActor;
    TestCustomActor custom( *impl ); // takes ownership

    impl->Initialize();

    return custom;
  }

  virtual ~TestCustomActor()
  {
  }

  Impl::TestCustomActor& GetImpl()
  {
    return static_cast<Impl::TestCustomActor&>(GetImplementation());
  }

  std::vector< std::string >& GetMethodsCalled()
  {
    return GetImpl().mMethodsCalled;
  }

  void ResetCallStack()
  {
    GetImpl().ResetCallStack();
  }

  void SetDaliProperty(std::string s)
  {
    GetImpl().SetDaliProperty(s);
  }

  Vector3 GetSize()
  {
    return GetImpl().mSizeSet;
  }

  Vector3 GetTargetSize()
  {
    return GetImpl().mTargetSize;
  }

private:

  TestCustomActor( Impl::TestCustomActor& impl ) : CustomActor( impl )
  {
  }
};



using namespace Dali;

BaseHandle CreateActor()
{
  return TestCustomActor::New();
}

Dali::TypeRegistration mType( typeid(TestCustomActor), typeid(Dali::CustomActor), CreateActor );

}


int UtcDaliCustomActorDoAction(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::DoAction()");

  TestCustomActor custom = TestCustomActor::New();

  BaseHandle customActorObject = custom;

  DALI_TEST_CHECK(customActorObject);

  std::vector<Property::Value> attributes;

  // Check that an invalid command is not performed
  DALI_TEST_CHECK(customActorObject.DoAction("invalidCommand", attributes) == false);

  // Check that the custom actor is visible
  custom.SetVisible(true);
  DALI_TEST_CHECK(custom.IsVisible() == true);

  // Check the custom actor performed an action to hide itself
  DALI_TEST_CHECK(customActorObject.DoAction("hide", attributes) == true);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check that the custom actor is now invisible
  DALI_TEST_CHECK(custom.IsVisible() == false);

  // Check the custom actor performed an action to show itself
  DALI_TEST_CHECK(customActorObject.DoAction("show", attributes) == true);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check that the custom actor is now visible
  DALI_TEST_CHECK(custom.IsVisible() == true);
  END_TEST;
}

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
#include <tet_api.h>

#include <dali/public-api/dali-core.h>

#include <dali-test-suite-utils.h>

using namespace Dali;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

TEST_FUNCTION( UtcDaliTypeRegistryCreateDaliObjects, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTypeRegistryActionViaBaseHandle, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTypeRegistryNames, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTypeRegistryNameEquivalence, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTypeRegistryCustomActor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTypeRegistryCustomSignalFailure, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTypeRegistryInitFunctions, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTypeRegistryNameInitFunctions, POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

// Positive test case for a method
static void UtcDaliTypeRegistryCreateDaliObjects()
{
  TestApplication application;

  TypeRegistry registry; // like this for ctor test coverage
  registry = TypeRegistry::Get();

  TypeInfo type;

  type = registry.GetTypeInfo( "ImageActor" );
  DALI_TEST_CHECK( type );
  DALI_TEST_CHECK( type.GetCreator() );
  DALI_TEST_CHECK( ImageActor::DownCast( type.GetCreator()() ) );
  ImageActor ia = ImageActor::DownCast(type.CreateInstance());
  DALI_TEST_CHECK( ia );
  Stage::GetCurrent().Add( ia );
  application.Render();

  type = registry.GetTypeInfo( "TextActor" );
  DALI_TEST_CHECK( type );
  TextActor ta = TextActor::DownCast(type.CreateInstance());
  DALI_TEST_CHECK( ta );
  Stage::GetCurrent().Add( ta );
  application.Render();

  type = registry.GetTypeInfo( "CameraActor" );
  DALI_TEST_CHECK( type );
  CameraActor ca = CameraActor::DownCast(type.CreateInstance());
  DALI_TEST_CHECK( ca );
  Stage::GetCurrent().Add( ca );
  application.Render();

  type = registry.GetTypeInfo( "LightActor" );
  DALI_TEST_CHECK( type );
  LightActor la = LightActor::DownCast(type.CreateInstance());
  DALI_TEST_CHECK( la );
  Stage::GetCurrent().Add( la );
  application.Render();

  // animations
  type = registry.GetTypeInfo( "Animation" );
  DALI_TEST_CHECK( type );
  Animation an = Animation::DownCast(type.CreateInstance());
  DALI_TEST_CHECK( an );
  an.Play();
  application.Render();

  //
  type = registry.GetTypeInfo( "ShaderEffect" );
  DALI_TEST_CHECK( type );
  ShaderEffect ef = ShaderEffect::DownCast(type.CreateInstance());
  DALI_TEST_CHECK( ef );
  application.Render();

}

/*******************************************************************************
 *
 * Action through the base handle
 *
 ******************************************************************************/
static void UtcDaliTypeRegistryActionViaBaseHandle()
{
  TestApplication application;

  TypeInfo type;

  type = TypeRegistry::Get().GetTypeInfo( "Actor" );
  DALI_TEST_CHECK( type );

  BaseHandle hdl = type.CreateInstance();
  DALI_TEST_CHECK( hdl );

  Actor a = Actor::DownCast(hdl);
  DALI_TEST_CHECK( a );

  a.SetVisible(false);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK(!a.IsVisible());

  std::vector<Property::Value> attributes;

  DALI_TEST_CHECK(hdl.DoAction(Actor::ACTION_SHOW, attributes));

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK(a.IsVisible());

  DALI_TEST_CHECK(!hdl.DoAction("unknown-action", attributes));
}

static void UtcDaliTypeRegistryNames()
{
  TestApplication application;

  TypeInfo type;

  TypeRegistry::NameContainer names = TypeRegistry::Get().GetTypeNames();

  for(TypeRegistry::NameContainer::iterator iter = names.begin();
      iter != names.end(); ++iter)
  {
    type = TypeRegistry::Get().GetTypeInfo( *iter );
    DALI_TEST_CHECK( type );
  }

}

// Check named and typeid are equivalent
static void UtcDaliTypeRegistryNameEquivalence()
{
  TypeInfo named_type = TypeRegistry::Get().GetTypeInfo( "TextActor" );
  TypeInfo typeinfo_type = TypeRegistry::Get().GetTypeInfo( typeid(Dali::TextActor) );

  DALI_TEST_CHECK( named_type );
  DALI_TEST_CHECK( typeinfo_type );

  DALI_TEST_CHECK( named_type == typeinfo_type );

  DALI_TEST_CHECK( named_type.GetName() == typeinfo_type.GetName() );
  DALI_TEST_CHECK( named_type.GetBaseName() == typeinfo_type.GetBaseName() );

}

/*******************************************************************************
 *
 * Custom Actor
 *
 ******************************************************************************/
namespace Impl
{
struct MyTestCustomActor : public CustomActorImpl
{
  typedef SignalV2< void ()> SignalType;
  typedef SignalV2< void (float)> SignalTypeFloat;

  MyTestCustomActor() : CustomActorImpl( true ) // requires touch
  { }

  virtual ~MyTestCustomActor()
  { }

  void ResetCallStack()
  {
  }

  // From CustomActorImpl
  virtual void OnStageConnection()
  {
  }
  virtual void OnStageDisconnection()
  {
  }
  virtual void OnChildAdd(Actor& child)
  {
  }
  virtual void OnChildRemove(Actor& child)
  {
  }
  virtual void OnSizeSet(const Vector3& targetSize)
  {
  }
  virtual void OnSizeAnimation(Animation& animation, const Vector3& targetSize)
  {
  }
  virtual bool OnTouchEvent(const TouchEvent& event)
  {
    return true;
  }
  virtual bool OnMouseWheelEvent(const MouseWheelEvent& event)
  {
    return true;
  }
  virtual bool OnKeyEvent(const KeyEvent& event)
  {
    return true;
  }
  virtual void OnKeyInputFocusGained()
  {
  }
  virtual void OnKeyInputFocusLost()
  {
  }
  virtual Actor GetChildByAlias(const std::string& actorAlias)
  {
    return Actor::New();
  }

public:

  SignalType mSignal;
};

}; // namespace Impl

class MyTestCustomActor : public CustomActor
{
public:

  typedef SignalV2< void ()> SignalType;
  typedef SignalV2< void (float)> SignalTypeFloat;

  MyTestCustomActor()
  {
  }

  static MyTestCustomActor New()
  {
    Impl::MyTestCustomActor* p = new Impl::MyTestCustomActor;
    return MyTestCustomActor( *p ); // takes ownership
  }

  virtual ~MyTestCustomActor()
  {
  }

  static MyTestCustomActor DownCast( BaseHandle handle )
  {
    MyTestCustomActor result;

    CustomActor custom = Dali::CustomActor::DownCast( handle );
    if ( custom )
    {
      CustomActorImpl& customImpl = custom.GetImplementation();

      Impl::MyTestCustomActor* impl = dynamic_cast<Impl::MyTestCustomActor*>(&customImpl);

      if (impl)
      {
        result = MyTestCustomActor(customImpl.GetOwner());
      }
    }

    return result;
  }

  SignalType& GetCustomSignal()
  {
    Dali::RefObject& obj = GetImplementation();
    return static_cast<Impl::MyTestCustomActor&>( obj ).mSignal;
  }

private:

  MyTestCustomActor(Internal::CustomActor* internal)
  : CustomActor(internal)
  {
  }

  MyTestCustomActor( Impl::MyTestCustomActor& impl )
  : CustomActor( impl )
  {
  }
};

BaseHandle CreateCustom(void)
{
  return MyTestCustomActor::New();
}

static std::string lastSignalConnectionCustom;

bool DoConnectSignalCustom( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  lastSignalConnectionCustom = signalName;

  bool connected( true );

  Dali::BaseHandle handle(object);
  MyTestCustomActor customActor = MyTestCustomActor::DownCast(handle);

  if( "sig1" == signalName )
  {
    customActor.GetCustomSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

bool DoConnectSignalCustomFailure( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  lastSignalConnectionCustom = "failed";

  return false; // This is supposed to fail
}

struct CustomTestFunctor
{
  CustomTestFunctor()
  {
    ++mTotalInstanceCount;
    ++mCurrentInstanceCount;
  }

  CustomTestFunctor( const CustomTestFunctor& copyMe )
  {
    ++mTotalInstanceCount;
    ++mCurrentInstanceCount;
  }

  ~CustomTestFunctor()
  {
    --mCurrentInstanceCount;
  }

  void operator()()
  {
    ++mCallbackCount;
  }

  static int mTotalInstanceCount;
  static int mCurrentInstanceCount;
  static int mCallbackCount;
};

int CustomTestFunctor::mTotalInstanceCount = 0;
int CustomTestFunctor::mCurrentInstanceCount = 0;
int CustomTestFunctor::mCallbackCount = 0;

static void ResetFunctorCounts()
{
  CustomTestFunctor::mTotalInstanceCount   = 0;
  CustomTestFunctor::mCurrentInstanceCount = 0;
  CustomTestFunctor::mCallbackCount        = 0;
}

static std::string lastActionCustom;
bool DoActionCustom(BaseObject* object, const std::string& actionName, const std::vector<Property::Value>& attributes)
{
  lastActionCustom = actionName;
  return true;
}

// Custom type registration
static TypeRegistration customType1( typeid(MyTestCustomActor), typeid(Dali::CustomActor), CreateCustom );

// Custom signals
static SignalConnectorType customSignalConnector1( customType1, "sig1", DoConnectSignalCustom );
static SignalConnectorType customSignalConnector2( customType1, "sig2", DoConnectSignalCustomFailure );
static const int TEST_SIGNAL_COUNT = 2;

// Custom actions
static TypeAction customAction1( customType1, "act1", DoActionCustom);
static const int TEST_ACTION_COUNT = 1;

class TestConnectionTracker : public ConnectionTracker
{
public:

  TestConnectionTracker()
  {
  }
};

static void UtcDaliTypeRegistryCustomActor()
{
  ResetFunctorCounts();

  TestApplication application;

  TypeInfo type = TypeRegistry::Get().GetTypeInfo( "MyTestCustomActor" );
  DALI_TEST_CHECK( type );

  TypeInfo baseType = TypeRegistry::Get().GetTypeInfo( "CustomActor" );
  DALI_TEST_CHECK( baseType );

  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );

  MyTestCustomActor customHandle = MyTestCustomActor::DownCast( handle );
  DALI_TEST_CHECK( customHandle );

  DALI_TEST_EQUALS( type.GetActions().size(), TEST_ACTION_COUNT + baseType.GetActions().size(), TEST_LOCATION );
  DALI_TEST_EQUALS( type.GetSignals().size(), TEST_SIGNAL_COUNT + baseType.GetSignals().size(), TEST_LOCATION );

  {
    TestConnectionTracker tracker;

    bool connected = handle.ConnectSignal( &tracker, "sig1", CustomTestFunctor() );
    DALI_TEST_EQUALS( connected, true, TEST_LOCATION );
    DALI_TEST_CHECK( lastSignalConnectionCustom == "sig1" );
    DALI_TEST_EQUALS( CustomTestFunctor::mTotalInstanceCount, 2/*temporary copy + FunctorDelegate copy*/, TEST_LOCATION );
    DALI_TEST_EQUALS( CustomTestFunctor::mCurrentInstanceCount, 1, TEST_LOCATION );

    DALI_TEST_EQUALS( CustomTestFunctor::mCallbackCount, 0, TEST_LOCATION );
    customHandle.GetCustomSignal().Emit();
    DALI_TEST_EQUALS( CustomTestFunctor::mCallbackCount, 1, TEST_LOCATION );
    DALI_TEST_EQUALS( CustomTestFunctor::mTotalInstanceCount, 2/*temporary copy + FunctorDelegate copy*/, TEST_LOCATION );
    DALI_TEST_EQUALS( CustomTestFunctor::mCurrentInstanceCount, 1, TEST_LOCATION );
  }
  // tracker should automatically disconnect here
  DALI_TEST_EQUALS( CustomTestFunctor::mTotalInstanceCount, 2/*temporary copy + FunctorDelegate copy*/, TEST_LOCATION );
  DALI_TEST_EQUALS( CustomTestFunctor::mCurrentInstanceCount, 0, TEST_LOCATION );

  // Test that functor is disconnected
  DALI_TEST_EQUALS( CustomTestFunctor::mCallbackCount, 1, TEST_LOCATION );
  customHandle.GetCustomSignal().Emit();
  DALI_TEST_EQUALS( CustomTestFunctor::mCallbackCount, 1/*not incremented*/, TEST_LOCATION );
  DALI_TEST_EQUALS( CustomTestFunctor::mTotalInstanceCount, 2/*temporary copy + FunctorDelegate copy*/, TEST_LOCATION );
  DALI_TEST_EQUALS( CustomTestFunctor::mCurrentInstanceCount, 0, TEST_LOCATION );

  std::vector<Property::Value> attributes;
  handle.DoAction("act1", attributes);
  DALI_TEST_CHECK( lastActionCustom == "act1" );
}

static void UtcDaliTypeRegistryCustomSignalFailure()
{
  // Test what happens when signal connnector (DoConnectSignalFailure method) returns false

  ResetFunctorCounts();

  TestApplication application;

  TypeInfo type = TypeRegistry::Get().GetTypeInfo( "MyTestCustomActor" );
  DALI_TEST_CHECK( type );

  TypeInfo baseType = TypeRegistry::Get().GetTypeInfo( "CustomActor" );
  DALI_TEST_CHECK( baseType );

  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );

  MyTestCustomActor customHandle = MyTestCustomActor::DownCast( handle );
  DALI_TEST_CHECK( customHandle );

  DALI_TEST_EQUALS( type.GetActions().size(), TEST_ACTION_COUNT + baseType.GetActions().size(), TEST_LOCATION );
  DALI_TEST_EQUALS( type.GetSignals().size(), TEST_SIGNAL_COUNT + baseType.GetSignals().size(), TEST_LOCATION );

  {
    TestConnectionTracker tracker;

    bool connected = handle.ConnectSignal( &tracker, "sig2", CustomTestFunctor() );
    DALI_TEST_EQUALS( connected, false/*This is supposed to fail*/, TEST_LOCATION );
    DALI_TEST_CHECK( lastSignalConnectionCustom == "failed" );
    DALI_TEST_EQUALS( CustomTestFunctor::mTotalInstanceCount, 2/*temporary copy + FunctorDelegate copy*/, TEST_LOCATION );
    DALI_TEST_EQUALS( CustomTestFunctor::mCurrentInstanceCount, 0/*deleted along with FunctorDelegate*/, TEST_LOCATION );

    // Should be a NOOP
    DALI_TEST_EQUALS( CustomTestFunctor::mCallbackCount, 0, TEST_LOCATION );
    customHandle.GetCustomSignal().Emit();
    DALI_TEST_EQUALS( CustomTestFunctor::mCallbackCount, 0/*never called*/, TEST_LOCATION );
  }
  // tracker should have nothing to disconnect here

  // Should be a NOOP
  DALI_TEST_EQUALS( CustomTestFunctor::mCallbackCount, 0, TEST_LOCATION );
  customHandle.GetCustomSignal().Emit();
  DALI_TEST_EQUALS( CustomTestFunctor::mCallbackCount, 0/*never called*/, TEST_LOCATION );
}

//
// Create function as Init function called
//
static bool CreateCustomInitCalled = false;
BaseHandle CreateCustomInit(void)
{
  CreateCustomInitCalled = true;
  return BaseHandle();
}

class MyTestCustomActor2 : public CustomActor
{
public:

  MyTestCustomActor2()
  {
  }

  static MyTestCustomActor2 New()
  {
    return MyTestCustomActor2(); // takes ownership
  }

  virtual ~MyTestCustomActor2()
  {
  }

  static MyTestCustomActor2 DownCast( BaseHandle handle )
  {
    MyTestCustomActor2 result;

    CustomActor custom = Dali::CustomActor::DownCast( handle );
    if ( custom )
    {
      CustomActorImpl& customImpl = custom.GetImplementation();

      Impl::MyTestCustomActor* impl = dynamic_cast<Impl::MyTestCustomActor*>(&customImpl);

      if (impl)
      {
        result = MyTestCustomActor2(customImpl.GetOwner());
      }
    }

    return result;
  }

private:

  MyTestCustomActor2(Internal::CustomActor* internal)
  : CustomActor(internal)
  {
  }

  MyTestCustomActor2( Impl::MyTestCustomActor& impl )
  : CustomActor( impl )
  {
  }
};


static TypeRegistration customTypeInit( typeid(MyTestCustomActor2), typeid(Dali::CustomActor), CreateCustomInit, true );

static void UtcDaliTypeRegistryInitFunctions()
{
  TestApplication application;

  DALI_TEST_CHECK( "MyTestCustomActor2" == customTypeInit.RegisteredName() );

  DALI_TEST_CHECK( true == CreateCustomInitCalled );
  TypeInfo type = TypeRegistry::Get().GetTypeInfo( "MyTestCustomActor2" );
  DALI_TEST_CHECK( type );
}


static bool CreateCustomNamedInitCalled = false;
BaseHandle CreateCustomNamedInit(void)
{
  CreateCustomNamedInitCalled = true;
  return BaseHandle();
}

const std::string scriptedName("PopupStyle");
static TypeRegistration scriptedType( scriptedName, typeid(Dali::CustomActor), CreateCustomNamedInit );

static void UtcDaliTypeRegistryNameInitFunctions()
{
  TestApplication application;

  DALI_TEST_CHECK( scriptedName == scriptedType.RegisteredName() );

  TypeInfo baseType = TypeRegistry::Get().GetTypeInfo( scriptedName );
  DALI_TEST_CHECK( baseType );

  BaseHandle handle = baseType.CreateInstance();

  DALI_TEST_CHECK( true == CreateCustomNamedInitCalled );
  TypeInfo type = TypeRegistry::Get().GetTypeInfo( scriptedName );
  DALI_TEST_CHECK( type );
}

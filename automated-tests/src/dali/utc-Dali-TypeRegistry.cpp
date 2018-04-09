/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <limits>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>
#include <dali/internal/event/common/type-info-impl.h>
#include <dali/integration-api/events/long-press-gesture-event.h>
#include <dali/integration-api/events/pan-gesture-event.h>
#include <dali/integration-api/events/pinch-gesture-event.h>
#include <dali/integration-api/events/tap-gesture-event.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/hover-event-integ.h>

using namespace Dali;


namespace
{

// Stores data that is populated in the callback and will be read by the Test cases
struct SignalData
{
  SignalData()
  : functorCalled( false ),
    voidFunctorCalled( false ),
    receivedGesture( Gesture::Clear ),
    pressedActor()
  {}

  void Reset()
  {
    functorCalled = false;
    voidFunctorCalled = false;

    receivedGesture.numberOfTouches = 0u;
    receivedGesture.screenPoint = Vector2(0.0f, 0.0f);
    receivedGesture.localPoint = Vector2(0.0f, 0.0f);

    pressedActor.Reset();
  }

  bool functorCalled;
  bool voidFunctorCalled;
  LongPressGesture receivedGesture;
  Actor pressedActor;
};

// Functor that sets the data when called
struct GestureReceivedFunctor
{
  GestureReceivedFunctor(SignalData& data) : signalData(data) { }

  void operator()(Actor actor, LongPressGesture longPress)
  {
    signalData.functorCalled = true;
    signalData.receivedGesture = longPress;
    signalData.pressedActor = actor;
  }

  void operator()()
  {
    signalData.voidFunctorCalled = true;
  }

  SignalData& signalData;
};

// Generate a LongPressGestureEvent to send to Core
Integration::LongPressGestureEvent GenerateLongPress(
    Gesture::State state,
    unsigned int numberOfTouches,
    Vector2 point)
{
  Integration::LongPressGestureEvent longPress( state );

  longPress.numberOfTouches = numberOfTouches;
  longPress.point = point;

  return longPress;
}

// Generate a PanGestureEvent to send to Core
Integration::PanGestureEvent GeneratePan(
    Gesture::State state,
    Vector2 previousPosition,
    Vector2 currentPosition,
    unsigned long timeDelta,
    unsigned int numberOfTouches = 1,
    unsigned int time = 1u)
{
  Integration::PanGestureEvent pan(state);

  pan.previousPosition = previousPosition;
  pan.currentPosition = currentPosition;
  pan.timeDelta = timeDelta;
  pan.numberOfTouches = numberOfTouches;
  pan.time = time;

  return pan;
}
// Generate a PinchGestureEvent to send to Core
Integration::PinchGestureEvent GeneratePinch(
    Gesture::State state,
    float scale,
    float speed,
    Vector2 centerpoint)
{
  Integration::PinchGestureEvent pinch(state);

  pinch.scale = scale;
  pinch.speed = speed;
  pinch.centerPoint = centerpoint;

  return pinch;
}
// Generate a TapGestureEvent to send to Core
Integration::TapGestureEvent GenerateTap(
    Gesture::State state,
    unsigned int numberOfTaps,
    unsigned int numberOfTouches,
    Vector2 point)
{
  Integration::TapGestureEvent tap( state );

  tap.numberOfTaps = numberOfTaps;
  tap.numberOfTouches = numberOfTouches;
  tap.point = point;

  return tap;
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

static bool CreateCustomNamedInitCalled = false;
BaseHandle CreateCustomNamedInit(void)
{
  CreateCustomNamedInitCalled = true;
  return BaseHandle();
}

const std::string scriptedName("PopupStyle");
static TypeRegistration scriptedType( scriptedName, typeid(Dali::CustomActor), CreateCustomNamedInit );

// Property Registration
bool setPropertyCalled = false;
bool getPropertyCalled = false;
void SetProperty( BaseObject* object, Property::Index propertyIndex, const Property::Value& value )
{
  setPropertyCalled = true;
}
Property::Value GetProperty( BaseObject* object, Property::Index propertyIndex )
{
  getPropertyCalled = true;
  return Property::Value( true );
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
  typedef Signal< void ()> SignalType;
  typedef Signal< void (float)> SignalTypeFloat;

  MyTestCustomActor() : CustomActorImpl( ActorFlags( REQUIRES_TOUCH_EVENTS ) )
  { }

  virtual ~MyTestCustomActor()
  { }

  void ResetCallStack()
  {
  }

  // From CustomActorImpl
  virtual void OnStageConnection( int depth )
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
  virtual bool OnHoverEvent(const HoverEvent& event)
  {
    return true;
  }
  virtual bool OnWheelEvent(const WheelEvent& event)
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
  virtual Vector3 GetNaturalSize()
  {
    return Vector3( 0.0f, 0.0f, 0.0f );
  }

  virtual float GetHeightForWidth( float width )
  {
    return 0.0f;
  }

  virtual float GetWidthForHeight( float height )
  {
    return 0.0f;
  }

  virtual void OnRelayout( const Vector2& size, RelayoutContainer& container )
  {
  }

  virtual void OnSetResizePolicy( ResizePolicy::Type policy, Dimension::Type dimension )
  {
  }

  virtual void OnCalculateRelayoutSize( Dimension::Type dimension )
  {
  }

  virtual float CalculateChildSize( const Dali::Actor& child, Dimension::Type dimension )
  {
    return 0.0f;
  }

  virtual void OnLayoutNegotiated( float size, Dimension::Type dimension )
  {
  }

  virtual bool RelayoutDependentOnChildren( Dimension::Type dimension = Dimension::ALL_DIMENSIONS )
  {
    return false;
  }

public:

  SignalType mSignal;
};

}; // namespace Impl

class MyTestCustomActor : public CustomActor
{
public:

  typedef Signal< void ()> SignalType;
  typedef Signal< void (float)> SignalTypeFloat;

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


class MyTestCustomActor2 : public CustomActor
{
public:

  struct Property
  {
    enum
    {
      P1=Dali::PROPERTY_REGISTRATION_START_INDEX,
      P2
    };
  };


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

PropertyRegistration P1( customTypeInit, "propertyOne", MyTestCustomActor2::Property::P1, Property::INTEGER, &SetProperty, &GetProperty );
PropertyRegistration P2( customTypeInit, "propertyTwo", MyTestCustomActor2::Property::P2, Property::STRING, &SetProperty, &GetProperty );


class MyTestCustomActor3 : public CustomActor
{
public:
  MyTestCustomActor3()
  {
  }

  static MyTestCustomActor3 New()
  {
    return MyTestCustomActor3(); // takes ownership
  }

  virtual ~MyTestCustomActor3()
  {
  }

  static MyTestCustomActor3 DownCast( BaseHandle handle )
  {
    MyTestCustomActor3 result;

    CustomActor custom = Dali::CustomActor::DownCast( handle );
    if ( custom )
    {
      CustomActorImpl& customImpl = custom.GetImplementation();

      Impl::MyTestCustomActor* impl = dynamic_cast<Impl::MyTestCustomActor*>(&customImpl);

      if (impl)
      {
        result = MyTestCustomActor3(customImpl.GetOwner());
      }
    }

    return result;
  }

private:

  MyTestCustomActor3(Internal::CustomActor* internal)
  : CustomActor(internal)
  {
  }

  MyTestCustomActor3( Impl::MyTestCustomActor& impl )
  : CustomActor( impl )
  {
  }
};

static TypeRegistration customTypeBadInit( typeid(MyTestCustomActor3), typeid(Dali::CustomActor), NULL, false );

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
bool DoActionCustom(BaseObject* object, const std::string& actionName, const Property::Map& /*attributes*/)
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

BaseHandle CreateNamedActorType()
{
  Actor actor = Actor::New();
  actor.SetName( "NamedActor" );
  return actor;
}

TypeRegistration namedActorType( "MyNamedActor", typeid(Dali::Actor), CreateNamedActorType );
PropertyRegistration namedActorPropertyOne( namedActorType, "propName",  PROPERTY_REGISTRATION_START_INDEX, Property::BOOLEAN, &SetProperty, &GetProperty );

} // Anonymous namespace

// Note: No negative test case for UtcDaliTypeRegistryGet can be implemented.
int UtcDaliTypeRegistryGetP(void)
{
  TestApplication application;

  TypeRegistry registry = TypeRegistry::Get();
  DALI_TEST_CHECK( registry );

  END_TEST;
}

// Note: No negative test case for UtcDaliTypeRegistryConstructor can be implemented.
int UtcDaliTypeRegistryConstructorP(void)
{
  TestApplication application;

  TypeRegistry registry;
  DALI_TEST_CHECK( !registry );
  END_TEST;
}

// Note: No negative test case for UtcDaliTypeRegistryCopyConstructor can be implemented.
int UtcDaliTypeRegistryCopyConstructorP(void)
{
  TestApplication application;

  TypeRegistry registry = TypeRegistry::Get();
  DALI_TEST_CHECK( registry );

  TypeRegistry copy( registry );
  DALI_TEST_CHECK( copy );

  DALI_TEST_CHECK( registry.GetTypeInfo( "Actor" ).GetName() == copy.GetTypeInfo( "Actor" ).GetName() );

  END_TEST;
}

// Note: No negative test case for UtcDaliTypeRegistryAssignmentOperator can be implemented.
int UtcDaliTypeRegistryAssignmentOperatorP(void)
{
  TestApplication application;

  TypeRegistry registry = TypeRegistry::Get();
  DALI_TEST_CHECK( registry );

  TypeRegistry copy = registry;
  DALI_TEST_CHECK( copy );
  DALI_TEST_CHECK( registry == copy );

  DALI_TEST_CHECK( registry.GetTypeInfo( "Actor" ).GetName() == copy.GetTypeInfo( "Actor" ).GetName() );

  END_TEST;
}

int UtcDaliTypeRegistryAssignP(void)
{
  TestApplication application;

  TypeRegistry registry = TypeRegistry::Get();
  TypeRegistry registry2;
  registry2 = registry;
  DALI_TEST_CHECK( registry2 );

  DALI_TEST_CHECK( registry2.GetTypeInfo( "Actor" ).GetName() == registry2.GetTypeInfo( "Actor" ).GetName() );

  END_TEST;
}

int UtcDaliTypeRegistryGetTypeInfoFromTypeNameP(void)
{
  TestApplication application;

  TypeRegistry registry = TypeRegistry::Get();

  TypeInfo type;

  // camera actor
  type = registry.GetTypeInfo( "CameraActor" );
  DALI_TEST_CHECK( type );
  CameraActor ca = CameraActor::DownCast(type.CreateInstance());
  DALI_TEST_CHECK( ca );
  Stage::GetCurrent().Add( ca );
  application.Render();

  // animations
  type = registry.GetTypeInfo( "Animation" );
  DALI_TEST_CHECK( type );
  Animation an = Animation::DownCast(type.CreateInstance());
  DALI_TEST_CHECK( an );
  an.Play();
  application.Render();

  END_TEST;
}

int UtcDaliTypeRegistryGetTypeInfoFromTypeNameN(void)
{
  TestApplication application;

  TypeRegistry registry = TypeRegistry::Get();

  TypeInfo type;

  type = registry.GetTypeInfo( "MyDummyActor" );
  DALI_TEST_CHECK( !type );

  END_TEST;
}

int UtcDaliTypeRegistryGetTypeInfoFromTypeIdP(void)
{
  TypeInfo named_type = TypeRegistry::Get().GetTypeInfo( "CameraActor" );
  TypeInfo typeinfo_type = TypeRegistry::Get().GetTypeInfo( typeid(Dali::CameraActor) );

  DALI_TEST_CHECK( named_type );
  DALI_TEST_CHECK( typeinfo_type );

  // Check named and typeid are equivalent
  DALI_TEST_CHECK( named_type == typeinfo_type );

  DALI_TEST_CHECK( named_type.GetName() == typeinfo_type.GetName() );
  DALI_TEST_CHECK( named_type.GetBaseName() == typeinfo_type.GetBaseName() );

  END_TEST;
}

int UtcDaliTypeRegistryGetTypeInfoFromTypeIdN(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  TypeInfo typeInfo = typeRegistry.GetTypeInfo( typeid(Vector2) );
  DALI_TEST_CHECK( !typeInfo );

  END_TEST;
}

int UtcDaliTypeRegistryGetTypeNameCountP(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();
  TypeInfo type;

  for(size_t i = 0; i < typeRegistry.GetTypeNameCount(); i++)
  {
    type = typeRegistry.GetTypeInfo( typeRegistry.GetTypeName(i) );
    DALI_TEST_CHECK( type );
  }

  END_TEST;
}


int UtcDaliTypeRegistryGetTypeNamesP(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();
  TypeInfo type;

  for(size_t i = 0; i < typeRegistry.GetTypeNameCount(); i++)
  {
    type = typeRegistry.GetTypeInfo( typeRegistry.GetTypeName(i) );
    DALI_TEST_CHECK( type );
  }

  END_TEST;
}


// Note: No negative test case for UtcDaliTypeRegistryTypeRegistration can be implemented.
int UtcDaliTypeRegistryTypeRegistrationNotCallingCreateOnInitP(void)
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

  DALI_TEST_EQUALS( type.GetActionCount(), TEST_ACTION_COUNT + baseType.GetActionCount(), TEST_LOCATION );

  DALI_TEST_EQUALS( type.GetSignalCount(), TEST_SIGNAL_COUNT + baseType.GetSignalCount(), TEST_LOCATION );

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

  Property::Map attributes;
  handle.DoAction("act1", attributes);
  DALI_TEST_CHECK( lastActionCustom == "act1" );
  END_TEST;
}

// Note: No negative test case for UtcDaliTypeRegistryTypeRegistration can be implemented.
int UtcDaliTypeRegistryTypeRegistrationCallingCreateOnInitP(void)
{
  TestApplication application;

  DALI_TEST_CHECK( "MyTestCustomActor2" == customTypeInit.RegisteredName() );

  DALI_TEST_CHECK( true == CreateCustomInitCalled );
  TypeInfo type = TypeRegistry::Get().GetTypeInfo( "MyTestCustomActor2" );
  DALI_TEST_CHECK( type );
  END_TEST;
}

// Note: No negative test case for UtcDaliTypeRegistryTypeRegistration can be implemented.
int UtcDaliTypeRegistryTypeRegistrationForNamedTypeP(void)
{
  TestApplication application;

  // Create Named Actor Type
  TypeInfo type = TypeRegistry::Get().GetTypeInfo( "MyNamedActor" );
  DALI_TEST_CHECK( type );

  BaseHandle namedHandle = type.CreateInstance();
  DALI_TEST_CHECK( namedHandle );
  Actor namedActor( Actor::DownCast( namedHandle ) );
  DALI_TEST_CHECK( namedActor );

  DALI_TEST_CHECK( namedActor.GetName() == "NamedActor" );
  DALI_TEST_CHECK( type.GetName() == "MyNamedActor" );
  DALI_TEST_CHECK( type.GetBaseName() == "Actor" );

  END_TEST;
}

int UtcDaliTypeRegistryRegisteredNameP(void)
{
  TestApplication application;

  DALI_TEST_CHECK( scriptedName == scriptedType.RegisteredName() );

  TypeInfo baseType = TypeRegistry::Get().GetTypeInfo( scriptedName );
  DALI_TEST_CHECK( baseType );

  BaseHandle handle = baseType.CreateInstance();

  DALI_TEST_CHECK( true == CreateCustomNamedInitCalled );
  TypeInfo type = TypeRegistry::Get().GetTypeInfo( scriptedName );
  DALI_TEST_CHECK( type );
  END_TEST;
}


int UtcDaliTypeRegistryRegisteredNameN(void)
{
  TestApplication application;

  DALI_TEST_CHECK( scriptedName == scriptedType.RegisteredName() );

  TypeInfo baseType = TypeRegistry::Get().GetTypeInfo( scriptedName );
  DALI_TEST_CHECK( baseType );

  // should cause an assert because we're registering same type twice
  // once statically at the start of this file, then again now
  try
  {
    TypeRegistration scriptedType( scriptedName, typeid(Dali::CustomActor), CreateCustomNamedInit );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "Duplicate type name in Type Registration", TEST_LOCATION );
  }

  END_TEST;
}


int UtcDaliTypeRegistrySignalConnectorTypeP(void)
{
  ResetFunctorCounts();

  TestApplication application;

  TypeInfo type = TypeRegistry::Get().GetTypeInfo( "MyTestCustomActor" );
  DALI_TEST_CHECK( type );

  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );

  MyTestCustomActor customHandle = MyTestCustomActor::DownCast( handle );
  DALI_TEST_CHECK( customHandle );

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

  END_TEST;
}

int UtcDaliTypeRegistrySignalConnectorTypeN(void)
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

  DALI_TEST_EQUALS( type.GetActionCount(), TEST_ACTION_COUNT + baseType.GetActionCount(), TEST_LOCATION );

  DALI_TEST_EQUALS( type.GetSignalCount(), TEST_SIGNAL_COUNT + baseType.GetSignalCount(), TEST_LOCATION );

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
  END_TEST;
}

int UtcDaliTypeRegistryTypeActionP(void)
{
  ResetFunctorCounts();

  TestApplication application;

  TypeInfo type = TypeRegistry::Get().GetTypeInfo( "MyTestCustomActor" );
  DALI_TEST_CHECK( type );

  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );

  Property::Map attributes;
  DALI_TEST_CHECK( handle.DoAction("act1", attributes) );
  DALI_TEST_CHECK( lastActionCustom == "act1" );

  END_TEST;
}

int UtcDaliTypeRegistryTypeActionN(void)
{
  ResetFunctorCounts();

  TestApplication application;

  TypeInfo type = TypeRegistry::Get().GetTypeInfo( "MyTestCustomActor" );
  DALI_TEST_CHECK( type );

  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );

  Property::Map attributes;
  DALI_TEST_CHECK( !handle.DoAction( "unknownAction",  attributes ) );

  END_TEST;
}

int UtcDaliTypeRegistryPropertyRegistrationP(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  // Check property count before property registration
  TypeInfo typeInfo = typeRegistry.GetTypeInfo( typeid(MyTestCustomActor) );
  DALI_TEST_CHECK( typeInfo );
  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( handle );
  Actor customActor = Actor::DownCast( handle );
  DALI_TEST_CHECK( customActor );
  unsigned int initialPropertyCount( customActor.GetPropertyCount() );

  std::string propertyName( "prop1" );
  int propertyIndex( PROPERTY_REGISTRATION_START_INDEX );
  Property::Type propertyType( Property::BOOLEAN );
  PropertyRegistration property1( customType1, propertyName, propertyIndex, propertyType, &SetProperty, &GetProperty );

  // Check property count after registration
  unsigned int postRegistrationPropertyCount( customActor.GetPropertyCount() );
  DALI_TEST_EQUALS( initialPropertyCount + 1u, postRegistrationPropertyCount, TEST_LOCATION );

  // Add custom property and check property count
  customActor.RegisterProperty( "customProp1",   true );
  unsigned int customPropertyCount( customActor.GetPropertyCount() );
  DALI_TEST_EQUALS( postRegistrationPropertyCount + 1u, customPropertyCount, TEST_LOCATION );

  // Set the property, ensure SetProperty called
  DALI_TEST_CHECK( !setPropertyCalled );
  customActor.SetProperty( propertyIndex, false );
  DALI_TEST_CHECK( setPropertyCalled );

  // Get the property, ensure GetProperty called
  DALI_TEST_CHECK( !getPropertyCalled );
  (void)customActor.GetProperty< bool >( propertyIndex );
  DALI_TEST_CHECK( getPropertyCalled );

  // Get the property using GetCurrentProperty and ensure GetProperty is called
  getPropertyCalled = false;
  DALI_TEST_CHECK( !getPropertyCalled );
  customActor.GetCurrentProperty< bool >( propertyIndex );
  DALI_TEST_CHECK( getPropertyCalled );

  // Check the property name
  DALI_TEST_EQUALS( customActor.GetPropertyName( propertyIndex ), propertyName, TEST_LOCATION );
  DALI_TEST_EQUALS( typeInfo.GetPropertyName( propertyIndex ), propertyName, TEST_LOCATION );

  // Check the property index
  DALI_TEST_EQUALS( customActor.GetPropertyIndex( propertyName ), propertyIndex, TEST_LOCATION );

  // Check the property type
  DALI_TEST_EQUALS( customActor.GetPropertyType( propertyIndex ), propertyType, TEST_LOCATION );

  // Check property count of type-info is 1
  Property::IndexContainer indices;
  typeInfo.GetPropertyIndices( indices );

  size_t typePropertyCount = typeInfo.GetPropertyCount();
  DALI_TEST_EQUALS( indices.Size(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( indices.Size(), typePropertyCount, TEST_LOCATION );

  // Ensure indices returned from actor and customActor differ by two
  Actor actor = Actor::New();
  actor.GetPropertyIndices( indices );
  unsigned int actorIndices = indices.Size();
  customActor.GetPropertyIndices( indices );
  unsigned int customActorIndices = indices.Size();
  DALI_TEST_EQUALS( actorIndices + 2u, customActorIndices, TEST_LOCATION ); // Custom property + registered property
  END_TEST;
}

int UtcDaliTypeRegistryPropertyRegistrationN(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  // Attempt to register a property type out-of-bounds index (less than)
  try
  {
    PropertyRegistration property1( customType1, "propName",  PROPERTY_REGISTRATION_START_INDEX - 1, Property::BOOLEAN, &SetProperty, &GetProperty );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "( index >= PROPERTY_REGISTRATION_START_INDEX ) && ( index <= PROPERTY_REGISTRATION_MAX_INDEX )", TEST_LOCATION );
  }

  // Attempt to register a property type out-of-bounds index (greater than)
  try
  {
    PropertyRegistration property1( customType1, "propName",  PROPERTY_REGISTRATION_MAX_INDEX + 1, Property::BOOLEAN, &SetProperty, &GetProperty );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "( index >= PROPERTY_REGISTRATION_START_INDEX ) && ( index <= PROPERTY_REGISTRATION_MAX_INDEX )", TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliTypeRegistryAnimatablePropertyRegistrationP(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  // Check property count before property registration
  TypeInfo typeInfo = typeRegistry.GetTypeInfo( typeid(MyTestCustomActor) );
  DALI_TEST_CHECK( typeInfo );
  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( handle );
  Actor customActor = Actor::DownCast( handle );
  DALI_TEST_CHECK( customActor );
  Stage::GetCurrent().Add(customActor);

  unsigned int customPropertyCount( customActor.GetPropertyCount() );

  // Register animatable property
  std::string animatablePropertyName( "animatableProp1" );
  int animatablePropertyIndex( ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX );
  Property::Type animatablePropertyType( Property::FLOAT );
  AnimatablePropertyRegistration animatableProperty( customType1, animatablePropertyName, animatablePropertyIndex, animatablePropertyType );

  // Check property count after registration
  DALI_TEST_EQUALS( customPropertyCount + 1u, customActor.GetPropertyCount(), TEST_LOCATION );

  // Set the animatable property value
  customActor.SetProperty( animatablePropertyIndex, 25.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Check the animatable property value
  DALI_TEST_EQUALS( customActor.GetProperty< float >( animatablePropertyIndex ), 25.f, TEST_LOCATION );

  // Check the animatable property name
  DALI_TEST_EQUALS( customActor.GetPropertyName( animatablePropertyIndex ), animatablePropertyName, TEST_LOCATION );

  // Check the animatable property index
  DALI_TEST_EQUALS( customActor.GetPropertyIndex( animatablePropertyName ), animatablePropertyIndex, TEST_LOCATION );

  // Check the animatable property type
  DALI_TEST_EQUALS( customActor.GetPropertyType( animatablePropertyIndex ), animatablePropertyType, TEST_LOCATION );

  // Check property count of type-info is 1
  Property::IndexContainer indices;
  typeInfo.GetPropertyIndices( indices );
  DALI_TEST_EQUALS( indices.Size(), 1u, TEST_LOCATION );

  // Ensure indices returned from actor and customActor differ by one
  Actor actor = Actor::New();
  actor.GetPropertyIndices( indices );
  unsigned int actorIndices = indices.Size();
  customActor.GetPropertyIndices( indices );
  unsigned int customActorIndices = indices.Size();
  DALI_TEST_EQUALS( actorIndices + 1u, customActorIndices, TEST_LOCATION ); // Custom property + registered property

  // check that the property is animatable
  Animation animation = Animation::New(0.2f);
  animation.AnimateTo( Property( customActor, animatablePropertyIndex ), 15.f, AlphaFunction::LINEAR );
  animation.Play();

  // Target value should change straight away
  DALI_TEST_EQUALS( customActor.GetProperty< float >( animatablePropertyIndex ), 15.0f, TEST_LOCATION );

  // Render and notify, animation play for 0.05 seconds
  application.SendNotification();
  application.Render(50);
  DALI_TEST_EQUALS( 0.25f, animation.GetCurrentProgress(), TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( animatablePropertyIndex ), 22.5f, TEST_LOCATION );

  // Render and notify, animation play for another 0.1 seconds
  application.SendNotification();
  application.Render(100);
  DALI_TEST_EQUALS( 0.75f, animation.GetCurrentProgress(), TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( animatablePropertyIndex ), 17.5f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTypeRegistryAnimatablePropertyRegistrationN(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  // Attempt to register an animatable property type out-of-bounds index (less than)
  try
  {
    AnimatablePropertyRegistration property1( customType1, "animPropName",   ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX - 1, Property::BOOLEAN );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX )", TEST_LOCATION );
  }

  // Attempt to register an animatable property type out-of-bounds index (greater than)
  try
  {
    AnimatablePropertyRegistration property1( customType1, "animPropName",   ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX + 1, Property::BOOLEAN );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX )", TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliTypeRegistryAnimatablePropertyRegistrationWithDefaultP(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  // Check property count before property registration
  TypeInfo typeInfo = typeRegistry.GetTypeInfo( typeid(MyTestCustomActor) );
  DALI_TEST_CHECK( typeInfo );
  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( handle );
  Actor customActor = Actor::DownCast( handle );
  DALI_TEST_CHECK( customActor );
  Stage::GetCurrent().Add(customActor);

  unsigned int customPropertyCount( customActor.GetPropertyCount() );

  // Register animatable property
  std::string animatablePropertyName( "animatableProp1" );
  int animatablePropertyIndex( ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX );
  AnimatablePropertyRegistration animatableProperty1( customType1, animatablePropertyName, animatablePropertyIndex, 10.f );

  // Check property count after registration
  DALI_TEST_EQUALS( customPropertyCount + 1u, customActor.GetPropertyCount(), TEST_LOCATION );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Check the animatable property value
  DALI_TEST_EQUALS( customActor.GetProperty< float >( animatablePropertyIndex ), 10.f, TEST_LOCATION );

  // Check the animatable property name
  DALI_TEST_EQUALS( customActor.GetPropertyName( animatablePropertyIndex ), animatablePropertyName, TEST_LOCATION );

  // Check the animatable property index
  DALI_TEST_EQUALS( customActor.GetPropertyIndex( animatablePropertyName ), animatablePropertyIndex, TEST_LOCATION );

  // Check the animatable property type
  DALI_TEST_EQUALS( customActor.GetPropertyType( animatablePropertyIndex ), Property::FLOAT, TEST_LOCATION );

  // Check property count of type-info is 1
  Property::IndexContainer indices;
  typeInfo.GetPropertyIndices( indices );
  DALI_TEST_EQUALS( indices.Size(), 1u, TEST_LOCATION );

  // Ensure indices returned from actor and customActor differ by one
  Actor actor = Actor::New();
  actor.GetPropertyIndices( indices );
  unsigned int actorIndices = indices.Size();
  customActor.GetPropertyIndices( indices );
  unsigned int customActorIndices = indices.Size();
  DALI_TEST_EQUALS( actorIndices + 1u, customActorIndices, TEST_LOCATION ); // Custom property + registered property

  // check that the property is animatable
  Animation animation = Animation::New(0.2f);
  animation.AnimateTo( Property( customActor, animatablePropertyIndex ), 20.f, AlphaFunction::LINEAR );
  animation.Play();

  // Target value should change straight away
  DALI_TEST_EQUALS( customActor.GetProperty< float >( animatablePropertyIndex ), 20.0f, TEST_LOCATION );

  // Render and notify, animation play for 0.05 seconds
  application.SendNotification();
  application.Render(50);
  DALI_TEST_EQUALS( 0.25f, animation.GetCurrentProgress(), TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( animatablePropertyIndex ), 12.5f, TEST_LOCATION );

  // Render and notify, animation play for another 0.1 seconds
  application.SendNotification();
  application.Render(100);
  DALI_TEST_EQUALS( 0.75f, animation.GetCurrentProgress(), TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( animatablePropertyIndex ), 17.5f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTypeRegistryAnimatablePropertyRegistrationWithDefaultN(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  // Attempt to register an animatable property type out-of-bounds index (less than)
  try
  {
    AnimatablePropertyRegistration property1( customType1, "animPropName",   ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX - 1, false );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX )", TEST_LOCATION );
  }

  // Attempt to register an animatable property type out-of-bounds index (greater than)
  try
  {
    AnimatablePropertyRegistration property1( customType1, "animPropName",   ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX + 1, true );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX )", TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliTypeRegistryAnimatablePropertyComponentRegistrationP(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  // Check property count before property registration
  TypeInfo typeInfo = typeRegistry.GetTypeInfo( typeid(MyTestCustomActor) );
  DALI_TEST_CHECK( typeInfo );
  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( handle );
  Actor customActor = Actor::DownCast( handle );
  DALI_TEST_CHECK( customActor );

  unsigned int customPropertyCount( customActor.GetPropertyCount() );

  // Register animatable property
  std::string animatablePropertyName( "animatableProp1" );
  int animatablePropertyIndex( ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX );
  Property::Type animatablePropertyType( Property::VECTOR2 );
  AnimatablePropertyRegistration animatableProperty1( customType1, animatablePropertyName, animatablePropertyIndex, animatablePropertyType );

  // Check property count after registration
  DALI_TEST_EQUALS( customPropertyCount + 1u, customActor.GetPropertyCount(), TEST_LOCATION );

  // Set the animatable property value
  customActor.SetProperty( animatablePropertyIndex, Vector2(25.0f, 50.0f) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Check the animatable property value
  DALI_TEST_EQUALS( customActor.GetProperty< Vector2 >( animatablePropertyIndex ), Vector2(25.0f, 50.0f), TEST_LOCATION );

  // Check the animatable property name
  DALI_TEST_EQUALS( customActor.GetPropertyName( animatablePropertyIndex ), animatablePropertyName, TEST_LOCATION );

  // Check the animatable property index
  DALI_TEST_EQUALS( customActor.GetPropertyIndex( animatablePropertyName ), animatablePropertyIndex, TEST_LOCATION );

  // Check the animatable property type
  DALI_TEST_EQUALS( customActor.GetPropertyType( animatablePropertyIndex ), animatablePropertyType, TEST_LOCATION );

  // Check property count of type-info is 1
  Property::IndexContainer indices;
  typeInfo.GetPropertyIndices( indices );
  DALI_TEST_EQUALS( indices.Size(), 1u, TEST_LOCATION );

  // Register animatable property components
  std::string animatablePropertyComponentName1( "animatableProp1X" );
  int animatablePropertyComponentIndex1( ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 1 );
  AnimatablePropertyComponentRegistration animatablePropertyComponent1( customType1, animatablePropertyComponentName1, animatablePropertyComponentIndex1, animatablePropertyIndex, 0 );

  std::string animatablePropertyComponentName2( "animatableProp1Y" );
  int animatablePropertyComponentIndex2( ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 2 );
  AnimatablePropertyComponentRegistration animatablePropertyComponent2( customType1, animatablePropertyComponentName2, animatablePropertyComponentIndex2, animatablePropertyIndex, 1 );

  // Check property count after registration
  DALI_TEST_EQUALS( customPropertyCount + 3u, customActor.GetPropertyCount(), TEST_LOCATION );

  // Check the animatable property component value
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( animatablePropertyComponentIndex1 ), 25.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( animatablePropertyComponentIndex2 ), 50.0f, TEST_LOCATION );

  // Set the animatable property component value
  customActor.SetProperty( animatablePropertyComponentIndex1, 150.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Check the animatable property value
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< Vector2 >( animatablePropertyIndex ), Vector2(150.0f, 50.0f), TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( animatablePropertyComponentIndex1 ), 150.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( animatablePropertyComponentIndex2 ), 50.0f, TEST_LOCATION );

  // Set the animatable property component value
  customActor.SetProperty( animatablePropertyComponentIndex2, 225.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Check the animatable property value
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< Vector2 >( animatablePropertyIndex ), Vector2(150.0f, 225.0f), TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( animatablePropertyComponentIndex1 ), 150.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( animatablePropertyComponentIndex2 ), 225.0f, TEST_LOCATION );

  // Ensure indices returned from actor and customActor differ by three
  Actor actor = Actor::New();
  actor.GetPropertyIndices( indices );
  unsigned int actorIndices = indices.Size();
  customActor.GetPropertyIndices( indices );
  unsigned int customActorIndices = indices.Size();
  DALI_TEST_EQUALS( actorIndices + 3u, customActorIndices, TEST_LOCATION ); // Custom property + registered property

  // Attempt to animate component property, it should not crash
  Animation animation = Animation::New( 1.0f );
  animation.AnimateTo( Property( customActor, animatablePropertyComponentIndex1 ), 200.0f );
  animation.Play();

  // Check the property value
  DALI_TEST_EQUALS( customActor.GetProperty< Vector2 >( animatablePropertyIndex ), Vector2(200.0f, 225.0f), TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( animatablePropertyComponentIndex1 ), 200.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( animatablePropertyComponentIndex2 ), 225.0f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTypeRegistryAnimatablePropertyComponentRegistrationVector2AnimateByP(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  TypeInfo typeInfo = typeRegistry.GetTypeInfo( typeid(MyTestCustomActor) );
  DALI_TEST_CHECK( typeInfo );
  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( handle );
  Actor customActor = Actor::DownCast( handle );
  DALI_TEST_CHECK( customActor );

  const unsigned int index = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX;
  const unsigned int xComponentIndex = index + 1;
  const unsigned int yComponentIndex = index + 2;
  const Vector2 initialValue( 20.0f, 40.0f );

  // Register animatable property & its components
  AnimatablePropertyRegistration animatableProperty1( customType1, "animatableProp1", index, initialValue );
  AnimatablePropertyComponentRegistration animatablePropertyComponent1( customType1, "animatableProp1X", xComponentIndex, index, 0 );
  AnimatablePropertyComponentRegistration animatablePropertyComponent2( customType1, "animatableProp1Y", yComponentIndex, index, 1 );

  // Check the animatable property value
  DALI_TEST_EQUALS( customActor.GetProperty< Vector2 >( index ), initialValue, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( xComponentIndex ), initialValue.x, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( yComponentIndex ), initialValue.y, TEST_LOCATION );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Check the animatable property current value
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< Vector2 >( index ), initialValue, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( xComponentIndex ), initialValue.x, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( yComponentIndex ), initialValue.y, TEST_LOCATION );

  // Do an AnimateBy
  const Vector2 targetValue( 45.0f, 53.0f );
  const Vector2 relativeValue( targetValue - initialValue );

  Animation animation = Animation::New( 1.0f );
  animation.AnimateBy( Property( customActor, xComponentIndex ), relativeValue.x );
  animation.AnimateBy( Property( customActor, yComponentIndex ), relativeValue.y );
  animation.Play();

  // Target values should change straight away
  DALI_TEST_EQUALS( customActor.GetProperty< Vector2 >( index ), targetValue, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( xComponentIndex ), targetValue.x, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( yComponentIndex ), targetValue.y, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTypeRegistryAnimatablePropertyComponentRegistrationVector3AnimateByP(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  TypeInfo typeInfo = typeRegistry.GetTypeInfo( typeid(MyTestCustomActor) );
  DALI_TEST_CHECK( typeInfo );
  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( handle );
  Actor customActor = Actor::DownCast( handle );
  DALI_TEST_CHECK( customActor );

  const unsigned int index = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX;
  const unsigned int xComponentIndex = index + 1;
  const unsigned int yComponentIndex = index + 2;
  const unsigned int zComponentIndex = index + 3;
  const Vector3 initialValue( 20.0f, 40.0f, 50.0f );

  // Register animatable property & its components
  AnimatablePropertyRegistration animatableProperty1( customType1, "animatableProp1", index, initialValue );
  AnimatablePropertyComponentRegistration animatablePropertyComponent1( customType1, "animatableProp1X", xComponentIndex, index, 0 );
  AnimatablePropertyComponentRegistration animatablePropertyComponent2( customType1, "animatableProp1Y", yComponentIndex, index, 1 );
  AnimatablePropertyComponentRegistration animatablePropertyComponent3( customType1, "animatableProp1Z", zComponentIndex, index, 2 );

  // Check the animatable property value
  DALI_TEST_EQUALS( customActor.GetProperty< Vector3 >( index ), initialValue, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( xComponentIndex ), initialValue.x, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( yComponentIndex ), initialValue.y, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( zComponentIndex ), initialValue.z, TEST_LOCATION );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Check the animatable property current value
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< Vector3 >( index ), initialValue, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( xComponentIndex ), initialValue.x, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( yComponentIndex ), initialValue.y, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( zComponentIndex ), initialValue.z, TEST_LOCATION );

  // Do an AnimateBy
  const Vector3 targetValue( 45.0f, 53.0f, 25.0f );
  const Vector3 relativeValue( targetValue - initialValue );

  Animation animation = Animation::New( 1.0f );
  animation.AnimateBy( Property( customActor, xComponentIndex ), relativeValue.x );
  animation.AnimateBy( Property( customActor, yComponentIndex ), relativeValue.y );
  animation.AnimateBy( Property( customActor, zComponentIndex ), relativeValue.z );
  animation.Play();

  // Target values should change straight away
  DALI_TEST_EQUALS( customActor.GetProperty< Vector3 >( index ), targetValue, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( xComponentIndex ), targetValue.x, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( yComponentIndex ), targetValue.y, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( zComponentIndex ), targetValue.z, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTypeRegistryAnimatablePropertyComponentRegistrationVector4AnimateByP(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  TypeInfo typeInfo = typeRegistry.GetTypeInfo( typeid(MyTestCustomActor) );
  DALI_TEST_CHECK( typeInfo );
  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( handle );
  Actor customActor = Actor::DownCast( handle );
  DALI_TEST_CHECK( customActor );

  const unsigned int index = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX;
  const unsigned int xComponentIndex = index + 1;
  const unsigned int yComponentIndex = index + 2;
  const unsigned int zComponentIndex = index + 3;
  const unsigned int wComponentIndex = index + 4;
  const Vector4 initialValue( 20.0f, 40.0f, 50.0f, 60.0f );

  // Register animatable property & its components
  AnimatablePropertyRegistration animatableProperty1( customType1, "animatableProp1", index, initialValue );
  AnimatablePropertyComponentRegistration animatablePropertyComponent1( customType1, "animatableProp1X", xComponentIndex, index, 0 );
  AnimatablePropertyComponentRegistration animatablePropertyComponent2( customType1, "animatableProp1Y", yComponentIndex, index, 1 );
  AnimatablePropertyComponentRegistration animatablePropertyComponent3( customType1, "animatableProp1Z", zComponentIndex, index, 2 );
  AnimatablePropertyComponentRegistration animatablePropertyComponent4( customType1, "animatableProp1W", wComponentIndex, index, 3 );

  // Check the animatable property value
  DALI_TEST_EQUALS( customActor.GetProperty< Vector4 >( index ), initialValue, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( xComponentIndex ), initialValue.x, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( yComponentIndex ), initialValue.y, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( zComponentIndex ), initialValue.z, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( wComponentIndex ), initialValue.w, TEST_LOCATION );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Check the animatable property current value
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< Vector4 >( index ), initialValue, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( xComponentIndex ), initialValue.x, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( yComponentIndex ), initialValue.y, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( zComponentIndex ), initialValue.z, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( wComponentIndex ), initialValue.w, TEST_LOCATION );

  // Do an AnimateBy
  const Vector4 targetValue( 45.0f, 53.0f, 25.0f, 13.0f );
  const Vector4 relativeValue( targetValue - initialValue );

  Animation animation = Animation::New( 1.0f );
  animation.AnimateBy( Property( customActor, xComponentIndex ), relativeValue.x );
  animation.AnimateBy( Property( customActor, yComponentIndex ), relativeValue.y );
  animation.AnimateBy( Property( customActor, zComponentIndex ), relativeValue.z );
  animation.AnimateBy( Property( customActor, wComponentIndex ), relativeValue.w );
  animation.Play();

  // Target values should change straight away
  DALI_TEST_EQUALS( customActor.GetProperty< Vector4 >( index ), targetValue, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( xComponentIndex ), targetValue.x, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( yComponentIndex ), targetValue.y, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( zComponentIndex ), targetValue.z, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( wComponentIndex ), targetValue.w, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTypeRegistryAnimatablePropertyComponentRegistrationN(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  // Register animatable property with the type of Vector2
  int animatablePropertyIndex1( ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX );
  AnimatablePropertyRegistration animatableProperty1( customType1, "animatableProp1",   animatablePropertyIndex1, Property::VECTOR2 );

  // Attempt to register an animatable property component out-of-bounds index (less than)
  try
  {
    AnimatablePropertyComponentRegistration propertyComponent1( customType1, "animatableProp1X",    ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX - 1, animatablePropertyIndex1, 0 );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX )", TEST_LOCATION );
  }

  // Attempt to register an animatable property component out-of-bounds index (greater than)
  try
  {
    AnimatablePropertyComponentRegistration propertyComponent1( customType1, "animatableProp1X",    ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX + 1, animatablePropertyIndex1, 0 );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX )", TEST_LOCATION );
  }

  // Register an animatable property component
  AnimatablePropertyComponentRegistration propertyComponent1( customType1, "animatableProp1X",    ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 1, animatablePropertyIndex1, 0 );

  // Attempt to register another animatable property component with the same component index
  try
  {
    AnimatablePropertyComponentRegistration propertyComponent2( customType1, "animatableProp1Y",    ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 2, animatablePropertyIndex1, 0 );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "Property component already registered", TEST_LOCATION );
  }

  // Register animatable property with the type of boolean
  int animatablePropertyIndex2( ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 2 );
  AnimatablePropertyRegistration animatableProperty2( customType1, "animatableProp2",   animatablePropertyIndex2, Property::BOOLEAN );

  // Attempt to register an animatable property component for the above property with boolean type
  try
  {
    AnimatablePropertyComponentRegistration propertyComponent1( customType1, "animatableProp2X",    animatablePropertyIndex2 + 1, animatablePropertyIndex2, 0 );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "Base property does not support component", TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliTypeRegistryChildPropertyRegistrationP(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  // Check property count before property registration
  TypeInfo typeInfo = typeRegistry.GetTypeInfo( typeid(MyTestCustomActor) );
  DALI_TEST_CHECK( typeInfo );
  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( handle );
  Actor customActor = Actor::DownCast( handle );
  DALI_TEST_CHECK( customActor );
  unsigned int initialPropertyCount( customActor.GetPropertyCount() );

  // Register child properties to the parent
  std::string propertyName( "childProp1" );
  int propertyIndex( CHILD_PROPERTY_REGISTRATION_START_INDEX );
  Property::Type propertyType( Property::BOOLEAN );
  ChildPropertyRegistration childProperty1( customType1, propertyName, propertyIndex, propertyType );

  std::string propertyName2( "childProp2" );
  int propertyIndex2( CHILD_PROPERTY_REGISTRATION_START_INDEX + 1 );
  Property::Type propertyType2( Property::INTEGER );
  ChildPropertyRegistration childProperty2( customType1, propertyName2, propertyIndex2, propertyType2 );

  std::string propertyName3( "childProp3" );
  int propertyIndex3( CHILD_PROPERTY_REGISTRATION_START_INDEX + 2 );
  Property::Type propertyType3( Property::FLOAT );
  ChildPropertyRegistration childProperty3( customType1, propertyName3, propertyIndex3, propertyType3 );

  std::string propertyName4( "childProp4" );
  int propertyIndex4( CHILD_PROPERTY_REGISTRATION_START_INDEX + 3 );
  Property::Type propertyType4( Property::INTEGER );
  ChildPropertyRegistration childProperty4( customType1, propertyName4, propertyIndex4, propertyType4 );

  // Check property count are not changed because the child properties will not be created for the parent
  DALI_TEST_EQUALS( initialPropertyCount, customActor.GetPropertyCount(), TEST_LOCATION );

  // check the child property type
  Internal::TypeInfo& typeInfoImpl = GetImplementation( typeInfo );
  Property::Type type = typeInfoImpl.GetChildPropertyType( typeInfoImpl.GetChildPropertyIndex("childProp4") );
  DALI_TEST_EQUALS( type, Property::INTEGER, TEST_LOCATION );

  std::string unRegisteredChildName( typeInfoImpl.GetChildPropertyName( CHILD_PROPERTY_REGISTRATION_START_INDEX + 4 ) );
  DALI_TEST_EQUALS( unRegisteredChildName, "", TEST_LOCATION );

  // Create a child actor
  Actor childActor = Actor::New();
  DALI_TEST_CHECK( childActor );
  unsigned int initialChildActorPropertyCount( childActor.GetPropertyCount() );

  // The type of child properties should be Property::None as the child hasn't registered any child property yet.
  DALI_TEST_EQUALS( childActor.GetPropertyType( propertyIndex ), Property::NONE, TEST_LOCATION );
  DALI_TEST_EQUALS( childActor.GetPropertyType( propertyIndex2 ), Property::NONE, TEST_LOCATION );
  DALI_TEST_EQUALS( childActor.GetPropertyType( propertyIndex3 ), Property::NONE, TEST_LOCATION );
  DALI_TEST_EQUALS( childActor.GetPropertyType( propertyIndex4 ), Property::NONE, TEST_LOCATION );

  // Set the value for the first child property when the child actor doesn't have a parent yet
  childActor.SetProperty(propertyIndex, true);

  // Check that the first child property is dynamically created
  DALI_TEST_EQUALS( initialChildActorPropertyCount + 1u, childActor.GetPropertyCount(), TEST_LOCATION );

  // Check the first child property value
  DALI_TEST_EQUALS( childActor.GetProperty< bool >( propertyIndex ), true, TEST_LOCATION );

  // Check the first child property type
  DALI_TEST_EQUALS( childActor.GetPropertyType( propertyIndex ), propertyType, TEST_LOCATION );

  // Check that the first child property have no name, as it doesn't have a parent yet.
  DALI_TEST_EQUALS( childActor.GetPropertyName( propertyIndex ), "", TEST_LOCATION );

  // Check that the first property can't be accessed through its name, as it doesn't have a parent yet.
  DALI_TEST_EQUALS( childActor.GetPropertyIndex( propertyName ), Property::INVALID_INDEX, TEST_LOCATION );

  // Create a custom property for the child with the same name as the second child property registered to the parent
  Property::Index customPropertyIndex = childActor.RegisterProperty(propertyName2, 100, Property::READ_WRITE);

  // Check that the custom property is created
  DALI_TEST_EQUALS( initialChildActorPropertyCount + 2u, childActor.GetPropertyCount(), TEST_LOCATION );

  // Check the property value
  DALI_TEST_EQUALS( childActor.GetProperty< int >( customPropertyIndex ), 100, TEST_LOCATION );

  // Check the property index
  DALI_TEST_EQUALS( childActor.GetPropertyIndex( propertyName2 ), customPropertyIndex, TEST_LOCATION );

  // Check the property type
  DALI_TEST_EQUALS( childActor.GetPropertyType( customPropertyIndex ), propertyType2, TEST_LOCATION );

  // Check the property name
  DALI_TEST_EQUALS( childActor.GetPropertyName( customPropertyIndex ), propertyName2, TEST_LOCATION );

  // Now add the child actor to the parent
  customActor.Add( childActor );

  // Check that the first child property now has the correct name as previously registered to the parent
  DALI_TEST_EQUALS( childActor.GetPropertyName( propertyIndex ), propertyName, TEST_LOCATION );

  // Check that the child property index for the first child property can now be retrieved through its child property name
  DALI_TEST_EQUALS( childActor.GetPropertyIndex( propertyName ), propertyIndex, TEST_LOCATION );

  // Check that the second child property now has the correct index as previously registered to the parent
  DALI_TEST_EQUALS( childActor.GetPropertyName( propertyIndex2 ), propertyName2, TEST_LOCATION );

  // Check that the second child property can be accessed through both its custom property index and its child property index
  DALI_TEST_EQUALS( childActor.GetProperty< int >( customPropertyIndex ), 100, TEST_LOCATION );
  DALI_TEST_EQUALS( childActor.GetProperty< int >( propertyIndex2 ), 100, TEST_LOCATION );
  DALI_TEST_EQUALS( childActor.GetPropertyType( customPropertyIndex ), propertyType2, TEST_LOCATION );
  DALI_TEST_EQUALS( childActor.GetPropertyType( propertyIndex2 ), propertyType2, TEST_LOCATION );

  // Check that the child property index for the second child property can now be retrieved through its child property name
  DALI_TEST_EQUALS( childActor.GetPropertyIndex( propertyName2 ), propertyIndex2, TEST_LOCATION );

  // Set the value for the third child property when the child actor is already added to the parent
  childActor.SetProperty(propertyIndex3, 0.15f);

  // Check that the third child property is dynamically created
  DALI_TEST_EQUALS( initialChildActorPropertyCount + 3u, childActor.GetPropertyCount(), TEST_LOCATION );

  // Check the third child property value
  DALI_TEST_EQUALS( childActor.GetProperty< float >( propertyIndex3 ), 0.15f, TEST_LOCATION );

  // Check the third child property type
  DALI_TEST_EQUALS( childActor.GetPropertyType( propertyIndex3 ), propertyType3, TEST_LOCATION );

  // Check the third child property name
  DALI_TEST_EQUALS( childActor.GetPropertyName( propertyIndex3 ), propertyName3, TEST_LOCATION );

  // Check the third child property index.
  DALI_TEST_EQUALS( childActor.GetPropertyIndex( propertyName3 ), propertyIndex3, TEST_LOCATION );

  // Create a custom property for the child with the same name as the fourth child property registered to the parent
  Property::Index customPropertyIndex2 = childActor.RegisterProperty(propertyName4, 20, Property::READ_WRITE);

  // Check that the custom property is created
  DALI_TEST_EQUALS( initialChildActorPropertyCount + 4u, childActor.GetPropertyCount(), TEST_LOCATION );

  // Check the fourth child property value
  DALI_TEST_EQUALS( childActor.GetProperty< int >( propertyIndex4 ), 20, TEST_LOCATION );
  DALI_TEST_EQUALS( childActor.GetProperty< int >( customPropertyIndex2 ), 20, TEST_LOCATION );

  // Check the fourth child property type
  DALI_TEST_EQUALS( childActor.GetPropertyType( propertyIndex4 ), propertyType4, TEST_LOCATION );
  DALI_TEST_EQUALS( childActor.GetPropertyType( customPropertyIndex2 ), propertyType4, TEST_LOCATION );

  // Check the fourth child property name
  DALI_TEST_EQUALS( childActor.GetPropertyName( propertyIndex4 ), propertyName4, TEST_LOCATION );
  DALI_TEST_EQUALS( childActor.GetPropertyName( customPropertyIndex2 ), propertyName4, TEST_LOCATION );

  // Check the fourth child property index.
  DALI_TEST_EQUALS( childActor.GetPropertyIndex( propertyName4 ), propertyIndex4, TEST_LOCATION );

  // Now create another parent actor with different child properties registered
  TypeInfo typeInfo2 = typeRegistry.GetTypeInfo( "MyNamedActor" );
  DALI_TEST_CHECK( typeInfo2 );
  BaseHandle handle2 = typeInfo2.CreateInstance();
  DALI_TEST_CHECK( handle2 );
  Actor customActor2 = Actor::DownCast( handle2 );
  DALI_TEST_CHECK( customActor2 );

  // Register child properties to the new parent
  std::string newPropertyName( "newChildProp" );
  int newPropertyIndex( CHILD_PROPERTY_REGISTRATION_START_INDEX ); // The same index as the first child property "childProp1" in the old parent
  Property::Type newPropertyType( Property::VECTOR2 );
  ChildPropertyRegistration newChildProperty( namedActorType, newPropertyName, newPropertyIndex, newPropertyType );

  std::string newPropertyName2( "childProp3" ); // The same name as the third child property in the old parent
  int newPropertyIndex2( CHILD_PROPERTY_REGISTRATION_START_INDEX + 1 ); // The same index as the second child property "childProp2" in the old parent
  Property::Type newPropertyType2( Property::FLOAT ); // The same type as the third child property in the old parent
  ChildPropertyRegistration newChildProperty2( namedActorType, newPropertyName2, newPropertyIndex2, newPropertyType2 );

  // Now move the child actor to the new parent
  customActor2.Add( childActor );

  // "childProp1" is not a valid child property supported by the new parent, so nothing changed
  DALI_TEST_EQUALS( childActor.GetPropertyType( propertyIndex ), propertyType, TEST_LOCATION );
  DALI_TEST_EQUALS( childActor.GetPropertyName( propertyIndex ), propertyName, TEST_LOCATION );
  DALI_TEST_EQUALS( childActor.GetPropertyIndex( propertyName ), propertyIndex, TEST_LOCATION );

  // "childProp3" is a valid child property supported by the new parent
  // So it should get its new child property index and should just work
  DALI_TEST_EQUALS( childActor.GetPropertyType( newPropertyIndex2 ), newPropertyType2, TEST_LOCATION );
  DALI_TEST_EQUALS( childActor.GetPropertyName( newPropertyIndex2 ), newPropertyName2, TEST_LOCATION );
  DALI_TEST_EQUALS( childActor.GetPropertyIndex( newPropertyName2 ), newPropertyIndex2, TEST_LOCATION );
  DALI_TEST_EQUALS( childActor.GetProperty< float >( newPropertyIndex2 ), 0.15f, TEST_LOCATION );

  // Now register a custom property called "newChildProp"
  Property::Index customPropertyIndex3 = childActor.RegisterProperty("newChildProp", Vector2( 10.0f, 10.0f ), Property::READ_WRITE);

  // Check that the custom property is created
  DALI_TEST_EQUALS( initialChildActorPropertyCount + 5u, childActor.GetPropertyCount(), TEST_LOCATION );

  // This is a valid child property registered to the new parent
  // So should be able to access it through both its custom property index and its registered child property index
  DALI_TEST_EQUALS( childActor.GetPropertyType( newPropertyIndex ), newPropertyType, TEST_LOCATION );
  DALI_TEST_EQUALS( childActor.GetPropertyType( customPropertyIndex3 ), newPropertyType, TEST_LOCATION );
  DALI_TEST_EQUALS( childActor.GetPropertyName( newPropertyIndex ), newPropertyName, TEST_LOCATION ); // This should return the new name, although the child property index remains the same
  DALI_TEST_EQUALS( childActor.GetPropertyName( customPropertyIndex3 ), newPropertyName, TEST_LOCATION );
  DALI_TEST_EQUALS( childActor.GetProperty< Vector2 >( newPropertyIndex ), Vector2( 10.0f, 10.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( childActor.GetProperty< Vector2 >( customPropertyIndex3 ), Vector2( 10.0f, 10.0f ), TEST_LOCATION );

  // Should return the child property index by given its name
  DALI_TEST_EQUALS( childActor.GetPropertyIndex( newPropertyName ), newPropertyIndex, TEST_LOCATION );


  END_TEST;
}

int UtcDaliTypeRegistryChildPropertyRegistrationN(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  // Attempt to register a child property type out-of-bounds index (less than)
  try
  {
    ChildPropertyRegistration property1( customType1, "propName",  CHILD_PROPERTY_REGISTRATION_START_INDEX - 1, Property::BOOLEAN );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "( index >= CHILD_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= CHILD_PROPERTY_REGISTRATION_MAX_INDEX )", TEST_LOCATION );
  }

  // Attempt to register a child property type out-of-bounds index (greater than)
  try
  {
    ChildPropertyRegistration property1( customType1, "propName",  CHILD_PROPERTY_REGISTRATION_MAX_INDEX + 1, Property::BOOLEAN );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "( index >= CHILD_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= CHILD_PROPERTY_REGISTRATION_MAX_INDEX )", TEST_LOCATION );
  }

  END_TEST;
}


/*******************************************************************************
 *
 * Action through the base handle
 *
 ******************************************************************************/
int UtcDaliTypeRegistryActionViaBaseHandle(void)
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

  Property::Map attributes;

  DALI_TEST_CHECK(hdl.DoAction("show", attributes));

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK(a.IsVisible());

  DALI_TEST_CHECK(!hdl.DoAction("unknownAction",  attributes));
  END_TEST;
}

int UtcDaliPropertyRegistrationFunctions(void)
{
  TestApplication application;
  int propertyIndex = PROPERTY_REGISTRATION_START_INDEX + 10;

  // Attempt to register a property without a setter
  try
  {
    PropertyRegistration property1( customType1, "propName",  propertyIndex++, Property::BOOLEAN, NULL, &GetProperty );
    tet_result( TET_PASS );
  }
  catch ( DaliException& e )
  {
    tet_result( TET_FAIL );
  }

  // Attempt to register a property without a getter
  try
  {
    PropertyRegistration property1( customType1, "propName",  propertyIndex++, Property::BOOLEAN, NULL, NULL );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "! \"GetProperty", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliPropertyRegistrationAddSameIndex(void)
{
  TestApplication application;
  int propertyIndex = PROPERTY_REGISTRATION_START_INDEX + 100;

  // Add one property with a valid property index
  PropertyRegistration property1( customType1, "propName",  propertyIndex, Property::BOOLEAN, &SetProperty, &GetProperty );

  // Attempt to add another property with the same index
  try
  {
    PropertyRegistration property2( customType1, "propName2",   propertyIndex, Property::BOOLEAN, &SetProperty, &GetProperty );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "! \"Property index already added", TEST_LOCATION );
  }

  int animatablePropertyIndex = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 100;

  // Add one property with a valid property index
  AnimatablePropertyRegistration property3( customType1, "animPropName",   animatablePropertyIndex, Property::BOOLEAN );

  // Attempt to add another property with the same index
  try
  {
    AnimatablePropertyRegistration property4( customType1, "animPropName2",    animatablePropertyIndex, Property::BOOLEAN );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "! \"Property index already added", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliPropertyRegistrationPropertyWritableP(void)
{
  TestApplication application;
  int propertyIndex1 = PROPERTY_REGISTRATION_START_INDEX + 200;
  int propertyIndex2 = PROPERTY_REGISTRATION_START_INDEX + 201;

  // Add two properties, one with SetProperty, one without
  PropertyRegistration property1( customType1, "propNameReadwrite",   propertyIndex1, Property::BOOLEAN, &SetProperty, &GetProperty );
  PropertyRegistration property2( customType1, "propNameReadonly",    propertyIndex2, Property::BOOLEAN, NULL, &GetProperty );

  // Create custom-actor
  TypeInfo typeInfo = TypeRegistry::Get().GetTypeInfo( typeid(MyTestCustomActor) );
  DALI_TEST_CHECK( typeInfo );
  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( handle );
  Actor customActor = Actor::DownCast( handle );
  DALI_TEST_CHECK( customActor );

  // Check whether properties are writable
  DALI_TEST_CHECK(   customActor.IsPropertyWritable( propertyIndex1 ) );
  DALI_TEST_CHECK( ! customActor.IsPropertyWritable( propertyIndex2 ) );


  // Check the property is writable in the type registry
  Internal::TypeInfo& typeInfoImpl = GetImplementation( typeInfo );

  DALI_TEST_EQUALS( typeInfoImpl.IsPropertyWritable( propertyIndex1 ), true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPropertyRegistrationPropertyWritableN(void)
{
  // Currently Actors don't register properties with the type registry

  TypeInfo typeInfo = TypeRegistry::Get().GetTypeInfo( typeid(MyTestCustomActor) );
  Internal::TypeInfo& typeInfoImpl = GetImplementation( typeInfo );

  try
  {
    typeInfoImpl.IsPropertyWritable( Actor::Property::COLOR);
    tet_result( TET_FAIL );

  }
  catch ( DaliException& e )
  {
     DALI_TEST_ASSERT( e, "Cannot find property index", TEST_LOCATION );
  }
  END_TEST;

}
int UtcDaliPropertyRegistrationPropertyAnimatable(void)
{
  TestApplication application;
  int propertyIndex = PROPERTY_REGISTRATION_START_INDEX + 400;
  int animatablePropertyIndex = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 400;

  // These properties are not animatable
  PropertyRegistration property1( customType1, "propName",  propertyIndex, Property::BOOLEAN, &SetProperty, &GetProperty );

  // These properties are animatable
  AnimatablePropertyRegistration property2( customType1, "animPropName",   animatablePropertyIndex, Property::BOOLEAN );

  // Create custom-actor
  TypeInfo typeInfo = TypeRegistry::Get().GetTypeInfo( typeid(MyTestCustomActor) );
  DALI_TEST_CHECK( typeInfo );
  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( handle );
  Actor customActor = Actor::DownCast( handle );
  DALI_TEST_CHECK( customActor );

  // Check if animatable
  DALI_TEST_CHECK( ! customActor.IsPropertyAnimatable( propertyIndex ) );
  DALI_TEST_CHECK( customActor.IsPropertyAnimatable( animatablePropertyIndex ) );

  // Create another instance of custom-actor
  BaseHandle handle2 = typeInfo.CreateInstance();
  DALI_TEST_CHECK( handle2 );
  Actor customActor2 = Actor::DownCast( handle2 );
  DALI_TEST_CHECK( customActor2 );

  // Check if animatable
  DALI_TEST_CHECK( ! customActor2.IsPropertyAnimatable( propertyIndex ) );
  DALI_TEST_CHECK( customActor2.IsPropertyAnimatable( animatablePropertyIndex ) );
  END_TEST;
}

int UtcDaliPropertyRegistrationInvalidGetAndSet(void)
{
  TestApplication application;
  int propertyIndex = PROPERTY_REGISTRATION_START_INDEX + 2000;
  int animatablePropertyIndex = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 2000;

  // Create custom-actor
  TypeInfo typeInfo = TypeRegistry::Get().GetTypeInfo( typeid(MyTestCustomActor) );
  DALI_TEST_CHECK( typeInfo );
  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( handle );
  Actor customActor = Actor::DownCast( handle );
  DALI_TEST_CHECK( customActor );

  // Try to set an index that hasn't been added
  try
  {
    customActor.SetProperty( propertyIndex, true );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "! \"Cannot find property index", TEST_LOCATION );
  }

  try
  {
    customActor.SetProperty( animatablePropertyIndex, true );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "! \"Cannot find property index", TEST_LOCATION );
  }

  // Try to get an index that hasn't been added
  try
  {
    (void) customActor.GetProperty< bool >( propertyIndex );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "! \"Cannot find property index", TEST_LOCATION );
  }

  try
  {
    (void) customActor.GetProperty< bool >( animatablePropertyIndex );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "! \"Cannot find property index", TEST_LOCATION );
  }
  END_TEST;
}


int UtcDaliLongPressGestureDetectorTypeRegistry(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo( "LongPressGestureDetector" );
  DALI_TEST_CHECK( type );
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );
  LongPressGestureDetector detector = LongPressGestureDetector::DownCast( handle );
  DALI_TEST_CHECK( detector );

  // Attach actor to detector
  SignalData data;
  GestureReceivedFunctor functor( data );
  detector.Attach(actor);

  // Connect to signal through type
  handle.ConnectSignal( &application, "longPressDetected",   functor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit gesture
  application.ProcessEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(50.0f, 10.0f)));
  application.ProcessEvent(GenerateLongPress(Gesture::Started, 1u, Vector2(50.0f, 10.0f)));
  application.ProcessEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.voidFunctorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureDetectorTypeRegistry(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo( "PanGestureDetector" );
  DALI_TEST_CHECK( type );
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );
  PanGestureDetector detector = PanGestureDetector::DownCast( handle );
  DALI_TEST_CHECK( detector );

  // Attach actor to detector
  SignalData data;
  GestureReceivedFunctor functor( data );
  detector.Attach(actor);

  // Connect to signal through type
  handle.ConnectSignal( &application, "panDetected",  functor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit gesture
  application.ProcessEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  application.ProcessEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  application.ProcessEvent(GeneratePan(Gesture::Finished, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.voidFunctorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureDetectorTypeRegistry(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo( "PinchGestureDetector" );
  DALI_TEST_CHECK( type );
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );
  PinchGestureDetector detector = PinchGestureDetector::DownCast( handle );
  DALI_TEST_CHECK( detector );

  // Attach actor to detector
  SignalData data;
  GestureReceivedFunctor functor( data );
  detector.Attach(actor);

  // Connect to signal through type
  handle.ConnectSignal( &application, "pinchDetected",  functor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit gesture
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.voidFunctorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureDetectorTypeRegistry(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo( "TapGestureDetector" );
  DALI_TEST_CHECK( type );
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );
  TapGestureDetector detector = TapGestureDetector::DownCast( handle );
  DALI_TEST_CHECK( detector );

  // Attach actor to detector
  SignalData data;
  GestureReceivedFunctor functor( data );
  detector.Attach(actor);

  // Connect to signal through type
  handle.ConnectSignal( &application, "tapDetected",  functor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit gesture
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(50.0f, 10.0f)));
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.voidFunctorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTypeRegistryNamedType(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  // Create a normal actor
  BaseHandle actorHandle = typeRegistry.GetTypeInfo( "Actor" ).CreateInstance();
  DALI_TEST_CHECK( actorHandle );
  Actor actor( Actor::DownCast( actorHandle ) );
  DALI_TEST_CHECK( actor );
  unsigned int actorPropertyCount( actor.GetPropertyCount() );

  // Create Named Actor Type
  BaseHandle namedHandle = typeRegistry.GetTypeInfo( "MyNamedActor" ).CreateInstance();
  DALI_TEST_CHECK( namedHandle );
  Actor namedActor( Actor::DownCast( namedHandle ) );
  DALI_TEST_CHECK( namedActor );
  unsigned int namedActorPropertyCount( namedActor.GetPropertyCount() );

  DALI_TEST_CHECK( namedActorPropertyCount > actorPropertyCount );
  END_TEST;
}

int UtcDaliTypeInfoGetActionNameP(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  TypeInfo typeInfo = typeRegistry.GetTypeInfo( "Actor" );
  DALI_TEST_CHECK( typeInfo );

  DALI_TEST_CHECK( 0 != typeInfo.GetActionCount() );

  std::string name = typeInfo.GetActionName(0);

  DALI_TEST_EQUALS( name, "show", TEST_LOCATION );


  TypeInfo typeInfo2 = typeRegistry.GetTypeInfo( "MyTestCustomActor" );

  //  search for show action in base class, given a derived class
  bool foundChildAction = false;
  for( std::size_t i = 0; i < typeInfo2.GetActionCount(); i++ )
  {

       std::string name = typeInfo2.GetActionName( i );
       if( name == "show")
       {
         foundChildAction = true;
       }

  }

  DALI_TEST_EQUALS( foundChildAction, true, TEST_LOCATION );


  END_TEST;
}

int UtcDaliTypeInfoGetActionNameN(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  TypeInfo typeInfo = typeRegistry.GetTypeInfo( "Actor" );
  DALI_TEST_CHECK( typeInfo );

  DALI_TEST_CHECK( 0 != typeInfo.GetActionCount() );

  std::string name = typeInfo.GetActionName(std::numeric_limits<size_t>::max());

  DALI_TEST_EQUALS( 0u, name.size(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliTypeInfoGetSignalNameP(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  TypeInfo typeInfo = typeRegistry.GetTypeInfo( "Actor" );
  DALI_TEST_CHECK( typeInfo );

  DALI_TEST_CHECK( 0 != typeInfo.GetSignalCount() );

  std::string name = typeInfo.GetSignalName(0);

  DALI_TEST_EQUALS( name, "touched", TEST_LOCATION );

  TypeInfo typeInfo2 = typeRegistry.GetTypeInfo( "MyTestCustomActor" );

  //  search for signal in base class, given a derived class
  bool foundSignal = false;
  for( std::size_t i = 0; i < typeInfo2.GetSignalCount(); i++ )
  {

       std::string name = typeInfo2.GetSignalName( i );
       if( name == "touched")
       {
         foundSignal = true;
       }

  }

  DALI_TEST_EQUALS( foundSignal, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTypeInfoGetSignalNameN(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  TypeInfo typeInfo = typeRegistry.GetTypeInfo( "Actor" );
  DALI_TEST_CHECK( typeInfo );

  DALI_TEST_CHECK( 0 != typeInfo.GetSignalCount() );

  std::string name = typeInfo.GetSignalName(std::numeric_limits<size_t>::max());

  DALI_TEST_EQUALS( 0u, name.size(), TEST_LOCATION );

  END_TEST;
}


int UtcDaliTypeInfoGetCreatorP(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  TypeInfo typeInfo = typeRegistry.GetTypeInfo( "Actor" );
  DALI_TEST_CHECK( typeInfo );

  TypeInfo::CreateFunction createFn = typeInfo.GetCreator();
  DALI_TEST_EQUALS( createFn != NULL, true, TEST_LOCATION );
  if( createFn )
  {
    // try calling it:
    BaseHandle handle = createFn();
    DALI_TEST_EQUALS( (bool)handle, true, TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliTypeInfoGetCreatorN(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  TypeInfo typeInfo = typeRegistry.GetTypeInfo( "MyTestCustomActor3" );
  DALI_TEST_CHECK( typeInfo );

  TypeInfo::CreateFunction createFn = typeInfo.GetCreator();
  DALI_TEST_EQUALS( createFn == NULL, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTypeInfoGetPropertyCountP1(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  TypeInfo typeInfo = typeRegistry.GetTypeInfo( "Actor" );
  DALI_TEST_CHECK( typeInfo );
  size_t actorPropertyCount = typeInfo.GetPropertyCount();

  DALI_TEST_EQUALS( actorPropertyCount == 0 , true, TEST_LOCATION ); // No event only props
  END_TEST;
}

int UtcDaliTypeInfoGetPropertyCountP2(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  TypeInfo typeInfo = typeRegistry.GetTypeInfo( "MyTestCustomActor2" );
  DALI_TEST_CHECK( typeInfo );
  size_t propertyCount = typeInfo.GetPropertyCount();
  Property::IndexContainer indices;
  typeInfo.GetPropertyIndices( indices );

  DALI_TEST_EQUALS( propertyCount > 0 && propertyCount <= indices.Size(), true, TEST_LOCATION );
  DALI_TEST_EQUALS( propertyCount == 2, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPropertyRegistrationPropertyAnimatableSynchronousSetGet01(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  tet_infoline( "Register a type registered animatable property and ensure set/get behaviour works synchronously" );

  // Register animatable property
  const int animatablePropertyIndex( ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX );
  AnimatablePropertyRegistration animatableProperty( customType1, "animatableProp1", animatablePropertyIndex, Property::FLOAT );

  // Check property count before property registration
  TypeInfo typeInfo = typeRegistry.GetTypeInfo( typeid(MyTestCustomActor) );
  DALI_TEST_CHECK( typeInfo );
  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( handle );
  Actor customActor = Actor::DownCast( handle );
  DALI_TEST_CHECK( customActor );
  Stage::GetCurrent().Add(customActor);

  tet_infoline( "Set the value and ensure it changes straight away" );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( animatablePropertyIndex ), 0.0f, TEST_LOCATION );
  customActor.SetProperty( animatablePropertyIndex, 25.0f );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( animatablePropertyIndex ), 25.0f, TEST_LOCATION );

  tet_infoline( "Check latest scene-graph value is unchanged" );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( animatablePropertyIndex ), 0.0f, TEST_LOCATION );

  // Render and notify
  application.SendNotification();
  application.Render();

  tet_infoline( "Check values after rendering and both retrieval methods should return the latest" );

  DALI_TEST_EQUALS( customActor.GetProperty< float >( animatablePropertyIndex ), 25.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( animatablePropertyIndex ), 25.0f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPropertyRegistrationPropertyAnimatableSynchronousSetGetWithComponentsVector2(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  tet_infoline( "Register a type registered animatable property that has component indices and ensure set/get behaviour works synchronously and is the same regardless of how the property is set" );

  // Register the animatable propeties
  const int basePropertyIndex( ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX );
  const int componentZeroPropertyIndex( basePropertyIndex + 1 );
  const int componentOnePropertyIndex( componentZeroPropertyIndex + 1 );
  AnimatablePropertyRegistration baseAnimatableProperty( customType1, "baseProp", basePropertyIndex, Vector2( 13.0f, 24.0f ) );
  AnimatablePropertyComponentRegistration componentZeroAnimatableProperty( customType1, "componentZeroProp", componentZeroPropertyIndex, basePropertyIndex, 0 );
  AnimatablePropertyComponentRegistration componentOneAnimatableProperty( customType1, "componentOneProp", componentOnePropertyIndex, basePropertyIndex, 1 );

  // Check property count before property registration
  TypeInfo typeInfo = typeRegistry.GetTypeInfo( typeid(MyTestCustomActor) );
  DALI_TEST_CHECK( typeInfo );
  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( handle );
  Actor customActor = Actor::DownCast( handle );
  DALI_TEST_CHECK( customActor );
  Stage::GetCurrent().Add(customActor);

  tet_infoline( "Get the component values, they should be the default value of the base-property" );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentZeroPropertyIndex ), 13.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentOnePropertyIndex ), 24.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< Vector2 >( basePropertyIndex ), Vector2( 13.0f, 24.0f ), TEST_LOCATION );

  tet_infoline( "Set a component value and ensure it changes for the base property as well" );
  customActor.SetProperty( componentZeroPropertyIndex, 125.0f );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentZeroPropertyIndex ), 125.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< Vector2 >( basePropertyIndex ), Vector2( 125.0f, 24.0f ), TEST_LOCATION );

  customActor.SetProperty( componentOnePropertyIndex, 225.0f );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentOnePropertyIndex ), 225.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< Vector2 >( basePropertyIndex ), Vector2( 125.0f, 225.0f ), TEST_LOCATION );

  tet_infoline( "Check latest scene-graph value is unchanged" );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< Vector2 >( basePropertyIndex ), Vector2( 13.0f, 24.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( componentZeroPropertyIndex ), 13.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( componentOnePropertyIndex ), 24.0f, TEST_LOCATION );

  // Render and notify
  application.SendNotification();
  application.Render();

  tet_infoline( "Check values after rendering and both retrieval methods should return the latest" );
  DALI_TEST_EQUALS( customActor.GetProperty< Vector2 >( basePropertyIndex ), Vector2( 125.0f, 225.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentZeroPropertyIndex ), 125.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentOnePropertyIndex ), 225.0f, TEST_LOCATION );

  DALI_TEST_EQUALS( customActor.GetCurrentProperty< Vector2 >( basePropertyIndex ), Vector2( 125.0f, 225.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( componentZeroPropertyIndex ), 125.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( componentOnePropertyIndex ), 225.0f, TEST_LOCATION );

  tet_infoline( "Set the base property value and ensure the component values reflect the change" );
  customActor.SetProperty( basePropertyIndex, Vector2( 1.0f, 2.0f ) );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentZeroPropertyIndex ), 1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentOnePropertyIndex ), 2.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< Vector2 >( basePropertyIndex ), Vector2( 1.0f, 2.0f ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliPropertyRegistrationPropertyAnimatableSynchronousSetGetWithComponentsVector3(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  tet_infoline( "Register a type registered animatable property that has component indices and ensure set/get behaviour works synchronously and is the same regardless of how the property is set" );

  // Register the animatable propeties
  const int basePropertyIndex( ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX );
  const int componentZeroPropertyIndex( basePropertyIndex + 1 );
  const int componentOnePropertyIndex( componentZeroPropertyIndex + 1 );
  const int componentTwoPropertyIndex( componentOnePropertyIndex + 1 );
  AnimatablePropertyRegistration baseAnimatableProperty( customType1, "baseProp", basePropertyIndex, Vector3( 13.0f, 24.0f, 35.0 ) );
  AnimatablePropertyComponentRegistration componentZeroAnimatableProperty( customType1, "componentZeroProp", componentZeroPropertyIndex, basePropertyIndex, 0 );
  AnimatablePropertyComponentRegistration componentOneAnimatableProperty( customType1, "componentOneProp", componentOnePropertyIndex, basePropertyIndex, 1 );
  AnimatablePropertyComponentRegistration componentTwoAnimatableProperty( customType1, "componentTwoProp", componentTwoPropertyIndex, basePropertyIndex, 2 );

  // Check property count before property registration
  TypeInfo typeInfo = typeRegistry.GetTypeInfo( typeid(MyTestCustomActor) );
  DALI_TEST_CHECK( typeInfo );
  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( handle );
  Actor customActor = Actor::DownCast( handle );
  DALI_TEST_CHECK( customActor );
  Stage::GetCurrent().Add(customActor);

  tet_infoline( "Get the component values, they should be the default value of the base-property" );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentZeroPropertyIndex ), 13.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentOnePropertyIndex ), 24.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentTwoPropertyIndex ), 35.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< Vector3 >( basePropertyIndex ), Vector3( 13.0f, 24.0f, 35.0f ), TEST_LOCATION );

  tet_infoline( "Set a component value and ensure it changes for the base property as well" );
  customActor.SetProperty( componentZeroPropertyIndex, 125.0f );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentZeroPropertyIndex ), 125.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< Vector3 >( basePropertyIndex ), Vector3( 125.0f, 24.0f, 35.0f ), TEST_LOCATION );

  customActor.SetProperty( componentOnePropertyIndex, 225.0f );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentOnePropertyIndex ), 225.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< Vector3 >( basePropertyIndex ), Vector3( 125.0f, 225.0f, 35.0f ), TEST_LOCATION );

  customActor.SetProperty( componentTwoPropertyIndex, 325.0f );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentTwoPropertyIndex ), 325.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< Vector3 >( basePropertyIndex ), Vector3( 125.0f, 225.0f, 325.0f ), TEST_LOCATION );

  tet_infoline( "Check latest scene-graph value is unchanged" );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< Vector3 >( basePropertyIndex ), Vector3( 13.0f, 24.0f, 35.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( componentZeroPropertyIndex ), 13.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( componentOnePropertyIndex ), 24.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( componentTwoPropertyIndex ), 35.0f, TEST_LOCATION );

  // Render and notify
  application.SendNotification();
  application.Render();

  tet_infoline( "Check values after rendering and both retrieval methods should return the latest" );
  DALI_TEST_EQUALS( customActor.GetProperty< Vector3 >( basePropertyIndex ), Vector3( 125.0f, 225.0f, 325.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentZeroPropertyIndex ), 125.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentOnePropertyIndex ), 225.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentTwoPropertyIndex ), 325.0f, TEST_LOCATION );

  DALI_TEST_EQUALS( customActor.GetCurrentProperty< Vector3 >( basePropertyIndex ), Vector3( 125.0f, 225.0f, 325.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( componentZeroPropertyIndex ), 125.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( componentOnePropertyIndex ), 225.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( componentTwoPropertyIndex ), 325.0f, TEST_LOCATION );

  tet_infoline( "Set the base property value and ensure the component values reflect the change" );
  customActor.SetProperty( basePropertyIndex, Vector3( 1.0f, 2.0f, 3.0f ) );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentZeroPropertyIndex ), 1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentOnePropertyIndex ), 2.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentTwoPropertyIndex ), 3.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< Vector3 >( basePropertyIndex ), Vector3( 1.0f, 2.0f, 3.0f ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliPropertyRegistrationPropertyAnimatableSynchronousSetGetWithComponentsVector4(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  tet_infoline( "Register a type registered animatable property that has component indices and ensure set/get behaviour works synchronously and is the same regardless of how the property is set" );

  // Register the animatable propeties
  const int basePropertyIndex( ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX );
  const int componentZeroPropertyIndex( basePropertyIndex + 1 );
  const int componentOnePropertyIndex( componentZeroPropertyIndex + 1 );
  const int componentTwoPropertyIndex( componentOnePropertyIndex + 1 );
  const int componentThreePropertyIndex( componentTwoPropertyIndex + 1 );
  AnimatablePropertyRegistration baseAnimatableProperty( customType1, "baseProp", basePropertyIndex, Vector4( 13.0f, 24.0f, 35.0, 47.0f ) );
  AnimatablePropertyComponentRegistration componentZeroAnimatableProperty( customType1, "componentZeroProp", componentZeroPropertyIndex, basePropertyIndex, 0 );
  AnimatablePropertyComponentRegistration componentOneAnimatableProperty( customType1, "componentOneProp", componentOnePropertyIndex, basePropertyIndex, 1 );
  AnimatablePropertyComponentRegistration componentTwoAnimatableProperty( customType1, "componentTwoProp", componentTwoPropertyIndex, basePropertyIndex, 2 );
  AnimatablePropertyComponentRegistration componentThreeAnimatableProperty( customType1, "componentThreeProp", componentThreePropertyIndex, basePropertyIndex, 3 );

  // Check property count before property registration
  TypeInfo typeInfo = typeRegistry.GetTypeInfo( typeid(MyTestCustomActor) );
  DALI_TEST_CHECK( typeInfo );
  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( handle );
  Actor customActor = Actor::DownCast( handle );
  DALI_TEST_CHECK( customActor );
  Stage::GetCurrent().Add(customActor);

  tet_infoline( "Get the component values, they should be the default value of the base-property" );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentZeroPropertyIndex ), 13.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentOnePropertyIndex ), 24.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentTwoPropertyIndex ), 35.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentThreePropertyIndex ), 47.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< Vector4 >( basePropertyIndex ), Vector4( 13.0f, 24.0f, 35.0f, 47.0f ), TEST_LOCATION );

  tet_infoline( "Set a component value and ensure it changes for the base property as well" );
  customActor.SetProperty( componentZeroPropertyIndex, 125.0f );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentZeroPropertyIndex ), 125.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< Vector4 >( basePropertyIndex ), Vector4( 125.0f, 24.0f, 35.0f, 47.0f ), TEST_LOCATION );

  customActor.SetProperty( componentOnePropertyIndex, 225.0f );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentOnePropertyIndex ), 225.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< Vector4 >( basePropertyIndex ), Vector4( 125.0f, 225.0f, 35.0f, 47.0f ), TEST_LOCATION );

  customActor.SetProperty( componentTwoPropertyIndex, 325.0f );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentTwoPropertyIndex ), 325.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< Vector4 >( basePropertyIndex ), Vector4( 125.0f, 225.0f, 325.0f, 47.0f ), TEST_LOCATION );

  customActor.SetProperty( componentThreePropertyIndex, 435.0f );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentThreePropertyIndex ), 435.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< Vector4 >( basePropertyIndex ), Vector4( 125.0f, 225.0f, 325.0f, 435.0f ), TEST_LOCATION );

  tet_infoline( "Check latest scene-graph value is unchanged" );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< Vector4 >( basePropertyIndex ), Vector4( 13.0f, 24.0f, 35.0f, 47.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( componentZeroPropertyIndex ), 13.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( componentOnePropertyIndex ), 24.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( componentTwoPropertyIndex ), 35.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( componentThreePropertyIndex ), 47.0f, TEST_LOCATION );

  // Render and notify
  application.SendNotification();
  application.Render();

  tet_infoline( "Check values after rendering and both retrieval methods should return the latest" );
  DALI_TEST_EQUALS( customActor.GetProperty< Vector4 >( basePropertyIndex ), Vector4( 125.0f, 225.0f, 325.0f, 435.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentZeroPropertyIndex ), 125.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentOnePropertyIndex ), 225.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentTwoPropertyIndex ), 325.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentThreePropertyIndex ), 435.0f, TEST_LOCATION );

  DALI_TEST_EQUALS( customActor.GetCurrentProperty< Vector4 >( basePropertyIndex ), Vector4( 125.0f, 225.0f, 325.0f, 435.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( componentZeroPropertyIndex ), 125.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( componentOnePropertyIndex ), 225.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( componentTwoPropertyIndex ), 325.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetCurrentProperty< float >( componentThreePropertyIndex ), 435.0f, TEST_LOCATION );

  tet_infoline( "Set the base property value and ensure the component values reflect the change" );
  customActor.SetProperty( basePropertyIndex, Vector4( 1.0f, 2.0f, 3.0f, 4.0f ) );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentZeroPropertyIndex ), 1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentOnePropertyIndex ), 2.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentTwoPropertyIndex ), 3.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( componentThreePropertyIndex ), 4.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< Vector4 >( basePropertyIndex ), Vector4( 1.0f, 2.0f, 3.0f, 4.0f ), TEST_LOCATION );

  END_TEST;
}


int UtcDaliTypeInfoRegisterChildProperties01(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  tet_infoline( "Register child properties on a type via name" );

  auto customActorTypeInfo = typeRegistry.GetTypeInfo( typeid(CustomActor) );
  auto myCustomTypeInfo = typeRegistry.GetTypeInfo( typeid(MyTestCustomActor) );
  DALI_TEST_CHECK( customActorTypeInfo );
  DALI_TEST_CHECK( myCustomTypeInfo );

  const Property::Index WIDTH_SPECIFICATION( CHILD_PROPERTY_REGISTRATION_START_INDEX );
  const Property::Index HEIGHT_SPECIFICATION( CHILD_PROPERTY_REGISTRATION_START_INDEX + 1);
  const Property::Index MARGIN_SPECIFICATION( CHILD_PROPERTY_REGISTRATION_START_INDEX + 100);

  ChildPropertyRegistration( customActorTypeInfo.GetName(), "widthSpecification", WIDTH_SPECIFICATION, Property::INTEGER );
  ChildPropertyRegistration( customActorTypeInfo.GetName(), "heightSpecification", HEIGHT_SPECIFICATION, Property::INTEGER );
  ChildPropertyRegistration( myCustomTypeInfo.GetName(), "marginSpecification", MARGIN_SPECIFICATION, Property::EXTENTS );

  auto customActor = MyTestCustomActor::New();
  Stage::GetCurrent().Add( customActor );
  auto child = Actor::New();
  customActor.Add( child );

  child.SetProperty( WIDTH_SPECIFICATION, 33 );

  auto value = child.GetProperty( WIDTH_SPECIFICATION );
  DALI_TEST_EQUALS( value, Property::Value(33), TEST_LOCATION );

  child.SetProperty( HEIGHT_SPECIFICATION, 44 );
  value = child.GetProperty( HEIGHT_SPECIFICATION );
  DALI_TEST_EQUALS( value, Property::Value(44), TEST_LOCATION );

  child.SetProperty( MARGIN_SPECIFICATION, Extents(10, 10, 10, 10) );
  value = child.GetProperty( MARGIN_SPECIFICATION );
  DALI_TEST_EQUALS( value, Property::Value(Extents(10,10,10,10)), TEST_LOCATION );

  END_TEST;
}


int UtcDaliTypeInfoRegisterChildProperties02(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  tet_infoline( "Register child properties on a type via name" );

  auto customActorTypeInfo = typeRegistry.GetTypeInfo( typeid(CustomActor) );
  auto myCustomTypeInfo = typeRegistry.GetTypeInfo( typeid(MyTestCustomActor) );
  DALI_TEST_CHECK( customActorTypeInfo );
  DALI_TEST_CHECK( myCustomTypeInfo );

  const Property::Index WIDTH_SPECIFICATION( CHILD_PROPERTY_REGISTRATION_START_INDEX );
  const Property::Index HEIGHT_SPECIFICATION( CHILD_PROPERTY_REGISTRATION_START_INDEX + 1);
  const Property::Index MARGIN_SPECIFICATION( CHILD_PROPERTY_REGISTRATION_START_INDEX + 100);

  ChildPropertyRegistration( customActorTypeInfo.GetName(), "widthSpecification", WIDTH_SPECIFICATION, Property::INTEGER );
  ChildPropertyRegistration( customActorTypeInfo.GetName(), "heightSpecification", HEIGHT_SPECIFICATION, Property::INTEGER );
  ChildPropertyRegistration( myCustomTypeInfo.GetName(), "marginSpecification", MARGIN_SPECIFICATION, Property::EXTENTS );


  auto index = customActorTypeInfo.GetChildPropertyIndex( "widthSpecification" );
  DALI_TEST_EQUALS( index, WIDTH_SPECIFICATION, TEST_LOCATION );

  index = customActorTypeInfo.GetChildPropertyIndex( "heightSpecification" );
  DALI_TEST_EQUALS( index, HEIGHT_SPECIFICATION, TEST_LOCATION );

  index = customActorTypeInfo.GetChildPropertyIndex( "marginSpecification" );
  DALI_TEST_EQUALS( index, Property::INVALID_INDEX, TEST_LOCATION );

  index = myCustomTypeInfo.GetChildPropertyIndex( "marginSpecification" );
  DALI_TEST_EQUALS( index, MARGIN_SPECIFICATION, TEST_LOCATION );


  auto name = customActorTypeInfo.GetChildPropertyName( WIDTH_SPECIFICATION );
  DALI_TEST_EQUALS( name, "widthSpecification", TEST_LOCATION );

  name = customActorTypeInfo.GetChildPropertyName( HEIGHT_SPECIFICATION );
  DALI_TEST_EQUALS( name, "heightSpecification", TEST_LOCATION );

  name = myCustomTypeInfo.GetChildPropertyName( MARGIN_SPECIFICATION );
  DALI_TEST_EQUALS( name, "marginSpecification", TEST_LOCATION );


  auto type = customActorTypeInfo.GetChildPropertyType( WIDTH_SPECIFICATION );
  DALI_TEST_EQUALS( type, Property::INTEGER, TEST_LOCATION );

  type = customActorTypeInfo.GetChildPropertyType( HEIGHT_SPECIFICATION );
  DALI_TEST_EQUALS( type, Property::INTEGER, TEST_LOCATION );

  type = myCustomTypeInfo.GetChildPropertyType( MARGIN_SPECIFICATION );
  DALI_TEST_EQUALS( type, Property::EXTENTS, TEST_LOCATION );


  END_TEST;
}


int UtcDaliTypeInfoRegisterChildProperties03(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  tet_infoline( "Check registered child properties can be retrieved" );

  auto customActorTypeInfo = typeRegistry.GetTypeInfo( typeid(CustomActor) );
  auto myCustomTypeInfo = typeRegistry.GetTypeInfo( typeid(MyTestCustomActor) );
  DALI_TEST_CHECK( customActorTypeInfo );
  DALI_TEST_CHECK( myCustomTypeInfo );

  const Property::Index WIDTH_SPECIFICATION( CHILD_PROPERTY_REGISTRATION_START_INDEX );
  const Property::Index HEIGHT_SPECIFICATION( CHILD_PROPERTY_REGISTRATION_START_INDEX + 1);
  const Property::Index MARGIN_SPECIFICATION( CHILD_PROPERTY_REGISTRATION_START_INDEX + 100);

  ChildPropertyRegistration( customActorTypeInfo.GetName(), "widthSpecification", WIDTH_SPECIFICATION, Property::INTEGER );
  ChildPropertyRegistration( customActorTypeInfo.GetName(), "heightSpecification", HEIGHT_SPECIFICATION, Property::INTEGER );
  ChildPropertyRegistration( myCustomTypeInfo.GetName(), "marginSpecification", MARGIN_SPECIFICATION, Property::EXTENTS );

  Property::IndexContainer indices;
  myCustomTypeInfo.GetChildPropertyIndices( indices );

  auto result = std::find( indices.Begin(), indices.End(), WIDTH_SPECIFICATION );
  DALI_TEST_EQUALS( result != indices.End(), true, TEST_LOCATION );

  result = std::find( indices.Begin(), indices.End(), HEIGHT_SPECIFICATION );
  DALI_TEST_EQUALS( result != indices.End(), true, TEST_LOCATION );

  result = std::find( indices.Begin(), indices.End(), MARGIN_SPECIFICATION );
  DALI_TEST_EQUALS( result != indices.End(), true, TEST_LOCATION );

  END_TEST;
}

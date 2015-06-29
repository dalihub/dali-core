/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
PropertyRegistration namedActorPropertyOne( namedActorType, "prop-name", PROPERTY_REGISTRATION_START_INDEX, Property::BOOLEAN, &SetProperty, &GetProperty );

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

  // image actor
  type = registry.GetTypeInfo( "ImageActor" );
  DALI_TEST_CHECK( type );
  DALI_TEST_CHECK( type.GetCreator() );
  DALI_TEST_CHECK( ImageActor::DownCast( type.GetCreator()() ) );
  ImageActor ia = ImageActor::DownCast(type.CreateInstance());
  DALI_TEST_CHECK( ia );
  Stage::GetCurrent().Add( ia );
  application.Render();

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

  // shader effect
  type = registry.GetTypeInfo( "ShaderEffect" );
  DALI_TEST_CHECK( type );
  ShaderEffect ef = ShaderEffect::DownCast(type.CreateInstance());
  DALI_TEST_CHECK( ef );
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
  TypeInfo named_type = TypeRegistry::Get().GetTypeInfo( "ImageActor" );
  TypeInfo typeinfo_type = TypeRegistry::Get().GetTypeInfo( typeid(Dali::ImageActor) );

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

// Note: No negative test case for UtcDaliTypeRegistryRegisteredName can be implemented.
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
  DALI_TEST_CHECK( !handle.DoAction( "unknown-action", attributes ) );

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

  std::string propertyName( "prop-1" );
  int propertyIndex( PROPERTY_REGISTRATION_START_INDEX );
  Property::Type propertyType( Property::BOOLEAN );
  PropertyRegistration property1( customType1, propertyName, propertyIndex, propertyType, &SetProperty, &GetProperty );

  // Check property count after registration
  unsigned int postRegistrationPropertyCount( customActor.GetPropertyCount() );
  DALI_TEST_EQUALS( initialPropertyCount + 1u, postRegistrationPropertyCount, TEST_LOCATION );

  // Add custom property and check property count
  customActor.RegisterProperty( "custom-prop-1", true );
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
  DALI_TEST_EQUALS( indices.Size(), 1u, TEST_LOCATION );

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
    PropertyRegistration property1( customType1, "prop-name", PROPERTY_REGISTRATION_START_INDEX - 1, Property::BOOLEAN, &SetProperty, &GetProperty );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "( index >= PROPERTY_REGISTRATION_START_INDEX ) && ( index <= PROPERTY_REGISTRATION_MAX_INDEX )", TEST_LOCATION );
  }

  // Attempt to register a property type out-of-bounds index (greater than)
  try
  {
    PropertyRegistration property1( customType1, "prop-name", PROPERTY_REGISTRATION_MAX_INDEX + 1, Property::BOOLEAN, &SetProperty, &GetProperty );
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

  unsigned int customPropertyCount( customActor.GetPropertyCount() );

  // Register animatable property
  std::string animatablePropertyName( "animatable-prop-1" );
  int animatablePropertyIndex( ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX );
  Property::Type animatablePropertyType( Property::FLOAT );
  AnimatablePropertyRegistration animatableProperty1( customType1, animatablePropertyName, animatablePropertyIndex, animatablePropertyType );

  // Check property count after registration
  DALI_TEST_EQUALS( customPropertyCount + 1u, customActor.GetPropertyCount(), TEST_LOCATION );

  // Set the animatable property value
  customActor.SetProperty( animatablePropertyIndex, 25.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Check the animatable property value
  DALI_TEST_EQUALS( customActor.GetProperty< float >( animatablePropertyIndex ), 25.0f, TEST_LOCATION );

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

  END_TEST;
}

int UtcDaliTypeRegistryAnimatablePropertyRegistrationN(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  // Attempt to register an animatable property type out-of-bounds index (less than)
  try
  {
    AnimatablePropertyRegistration property1( customType1, "anim-prop-name", ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX - 1, Property::BOOLEAN );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX )", TEST_LOCATION );
  }

  // Attempt to register an animatable property type out-of-bounds index (greater than)
  try
  {
    AnimatablePropertyRegistration property1( customType1, "anim-prop-name", ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX + 1, Property::BOOLEAN );
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
  std::string animatablePropertyName( "animatable-prop-1" );
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
  std::string animatablePropertyComponentName1( "animatable-prop-1-x" );
  int animatablePropertyComponentIndex1( ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 1 );
  AnimatablePropertyComponentRegistration animatablePropertyComponent1( customType1, animatablePropertyComponentName1, animatablePropertyComponentIndex1, animatablePropertyIndex, 0 );

  std::string animatablePropertyComponentName2( "animatable-prop-1-y" );
  int animatablePropertyComponentIndex2( ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 2 );
  AnimatablePropertyComponentRegistration animatablePropertyComponent2( customType1, animatablePropertyComponentName2, animatablePropertyComponentIndex2, animatablePropertyIndex, 1 );

  // Check property count after registration
  DALI_TEST_EQUALS( customPropertyCount + 3u, customActor.GetPropertyCount(), TEST_LOCATION );

  // Check the animatable property component value
  DALI_TEST_EQUALS( customActor.GetProperty< float >( animatablePropertyComponentIndex1 ), 25.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( animatablePropertyComponentIndex2 ), 50.0f, TEST_LOCATION );

  // Set the animatable property component value
  customActor.SetProperty( animatablePropertyComponentIndex1, 150.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Check the animatable property value
  DALI_TEST_EQUALS( customActor.GetProperty< Vector2 >( animatablePropertyIndex ), Vector2(150.0f, 50.0f), TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( animatablePropertyComponentIndex1 ), 150.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( animatablePropertyComponentIndex2 ), 50.0f, TEST_LOCATION );

  // Set the animatable property component value
  customActor.SetProperty( animatablePropertyComponentIndex2, 225.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Check the animatable property value
  DALI_TEST_EQUALS( customActor.GetProperty< Vector2 >( animatablePropertyIndex ), Vector2(150.0f, 225.0f), TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( animatablePropertyComponentIndex1 ), 150.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( customActor.GetProperty< float >( animatablePropertyComponentIndex2 ), 225.0f, TEST_LOCATION );

  // Ensure indices returned from actor and customActor differ by three
  Actor actor = Actor::New();
  actor.GetPropertyIndices( indices );
  unsigned int actorIndices = indices.Size();
  customActor.GetPropertyIndices( indices );
  unsigned int customActorIndices = indices.Size();
  DALI_TEST_EQUALS( actorIndices + 3u, customActorIndices, TEST_LOCATION ); // Custom property + registered property

  END_TEST;
}

int UtcDaliTypeRegistryAnimatablePropertyComponentRegistrationN(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  // Register animatable property with the type of Vector2
  int animatablePropertyIndex1( ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX );
  AnimatablePropertyRegistration animatableProperty1( customType1, "animatable-prop-1", animatablePropertyIndex1, Property::VECTOR2 );

  // Attempt to register an animatable property component out-of-bounds index (less than)
  try
  {
    AnimatablePropertyComponentRegistration propertyComponent1( customType1, "animatable-prop-1-x", ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX - 1, animatablePropertyIndex1, 0 );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX )", TEST_LOCATION );
  }

  // Attempt to register an animatable property component out-of-bounds index (greater than)
  try
  {
    AnimatablePropertyComponentRegistration propertyComponent1( customType1, "animatable-prop-1-x", ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX + 1, animatablePropertyIndex1, 0 );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX )", TEST_LOCATION );
  }

  // Register an animatable property component
  AnimatablePropertyComponentRegistration propertyComponent1( customType1, "animatable-prop-1-x", ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 1, animatablePropertyIndex1, 0 );

  // Attempt to register another animatable property component with the same component index
  try
  {
    AnimatablePropertyComponentRegistration propertyComponent2( customType1, "animatable-prop-1-y", ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 2, animatablePropertyIndex1, 0 );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "Property component already registered", TEST_LOCATION );
  }

  // Register animatable property with the type of boolean
  int animatablePropertyIndex2( ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 2 );
  AnimatablePropertyRegistration animatableProperty2( customType1, "animatable-prop-2", animatablePropertyIndex2, Property::BOOLEAN );

  // Attempt to register an animatable property component for the above property with boolean type
  try
  {
    AnimatablePropertyComponentRegistration propertyComponent1( customType1, "animatable-prop-2-x", animatablePropertyIndex2 + 1, animatablePropertyIndex2, 0 );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "Base property does not support component", TEST_LOCATION );
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

  DALI_TEST_CHECK(!hdl.DoAction("unknown-action", attributes));
  END_TEST;
}

int UtcDaliPropertyRegistrationFunctions(void)
{
  TestApplication application;
  int propertyIndex = PROPERTY_REGISTRATION_START_INDEX + 10;

  // Attempt to register a property without a setter
  try
  {
    PropertyRegistration property1( customType1, "prop-name", propertyIndex++, Property::BOOLEAN, NULL, &GetProperty );
    tet_result( TET_PASS );
  }
  catch ( DaliException& e )
  {
    tet_result( TET_FAIL );
  }

  // Attempt to register a property without a getter
  try
  {
    PropertyRegistration property1( customType1, "prop-name", propertyIndex++, Property::BOOLEAN, NULL, NULL );
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
  PropertyRegistration property1( customType1, "prop-name", propertyIndex, Property::BOOLEAN, &SetProperty, &GetProperty );

  // Attempt to add another property with the same index
  try
  {
    PropertyRegistration property2( customType1, "prop-name-2", propertyIndex, Property::BOOLEAN, &SetProperty, &GetProperty );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "! \"Property index already added", TEST_LOCATION );
  }

  int animatablePropertyIndex = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 100;

  // Add one property with a valid property index
  AnimatablePropertyRegistration property3( customType1, "anim-prop-name", animatablePropertyIndex, Property::BOOLEAN );

  // Attempt to add another property with the same index
  try
  {
    AnimatablePropertyRegistration property4( customType1, "anim-prop-name-2", animatablePropertyIndex, Property::BOOLEAN );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "! \"Property index already added", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliPropertyRegistrationPropertyWritable(void)
{
  TestApplication application;
  int propertyIndex1 = PROPERTY_REGISTRATION_START_INDEX + 200;
  int propertyIndex2 = PROPERTY_REGISTRATION_START_INDEX + 201;

  // Add two properties, one with SetProperty, one without
  PropertyRegistration property1( customType1, "prop-name-readwrite", propertyIndex1, Property::BOOLEAN, &SetProperty, &GetProperty );
  PropertyRegistration property2( customType1, "prop-name-readonly",  propertyIndex2, Property::BOOLEAN, NULL, &GetProperty );

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
  END_TEST;
}

int UtcDaliPropertyRegistrationPropertyAnimatable(void)
{
  TestApplication application;
  int propertyIndex = PROPERTY_REGISTRATION_START_INDEX + 400;
  int animatablePropertyIndex = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 400;

  // These properties are not animatable
  PropertyRegistration property1( customType1, "prop-name", propertyIndex, Property::BOOLEAN, &SetProperty, &GetProperty );

  // These properties are animatable
  AnimatablePropertyRegistration property2( customType1, "anim-prop-name", animatablePropertyIndex, Property::BOOLEAN );

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
  handle.ConnectSignal( &application, "long-press-detected", functor );

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
  handle.ConnectSignal( &application, "pan-detected", functor );

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
  handle.ConnectSignal( &application, "pinch-detected", functor );

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
  handle.ConnectSignal( &application, "tap-detected", functor );

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

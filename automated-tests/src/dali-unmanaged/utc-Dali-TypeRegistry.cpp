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
  virtual bool OnHoverEvent(const HoverEvent& event)
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

  virtual Vector3 GetNaturalSize()
  {
    return Vector3( 0.0f, 0.0f, 0.0f );
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

BaseHandle CreateNamedActorType()
{
  Actor actor = Actor::New();
  actor.SetName( "NamedActor" );
  return actor;
}

TypeRegistration namedActorType( "MyNamedActor", typeid(Dali::Actor), CreateNamedActorType );
PropertyRegistration namedActorPropertyOne( namedActorType, "prop-name", PROPERTY_REGISTRATION_START_INDEX, Property::BOOLEAN, &SetProperty, &GetProperty );

} // Anonymous namespace


// Positive test case for a method
int UtcDaliTypeRegistryCreateDaliObjects(void)
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

  std::vector<Property::Value> attributes;

  DALI_TEST_CHECK(hdl.DoAction(Actor::ACTION_SHOW, attributes));

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK(a.IsVisible());

  DALI_TEST_CHECK(!hdl.DoAction("unknown-action", attributes));
  END_TEST;
}

int UtcDaliTypeRegistryNames(void)
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

  END_TEST;
}

// Check named and typeid are equivalent
int UtcDaliTypeRegistryNameEquivalence(void)
{
  TypeInfo named_type = TypeRegistry::Get().GetTypeInfo( "TextActor" );
  TypeInfo typeinfo_type = TypeRegistry::Get().GetTypeInfo( typeid(Dali::TextActor) );

  DALI_TEST_CHECK( named_type );
  DALI_TEST_CHECK( typeinfo_type );

  DALI_TEST_CHECK( named_type == typeinfo_type );

  DALI_TEST_CHECK( named_type.GetName() == typeinfo_type.GetName() );
  DALI_TEST_CHECK( named_type.GetBaseName() == typeinfo_type.GetBaseName() );

  END_TEST;
}


int UtcDaliTypeRegistryCustomActor(void)
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

  TypeInfo::NameContainer names;
  type.GetActions(names);
  TypeInfo::NameContainer baseNames;
  baseType.GetActions(baseNames);
  DALI_TEST_EQUALS( names.size(), TEST_ACTION_COUNT + baseNames.size(), TEST_LOCATION );

  names.clear();
  type.GetSignals(names);

  baseNames.clear();
  baseType.GetSignals(baseNames);

  DALI_TEST_EQUALS( names.size(), TEST_SIGNAL_COUNT + baseNames.size(), TEST_LOCATION );

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
  END_TEST;
}

int UtcDaliTypeRegistryCustomSignalFailure(void)
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

  TypeInfo::NameContainer names;
  TypeInfo::NameContainer baseNames;

  type.GetActions(names);
  baseType.GetActions(baseNames);

  DALI_TEST_EQUALS( names.size(), TEST_ACTION_COUNT + baseNames.size(), TEST_LOCATION );

  names.clear();
  baseNames.clear();

  type.GetSignals(names);
  baseType.GetSignals(baseNames);

  DALI_TEST_EQUALS( names.size(), TEST_SIGNAL_COUNT + baseNames.size(), TEST_LOCATION );

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



int UtcDaliTypeRegistryInitFunctions(void)
{
  TestApplication application;

  DALI_TEST_CHECK( "MyTestCustomActor2" == customTypeInit.RegisteredName() );

  DALI_TEST_CHECK( true == CreateCustomInitCalled );
  TypeInfo type = TypeRegistry::Get().GetTypeInfo( "MyTestCustomActor2" );
  DALI_TEST_CHECK( type );
  END_TEST;
}




int UtcDaliTypeRegistryNameInitFunctions(void)
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


int UtcDaliPropertyRegistration(void)
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
  DALI_TEST_EQUALS( indices.size(), 1u, TEST_LOCATION );

  // Ensure indices returned from actor and customActor differ by two
  Actor actor = Actor::New();
  actor.GetPropertyIndices( indices );
  unsigned int actorIndices = indices.size();
  customActor.GetPropertyIndices( indices );
  unsigned int customActorIndices = indices.size();
  DALI_TEST_EQUALS( actorIndices + 2u, customActorIndices, TEST_LOCATION ); // Custom property + registered property
  END_TEST;
}

int UtcDaliPropertyRegistrationIndexOutOfBounds(void)
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

  // These properties are not animatable
  PropertyRegistration property1( customType1, "prop-name", propertyIndex, Property::BOOLEAN, &SetProperty, &GetProperty );

  // Create custom-actor
  TypeInfo typeInfo = TypeRegistry::Get().GetTypeInfo( typeid(MyTestCustomActor) );
  DALI_TEST_CHECK( typeInfo );
  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( handle );
  Actor customActor = Actor::DownCast( handle );
  DALI_TEST_CHECK( customActor );

  // Check if animatable
  DALI_TEST_CHECK( ! customActor.IsPropertyAnimatable( propertyIndex ) );
  END_TEST;
}

int UtcDaliPropertyRegistrationInvalidGetAndSet(void)
{
  TestApplication application;
  int propertyIndex = PROPERTY_REGISTRATION_START_INDEX + 2000;

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
  handle.ConnectSignal( &application, LongPressGestureDetector::SIGNAL_LONG_PRESS_DETECTED, functor );

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
  handle.ConnectSignal( &application, PanGestureDetector::SIGNAL_PAN_DETECTED, functor );

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
  handle.ConnectSignal( &application, PinchGestureDetector::SIGNAL_PINCH_DETECTED, functor );

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
  handle.ConnectSignal( &application, TapGestureDetector::SIGNAL_TAP_DETECTED, functor );

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

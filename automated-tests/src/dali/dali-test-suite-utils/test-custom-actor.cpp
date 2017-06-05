#include "test-custom-actor.h"

using namespace Dali;

std::vector< std::string > MasterCallStack;
bool gOnRelayout = false;

namespace Test
{
namespace Impl
{
struct TestCustomActor;
}


TestCustomActor TestCustomActor::New()
{
  Impl::TestCustomActor* impl = new Impl::TestCustomActor;
  TestCustomActor custom( *impl ); // takes ownership

  impl->Initialize();

  return custom;
}

TestCustomActor TestCustomActor::NewNegoSize()
{
  Impl::TestCustomActor* impl = new Impl::TestCustomActor( true );
  TestCustomActor custom( *impl ); // takes ownership
  custom.SetName( "SizeNegotiationActor" );

  impl->Initialize();

  return custom;
}

TestCustomActor TestCustomActor::NewVariant1( Actor childToAdd )
{
  Impl::TestCustomActor* impl = new Impl::TestCustomActorVariant1( childToAdd );
  TestCustomActor custom( *impl ); // takes ownership

  impl->Initialize();

  return custom;
}

TestCustomActor TestCustomActor::NewVariant2()
{
  Impl::TestCustomActor* impl = new Impl::TestCustomActorVariant2();
  TestCustomActor custom( *impl ); // takes ownership

  impl->Initialize();

  return custom;
}

TestCustomActor TestCustomActor::NewVariant3( Actor childToAdd )
{
  Impl::TestCustomActor* impl = new Impl::TestCustomActorVariant3( childToAdd );
  TestCustomActor custom( *impl ); // takes ownership

  impl->Initialize();

  return custom;
}

TestCustomActor TestCustomActor::NewVariant4()
{
  Impl::TestCustomActor* impl = new Impl::TestCustomActorVariant4();
  TestCustomActor custom( *impl ); // takes ownership

  impl->Initialize();

  return custom;
}

TestCustomActor TestCustomActor::NewVariant5()
{
  Impl::TestCustomActor* impl = new Impl::TestCustomActorVariant5();
  TestCustomActor custom( *impl ); // takes ownership

  impl->Initialize();

  return custom;
}

TestCustomActor TestCustomActor::NewVariant6()
{
  Impl::TestCustomActor* impl = new Impl::TestCustomActorVariant6();
  TestCustomActor custom( *impl ); // takes ownership

  impl->Initialize();

  return custom;
}

TestCustomActor TestCustomActor::NewVariant7( const char* name )
{
  Impl::TestCustomActor* impl = new Impl::TestCustomActorVariant7();
  TestCustomActor custom( *impl ); // takes ownership

  impl->Initialize( name );

  return custom;
}

TestCustomActor TestCustomActor::NewVariant8( Actor rival )
{
  Impl::TestCustomActor* impl = new Impl::TestCustomActorVariant8( rival );
  TestCustomActor custom( *impl ); // takes ownership

  impl->Initialize();

  return custom;
}

TestCustomActor::~TestCustomActor()
{
}

Impl::TestCustomActor& TestCustomActor::GetImpl()
{
  return static_cast<Impl::TestCustomActor&>(GetImplementation());
}

std::vector< std::string >& TestCustomActor::GetMethodsCalled()
{
  return GetImpl().mMethodsCalled;
}

void TestCustomActor::ResetCallStack()
{
  GetImpl().ResetCallStack();
}

void TestCustomActor::SetDaliProperty(std::string s)
{
  GetImpl().SetDaliProperty(s);
}

Vector3 TestCustomActor::GetSize()
{
  return GetImpl().mSizeSet;
}

Vector3 TestCustomActor::GetTargetSize()
{
  return GetImpl().mTargetSize;
}

Vector3 TestCustomActor::GetNaturalSize()
{
  return Vector3( 0.0f, 0.0f, 0.0f );
}

float TestCustomActor::GetHeightForWidth( float width )
{
  return 0.0f;
}

float TestCustomActor::GetWidthForHeight( float height )
{
  return 0.0f;
}

void TestCustomActor::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
}

void TestCustomActor::OnLayoutNegotiated( float size, Dimension::Type dimension )
{
}

void TestCustomActor::OnCalculateRelayoutSize( Dimension::Type dimension )
{
}

void TestCustomActor::TestRelayoutRequest()
{
  GetImpl().TestRelayoutRequest();
}

float TestCustomActor::TestGetHeightForWidthBase( float width )
{
  return GetImpl().TestGetHeightForWidthBase( width );
}

float TestCustomActor::TestGetWidthForHeightBase( float height )
{
  return GetImpl().TestGetWidthForHeightBase( height );
}

float TestCustomActor::TestCalculateChildSizeBase( const Dali::Actor& child, Dimension::Type dimension )
{
  return GetImpl().TestCalculateChildSizeBase( child, dimension );
}

bool TestCustomActor::TestRelayoutDependentOnChildrenBase( Dimension::Type dimension )
{
  return GetImpl().TestRelayoutDependentOnChildrenBase( dimension );
}

unsigned int TestCustomActor::GetDepth()
{
  return GetImpl().mDepth;
}

TestCustomActor::TestCustomActor()
{
}

TestCustomActor::TestCustomActor( Impl::TestCustomActor& impl )
: CustomActor( impl )
{
}

TestCustomActor::TestCustomActor( Dali::Internal::CustomActor* internal )
: CustomActor( internal )
{
}

TestCustomActor TestCustomActor::DownCast( BaseHandle handle )
{
  TestCustomActor actor;
  CustomActor custom = Dali::CustomActor::DownCast(handle);
  if(custom)
  {
    CustomActorImpl& customImpl = custom.GetImplementation();

    Test::Impl::TestCustomActor* impl = dynamic_cast<Test::Impl::TestCustomActor*>(&customImpl);
    if( impl )
    {
      actor = TestCustomActor(customImpl.GetOwner());
    }
  }
  return actor;
}

namespace Impl
{

TestCustomActor::TestCustomActor()
: CustomActorImpl( ActorFlags( REQUIRES_TOUCH_EVENTS | REQUIRES_WHEEL_EVENTS | REQUIRES_HOVER_EVENTS | DISABLE_SIZE_NEGOTIATION ) ),
  mDaliProperty( Property::INVALID_INDEX ),
  mSizeSet( Vector3::ZERO ),
  mTargetSize( Vector3::ZERO ),
  mNego( false ),
  mDepth(0u),
  develProp6( 10.0f )
{
}

TestCustomActor::TestCustomActor(bool nego)
: CustomActorImpl( ActorFlags( REQUIRES_TOUCH_EVENTS | REQUIRES_WHEEL_EVENTS | REQUIRES_HOVER_EVENTS ) ),
  mDaliProperty( Property::INVALID_INDEX ),
  mSizeSet( Vector3::ZERO ),
  mTargetSize( Vector3::ZERO ),
  mNego( nego ),
  develProp6( 10.0f )
{
}
/**
 * Destructor
 */
TestCustomActor::~TestCustomActor()
{
}

void TestCustomActor::Initialize( const char* name )
{
  mDaliProperty = Self().RegisterProperty( "Dali", std::string("no"), Property::READ_WRITE);

  OnInitialize( name );
}

void TestCustomActor::OnInitialize( const char* name ) {}

/**
 * Resets the call stack
 */
void TestCustomActor::ResetCallStack()
{
  mSizeSet = Vector3();
  mTargetSize = Vector3();
  mMethodsCalled.clear();
}

void TestCustomActor::AddToCallStacks( const char* method )
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
void TestCustomActor::OnStageConnection( int depth )
{
  AddToCallStacks("OnStageConnection");
  mDepth = depth;
}
void TestCustomActor::OnStageDisconnection()
{
  AddToCallStacks("OnStageDisconnection");
}
void TestCustomActor::OnChildAdd(Actor& child)
{
  AddToCallStacks("OnChildAdd");
}
void TestCustomActor::OnChildRemove(Actor& child)
{
  AddToCallStacks("OnChildRemove");
}
void TestCustomActor::OnPropertySet( Property::Index index, Property::Value propertyValue )
{
  AddToCallStacks("OnPropertySet");
}
void TestCustomActor::OnSizeSet(const Vector3& targetSize)
{
  mSizeSet = targetSize;
  AddToCallStacks("OnSizeSet");
}
void TestCustomActor::OnSizeAnimation(Animation& animation, const Vector3& targetSize)
{
  mTargetSize = targetSize;
  AddToCallStacks("OnSizeAnimation");
}
bool TestCustomActor::OnTouchEvent(const TouchEvent& event)
{
  AddToCallStacks("OnTouchEvent");
  return true;
}
bool TestCustomActor::OnHoverEvent(const HoverEvent& event)
{
  AddToCallStacks("OnHoverEvent");
  return true;
}
bool TestCustomActor::OnWheelEvent(const WheelEvent& event)
{
  AddToCallStacks("OnWheelEvent");
  return true;
}
bool TestCustomActor::OnKeyEvent(const KeyEvent& event)
{
  AddToCallStacks("OnKeyEvent");
  return true;
}
void TestCustomActor::OnKeyInputFocusGained()
{
  AddToCallStacks("OnKeyInputFocusGained");
}
void TestCustomActor::OnKeyInputFocusLost()
{
  AddToCallStacks("OnKeyInputFocusLost");
}
Vector3 TestCustomActor::GetNaturalSize()
{
  return Vector3( 0.0f, 0.0f, 0.0f );
}

float TestCustomActor::GetHeightForWidth( float width )
{
  return 0.0f;
}

float TestCustomActor::GetWidthForHeight( float height )
{
  return 0.0f;
}

void TestCustomActor::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  gOnRelayout = true;
}

void TestCustomActor::OnSetResizePolicy( ResizePolicy::Type policy, Dimension::Type dimension )
{
}

void TestCustomActor::OnCalculateRelayoutSize( Dimension::Type dimension )
{
}

float TestCustomActor::CalculateChildSize( const Dali::Actor& child, Dimension::Type dimension )
{
  return 0.0f;
}

void TestCustomActor::OnLayoutNegotiated( float size, Dimension::Type dimension )
{
}

bool TestCustomActor::RelayoutDependentOnChildren( Dimension::Type dimension )
{
  return false;
}

void TestCustomActor::SetDaliProperty(std::string s)
{
  Self().SetProperty(mDaliProperty, s);
}
void TestCustomActor::TestRelayoutRequest()
{
  RelayoutRequest();
}

float TestCustomActor::TestGetHeightForWidthBase( float width )
{
  return GetHeightForWidthBase( width );
}

float TestCustomActor::TestGetWidthForHeightBase( float height )
{
  return GetWidthForHeightBase( height );
}

float TestCustomActor::TestCalculateChildSizeBase( const Dali::Actor& child, Dimension::Type dimension )
{
  return CalculateChildSizeBase( child, dimension );
}

bool TestCustomActor::TestRelayoutDependentOnChildrenBase( Dimension::Type dimension )
{
  return RelayoutDependentOnChildrenBase( dimension );
}

void TestCustomActor::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Test::TestCustomActor actor = Test::TestCustomActor::DownCast( Dali::BaseHandle( object ) );

  if ( actor )
  {
    TestCustomActor& actorImpl = GetImpl(actor);
    switch(index)
    {
      case Test::TestCustomActor::Property::TEST_PROPERTY1:
      {
        actorImpl.prop1 = value.Get<float>();
        break;
      }
      case Test::TestCustomActor::Property::TEST_PROPERTY2:
      {
        actorImpl.prop2 = value.Get<Vector4>();
        break;
      }
      case Test::DevelTestCustomActor::Property::DEVEL_TEST_PROPERTY3:
      {
        actorImpl.develProp3 = value.Get<Vector4>();
        break;
      }
      case Test::DevelTestCustomActor::Property::DEVEL_TEST_PROPERTY4:
      {
        actorImpl.develProp4 = value.Get<int>();
        break;
      }
      case Test::DevelTestCustomActor::Property::DEVEL_TEST_PROPERTY5:
      {
        actorImpl.develProp5 = value.Get<float>();
        break;
      }
    }
  }
}

Property::Value TestCustomActor::GetProperty( BaseObject* object, Property::Index index )
{
  Test::TestCustomActor actor = Test::TestCustomActor::DownCast( Dali::BaseHandle( object ) );

  if ( actor )
  {
    TestCustomActor& actorImpl = GetImpl(actor);
    switch(index)
    {
      case Test::TestCustomActor::Property::TEST_PROPERTY1:
      {
        return Property::Value(actorImpl.prop1);
      }
      case Test::TestCustomActor::Property::TEST_PROPERTY2:
      {
        return Property::Value(actorImpl.prop2);
      }
      case Test::DevelTestCustomActor::Property::DEVEL_TEST_PROPERTY3:
      {
        return Property::Value(actorImpl.develProp3);
      }
      case Test::DevelTestCustomActor::Property::DEVEL_TEST_PROPERTY4:
      {
        return Property::Value(actorImpl.develProp4);
      }
      case Test::DevelTestCustomActor::Property::DEVEL_TEST_PROPERTY5:
      {
        return Property::Value(actorImpl.develProp5);
      }
      case Test::DevelTestCustomActor::Property::DEVEL_TEST_PROPERTY6:
      {
        return Property::Value(actorImpl.develProp6);
      }
    }
  }
  return Property::Value();
}


BaseHandle CreateActor()
{
  return Test::TestCustomActor::New();
}

DALI_TYPE_REGISTRATION_BEGIN( Test::TestCustomActor, Dali::CustomActor, CreateActor );

DALI_PROPERTY_REGISTRATION( Test, TestCustomActor, "testProperty1", FLOAT, TEST_PROPERTY1)
DALI_PROPERTY_REGISTRATION( Test, TestCustomActor, "testProperty2", VECTOR4, TEST_PROPERTY2)
DALI_DEVEL_PROPERTY_REGISTRATION( Test, TestCustomActor, "develTestProperty3", VECTOR4, DEVEL_TEST_PROPERTY3)
DALI_DEVEL_PROPERTY_REGISTRATION( Test, TestCustomActor, "develTestProperty4", INTEGER, DEVEL_TEST_PROPERTY4)
DALI_DEVEL_PROPERTY_REGISTRATION( Test, TestCustomActor, "develTestProperty5", FLOAT, DEVEL_TEST_PROPERTY5)
DALI_DEVEL_PROPERTY_REGISTRATION_READ_ONLY( Test, TestCustomActor, "develTestProperty6", FLOAT, DEVEL_TEST_PROPERTY6)

DALI_TYPE_REGISTRATION_END()

} // Impl
} // Test namespace

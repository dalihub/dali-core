#ifndef TEST_CUSTOM_ACTOR_H
#define TEST_CUSTOM_ACTOR_H

#include <dali/public-api/dali-core.h>
#include <dali/devel-api/object/property-helper-devel.h>

extern std::vector< std::string > MasterCallStack;
extern bool gOnRelayout;

namespace Test
{
namespace Impl
{
struct TestCustomActor;
}

/**
 * Test custom actor handle
 */
class TestCustomActor : public Dali::CustomActor
{
public:
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Dali::PROPERTY_REGISTRATION_START_INDEX,
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000
  };

  struct Property
  {
    enum Type
    {
      TEST_PROPERTY1 = PROPERTY_START_INDEX,
      TEST_PROPERTY2
    };
  };

  TestCustomActor();
  static TestCustomActor New();
  static TestCustomActor NewNegoSize();
  static TestCustomActor NewVariant1( Actor childToAdd );
  static TestCustomActor NewVariant2();
  static TestCustomActor NewVariant3( Actor childToAdd );
  static TestCustomActor NewVariant4();
  static TestCustomActor NewVariant5();
  static TestCustomActor NewVariant6();
  static TestCustomActor NewVariant7( const char* name );
  static TestCustomActor NewVariant8( Actor rival );
  static TestCustomActor DownCast( Dali::BaseHandle handle );

  virtual ~TestCustomActor();
  Impl::TestCustomActor& GetImpl();

  std::vector< std::string >& GetMethodsCalled();
  void ResetCallStack();
  void SetDaliProperty(std::string s);
  Dali::Vector3 GetSize();
  Dali::Vector3 GetTargetSize();
  virtual Dali::Vector3 GetNaturalSize();
  virtual float GetHeightForWidth( float width );
  virtual float GetWidthForHeight( float height );
  virtual void OnRelayout( const Dali::Vector2& size, Dali::RelayoutContainer& container );
  virtual void OnLayoutNegotiated( float size, Dali::Dimension::Type dimension );
  virtual void OnCalculateRelayoutSize( Dali::Dimension::Type dimension );
  void TestRelayoutRequest();
  float TestGetHeightForWidthBase( float width );
  float TestGetWidthForHeightBase( float height );
  float TestCalculateChildSizeBase( const Dali::Actor& child, Dali::Dimension::Type dimension );
  bool TestRelayoutDependentOnChildrenBase( Dali::Dimension::Type dimension );
  unsigned int GetDepth();

private:
  TestCustomActor( Impl::TestCustomActor& impl );
  explicit TestCustomActor(Dali::Internal::CustomActor* internal);
};


namespace DevelTestCustomActor
{
namespace Property
{
enum Type
{
  TEST_PROPERTY1 = Test::TestCustomActor::Property::TEST_PROPERTY1,
  TEST_PROPERTY2 = Test::TestCustomActor::Property::TEST_PROPERTY2,
  DEVEL_TEST_PROPERTY3 = TEST_PROPERTY2+1,
  DEVEL_TEST_PROPERTY4 = TEST_PROPERTY2+2,
  DEVEL_TEST_PROPERTY5 = TEST_PROPERTY2+3
};

} // Namespace Property
} // Namespace DevelTestCustomActor


// TypeRegistry needs custom actor Implementations to have the same name (namespaces are ignored so we use one here)
namespace Impl
{

struct TestCustomActor : public Dali::CustomActorImpl
{
public:
  /**
   * Constructor
   */
  TestCustomActor();
  TestCustomActor(bool nego);
  virtual ~TestCustomActor();
  void Initialize( const char* name = NULL );
  virtual void OnInitialize( const char* name );
  void ResetCallStack();
  void AddToCallStacks( const char* method );
  virtual void OnStageConnection( int depth );
  virtual void OnStageDisconnection();
  virtual void OnChildAdd(Dali::Actor& child);
  virtual void OnChildRemove(Dali::Actor& child);
  virtual void OnPropertySet( Dali::Property::Index index, Dali::Property::Value propertyValue );
  virtual void OnSizeSet(const Dali::Vector3& targetSize);
  virtual void OnSizeAnimation(Dali::Animation& animation, const Dali::Vector3& targetSize);
  virtual bool OnTouchEvent(const Dali::TouchEvent& event);
  virtual bool OnHoverEvent(const Dali::HoverEvent& event);
  virtual bool OnWheelEvent(const Dali::WheelEvent& event);
  virtual bool OnKeyEvent(const Dali::KeyEvent& event);
  virtual void OnKeyInputFocusGained();
  virtual void OnKeyInputFocusLost();
  virtual Dali::Vector3 GetNaturalSize();
  virtual float GetHeightForWidth( float width );
  virtual float GetWidthForHeight( float height );
  virtual void OnRelayout( const Dali::Vector2& size, Dali::RelayoutContainer& container );
  virtual void OnSetResizePolicy( Dali::ResizePolicy::Type policy, Dali::Dimension::Type dimension );
  virtual void OnCalculateRelayoutSize( Dali::Dimension::Type dimension );
  virtual float CalculateChildSize( const Dali::Actor& child, Dali::Dimension::Type dimension );
  virtual void OnLayoutNegotiated( float size, Dali::Dimension::Type dimension );
  virtual bool RelayoutDependentOnChildren( Dali::Dimension::Type dimension = Dali::Dimension::ALL_DIMENSIONS );
  static void SetProperty( Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value );
  static Dali::Property::Value GetProperty( Dali::BaseObject* object, Dali::Property::Index index );

  void SetDaliProperty(std::string s);
  void TestRelayoutRequest();
  float TestGetHeightForWidthBase( float width );
  float TestGetWidthForHeightBase( float height );
  float TestCalculateChildSizeBase( const Dali::Actor& child, Dali::Dimension::Type dimension );
  bool TestRelayoutDependentOnChildrenBase( Dali::Dimension::Type dimension );

public:
  Dali::Property::Index mDaliProperty;
  std::vector< std::string > mMethodsCalled;
  Dali::Vector3         mSizeSet;
  Dali::Vector3         mTargetSize;
  bool                  mNego;
  unsigned int          mDepth;

  float         prop1;
  Dali::Vector4 prop2;
  Dali::Vector4 develProp3;
  int           develProp4;
  float         develProp5;
};

inline TestCustomActor& GetImpl( Test::TestCustomActor& handle )
{
  DALI_ASSERT_ALWAYS( handle );
  Dali::RefObject& object = handle.GetImplementation();
  return static_cast<TestCustomActor&>( object );
}

inline const TestCustomActor& GetImpl( const Test::TestCustomActor& handle )
{
  DALI_ASSERT_ALWAYS( handle );
  const Dali::RefObject& object = handle.GetImplementation();
  return static_cast<const TestCustomActor&>( object );
}


/**
 * Variant which adds a new child during OnStageConnection
 */
struct TestCustomActorVariant1 : public TestCustomActor
{
  /**
   * Constructor
   */
  TestCustomActorVariant1( Dali::Actor childToAdd )
  : mChildToAdd( childToAdd )
  {
  }

  // From CustomActorImpl
  virtual void OnStageConnection( int depth )
  {
    // Chain up first
    TestCustomActor::OnStageConnection( depth );

    // Add the child
    Self().Add( mChildToAdd );
  }

  Dali::Actor mChildToAdd;
};

/**
 * Variant which removes children during OnStageConnection
 */
struct TestCustomActorVariant2 : public TestCustomActor
{
  /**
   * Constructor
   */
  TestCustomActorVariant2()
  {
  }

  // From CustomActorImpl
  virtual void OnStageConnection( int depth )
  {
    // Chain up first
    TestCustomActor::OnStageConnection( depth );

    // Remove all the children
    for( unsigned int i=0, num=Self().GetChildCount(); i<num; ++i )
    {
      Self().Remove( Self().GetChildAt(0) );
    }
  }
};

/**
 * Variant which adds a new child during OnStageDisconnection
 */
struct TestCustomActorVariant3 : public TestCustomActor
{
  /**
   * Constructor
   */
  TestCustomActorVariant3( Dali::Actor childToAdd )
  : mChildToAdd( childToAdd )
  {
  }

  // From CustomActorImpl
  virtual void OnStageDisconnection()
  {
    // Chain up first
    TestCustomActor::OnStageDisconnection();

    // Add the child
    Self().Add( mChildToAdd );
  }

  Dali::Actor mChildToAdd;
};

/**
 * Variant which removes children during OnStageDisconnection
 */
struct TestCustomActorVariant4 : public TestCustomActor
{
  /**
   * Constructor
   */
  TestCustomActorVariant4()
  {
  }

  // From CustomActorImpl
  virtual void OnStageDisconnection()
  {
    // Chain up first
    TestCustomActor::OnStageDisconnection();

    // Remove all the children
    for( unsigned int i=0, num=Self().GetChildCount(); i<num; ++i )
    {
      Self().Remove( Self().GetChildAt(0) );
    }
  }
};

/**
 * Variant which removes its parent from Stage during OnStageConnection
 */
struct TestCustomActorVariant5 : public TestCustomActor
{
  /**
   * Constructor
   */
  TestCustomActorVariant5()
  {
  }

  // From CustomActorImpl
  virtual void OnStageConnection( int depth )
  {
    // Chain up first
    TestCustomActor::OnStageConnection( depth );

    // Take parent off-stage
    Dali::Actor parent = Self().GetParent();
    if ( parent )
    {
      Dali::Stage::GetCurrent().Remove( parent );
    }
  }
};

/**
 * Variant which adds its parent to Stage during OnStageDisconnection
 */
struct TestCustomActorVariant6 : public TestCustomActor
{
  /**
   * Constructor
   */
  TestCustomActorVariant6()
  {
  }

  // From CustomActorImpl
  virtual void OnStageDisconnection()
  {
    // Chain up first
    TestCustomActor::OnStageDisconnection();

    // Put parent on-stage
    Dali::Actor parent = Self().GetParent();
    if ( parent )
    {
      Dali::Stage::GetCurrent().Add( parent );
    }
  }
};

/**
 * Variant which reparents its children into a separate container
 */
struct TestCustomActorVariant7 : public TestCustomActor
{
  /**
   * Constructor
   */
  TestCustomActorVariant7()
  {
  }

  virtual void OnInitialize( const char* name )
  {
    // We need to do this early, before the OnChildAdd is recorded
    Self().SetName( name );

    mContainer = Dali::Actor::New();
    mContainer.SetName( "Container" );
    Self().Add( mContainer );
  }

  // From CustomActorImpl
  virtual void OnChildAdd(Dali::Actor& child)
  {
    // Chain up first
    TestCustomActor::OnChildAdd(child);

    // Reparent child
    if ( child != mContainer )
    {
      mContainer.Add( child );
    }
  }

  Dali::Actor mContainer;
};

/**
 * Variant which attempts to interfere with the reparenting of a child to another container
 */
struct TestCustomActorVariant8 : public TestCustomActor
{
  /**
   * Constructor
   */
  TestCustomActorVariant8( Dali::Actor rival )
  : mRivalContainer( rival )
  {
  }

  // From CustomActorImpl
  virtual void OnChildRemove(Dali::Actor& child)
  {
    // Chain up first
    TestCustomActor::OnChildRemove(child);

    mRivalContainer.Remove( child ); // attempt to block reparenting to rival (should be a NOOP)
  }

  Dali::Actor mRivalContainer;
};

// Need a class that doesn't override virtual methods
class SimpleTestCustomActor : public Dali::CustomActorImpl
{
public:

  /**
   * Constructor
   */
  SimpleTestCustomActor()
  : CustomActorImpl( ActorFlags( REQUIRES_TOUCH_EVENTS | DISABLE_SIZE_NEGOTIATION ) )
  {
  }

  /**
   * Destructor
   */
  virtual ~SimpleTestCustomActor()
  {
  }

  // From CustomActorImpl
  virtual void OnStageConnection( int depth )
  {
  }
  virtual void OnStageDisconnection()
  {
  }
  virtual void OnChildAdd(Dali::Actor& child)
  {
  }
  virtual void OnChildRemove(Dali::Actor& child)
  {
  }
  virtual void OnSizeSet(const Dali::Vector3& targetSize)
  {
  }
  virtual void OnSizeAnimation(Dali::Animation& animation, const Dali::Vector3& targetSize)
  {
  }
  virtual bool OnTouchEvent(const Dali::TouchEvent& event)
  {
    return true;
  }
  virtual bool OnHoverEvent(const Dali::HoverEvent& event)
  {
    return true;
  }
  virtual bool OnWheelEvent(const Dali::WheelEvent& event)
  {
    return true;
  }
  virtual bool OnKeyEvent(const Dali::KeyEvent& event)
  {
    return true;
  }
  virtual void OnKeyInputFocusGained()
  {
  }
  virtual void OnKeyInputFocusLost()
  {
  }

  virtual Dali::Vector3 GetNaturalSize()
  {
    return Dali::Vector3( 0.0f, 0.0f, 0.0f );
  }

  virtual float GetHeightForWidth( float width )
  {
    return 0.0f;
  }

  virtual float GetWidthForHeight( float height )
  {
    return 0.0f;
  }

  virtual void OnRelayout( const Dali::Vector2& size, Dali::RelayoutContainer& container )
  {
  }

  virtual void OnSetResizePolicy( Dali::ResizePolicy::Type policy, Dali::Dimension::Type dimension )
  {
  }

  virtual void OnCalculateRelayoutSize( Dali::Dimension::Type dimension )
  {
  }

  virtual float CalculateChildSize( const Dali::Actor& child, Dali::Dimension::Type dimension )
  {
    return 0.0f;
  }

  virtual void OnLayoutNegotiated( float size, Dali::Dimension::Type dimension )
  {
  }

  virtual bool RelayoutDependentOnChildren( Dali::Dimension::Type dimension = Dali::Dimension::ALL_DIMENSIONS )
  {
    return false;
  }
};

} //namespace Impl
} //namespace Test

#endif // TEST_CUSTOM_ACTOR_H

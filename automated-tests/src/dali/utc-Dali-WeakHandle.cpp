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

#include <dali/public-api/dali-core.h>
#include <dali/devel-api/object/weak-handle.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

namespace
{

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

  MyTestCustomActor(Internal::CustomActor* internal)
  : CustomActor(internal)
  {
  }

  MyTestCustomActor( Impl::MyTestCustomActor& impl )
  : CustomActor( impl )
  {
  }
};

}

int UtcDaliWeakHandleBaseConstructorVoid(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::WeakHandleBase::WeakHandleBase()");

  WeakHandleBase object;

  DALI_TEST_CHECK(!object.GetBaseHandle());

  END_TEST;
}

int UtcDaliWeakHandleBaseConstructorWithHandle(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::WeakHandleBase::WeakHandleBase(Handle)");

  Handle emptyHandle;
  WeakHandleBase emptyObject(emptyHandle);
  DALI_TEST_CHECK(!emptyObject.GetBaseHandle());

  Actor actor = Actor::New();
  WeakHandleBase object(actor);
  DALI_TEST_CHECK(object.GetBaseHandle() == actor);

  END_TEST;
}

int UtcDaliWeakHandleBaseCopyConstructor(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::WeakHandleBase::WeakHandleBase(const WeakHandleBase&)");

  Actor actor = Actor::New();
  DALI_TEST_EQUALS(1, actor.GetBaseObject().ReferenceCount(), TEST_LOCATION); // reference count of the actor is not increased

  WeakHandleBase object(actor);
  DALI_TEST_CHECK(object.GetBaseHandle() == actor);
  DALI_TEST_EQUALS(1, actor.GetBaseObject().ReferenceCount(), TEST_LOCATION); // reference count of the actor is not increased

  WeakHandleBase copy(object);
  DALI_TEST_CHECK(copy.GetBaseHandle() == actor);
  DALI_TEST_EQUALS(1, actor.GetBaseObject().ReferenceCount(), TEST_LOCATION); // reference count of the actor is not increased

  END_TEST;
}

int UtcDaliWeakHandleBaseAssignmentOperator(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::WeakHandleBase::operator=");

  Actor actor = Actor::New();
  DALI_TEST_EQUALS(1, actor.GetBaseObject().ReferenceCount(), TEST_LOCATION); // reference count of the actor is not increased

  WeakHandleBase object(actor);
  DALI_TEST_CHECK(object.GetBaseHandle() == actor);
  DALI_TEST_EQUALS(1, actor.GetBaseObject().ReferenceCount(), TEST_LOCATION); // reference count of the actor is not increased

  WeakHandleBase copy = object;
  DALI_TEST_CHECK(copy.GetBaseHandle() == actor);
  DALI_TEST_EQUALS(1, actor.GetBaseObject().ReferenceCount(), TEST_LOCATION); // reference count of the actor is not increased

  END_TEST;
}

int UtcDaliWeakHandleBaseEqualityOperatorP(void)
{
  TestApplication application;
  tet_infoline("Positive Test Dali::WeakHandleBase::operator==");

  WeakHandleBase object;
  WeakHandleBase theSameObject;
  DALI_TEST_CHECK(object == theSameObject);

  Actor actor = Actor::New();

  object = WeakHandleBase(actor);
  DALI_TEST_CHECK(object.GetBaseHandle() == actor);

  theSameObject = object;
  DALI_TEST_CHECK(theSameObject.GetBaseHandle() == actor);
  DALI_TEST_CHECK(object == theSameObject);

  END_TEST;
}

int UtcDaliWeakHandleBaseEqualityOperatorN(void)
{
  TestApplication application;
  tet_infoline("Negative Test Dali::WeakHandleBase::operator==");

  Actor actor = Actor::New();

  WeakHandleBase object(actor);
  DALI_TEST_CHECK(object.GetBaseHandle() == actor);

  Actor differentActor = Actor::New();
  WeakHandleBase aDifferentWeakHandleBase(differentActor);

  DALI_TEST_CHECK(!(object == aDifferentWeakHandleBase));

  END_TEST;
}

int UtcDaliWeakHandleBaseInequalityOperatorP(void)
{
  TestApplication application;
  tet_infoline("Positive Test Dali::WeakHandleBase::operator!=");

  Actor actor = Actor::New();

  WeakHandleBase object(actor);
  DALI_TEST_CHECK(object.GetBaseHandle() == actor);

  Actor differentActor = Actor::New();
  WeakHandleBase aDifferentWeakHandleBase(differentActor);

  DALI_TEST_CHECK(object != aDifferentWeakHandleBase);
  END_TEST;
}

int UtcDaliWeakHandleBaseInequalityOperatorN(void)
{
  TestApplication application;
  tet_infoline("Negative Test Dali::WeakHandleBase::operator!=");

  Actor actor = Actor::New();

  WeakHandleBase object(actor);
  DALI_TEST_CHECK(object.GetBaseHandle() == actor);

  WeakHandleBase theSameWeakHandleBase = object;

  DALI_TEST_CHECK(!(object != theSameWeakHandleBase));
  END_TEST;
}

int UtcDaliWeakHandleBaseGetBaseHandle(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::WeakHandleBase::GetBaseHandle()");

  Handle emptyHandle;
  WeakHandleBase emptyObject(emptyHandle);
  DALI_TEST_CHECK(!emptyObject.GetBaseHandle());

  Actor actor = Actor::New();
  WeakHandleBase object(actor);
  DALI_TEST_CHECK(object.GetBaseHandle() == actor);

  WeakHandleBase theSameObject = WeakHandleBase(actor);
  DALI_TEST_CHECK(object.GetBaseHandle() == theSameObject.GetBaseHandle());

  Actor differentActor = Actor::New();
  WeakHandleBase aDifferentWeakHandleBase(differentActor);
  DALI_TEST_CHECK(object.GetBaseHandle() != aDifferentWeakHandleBase.GetBaseHandle());

  END_TEST;
}

int UtcDaliWeakHandleBaseReset(void)
{
  TestApplication application;
  tet_infoline( "Testing Daku::WeakHandleBase::Reset()" );

  Actor actor = Actor::New();
  WeakHandleBase object(actor);
  DALI_TEST_CHECK(object.GetBaseHandle() == actor);

  object.Reset();

  DALI_TEST_CHECK(object == WeakHandleBase());
  DALI_TEST_CHECK(object.GetBaseHandle() == Handle());

  END_TEST;
}

int UtcDaliWeakHandleGetHandle(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::WeakHandle::GetHandle()");

  Actor actor = Actor::New();
  WeakHandle<Actor> object(actor);
  DALI_TEST_CHECK(object.GetHandle() == actor);

  MyTestCustomActor customActor = MyTestCustomActor::New();
  WeakHandle<MyTestCustomActor> customObject(customActor);
  DALI_TEST_CHECK(customObject.GetHandle() == customActor);

  DALI_TEST_CHECK(object.GetHandle() != customObject.GetHandle());

  END_TEST;
}




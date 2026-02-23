#ifndef TEST_CUSTOM_ACTOR_H
#define TEST_CUSTOM_ACTOR_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <dali/devel-api/object/property-helper-devel.h>
#include <dali/integration-api/scene.h>
#include <dali/public-api/dali-core.h>

extern std::vector<std::string> MasterCallStack;
extern bool                     gOnRelayout;

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
    PROPERTY_END_INDEX   = PROPERTY_START_INDEX + 1000
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
  static TestCustomActor NewVariant1(Actor childToAdd);
  static TestCustomActor NewVariant2();
  static TestCustomActor NewVariant3(Actor childToAdd);
  static TestCustomActor NewVariant4();
  static TestCustomActor NewVariant5(Dali::Integration::Scene scene);
  static TestCustomActor NewVariant6(Dali::Integration::Scene scene);
  static TestCustomActor NewVariant7(const char* name);
  static TestCustomActor NewVariant8(Actor rival);
  static TestCustomActor DownCast(Dali::BaseHandle handle);

  virtual ~TestCustomActor();
  Impl::TestCustomActor& GetImpl();

  TestCustomActor(TestCustomActor&& rhs)            = default;
  TestCustomActor& operator=(TestCustomActor&& rhs) = default;

  std::vector<std::string>& GetMethodsCalled();
  void                      ResetCallStack();
  void                      SetDaliProperty(std::string s);
  Dali::Vector3             GetSize();
  Dali::Vector3             GetTargetSize();
  virtual Dali::Vector3     GetNaturalSize();
  virtual float             GetHeightForWidth(float width);
  virtual float             GetWidthForHeight(float height);
  virtual void              OnRelayout(const Dali::Vector2& size, Dali::RelayoutContainer& container);
  virtual void              OnLayoutNegotiated(float size, Dali::Dimension::Type dimension);
  virtual void              OnCalculateRelayoutSize(Dali::Dimension::Type dimension);
  void                      TestRelayoutRequest();
  float                     TestGetHeightForWidthBase(float width);
  float                     TestGetWidthForHeightBase(float height);
  float                     TestCalculateChildSizeBase(const Dali::Actor& child, Dali::Dimension::Type dimension);
  bool                      TestRelayoutDependentOnChildrenBase(Dali::Dimension::Type dimension);
  uint32_t                  GetDepth();
  void                      SetTransparent(bool transparent);
  bool                      IsTransparent();

private:
  TestCustomActor(Impl::TestCustomActor& impl);
  explicit TestCustomActor(Dali::Internal::CustomActor* internal);
};

namespace DevelTestCustomActor
{
namespace Property
{
enum Type
{
  TEST_PROPERTY1       = Test::TestCustomActor::Property::TEST_PROPERTY1,
  TEST_PROPERTY2       = Test::TestCustomActor::Property::TEST_PROPERTY2,
  DEVEL_TEST_PROPERTY3 = TEST_PROPERTY2 + 1,
  DEVEL_TEST_PROPERTY4 = TEST_PROPERTY2 + 2,
  DEVEL_TEST_PROPERTY5 = TEST_PROPERTY2 + 3,
  DEVEL_TEST_PROPERTY6 = TEST_PROPERTY2 + 4
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
  void          Initialize(const char* name = NULL);
  virtual void  OnInitialize(const char* name);
  void          ResetCallStack();
  void          AddToCallStacks(const char* method);
  void          OnSceneConnection(int depth) override;
  void          OnSceneDisconnection() override;
  void          OnChildAdd(Dali::Actor& child) override;
  void          OnChildRemove(Dali::Actor& child) override;
  void          OnPropertySet(Dali::Property::Index index, const Dali::Property::Value& propertyValue) override;
  void          OnSizeSet(const Dali::Vector3& targetSize) override;
  void          OnSizeAnimation(Dali::Animation& animation, const Dali::Vector3& targetSize) override;
  virtual void  OnKeyInputFocusGained();
  virtual void  OnKeyInputFocusLost();
  Dali::Vector3 GetNaturalSize() override;
  void          SetNaturalSize(const Dali::Vector3& size);
  void          SetHeightForWidthFactor(float factor)
  {
    mH4Wfactor = factor;
  }
  void SetWidthForHeightFactor(float factor)
  {
    mW4Hfactor = factor;
  }
  float                        GetHeightForWidth(float width) override;
  float                        GetWidthForHeight(float height) override;
  void                         OnRelayout(const Dali::Vector2& size, Dali::RelayoutContainer& container) override;
  void                         OnSetResizePolicy(Dali::ResizePolicy::Type policy, Dali::Dimension::Type dimension) override;
  void                         OnCalculateRelayoutSize(Dali::Dimension::Type dimension) override;
  float                        CalculateChildSize(const Dali::Actor& child, Dali::Dimension::Type dimension) override;
  void                         OnLayoutNegotiated(float size, Dali::Dimension::Type dimension) override;
  bool                         RelayoutDependentOnChildren(Dali::Dimension::Type dimension = Dali::Dimension::ALL_DIMENSIONS) override;
  void                         SetTransparent(bool transparent);
  bool                         IsTransparent() const;
  static void                  SetProperty(Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value);
  static Dali::Property::Value GetProperty(Dali::BaseObject* object, Dali::Property::Index index);

  void  SetDaliProperty(std::string s);
  void  TestRelayoutRequest();
  float TestGetHeightForWidthBase(float width);
  float TestGetWidthForHeightBase(float height);
  float TestCalculateChildSizeBase(const Dali::Actor& child, Dali::Dimension::Type dimension);
  bool  TestRelayoutDependentOnChildrenBase(Dali::Dimension::Type dimension);

  void GetOffScreenRenderTasks(Dali::Vector<Dali::RenderTask>& tasks, bool isForward) override
  {
  }

public:
  Dali::Property::Index    mDaliProperty;
  std::vector<std::string> mMethodsCalled;
  Dali::Vector3            mNaturalSize;
  Dali::Vector3            mSizeSet;
  Dali::Vector3            mTargetSize;
  float                    mW4Hfactor;
  float                    mH4Wfactor;
  bool                     mNego;
  uint32_t                 mDepth;

  float         prop1;
  Dali::Vector4 prop2;
  Dali::Vector4 develProp3;
  int           develProp4;
  float         develProp5;
  float         develProp6;
};

inline TestCustomActor& GetImpl(Test::TestCustomActor& handle)
{
  DALI_ASSERT_ALWAYS(handle);
  Dali::RefObject& object = handle.GetImplementation();
  return static_cast<TestCustomActor&>(object);
}

inline const TestCustomActor& GetImpl(const Test::TestCustomActor& handle)
{
  DALI_ASSERT_ALWAYS(handle);
  const Dali::RefObject& object = handle.GetImplementation();
  return static_cast<const TestCustomActor&>(object);
}

/**
 * Variant which adds a new child during OnSceneConnection
 */
struct TestCustomActorVariant1 : public TestCustomActor
{
  /**
   * Constructor
   */
  TestCustomActorVariant1(Dali::Actor childToAdd)
  : mChildToAdd(childToAdd)
  {
  }

  // From CustomActorImpl
  void OnSceneConnection(int depth) override
  {
    // Chain up first
    TestCustomActor::OnSceneConnection(depth);

    // Add the child
    Self().Add(mChildToAdd);
  }

  Dali::Actor mChildToAdd;
};

/**
 * Variant which removes children during OnSceneConnection
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
  virtual void OnSceneConnection(int depth)
  {
    // Chain up first
    TestCustomActor::OnSceneConnection(depth);

    // Remove all the children
    for(uint32_t i = 0, num = Self().GetChildCount(); i < num; ++i)
    {
      Self().Remove(Self().GetChildAt(0));
    }
  }
};

/**
 * Variant which adds a new child during OnSceneDisconnection
 */
struct TestCustomActorVariant3 : public TestCustomActor
{
  /**
   * Constructor
   */
  TestCustomActorVariant3(Dali::Actor childToAdd)
  : mChildToAdd(childToAdd)
  {
  }

  // From CustomActorImpl
  virtual void OnSceneDisconnection()
  {
    // Chain up first
    TestCustomActor::OnSceneDisconnection();

    // Add the child
    Self().Add(mChildToAdd);
  }

  Dali::Actor mChildToAdd;
};

/**
 * Variant which removes children during OnSceneDisconnection
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
  virtual void OnSceneDisconnection()
  {
    // Chain up first
    TestCustomActor::OnSceneDisconnection();

    // Remove all the children
    for(uint32_t i = 0, num = Self().GetChildCount(); i < num; ++i)
    {
      Self().Remove(Self().GetChildAt(0));
    }
  }
};

/**
 * Variant which removes its parent from Stage during OnSceneConnection
 */
struct TestCustomActorVariant5 : public TestCustomActor
{
  /**
   * Constructor
   */
  TestCustomActorVariant5(Dali::Integration::Scene scene)
  : mScene(scene)
  {
  }

  // From CustomActorImpl
  virtual void OnSceneConnection(int depth)
  {
    // Chain up first
    TestCustomActor::OnSceneConnection(depth);

    // Take parent off-stage
    Dali::Actor parent = Self().GetParent();
    if(parent)
    {
      mScene.Remove(parent);
    }
  }

private:
  Dali::Integration::Scene mScene;
};

/**
 * Variant which adds its parent to Stage during OnSceneDisconnection
 */
struct TestCustomActorVariant6 : public TestCustomActor
{
  /**
   * Constructor
   */
  TestCustomActorVariant6(Dali::Integration::Scene scene)
  : mScene(scene)
  {
  }

  // From CustomActorImpl
  virtual void OnSceneDisconnection()
  {
    // Chain up first
    TestCustomActor::OnSceneDisconnection();

    // Put parent on-stage
    Dali::Actor parent = Self().GetParent();
    if(parent)
    {
      mScene.Add(parent);
    }
  }

private:
  Dali::Integration::Scene mScene;
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

  virtual void OnInitialize(const char* name)
  {
    // We need to do this early, before the OnChildAdd is recorded
    Self().SetProperty(Dali::Actor::Property::NAME, name);

    mContainer = Dali::Actor::New();
    mContainer.SetProperty(Dali::Actor::Property::NAME, "Container");
    Self().Add(mContainer);
  }

  // From CustomActorImpl
  virtual void OnChildAdd(Dali::Actor& child)
  {
    // Chain up first
    TestCustomActor::OnChildAdd(child);

    // Reparent child
    if(child != mContainer)
    {
      mContainer.Add(child);
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
  TestCustomActorVariant8(Dali::Actor rival)
  : mRivalContainer(rival)
  {
  }

  // From CustomActorImpl
  virtual void OnChildRemove(Dali::Actor& child)
  {
    // Chain up first
    TestCustomActor::OnChildRemove(child);

    mRivalContainer.Remove(child); // attempt to block reparenting to rival (should be a NOOP)
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
  : CustomActorImpl(ActorFlags(DISABLE_SIZE_NEGOTIATION))
  {
  }

  /**
   * Destructor
   */
  virtual ~SimpleTestCustomActor()
  {
  }

  // From CustomActorImpl
  void OnSceneConnection(int depth) override
  {
  }
  void OnSceneDisconnection() override
  {
  }
  void OnChildAdd(Dali::Actor& child) override
  {
  }
  void OnChildRemove(Dali::Actor& child) override
  {
  }
  void OnSizeSet(const Dali::Vector3& targetSize) override
  {
  }
  void OnSizeAnimation(Dali::Animation& animation, const Dali::Vector3& targetSize) override
  {
  }
  virtual void OnKeyInputFocusGained()
  {
  }
  virtual void OnKeyInputFocusLost()
  {
  }
  Dali::Vector3 GetNaturalSize() override
  {
    return Dali::Vector3(0.0f, 0.0f, 0.0f);
  }

  float GetHeightForWidth(float width) override
  {
    return 0.0f;
  }

  float GetWidthForHeight(float height) override
  {
    return 0.0f;
  }

  void OnRelayout(const Dali::Vector2& size, Dali::RelayoutContainer& container) override
  {
  }

  void OnSetResizePolicy(Dali::ResizePolicy::Type policy, Dali::Dimension::Type dimension) override
  {
  }

  void OnCalculateRelayoutSize(Dali::Dimension::Type dimension) override
  {
  }

  float CalculateChildSize(const Dali::Actor& child, Dali::Dimension::Type dimension) override
  {
    return 0.0f;
  }

  void OnLayoutNegotiated(float size, Dali::Dimension::Type dimension) override
  {
  }

  bool RelayoutDependentOnChildren(Dali::Dimension::Type dimension = Dali::Dimension::ALL_DIMENSIONS) override
  {
    return false;
  }

  void GetOffScreenRenderTasks(Dali::Vector<Dali::RenderTask>& tasks, bool isForward) override
  {
  }
};

} //namespace Impl
} //namespace Test

#endif // TEST_CUSTOM_ACTOR_H

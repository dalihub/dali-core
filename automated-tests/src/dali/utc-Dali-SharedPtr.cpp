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

#include <dali/public-api/common/dali-utility.h>
#include <dali/public-api/common/shared-ptr.h>
#include <stdlib.h>

#include <iostream>

#include "dali-test-suite-utils/dali-test-suite-utils.h"

using namespace Dali;

void utc_shared_ptr_startup(void)
{
  test_return_value = TET_UNDEF;
}

// Called after each test
void utc_shared_ptr_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

bool gDiscardedBool = false;

// Test class with a custom destructor
class TestClass
{
public:
  TestClass(bool& destructorCalled = gDiscardedBool)
  : mDestructorCalled(destructorCalled)
  {
    mDestructorCalled = false;
  }

  ~TestClass()
  {
    mDestructorCalled = true;
  }
  bool& mDestructorCalled;
};

// Test class with constructors for MakeShared testing
class MakeSharedTestClass
{
public:
  MakeSharedTestClass()
  : mValue(0)
  {
  }

  MakeSharedTestClass(int value)
  : mValue(value)
  {
  }

  MakeSharedTestClass(int value, const std::string& name)
  : mValue(value),
    mName(name)
  {
  }

  int GetValue() const
  {
    return mValue;
  }
  std::string GetName() const
  {
    return mName;
  }

private:
  int         mValue;
  std::string mName;
};

// Test classes for polymorphic conversion
class BaseClass
{
public:
  BaseClass(int value = 0, bool& destructorCalled = gDiscardedBool)
  : mValue(value),
    mDestructorCalled(destructorCalled)
  {
    mDestructorCalled = false;
  }

  virtual ~BaseClass()
  {
    mDestructorCalled = true;
  }

  int GetValue() const
  {
    return mValue;
  }

  virtual int GetDerivedValue() const
  {
    return mValue;
  }

protected:
  int   mValue;
  bool& mDestructorCalled;
};

class DerivedClass : public BaseClass
{
public:
  DerivedClass(int value = 0, const std::string& name = "", bool& destructorCalled = gDiscardedBool)
  : BaseClass(value, destructorCalled),
    mName(name)
  {
  }

  ~DerivedClass() override
  {
  }

  std::string GetName() const
  {
    return mName;
  }

  int GetDerivedValue() const override
  {
    return mValue + 100;
  }

private:
  std::string mName;
};

// Test class for EnableSharedFromThis
class EnableSharedFromThisTestClass : public EnableSharedFromThis<EnableSharedFromThisTestClass>
{
public:
  EnableSharedFromThisTestClass(int value = 0)
  : mValue(value)
  {
  }

  int GetValue() const
  {
    return mValue;
  }

private:
  int mValue;
};

// Another polymorphic class for DynamicPointerCast testing
class AnotherBaseClass
{
public:
  virtual ~AnotherBaseClass() = default;
  virtual int GetType() const
  {
    return 0;
  }
};

class AnotherDerivedClass : public AnotherBaseClass
{
public:
  int GetType() const override
  {
    return 1;
  }
  int GetDerivedData() const
  {
    return 42;
  }
};

} // unnamed namespace

int UtcDaliSharedPtrDefaultConstructor(void)
{
  TestApplication      application;
  SharedPtr<TestClass> ptr;
  DALI_TEST_CHECK(!ptr);
  DALI_TEST_CHECK(ptr.Get() == nullptr);
  DALI_TEST_CHECK(ptr.UseCount() == 0);
  END_TEST;
}

int UtcDaliSharedPtrConstructorWithRawPointer(void)
{
  TestApplication application;
  bool            destructorCalled(false);
  {
    SharedPtr<TestClass> ptr(new TestClass(destructorCalled));
    DALI_TEST_CHECK(ptr);
    DALI_TEST_CHECK(ptr.Get() != nullptr);
    DALI_TEST_CHECK(ptr.UseCount() == 1);
    DALI_TEST_CHECK(!destructorCalled);
  }
  DALI_TEST_CHECK(destructorCalled);
  END_TEST;
}

int UtcDaliSharedPtrConstructorWithNullptr(void)
{
  TestApplication      application;
  SharedPtr<TestClass> ptr(nullptr);
  DALI_TEST_CHECK(!ptr);
  DALI_TEST_CHECK(ptr.Get() == nullptr);
  DALI_TEST_CHECK(ptr.UseCount() == 0);
  END_TEST;
}

int UtcDaliSharedPtrCopyConstructor(void)
{
  TestApplication application;
  bool            destructorCalled(false);
  {
    SharedPtr<TestClass> ptr1(new TestClass(destructorCalled));
    DALI_TEST_CHECK(ptr1);
    DALI_TEST_CHECK(ptr1.UseCount() == 1);

    SharedPtr<TestClass> ptr2(ptr1);
    DALI_TEST_CHECK(ptr2);
    DALI_TEST_CHECK(ptr1);
    DALI_TEST_CHECK(ptr1.Get() == ptr2.Get());
    DALI_TEST_CHECK(ptr1.UseCount() == 2);
    DALI_TEST_CHECK(ptr2.UseCount() == 2);
    DALI_TEST_CHECK(!destructorCalled);
  }
  DALI_TEST_CHECK(destructorCalled);
  END_TEST;
}

int UtcDaliSharedPtrMoveConstructor(void)
{
  TestApplication application;
  bool            destructorCalled(false);
  {
    SharedPtr<TestClass> ptr1(new TestClass(destructorCalled));
    DALI_TEST_CHECK(ptr1);
    DALI_TEST_CHECK(ptr1.UseCount() == 1);

    SharedPtr<TestClass> ptr2(std::move(ptr1));
    DALI_TEST_CHECK(ptr2);
    DALI_TEST_CHECK(!ptr1);
    DALI_TEST_CHECK(ptr1.Get() == nullptr);
    DALI_TEST_CHECK(ptr1.UseCount() == 0);
    DALI_TEST_CHECK(ptr2.UseCount() == 1);
    DALI_TEST_CHECK(!destructorCalled);
  }
  DALI_TEST_CHECK(destructorCalled);
  END_TEST;
}

int UtcDaliSharedPtrCopyAssignment(void)
{
  TestApplication application;
  bool            destructorCalled1(false);
  bool            destructorCalled2(false);
  {
    SharedPtr<TestClass> ptr1(new TestClass(destructorCalled1));
    SharedPtr<TestClass> ptr2(new TestClass(destructorCalled2));

    DALI_TEST_CHECK(ptr1);
    DALI_TEST_CHECK(ptr2);
    DALI_TEST_CHECK(ptr1.UseCount() == 1);
    DALI_TEST_CHECK(ptr2.UseCount() == 1);
    DALI_TEST_CHECK(!destructorCalled1);
    DALI_TEST_CHECK(!destructorCalled2);

    ptr2 = ptr1;

    DALI_TEST_CHECK(ptr1);
    DALI_TEST_CHECK(ptr2);
    DALI_TEST_CHECK(ptr1.Get() == ptr2.Get());
    DALI_TEST_CHECK(ptr1.UseCount() == 2);
    DALI_TEST_CHECK(ptr2.UseCount() == 2);
    DALI_TEST_CHECK(destructorCalled2); // Old ptr2 object was deleted
    DALI_TEST_CHECK(!destructorCalled1);
  }
  DALI_TEST_CHECK(destructorCalled1);
  END_TEST;
}

int UtcDaliSharedPtrMoveAssignment(void)
{
  TestApplication application;
  bool            destructorCalled1(false);
  bool            destructorCalled2(false);
  {
    SharedPtr<TestClass> ptr1(new TestClass(destructorCalled1));
    SharedPtr<TestClass> ptr2(new TestClass(destructorCalled2));

    DALI_TEST_CHECK(ptr1);
    DALI_TEST_CHECK(ptr2);
    DALI_TEST_CHECK(ptr1.UseCount() == 1);
    DALI_TEST_CHECK(ptr2.UseCount() == 1);
    DALI_TEST_CHECK(!destructorCalled1);
    DALI_TEST_CHECK(!destructorCalled2);

    ptr2 = std::move(ptr1);

    DALI_TEST_CHECK(ptr2);
    DALI_TEST_CHECK(!ptr1);
    DALI_TEST_CHECK(ptr1.Get() == nullptr);
    DALI_TEST_CHECK(ptr1.UseCount() == 0);
    DALI_TEST_CHECK(ptr2.UseCount() == 1);
    DALI_TEST_CHECK(destructorCalled2); // Old ptr2 object was deleted
    DALI_TEST_CHECK(!destructorCalled1);
  }
  DALI_TEST_CHECK(destructorCalled1);
  END_TEST;
}

int UtcDaliSharedPtrCopyAssignmentSelf(void)
{
  TestApplication application;
  bool            destructorCalled(false);
  {
    SharedPtr<TestClass> ptr(new TestClass(destructorCalled));
    DALI_TEST_CHECK(ptr);
    DALI_TEST_CHECK(ptr.UseCount() == 1);

    ptr = ptr;

    DALI_TEST_CHECK(ptr);
    DALI_TEST_CHECK(ptr.UseCount() == 1);
    DALI_TEST_CHECK(!destructorCalled);
  }
  DALI_TEST_CHECK(destructorCalled);
  END_TEST;
}

int UtcDaliSharedPtrMoveAssignmentSelf(void)
{
  TestApplication application;
  bool            destructorCalled(false);
  {
    SharedPtr<TestClass> ptr(new TestClass(destructorCalled));
    DALI_TEST_CHECK(ptr);
    DALI_TEST_CHECK(ptr.UseCount() == 1);

    ptr = std::move(ptr);

    DALI_TEST_CHECK(ptr);
    DALI_TEST_CHECK(ptr.UseCount() == 1);
    DALI_TEST_CHECK(!destructorCalled);
  }
  DALI_TEST_CHECK(destructorCalled);
  END_TEST;
}

int UtcDaliSharedPtrDereferenceOperator(void)
{
  TestApplication      application;
  SharedPtr<TestClass> ptr(new TestClass());

  DALI_TEST_CHECK(ptr);
  TestClass& ref = *ptr;
  DALI_TEST_CHECK(&ref == ptr.Get());

  END_TEST;
}

int UtcDaliSharedPtrDereferenceOperatorConst(void)
{
  TestApplication            application;
  const SharedPtr<TestClass> ptr(new TestClass());

  DALI_TEST_CHECK(ptr);
  const TestClass& ref = *ptr;
  DALI_TEST_CHECK(&ref == ptr.Get());

  END_TEST;
}

int UtcDaliSharedPtrArrowOperator(void)
{
  TestApplication      application;
  SharedPtr<TestClass> ptr(new TestClass());

  DALI_TEST_CHECK(ptr);
  TestClass* rawPtr = ptr.operator->();
  DALI_TEST_CHECK(rawPtr == ptr.Get());

  END_TEST;
}

int UtcDaliSharedPtrArrowOperatorConst(void)
{
  TestApplication            application;
  const SharedPtr<TestClass> ptr(new TestClass());

  DALI_TEST_CHECK(ptr);
  const TestClass* rawPtr = ptr.operator->();
  DALI_TEST_CHECK(rawPtr == ptr.Get());

  END_TEST;
}

int UtcDaliSharedPtrBoolConversion(void)
{
  TestApplication application;

  SharedPtr<TestClass> ptr1;
  DALI_TEST_CHECK(!static_cast<bool>(ptr1));

  SharedPtr<TestClass> ptr2(new TestClass());
  DALI_TEST_CHECK(static_cast<bool>(ptr2));

  END_TEST;
}

int UtcDaliSharedPtrGet(void)
{
  TestApplication application;

  SharedPtr<TestClass> ptr1;
  DALI_TEST_CHECK(ptr1.Get() == nullptr);

  TestClass*           rawPtr = new TestClass();
  SharedPtr<TestClass> ptr2(rawPtr);
  DALI_TEST_CHECK(ptr2.Get() == rawPtr);

  END_TEST;
}

int UtcDaliSharedPtrGetConst(void)
{
  TestApplication application;

  const SharedPtr<TestClass> ptr1;
  DALI_TEST_CHECK(ptr1.Get() == nullptr);

  TestClass*                 rawPtr = new TestClass();
  const SharedPtr<TestClass> ptr2(rawPtr);
  DALI_TEST_CHECK(ptr2.Get() == rawPtr);

  END_TEST;
}

int UtcDaliSharedPtrUseCount(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  {
    SharedPtr<TestClass> ptr1(new TestClass(destructorCalled));
    DALI_TEST_CHECK(ptr1.UseCount() == 1);

    {
      SharedPtr<TestClass> ptr2(ptr1);
      DALI_TEST_CHECK(ptr1.UseCount() == 2);
      DALI_TEST_CHECK(ptr2.UseCount() == 2);

      {
        SharedPtr<TestClass> ptr3 = ptr1;
        DALI_TEST_CHECK(ptr1.UseCount() == 3);
        DALI_TEST_CHECK(ptr2.UseCount() == 3);
        DALI_TEST_CHECK(ptr3.UseCount() == 3);
      }
      DALI_TEST_CHECK(ptr1.UseCount() == 2);
      DALI_TEST_CHECK(ptr2.UseCount() == 2);
    }
    DALI_TEST_CHECK(ptr1.UseCount() == 1);
  }
  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

int UtcDaliSharedPtrUseCountWithNullptr(void)
{
  TestApplication application;

  SharedPtr<TestClass> ptr1;
  DALI_TEST_CHECK(ptr1.UseCount() == 0);

  SharedPtr<TestClass> ptr2(nullptr);
  DALI_TEST_CHECK(ptr2.UseCount() == 0);

  END_TEST;
}

int UtcDaliSharedPtrReset(void)
{
  TestApplication application;
  bool            destructorCalled1(false);
  bool            destructorCalled2(false);

  SharedPtr<TestClass> ptr(new TestClass(destructorCalled1));

  DALI_TEST_CHECK(ptr);
  DALI_TEST_CHECK(ptr.UseCount() == 1);
  DALI_TEST_CHECK(!destructorCalled1);

  ptr.Reset(new TestClass(destructorCalled2));

  DALI_TEST_CHECK(ptr);
  DALI_TEST_CHECK(ptr.UseCount() == 1);
  DALI_TEST_CHECK(destructorCalled1);
  DALI_TEST_CHECK(!destructorCalled2);

  ptr.Reset();

  DALI_TEST_CHECK(!ptr);
  DALI_TEST_CHECK(ptr.UseCount() == 0);
  DALI_TEST_CHECK(destructorCalled2);

  END_TEST;
}

int UtcDaliSharedPtrResetWithNullptr(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  SharedPtr<TestClass> ptr(new TestClass(destructorCalled));

  DALI_TEST_CHECK(ptr);
  DALI_TEST_CHECK(ptr.UseCount() == 1);
  DALI_TEST_CHECK(!destructorCalled);

  ptr.Reset(nullptr);

  DALI_TEST_CHECK(!ptr);
  DALI_TEST_CHECK(ptr.UseCount() == 0);
  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

int UtcDaliSharedPtrResetWithSharedPointers(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  {
    SharedPtr<TestClass> ptr1(new TestClass(destructorCalled));
    DALI_TEST_CHECK(ptr1.UseCount() == 1);

    SharedPtr<TestClass> ptr2(ptr1);
    DALI_TEST_CHECK(ptr1.UseCount() == 2);
    DALI_TEST_CHECK(ptr2.UseCount() == 2);

    ptr1.Reset();
    DALI_TEST_CHECK(!ptr1);
    DALI_TEST_CHECK(ptr2);
    DALI_TEST_CHECK(ptr1.UseCount() == 0);
    DALI_TEST_CHECK(ptr2.UseCount() == 1);
    DALI_TEST_CHECK(!destructorCalled);
  }
  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

int UtcDaliSharedPtrSwap(void)
{
  TestApplication application;
  bool            destructorCalled1(false);
  bool            destructorCalled2(false);

  SharedPtr<TestClass> ptr1(new TestClass(destructorCalled1));
  SharedPtr<TestClass> ptr2(new TestClass(destructorCalled2));

  TestClass* rawPtr1 = ptr1.Get();
  TestClass* rawPtr2 = ptr2.Get();

  DALI_TEST_CHECK(ptr1.Get() == rawPtr1);
  DALI_TEST_CHECK(ptr2.Get() == rawPtr2);

  ptr1.Swap(ptr2);

  DALI_TEST_CHECK(ptr1.Get() == rawPtr2);
  DALI_TEST_CHECK(ptr2.Get() == rawPtr1);
  DALI_TEST_CHECK(ptr1.UseCount() == 1);
  DALI_TEST_CHECK(ptr2.UseCount() == 1);

  END_TEST;
}

int UtcDaliSharedPtrSwapWithNullptr(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  SharedPtr<TestClass> ptr1(new TestClass(destructorCalled));
  SharedPtr<TestClass> ptr2;

  TestClass* rawPtr1 = ptr1.Get();

  DALI_TEST_CHECK(ptr1);
  DALI_TEST_CHECK(!ptr2);

  ptr1.Swap(ptr2);

  DALI_TEST_CHECK(!ptr1);
  DALI_TEST_CHECK(ptr2);
  DALI_TEST_CHECK(ptr2.Get() == rawPtr1);
  DALI_TEST_CHECK(!destructorCalled);

  END_TEST;
}

int UtcDaliMakeSharedNoArgs(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  {
    auto ptr = MakeShared<TestClass>(destructorCalled);
    DALI_TEST_CHECK(ptr);
    DALI_TEST_CHECK(ptr.UseCount() == 1);
    DALI_TEST_CHECK(!destructorCalled);
  }
  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

int UtcDaliMakeSharedWithArgs(void)
{
  TestApplication application;

  auto ptr1 = MakeShared<MakeSharedTestClass>(42);
  DALI_TEST_CHECK(ptr1);
  DALI_TEST_CHECK(ptr1->GetValue() == 42);

  auto ptr2 = MakeShared<MakeSharedTestClass>(100, "test");
  DALI_TEST_CHECK(ptr2);
  DALI_TEST_CHECK(ptr2->GetValue() == 100);
  DALI_TEST_CHECK(ptr2->GetName() == "test");

  END_TEST;
}

int UtcDaliMakeSharedDefaultConstruction(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  {
    auto ptr = MakeShared<TestClass>(destructorCalled);
    DALI_TEST_CHECK(ptr);
    DALI_TEST_CHECK(ptr.UseCount() == 1);
    DALI_TEST_CHECK(!destructorCalled);
  }
  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

int UtcDaliMakeSharedCopyConstructor(void)
{
  TestApplication application;

  auto ptr1 = MakeShared<MakeSharedTestClass>(123);
  DALI_TEST_CHECK(ptr1);

  SharedPtr<MakeSharedTestClass> ptr2(ptr1);
  DALI_TEST_CHECK(ptr2);
  DALI_TEST_CHECK(ptr1);
  DALI_TEST_CHECK(ptr1.Get() == ptr2.Get());
  DALI_TEST_CHECK(ptr1.UseCount() == 2);
  DALI_TEST_CHECK(ptr2.UseCount() == 2);
  DALI_TEST_CHECK(ptr2->GetValue() == 123);

  END_TEST;
}

int UtcDaliMakeSharedMoveConstructor(void)
{
  TestApplication application;

  auto ptr1 = MakeShared<MakeSharedTestClass>(123);
  DALI_TEST_CHECK(ptr1);

  SharedPtr<MakeSharedTestClass> ptr2(std::move(ptr1));
  DALI_TEST_CHECK(ptr2);
  DALI_TEST_CHECK(!ptr1);
  DALI_TEST_CHECK(ptr2.UseCount() == 1);
  DALI_TEST_CHECK(ptr2->GetValue() == 123);

  END_TEST;
}

int UtcDaliMakeSharedDereference(void)
{
  TestApplication application;

  auto ptr = MakeShared<MakeSharedTestClass>(456, "test");
  DALI_TEST_CHECK(ptr);

  MakeSharedTestClass& ref = *ptr;
  DALI_TEST_CHECK(ref.GetValue() == 456);
  DALI_TEST_CHECK(ref.GetName() == "test");

  END_TEST;
}

int UtcDaliMakeSharedConstReferenceParameter(void)
{
  TestApplication application;

  std::string name = "const_test";
  auto        ptr  = MakeShared<MakeSharedTestClass>(789, name);
  DALI_TEST_CHECK(ptr);
  DALI_TEST_CHECK(ptr->GetValue() == 789);
  DALI_TEST_CHECK(ptr->GetName() == "const_test");

  END_TEST;
}

int UtcDaliMakeSharedMultipleReferences(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  {
    auto ptr1 = MakeShared<TestClass>(destructorCalled);
    DALI_TEST_CHECK(ptr1.UseCount() == 1);

    {
      SharedPtr<TestClass> ptr2(ptr1);
      DALI_TEST_CHECK(ptr1.UseCount() == 2);
      DALI_TEST_CHECK(ptr2.UseCount() == 2);

      {
        SharedPtr<TestClass> ptr3 = ptr1;
        DALI_TEST_CHECK(ptr1.UseCount() == 3);
        DALI_TEST_CHECK(ptr2.UseCount() == 3);
        DALI_TEST_CHECK(ptr3.UseCount() == 3);
      }
      DALI_TEST_CHECK(ptr1.UseCount() == 2);
      DALI_TEST_CHECK(ptr2.UseCount() == 2);
    }
    DALI_TEST_CHECK(ptr1.UseCount() == 1);
  }
  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

int UtcDaliMakeSharedPerfectForwardingLValue(void)
{
  TestApplication application;

  // Test that lvalue arguments are properly forwarded as references
  std::string name = "test_lvalue";
  auto        ptr  = MakeShared<MakeSharedTestClass>(100, name);

  DALI_TEST_CHECK(ptr);
  DALI_TEST_CHECK(ptr->GetValue() == 100);
  DALI_TEST_CHECK(ptr->GetName() == "test_lvalue");

  // Modify the original string and verify it wasn't moved (lvalue reference was used)
  name = "modified";
  DALI_TEST_CHECK(ptr->GetName() == "test_lvalue"); // Should still be original value

  END_TEST;
}

int UtcDaliMakeSharedPerfectForwardingRValue(void)
{
  TestApplication application;

  // Test that rvalue arguments are properly forwarded via move semantics
  std::string name = "test_rvalue";
  auto        ptr  = MakeShared<MakeSharedTestClass>(200, std::move(name));

  DALI_TEST_CHECK(ptr);
  DALI_TEST_CHECK(ptr->GetValue() == 200);
  DALI_TEST_CHECK(ptr->GetName() == "test_rvalue");

  // The original string should be in a valid but unspecified state after move
  DALI_TEST_CHECK(name.empty() || name == "test_rvalue"); // Either is valid after move

  END_TEST;
}

int UtcDaliSharedPtrEqualityOperator(void)
{
  TestApplication application;

  SharedPtr<TestClass> ptr1(new TestClass());
  SharedPtr<TestClass> ptr2(ptr1);
  SharedPtr<TestClass> ptr3(new TestClass());
  SharedPtr<TestClass> ptr4;

  DALI_TEST_CHECK(ptr1 == ptr2);
  DALI_TEST_CHECK(!(ptr1 == ptr3));
  DALI_TEST_CHECK(!(ptr1 == ptr4));
  DALI_TEST_CHECK(ptr4 == SharedPtr<TestClass>());

  END_TEST;
}

int UtcDaliSharedPtrInequalityOperator(void)
{
  TestApplication application;

  SharedPtr<TestClass> ptr1(new TestClass());
  SharedPtr<TestClass> ptr2(ptr1);
  SharedPtr<TestClass> ptr3(new TestClass());
  SharedPtr<TestClass> ptr4;

  DALI_TEST_CHECK(!(ptr1 != ptr2));
  DALI_TEST_CHECK(ptr1 != ptr3);
  DALI_TEST_CHECK(ptr1 != ptr4);
  DALI_TEST_CHECK(!(ptr4 != SharedPtr<TestClass>()));

  END_TEST;
}

int UtcDaliSharedPtrLessThanOperator(void)
{
  TestApplication application;

  SharedPtr<TestClass> ptr1(new TestClass());
  SharedPtr<TestClass> ptr2(ptr1);
  SharedPtr<TestClass> ptr3(new TestClass());
  SharedPtr<TestClass> ptr4;

  DALI_TEST_CHECK(!(ptr1 < ptr2));
  DALI_TEST_CHECK(!(ptr2 < ptr1));
  DALI_TEST_CHECK((ptr1 < ptr3) || (ptr3 < ptr1)); // One must be true
  DALI_TEST_CHECK(!(ptr4 < SharedPtr<TestClass>()));
  DALI_TEST_CHECK(!(SharedPtr<TestClass>() < ptr4));

  END_TEST;
}

int UtcDaliSharedPtrLessThanOrEqualOperator(void)
{
  TestApplication application;

  SharedPtr<TestClass> ptr1(new TestClass());
  SharedPtr<TestClass> ptr2(ptr1);
  SharedPtr<TestClass> ptr3(new TestClass());
  SharedPtr<TestClass> ptr4;

  DALI_TEST_CHECK(ptr1 <= ptr2);
  DALI_TEST_CHECK(ptr2 <= ptr1);
  DALI_TEST_CHECK((ptr1 <= ptr3) || (ptr3 <= ptr1)); // At least one must be true
  DALI_TEST_CHECK(ptr4 <= SharedPtr<TestClass>());
  DALI_TEST_CHECK(SharedPtr<TestClass>() <= ptr4);

  END_TEST;
}

int UtcDaliSharedPtrGreaterThanOperator(void)
{
  TestApplication application;

  SharedPtr<TestClass> ptr1(new TestClass());
  SharedPtr<TestClass> ptr2(ptr1);
  SharedPtr<TestClass> ptr3(new TestClass());
  SharedPtr<TestClass> ptr4;

  DALI_TEST_CHECK(!(ptr1 > ptr2));
  DALI_TEST_CHECK(!(ptr2 > ptr1));
  DALI_TEST_CHECK((ptr1 > ptr3) || (ptr3 > ptr1)); // One must be true
  DALI_TEST_CHECK(!(ptr4 > SharedPtr<TestClass>()));
  DALI_TEST_CHECK(!(SharedPtr<TestClass>() > ptr4));

  END_TEST;
}

int UtcDaliSharedPtrGreaterThanOrEqualOperator(void)
{
  TestApplication application;

  SharedPtr<TestClass> ptr1(new TestClass());
  SharedPtr<TestClass> ptr2(ptr1);
  SharedPtr<TestClass> ptr3(new TestClass());
  SharedPtr<TestClass> ptr4;

  DALI_TEST_CHECK(ptr1 >= ptr2);
  DALI_TEST_CHECK(ptr2 >= ptr1);
  DALI_TEST_CHECK((ptr1 >= ptr3) || (ptr3 >= ptr1)); // At least one must be true
  DALI_TEST_CHECK(ptr4 >= SharedPtr<TestClass>());
  DALI_TEST_CHECK(SharedPtr<TestClass>() >= ptr4);

  END_TEST;
}

int UtcDaliSharedPtrMultipleSharedPointerDestruction(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  SharedPtr<TestClass>* ptrs = new SharedPtr<TestClass>[5];
  ptrs[0]                    = SharedPtr<TestClass>(new TestClass(destructorCalled));

  for(auto i = 1u; i < 5u; ++i)
  {
    ptrs[i] = ptrs[0];
    DALI_TEST_CHECK(ptrs[i].UseCount() == (i + 1));
  }

  DALI_TEST_CHECK(!destructorCalled);

  delete[] ptrs;
  // Object should be destroyed when all references are gone
  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

int UtcDaliSharedPtrCopyAndMoveMixing(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  {
    SharedPtr<TestClass> ptr1(new TestClass(destructorCalled));
    DALI_TEST_CHECK(ptr1.UseCount() == 1);

    SharedPtr<TestClass> ptr2(ptr1);
    DALI_TEST_CHECK(ptr1.UseCount() == 2);
    DALI_TEST_CHECK(ptr2.UseCount() == 2);

    SharedPtr<TestClass> ptr3(std::move(ptr2));
    DALI_TEST_CHECK(ptr1.UseCount() == 2);
    DALI_TEST_CHECK(ptr2.UseCount() == 0);
    DALI_TEST_CHECK(ptr3.UseCount() == 2);

    SharedPtr<TestClass> ptr4 = ptr1;
    DALI_TEST_CHECK(ptr1.UseCount() == 3);
    DALI_TEST_CHECK(ptr3.UseCount() == 3);
    DALI_TEST_CHECK(ptr4.UseCount() == 3);

    ptr2 = std::move(ptr4);
    DALI_TEST_CHECK(ptr1.UseCount() == 3);
    DALI_TEST_CHECK(ptr2.UseCount() == 3);
    DALI_TEST_CHECK(ptr3.UseCount() == 3);
    DALI_TEST_CHECK(ptr4.UseCount() == 0);

    DALI_TEST_CHECK(!destructorCalled);
  }
  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

int UtcDaliSharedPtrPolymorphicBehaviorCopyConstructor(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  {
    SharedPtr<DerivedClass> derivedPtr(new DerivedClass(42, "test", destructorCalled));
    DALI_TEST_CHECK(derivedPtr);
    DALI_TEST_CHECK(derivedPtr->GetValue() == 42);
    DALI_TEST_CHECK(derivedPtr->GetName() == "test");
    DALI_TEST_CHECK(derivedPtr->GetDerivedValue() == 142);

    // Copy to base class pointer
    SharedPtr<BaseClass> basePtr(derivedPtr);
    DALI_TEST_CHECK(basePtr);
    DALI_TEST_CHECK(derivedPtr);
    DALI_TEST_CHECK(basePtr.Get() == derivedPtr.Get());
    DALI_TEST_CHECK(basePtr->GetValue() == 42);
    DALI_TEST_CHECK(basePtr->GetDerivedValue() == 142);
    DALI_TEST_CHECK(basePtr.UseCount() == 2);
    DALI_TEST_CHECK(derivedPtr.UseCount() == 2);
    DALI_TEST_CHECK(!destructorCalled);
  }

  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

int UtcDaliSharedPtrPolymorphicBehaviorCopyAssignment(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  {
    SharedPtr<DerivedClass> derivedPtr(new DerivedClass(42, "test", destructorCalled));
    DALI_TEST_CHECK(derivedPtr);
    DALI_TEST_CHECK(derivedPtr->GetValue() == 42);
    DALI_TEST_CHECK(derivedPtr->GetName() == "test");
    DALI_TEST_CHECK(derivedPtr->GetDerivedValue() == 142);

    // Copy to base class pointer
    SharedPtr<BaseClass> basePtr;
    DALI_TEST_CHECK(!basePtr);
    basePtr = derivedPtr;
    DALI_TEST_CHECK(basePtr);
    DALI_TEST_CHECK(derivedPtr);
    DALI_TEST_CHECK(basePtr.Get() == derivedPtr.Get());
    DALI_TEST_CHECK(basePtr->GetValue() == 42);
    DALI_TEST_CHECK(basePtr->GetDerivedValue() == 142);
    DALI_TEST_CHECK(basePtr.UseCount() == 2);
    DALI_TEST_CHECK(derivedPtr.UseCount() == 2);
    DALI_TEST_CHECK(!destructorCalled);
  }

  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

int UtcDaliSharedPtrPolymorphicBehaviorMoveConstructor(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  {
    SharedPtr<DerivedClass> derivedPtr(new DerivedClass(42, "test", destructorCalled));
    DALI_TEST_CHECK(derivedPtr);
    DALI_TEST_CHECK(derivedPtr->GetValue() == 42);
    DALI_TEST_CHECK(derivedPtr->GetName() == "test");
    DALI_TEST_CHECK(derivedPtr->GetDerivedValue() == 142);

    // Move to base class pointer
    SharedPtr<BaseClass> basePtr(std::move(derivedPtr));
    DALI_TEST_CHECK(basePtr);
    DALI_TEST_CHECK(!derivedPtr);
    DALI_TEST_CHECK(basePtr.Get() != derivedPtr.Get());
    DALI_TEST_CHECK(basePtr->GetValue() == 42);
    DALI_TEST_CHECK(basePtr->GetDerivedValue() == 142);
    DALI_TEST_CHECK(basePtr.UseCount() == 1);
    DALI_TEST_CHECK(derivedPtr.UseCount() == 0);
    DALI_TEST_CHECK(!destructorCalled);
  }

  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

int UtcDaliSharedPtrPolymorphicBehaviorMoveAssignment(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  {
    SharedPtr<DerivedClass> derivedPtr(new DerivedClass(42, "test", destructorCalled));
    DALI_TEST_CHECK(derivedPtr);
    DALI_TEST_CHECK(derivedPtr->GetValue() == 42);
    DALI_TEST_CHECK(derivedPtr->GetName() == "test");
    DALI_TEST_CHECK(derivedPtr->GetDerivedValue() == 142);

    // Move to base class pointer
    SharedPtr<BaseClass> basePtr;
    DALI_TEST_CHECK(!basePtr);
    basePtr = std::move(derivedPtr);
    DALI_TEST_CHECK(basePtr);
    DALI_TEST_CHECK(!derivedPtr);
    DALI_TEST_CHECK(basePtr.Get() != derivedPtr.Get());
    DALI_TEST_CHECK(basePtr->GetValue() == 42);
    DALI_TEST_CHECK(basePtr->GetDerivedValue() == 142);
    DALI_TEST_CHECK(basePtr.UseCount() == 1);
    DALI_TEST_CHECK(derivedPtr.UseCount() == 0);
    DALI_TEST_CHECK(!destructorCalled);
  }

  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

int UtcDaliSharedPtrComparisonOperatorsDifferentObjects(void)
{
  TestApplication application;

  // Create multiple objects and verify comparison operators work correctly
  SharedPtr<TestClass> ptr1(new TestClass());
  SharedPtr<TestClass> ptr2(new TestClass());
  SharedPtr<TestClass> ptr3 = ptr1;
  SharedPtr<TestClass> ptr4;

  // Test all comparison operators
  bool lessThan = (ptr1.Get() < ptr2.Get());
  DALI_TEST_CHECK((ptr1 < ptr2) == lessThan);
  DALI_TEST_CHECK((ptr1 > ptr2) == !lessThan);
  DALI_TEST_CHECK((ptr1 <= ptr2) == lessThan || (ptr1.Get() == ptr2.Get()));
  DALI_TEST_CHECK((ptr1 >= ptr2) == !lessThan || (ptr1.Get() == ptr2.Get()));

  // Test with same pointers
  DALI_TEST_CHECK(ptr1 == ptr3);
  DALI_TEST_CHECK(!(ptr1 != ptr3));
  DALI_TEST_CHECK(!(ptr1 < ptr3));
  DALI_TEST_CHECK(!(ptr1 > ptr3));
  DALI_TEST_CHECK(ptr1 <= ptr3);
  DALI_TEST_CHECK(ptr1 >= ptr3);

  // Test with nullptr
  DALI_TEST_CHECK(ptr4 == SharedPtr<TestClass>());
  DALI_TEST_CHECK(!(ptr4 != SharedPtr<TestClass>()));
  DALI_TEST_CHECK(ptr1 != ptr4);
  DALI_TEST_CHECK(!(ptr1 == ptr4));

  END_TEST;
}

int UtcDaliSharedPtrAssignmentChaining(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  {
    SharedPtr<TestClass> ptr1(new TestClass(destructorCalled));
    SharedPtr<TestClass> ptr2, ptr3, ptr4;

    ptr4 = ptr3 = ptr2 = ptr1;

    DALI_TEST_CHECK(ptr1.UseCount() == 4);
    DALI_TEST_CHECK(ptr2.UseCount() == 4);
    DALI_TEST_CHECK(ptr3.UseCount() == 4);
    DALI_TEST_CHECK(ptr4.UseCount() == 4);
    DALI_TEST_CHECK(ptr1.Get() == ptr2.Get());
    DALI_TEST_CHECK(ptr2.Get() == ptr3.Get());
    DALI_TEST_CHECK(ptr3.Get() == ptr4.Get());
    DALI_TEST_CHECK(!destructorCalled);
  }
  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

int UtcDaliSharedPtrConstructorWithNewAndReset(void)
{
  TestApplication application;
  bool            destructorCalled1(false);
  bool            destructorCalled2(false);

  SharedPtr<TestClass> ptr(new TestClass(destructorCalled1));
  DALI_TEST_CHECK(ptr.UseCount() == 1);

  ptr.Reset(new TestClass(destructorCalled2));
  DALI_TEST_CHECK(ptr.UseCount() == 1);
  DALI_TEST_CHECK(destructorCalled1);
  DALI_TEST_CHECK(!destructorCalled2);

  ptr.Reset();
  DALI_TEST_CHECK(ptr.UseCount() == 0);
  DALI_TEST_CHECK(destructorCalled2);

  END_TEST;
}

int UtcDaliSharedPtrResetWithSamePtr(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  SharedPtr<TestClass> ptr(new TestClass(destructorCalled));
  DALI_TEST_CHECK(!destructorCalled);
  DALI_TEST_CHECK(ptr.UseCount() == 1);

  auto rawPtr = ptr.Get();
  ptr.Reset(rawPtr);

  DALI_TEST_CHECK(!destructorCalled);

  END_TEST;
}

int UtcDaliSharedPtrIsEmpty(void)
{
  TestApplication application;

  // Test empty SharedPtr
  SharedPtr<TestClass> ptr1;
  DALI_TEST_CHECK(ptr1.IsEmpty());
  DALI_TEST_CHECK(!ptr1);

  // Test SharedPtr constructed with nullptr
  SharedPtr<TestClass> ptr2(nullptr);
  DALI_TEST_CHECK(ptr2.IsEmpty());
  DALI_TEST_CHECK(!ptr2);

  // Test non-empty SharedPtr
  SharedPtr<TestClass> ptr3(new TestClass());
  DALI_TEST_CHECK(!ptr3.IsEmpty());
  DALI_TEST_CHECK(ptr3);

  // Test after reset
  ptr3.Reset();
  DALI_TEST_CHECK(ptr3.IsEmpty());
  DALI_TEST_CHECK(!ptr3);

  END_TEST;
}

int UtcDaliEnableSharedFromThis(void)
{
  TestApplication application;

  // Create a SharedPtr using MakeShared
  auto ptr = MakeShared<EnableSharedFromThisTestClass>(42);
  DALI_TEST_CHECK(ptr);
  DALI_TEST_CHECK(ptr->GetValue() == 42);
  DALI_TEST_CHECK(ptr.UseCount() == 1);

  // Call SharedFromThis() to get another SharedPtr to the same object
  SharedPtr<EnableSharedFromThisTestClass> ptr2 = ptr->SharedFromThis();
  DALI_TEST_CHECK(ptr2);
  DALI_TEST_CHECK(ptr2.Get() == ptr.Get());
  DALI_TEST_CHECK(ptr.UseCount() == 2);
  DALI_TEST_CHECK(ptr2.UseCount() == 2);
  DALI_TEST_CHECK(ptr2->GetValue() == 42);

  END_TEST;
}

int UtcDaliEnableSharedFromThisConst(void)
{
  TestApplication application;

  // Create a const SharedPtr
  const auto ptr = MakeShared<EnableSharedFromThisTestClass>(100);
  DALI_TEST_CHECK(ptr);
  DALI_TEST_CHECK(ptr->GetValue() == 100);

  // Call SharedFromThis() on a const object
  SharedPtr<const EnableSharedFromThisTestClass> ptr2 = ptr->SharedFromThis();
  DALI_TEST_CHECK(ptr2);
  DALI_TEST_CHECK(ptr2.Get() == ptr.Get());
  DALI_TEST_CHECK(ptr.UseCount() == 2);
  DALI_TEST_CHECK(ptr2.UseCount() == 2);

  END_TEST;
}

int UtcDaliEnableSharedFromThisCopy(void)
{
  TestApplication application;

  auto ptr1 = MakeShared<EnableSharedFromThisTestClass>(55);
  DALI_TEST_CHECK(ptr1.UseCount() == 1);

  // Get SharedFromThis and verify reference counting
  SharedPtr<EnableSharedFromThisTestClass> ptr2 = ptr1->SharedFromThis();
  DALI_TEST_CHECK(ptr1.UseCount() == 2);
  DALI_TEST_CHECK(ptr2.UseCount() == 2);

  // Create another copy
  SharedPtr<EnableSharedFromThisTestClass> ptr3(ptr1);
  DALI_TEST_CHECK(ptr1.UseCount() == 3);
  DALI_TEST_CHECK(ptr2.UseCount() == 3);
  DALI_TEST_CHECK(ptr3.UseCount() == 3);

  // All should point to the same object
  DALI_TEST_CHECK(ptr1.Get() == ptr2.Get());
  DALI_TEST_CHECK(ptr2.Get() == ptr3.Get());

  END_TEST;
}

int UtcDaliEnableSharedFromThisWithRawPointer(void)
{
  TestApplication application;

  // Create using raw pointer constructor
  SharedPtr<EnableSharedFromThisTestClass> ptr(new EnableSharedFromThisTestClass(77));
  DALI_TEST_CHECK(ptr);
  DALI_TEST_CHECK(ptr->GetValue() == 77);

  // SharedFromThis should work
  SharedPtr<EnableSharedFromThisTestClass> ptr2 = ptr->SharedFromThis();
  DALI_TEST_CHECK(ptr2.Get() == ptr.Get());
  DALI_TEST_CHECK(ptr.UseCount() == 2);
  DALI_TEST_CHECK(ptr2.UseCount() == 2);

  END_TEST;
}

int UtcDaliDynamicPointerCast(void)
{
  TestApplication application;

  // Create a derived class object held by base class pointer
  SharedPtr<AnotherBaseClass> basePtr(new AnotherDerivedClass());
  DALI_TEST_CHECK(basePtr);
  DALI_TEST_CHECK(basePtr->GetType() == 1);

  // Dynamic cast to derived class
  SharedPtr<AnotherDerivedClass> derivedPtr = DynamicPointerCast<AnotherDerivedClass>(basePtr);
  DALI_TEST_CHECK(derivedPtr);
  DALI_TEST_CHECK(derivedPtr->GetType() == 1);
  DALI_TEST_CHECK(derivedPtr->GetDerivedData() == 42);

  // Both pointers should share ownership
  DALI_TEST_CHECK(basePtr.UseCount() == 2);
  DALI_TEST_CHECK(derivedPtr.UseCount() == 2);

  END_TEST;
}

int UtcDaliDynamicPointerCastFail(void)
{
  TestApplication application;

  // Create a base class object (not a derived)
  SharedPtr<AnotherBaseClass> basePtr(new AnotherBaseClass());
  DALI_TEST_CHECK(basePtr);
  DALI_TEST_CHECK(basePtr->GetType() == 0);

  // Attempt to cast to derived - should fail and return empty SharedPtr
  SharedPtr<AnotherDerivedClass> derivedPtr = DynamicPointerCast<AnotherDerivedClass>(basePtr);
  DALI_TEST_CHECK(!derivedPtr);
  DALI_TEST_CHECK(derivedPtr.IsEmpty());

  // Original pointer should still be valid with use count of 1
  DALI_TEST_CHECK(basePtr);
  DALI_TEST_CHECK(basePtr.UseCount() == 1);

  END_TEST;
}

int UtcDaliDynamicPointerCastNull(void)
{
  TestApplication application;

  // Cast from null SharedPtr
  SharedPtr<AnotherBaseClass> basePtr;
  DALI_TEST_CHECK(!basePtr);

  SharedPtr<AnotherDerivedClass> derivedPtr = DynamicPointerCast<AnotherDerivedClass>(basePtr);
  DALI_TEST_CHECK(!derivedPtr);
  DALI_TEST_CHECK(derivedPtr.IsEmpty());

  END_TEST;
}

int UtcDaliDynamicPointerCastShared(void)
{
  TestApplication application;

  // Create derived object
  SharedPtr<AnotherDerivedClass> originalPtr(new AnotherDerivedClass());
  DALI_TEST_CHECK(originalPtr.UseCount() == 1);

  // Assign to base pointer
  SharedPtr<AnotherBaseClass> basePtr = originalPtr;
  DALI_TEST_CHECK(originalPtr.UseCount() == 2);
  DALI_TEST_CHECK(basePtr.UseCount() == 2);

  // Dynamic cast back to derived
  SharedPtr<AnotherDerivedClass> derivedPtr = DynamicPointerCast<AnotherDerivedClass>(basePtr);
  DALI_TEST_CHECK(derivedPtr);
  DALI_TEST_CHECK(derivedPtr.Get() == originalPtr.Get());

  // All three should share ownership
  DALI_TEST_CHECK(originalPtr.UseCount() == 3);
  DALI_TEST_CHECK(basePtr.UseCount() == 3);
  DALI_TEST_CHECK(derivedPtr.UseCount() == 3);

  END_TEST;
}

int UtcDaliDynamicPointerCastWithBaseClass(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  {
    // Use the existing BaseClass/DerivedClass hierarchy
    SharedPtr<BaseClass> basePtr(new DerivedClass(42, "test", destructorCalled));
    DALI_TEST_CHECK(basePtr);
    DALI_TEST_CHECK(basePtr->GetValue() == 42);
    DALI_TEST_CHECK(basePtr->GetDerivedValue() == 142); // Virtual call

    // Dynamic cast to derived
    SharedPtr<DerivedClass> derivedPtr = DynamicPointerCast<DerivedClass>(basePtr);
    DALI_TEST_CHECK(derivedPtr);
    DALI_TEST_CHECK(derivedPtr->GetName() == "test");
    DALI_TEST_CHECK(derivedPtr->GetDerivedValue() == 142);

    // Both share ownership
    DALI_TEST_CHECK(basePtr.UseCount() == 2);
    DALI_TEST_CHECK(derivedPtr.UseCount() == 2);
    DALI_TEST_CHECK(!destructorCalled);
  }

  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

///////////////////////////////////////////////////////////////////////////////////
// WeakPtr test cases
///////////////////////////////////////////////////////////////////////////////////

int UtcDaliWeakPtrDefaultConstructor(void)
{
  TestApplication application;

  WeakPtr<TestClass> weakPtr;
  DALI_TEST_CHECK(weakPtr.Expired());
  DALI_TEST_CHECK(weakPtr.UseCount() == 0);
  DALI_TEST_CHECK(weakPtr.Lock().IsEmpty());

  END_TEST;
}

int UtcDaliWeakPtrFromSharedPtr(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  {
    SharedPtr<TestClass> sharedPtr(new TestClass(destructorCalled));
    WeakPtr<TestClass>   weakPtr(sharedPtr);

    DALI_TEST_CHECK(!weakPtr.Expired());
    DALI_TEST_CHECK(weakPtr.UseCount() == 1);

    // Lock should return a valid SharedPtr
    SharedPtr<TestClass> lockedPtr = weakPtr.Lock();
    DALI_TEST_CHECK(lockedPtr);
    DALI_TEST_CHECK(lockedPtr.Get() == sharedPtr.Get());
    DALI_TEST_CHECK(sharedPtr.UseCount() == 2);
    DALI_TEST_CHECK(lockedPtr.UseCount() == 2);
    DALI_TEST_CHECK(!destructorCalled);
  }

  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

int UtcDaliWeakPtrExpired(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  WeakPtr<TestClass> weakPtr;
  {
    SharedPtr<TestClass> sharedPtr(new TestClass(destructorCalled));
    weakPtr = sharedPtr;
    DALI_TEST_CHECK(!weakPtr.Expired());
  }

  // SharedPtr is destroyed, weakPtr should be expired
  DALI_TEST_CHECK(weakPtr.Expired());
  DALI_TEST_CHECK(weakPtr.Lock().IsEmpty());
  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

int UtcDaliWeakPtrCopyConstructor(void)
{
  TestApplication application;

  SharedPtr<TestClass> sharedPtr(new TestClass());
  WeakPtr<TestClass>   weakPtr1(sharedPtr);

  WeakPtr<TestClass> weakPtr2(weakPtr1);

  DALI_TEST_CHECK(!weakPtr1.Expired());
  DALI_TEST_CHECK(!weakPtr2.Expired());
  DALI_TEST_CHECK(weakPtr1.Lock().Get() == weakPtr2.Lock().Get());

  END_TEST;
}

int UtcDaliWeakPtrMoveConstructor(void)
{
  TestApplication application;

  SharedPtr<TestClass> sharedPtr(new TestClass());
  WeakPtr<TestClass>   weakPtr1(sharedPtr);

  WeakPtr<TestClass> weakPtr2(std::move(weakPtr1));

  DALI_TEST_CHECK(weakPtr1.Expired()); // Moved-from state
  DALI_TEST_CHECK(!weakPtr2.Expired());
  DALI_TEST_CHECK(weakPtr2.Lock().Get() == sharedPtr.Get());

  END_TEST;
}

int UtcDaliWeakPtrAssignment(void)
{
  TestApplication application;

  SharedPtr<TestClass> sharedPtr1(new TestClass());
  SharedPtr<TestClass> sharedPtr2(new TestClass());

  WeakPtr<TestClass> weakPtr(sharedPtr1);
  DALI_TEST_CHECK(weakPtr.Lock().Get() == sharedPtr1.Get());

  weakPtr = sharedPtr2;
  DALI_TEST_CHECK(weakPtr.Lock().Get() == sharedPtr2.Get());

  END_TEST;
}

int UtcDaliWeakPtrReset(void)
{
  TestApplication application;

  SharedPtr<TestClass> sharedPtr(new TestClass());
  WeakPtr<TestClass>   weakPtr(sharedPtr);

  DALI_TEST_CHECK(!weakPtr.Expired());

  weakPtr.Reset();

  DALI_TEST_CHECK(weakPtr.Expired());
  DALI_TEST_CHECK(weakPtr.Lock().IsEmpty());

  // Original SharedPtr should still be valid
  DALI_TEST_CHECK(sharedPtr);
  DALI_TEST_CHECK(sharedPtr.UseCount() == 1);

  END_TEST;
}

int UtcDaliWeakPtrWithEnableSharedFromThis(void)
{
  TestApplication application;

  auto sharedPtr = MakeShared<EnableSharedFromThisTestClass>(42);
  DALI_TEST_CHECK(sharedPtr);

  // Get WeakPtr from the object
  WeakPtr<EnableSharedFromThisTestClass> weakPtr = sharedPtr->WeakFromThis();
  DALI_TEST_CHECK(!weakPtr.Expired());

  // Lock should return a valid SharedPtr
  auto lockedPtr = weakPtr.Lock();
  DALI_TEST_CHECK(lockedPtr);
  DALI_TEST_CHECK(lockedPtr.Get() == sharedPtr.Get());
  DALI_TEST_CHECK(sharedPtr.UseCount() == 2);

  END_TEST;
}

int UtcDaliWeakPtrPolymorphic(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  {
    SharedPtr<DerivedClass> derivedPtr(new DerivedClass(42, "test", destructorCalled));
    SharedPtr<BaseClass>    basePtr(derivedPtr);

    WeakPtr<BaseClass> weakBasePtr(basePtr);
    DALI_TEST_CHECK(!weakBasePtr.Expired());

    auto lockedPtr = weakBasePtr.Lock();
    DALI_TEST_CHECK(lockedPtr);
    DALI_TEST_CHECK(lockedPtr.Get() == basePtr.Get());
    DALI_TEST_CHECK(basePtr.UseCount() == 3); // derivedPtr, basePtr, lockedPtr
  }

  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

int UtcDaliWeakPtrMultipleWeakRefs(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  {
    SharedPtr<TestClass> sharedPtr(new TestClass(destructorCalled));

    WeakPtr<TestClass> weak1(sharedPtr);
    WeakPtr<TestClass> weak2(sharedPtr);
    WeakPtr<TestClass> weak3(weak1);

    DALI_TEST_CHECK(!weak1.Expired());
    DALI_TEST_CHECK(!weak2.Expired());
    DALI_TEST_CHECK(!weak3.Expired());

    // All should lock to the same object
    DALI_TEST_CHECK(weak1.Lock().Get() == sharedPtr.Get());
    DALI_TEST_CHECK(weak2.Lock().Get() == sharedPtr.Get());
    DALI_TEST_CHECK(weak3.Lock().Get() == sharedPtr.Get());
    DALI_TEST_CHECK(!destructorCalled);
  }

  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

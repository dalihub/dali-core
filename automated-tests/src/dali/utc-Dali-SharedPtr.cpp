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

    // Self std::move assignment
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

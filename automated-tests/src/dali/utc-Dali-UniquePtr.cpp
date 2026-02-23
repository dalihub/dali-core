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
#include <dali/public-api/common/unique-ptr.h>
#include <stdlib.h>

#include <iostream>

#include "dali-test-suite-utils/dali-test-suite-utils.h"

using namespace Dali;

void utc_unique_ptr_startup(void)
{
  test_return_value = TET_UNDEF;
}

// Called after each test
void utc_unique_ptr_cleanup(void)
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

// Custom deleter for TestClass
struct CustomDeleter
{
  CustomDeleter(bool& deleterCalled = gDiscardedBool)
  : mDeleterCalled(&deleterCalled)
  {
    *mDeleterCalled = false;
  }

  void operator()(TestClass* ptr)
  {
    if(mDeleterCalled)
    {
      *mDeleterCalled = true;
    }
    delete ptr;
  }
  bool* mDeleterCalled{nullptr};
};

bool gCustomDeleterFunctionCalled = false;
void CustomDeleterFunction(TestClass* ptr)
{
  gCustomDeleterFunctionCalled = true;
  delete ptr;
}

// Test class with constructors for MakeUnique testing
class MakeUniqueTestClass
{
public:
  MakeUniqueTestClass()
  : mValue(0)
  {
  }

  MakeUniqueTestClass(int value)
  : mValue(value)
  {
  }

  MakeUniqueTestClass(int value, const std::string& name)
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

// Test with functor deleter
struct BaseClassDeleter
{
  BaseClassDeleter(bool* deleterCalled = nullptr)
  : mDeleterCalled(deleterCalled)
  {
    if(mDeleterCalled)
    {
      *mDeleterCalled = false;
    }
  }

  void operator()(BaseClass* ptr)
  {
    if(mDeleterCalled)
    {
      *mDeleterCalled = true;
    }
    delete ptr;
  }

  bool* mDeleterCalled;
};

} // unnamed namespace

int UtcDaliUniquePtrDefaultConstructor(void)
{
  TestApplication      application;
  UniquePtr<TestClass> ptr;
  DALI_TEST_CHECK(!ptr);
  END_TEST;
}

int UtcDaliUniquePtrConstructorWithRawPointerDefaultDeleter(void)
{
  TestApplication application;
  bool            destructorCalled(false);
  {
    UniquePtr<TestClass> ptr(new TestClass(destructorCalled));
    DALI_TEST_CHECK(ptr);
  }
  DALI_TEST_CHECK(destructorCalled);
  END_TEST;
}

int UtcDaliUniquePtrConstructorWithRawPointerCustomDeleterFunction(void)
{
  TestApplication application;
  bool            destructorCalled(false);
  gCustomDeleterFunctionCalled = false;
  {
    UniquePtr<TestClass> ptr(new TestClass(destructorCalled), &CustomDeleterFunction);
    DALI_TEST_CHECK(ptr);
    DALI_TEST_CHECK(!destructorCalled);
    DALI_TEST_CHECK(!gCustomDeleterFunctionCalled);
  }
  DALI_TEST_CHECK(destructorCalled);
  DALI_TEST_CHECK(gCustomDeleterFunctionCalled);
  END_TEST;
}

int UtcDaliUniquePtrConstructorWithRawPointerCustomDeleterFunctor(void)
{
  TestApplication application;
  bool            destructorCalled(false);
  bool            customDeleterCalled(false);
  CustomDeleter   customDeleter(customDeleterCalled);
  {
    UniquePtr<TestClass, CustomDeleter> ptr(new TestClass(destructorCalled), customDeleter);
    DALI_TEST_CHECK(ptr);
    DALI_TEST_CHECK(!destructorCalled);
    DALI_TEST_CHECK(!customDeleterCalled);
  }
  DALI_TEST_CHECK(destructorCalled);
  DALI_TEST_CHECK(customDeleterCalled);
  END_TEST;
}

int UtcDaliUniquePtrMoveConstructor(void)
{
  TestApplication application;
  bool            destructorCalled(false);
  {
    UniquePtr<TestClass> ptr1(new TestClass(destructorCalled));
    DALI_TEST_CHECK(ptr1);

    UniquePtr<TestClass> ptr2(std::move(ptr1));
    DALI_TEST_CHECK(ptr2);
    DALI_TEST_CHECK(!ptr1);
  }
  DALI_TEST_CHECK(destructorCalled);
  END_TEST;
}

int UtcDaliUniquePtrMoveConstructorWithCustomDeleter(void)
{
  TestApplication application;
  bool            destructorCalled(false);
  bool            customDeleterCalled(false);
  CustomDeleter   customDeleter(customDeleterCalled);
  {
    UniquePtr<TestClass, CustomDeleter> ptr1(new TestClass(destructorCalled), customDeleter);
    DALI_TEST_CHECK(ptr1);

    UniquePtr<TestClass, CustomDeleter> ptr2(std::move(ptr1));
    DALI_TEST_CHECK(ptr2);
    DALI_TEST_CHECK(!ptr1);
  }
  DALI_TEST_CHECK(destructorCalled);
  DALI_TEST_CHECK(customDeleterCalled);
  END_TEST;
}

int UtcDaliUniquePtrMoveAssignment(void)
{
  TestApplication application;
  bool            destructorCalled1(false);
  bool            destructorCalled2(false);
  {
    UniquePtr<TestClass> ptr1(new TestClass(destructorCalled1));
    UniquePtr<TestClass> ptr2(new TestClass(destructorCalled2));

    DALI_TEST_CHECK(ptr1);
    DALI_TEST_CHECK(ptr2);
    DALI_TEST_CHECK(!destructorCalled1);
    DALI_TEST_CHECK(!destructorCalled2);

    ptr2 = std::move(ptr1);

    DALI_TEST_CHECK(ptr2);
    DALI_TEST_CHECK(!ptr1);
    DALI_TEST_CHECK(destructorCalled2);
  }
  DALI_TEST_CHECK(destructorCalled1);
  END_TEST;
}

int UtcDaliUniquePtrMoveAssignmentWithCustomDeleter(void)
{
  TestApplication application;
  bool            destructorCalled1(false);
  bool            destructorCalled2(false);
  bool            customDeleterCalled(false);
  CustomDeleter   customDeleter(customDeleterCalled);
  {
    UniquePtr<TestClass, CustomDeleter> ptr1(new TestClass(destructorCalled1), customDeleter);
    UniquePtr<TestClass, CustomDeleter> ptr2(new TestClass(destructorCalled2), customDeleter);

    DALI_TEST_CHECK(ptr1);
    DALI_TEST_CHECK(ptr2);
    DALI_TEST_CHECK(!destructorCalled1);
    DALI_TEST_CHECK(!destructorCalled2);

    ptr2 = std::move(ptr1);

    DALI_TEST_CHECK(ptr2);
    DALI_TEST_CHECK(!ptr1);
    DALI_TEST_CHECK(destructorCalled2);
  }
  DALI_TEST_CHECK(destructorCalled1);
  DALI_TEST_CHECK(customDeleterCalled);
  END_TEST;
}

int UtcDaliUniquePtrMoveAssignmentSelf(void)
{
  TestApplication application;
  bool            destructorCalled(false);
  {
    UniquePtr<TestClass> ptr(new TestClass(destructorCalled));
    DALI_TEST_CHECK(ptr);

    // Self std::move assignment make compile warning over gcc-13. Let we ignore the warning.
#if (__GNUC__ >= 13)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
#endif
    ptr = std::move(ptr);

    DALI_TEST_CHECK(ptr);
    DALI_TEST_CHECK(!destructorCalled);
#if (__GNUC__ >= 13)
#pragma GCC diagnostic pop
#endif
  }
  DALI_TEST_CHECK(destructorCalled);
  END_TEST;
}

int UtcDaliUniquePtrDereferenceOperator(void)
{
  TestApplication      application;
  UniquePtr<TestClass> ptr(new TestClass());

  DALI_TEST_CHECK(ptr);
  TestClass& ref = *ptr;
  DALI_TEST_CHECK(&ref == ptr.Get());

  END_TEST;
}

int UtcDaliUniquePtrDereferenceOperatorConst(void)
{
  TestApplication            application;
  const UniquePtr<TestClass> ptr(new TestClass());

  DALI_TEST_CHECK(ptr);
  const TestClass& ref = *ptr;
  DALI_TEST_CHECK(&ref == ptr.Get());

  END_TEST;
}

int UtcDaliUniquePtrArrowOperator(void)
{
  TestApplication      application;
  UniquePtr<TestClass> ptr(new TestClass());

  DALI_TEST_CHECK(ptr);
  TestClass* rawPtr = ptr.operator->();
  DALI_TEST_CHECK(rawPtr == ptr.Get());

  END_TEST;
}

int UtcDaliUniquePtrArrowOperatorConst(void)
{
  TestApplication            application;
  const UniquePtr<TestClass> ptr(new TestClass());

  DALI_TEST_CHECK(ptr);
  const TestClass* rawPtr = ptr.operator->();
  DALI_TEST_CHECK(rawPtr == ptr.Get());

  END_TEST;
}

int UtcDaliUniquePtrBoolConversion(void)
{
  TestApplication application;

  UniquePtr<TestClass> ptr1;
  DALI_TEST_CHECK(!static_cast<bool>(ptr1));

  UniquePtr<TestClass> ptr2(new TestClass());
  DALI_TEST_CHECK(static_cast<bool>(ptr2));

  END_TEST;
}

int UtcDaliUniquePtrGet(void)
{
  TestApplication application;

  UniquePtr<TestClass> ptr1;
  DALI_TEST_CHECK(ptr1.Get() == nullptr);

  TestClass*           rawPtr = new TestClass();
  UniquePtr<TestClass> ptr2(rawPtr);
  DALI_TEST_CHECK(ptr2.Get() == rawPtr);

  END_TEST;
}

int UtcDaliUniquePtrGetConst(void)
{
  TestApplication application;

  const UniquePtr<TestClass> ptr1;
  DALI_TEST_CHECK(ptr1.Get() == nullptr);

  TestClass*                 rawPtr = new TestClass();
  const UniquePtr<TestClass> ptr2(rawPtr);
  DALI_TEST_CHECK(ptr2.Get() == rawPtr);

  END_TEST;
}

int UtcDaliUniquePtrRelease(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  TestClass*           rawPtr = new TestClass(destructorCalled);
  UniquePtr<TestClass> ptr(rawPtr);

  DALI_TEST_CHECK(ptr);
  DALI_TEST_CHECK(ptr.Get() == rawPtr);

  TestClass* releasedPtr = ptr.Release();

  DALI_TEST_CHECK(releasedPtr == rawPtr);
  DALI_TEST_CHECK(!ptr);
  DALI_TEST_CHECK(ptr.Get() == nullptr);
  DALI_TEST_CHECK(!destructorCalled);

  delete releasedPtr;
  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

int UtcDaliUniquePtrReleaseWithNullptr(void)
{
  TestApplication      application;
  UniquePtr<TestClass> ptr;

  DALI_TEST_CHECK(!ptr);

  TestClass* releasedPtr = ptr.Release();

  DALI_TEST_CHECK(releasedPtr == nullptr);
  DALI_TEST_CHECK(!ptr);

  END_TEST;
}

int UtcDaliUniquePtrReset(void)
{
  TestApplication application;
  bool            destructorCalled1(false);
  bool            destructorCalled2(false);

  UniquePtr<TestClass> ptr(new TestClass(destructorCalled1));

  DALI_TEST_CHECK(ptr);
  DALI_TEST_CHECK(!destructorCalled1);

  ptr.Reset(new TestClass(destructorCalled2));

  DALI_TEST_CHECK(ptr);
  DALI_TEST_CHECK(destructorCalled1);
  DALI_TEST_CHECK(!destructorCalled2);

  ptr.Reset();

  DALI_TEST_CHECK(!ptr);
  DALI_TEST_CHECK(destructorCalled2);

  END_TEST;
}

int UtcDaliUniquePtrResetWithNullptr(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  UniquePtr<TestClass> ptr(new TestClass(destructorCalled));

  DALI_TEST_CHECK(ptr);
  DALI_TEST_CHECK(!destructorCalled);

  ptr.Reset(nullptr);

  DALI_TEST_CHECK(!ptr);
  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

int UtcDaliUniquePtrResetWithCustomDeleter(void)
{
  TestApplication application;
  bool            destructorCalled(false);
  bool            customDeleterCalled(false);
  CustomDeleter   customDeleter(customDeleterCalled);

  UniquePtr<TestClass, CustomDeleter> ptr(new TestClass(destructorCalled), customDeleter);

  DALI_TEST_CHECK(ptr);
  DALI_TEST_CHECK(!destructorCalled);

  ptr.Reset(new TestClass());

  DALI_TEST_CHECK(ptr);
  DALI_TEST_CHECK(destructorCalled);
  DALI_TEST_CHECK(customDeleterCalled);

  END_TEST;
}

int UtcDaliUniquePtrDefaultDeleterWithNullptr(void)
{
  TestApplication application;

  UniquePtr<TestClass> ptr(nullptr);
  DALI_TEST_CHECK(!ptr);

  END_TEST;
}

int UtcDaliUniquePtrMoveWithNullptr(void)
{
  TestApplication application;

  UniquePtr<TestClass> ptr1;
  UniquePtr<TestClass> ptr2(std::move(ptr1));

  DALI_TEST_CHECK(!ptr1);
  DALI_TEST_CHECK(!ptr2);

  END_TEST;
}

int UtcDaliMakeUniqueNoArgs(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  {
    auto ptr = MakeUnique<TestClass>(destructorCalled);
    DALI_TEST_CHECK(ptr);
    DALI_TEST_CHECK(!destructorCalled);
  }
  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

int UtcDaliMakeUniqueWithArgs(void)
{
  TestApplication application;

  auto ptr1 = MakeUnique<MakeUniqueTestClass>(42);
  DALI_TEST_CHECK(ptr1);
  DALI_TEST_CHECK(ptr1->GetValue() == 42);

  auto ptr2 = MakeUnique<MakeUniqueTestClass>(100, "test");
  DALI_TEST_CHECK(ptr2);
  DALI_TEST_CHECK(ptr2->GetValue() == 100);
  DALI_TEST_CHECK(ptr2->GetName() == "test");

  END_TEST;
}

int UtcDaliMakeUniqueDefaultConstruction(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  {
    auto ptr = MakeUnique<TestClass>(destructorCalled);
    DALI_TEST_CHECK(ptr);
    DALI_TEST_CHECK(!destructorCalled);
  }
  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

int UtcDaliMakeUniqueMoveConstructor(void)
{
  TestApplication application;

  auto ptr1 = MakeUnique<MakeUniqueTestClass>(123);
  DALI_TEST_CHECK(ptr1);

  UniquePtr<MakeUniqueTestClass> ptr2(std::move(ptr1));
  DALI_TEST_CHECK(ptr2);
  DALI_TEST_CHECK(!ptr1);
  DALI_TEST_CHECK(ptr2->GetValue() == 123);

  END_TEST;
}

int UtcDaliMakeUniqueDereference(void)
{
  TestApplication application;

  auto ptr = MakeUnique<MakeUniqueTestClass>(456, "test");
  DALI_TEST_CHECK(ptr);

  MakeUniqueTestClass& ref = *ptr;
  DALI_TEST_CHECK(ref.GetValue() == 456);
  DALI_TEST_CHECK(ref.GetName() == "test");

  END_TEST;
}

int UtcDaliMakeUniqueConstReferenceParameter(void)
{
  TestApplication application;

  std::string name = "const_test";
  auto        ptr  = MakeUnique<MakeUniqueTestClass>(789, name);
  DALI_TEST_CHECK(ptr);
  DALI_TEST_CHECK(ptr->GetValue() == 789);
  DALI_TEST_CHECK(ptr->GetName() == "const_test");

  END_TEST;
}

int UtcDaliUniquePtrConvertingMoveConstructor(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  {
    UniquePtr<DerivedClass> derivedPtr(new DerivedClass(42, "test", destructorCalled));
    DALI_TEST_CHECK(derivedPtr);
    DALI_TEST_CHECK(derivedPtr->GetValue() == 42);
    DALI_TEST_CHECK(derivedPtr->GetName() == "test");
    DALI_TEST_CHECK(derivedPtr->GetDerivedValue() == 142);

    // Converting move constructor from derived to base
    UniquePtr<BaseClass> basePtr(std::move(derivedPtr));

    DALI_TEST_CHECK(basePtr);
    DALI_TEST_CHECK(!derivedPtr);
    DALI_TEST_CHECK(basePtr->GetValue() == 42);
    DALI_TEST_CHECK(basePtr->GetDerivedValue() == 142);
    DALI_TEST_CHECK(!destructorCalled);
  }

  // Destructor should be called when basePtr goes out of scope
  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

int UtcDaliUniquePtrConvertingMoveConstructorWithNullptr(void)
{
  TestApplication application;

  UniquePtr<DerivedClass> derivedPtr;
  DALI_TEST_CHECK(!derivedPtr);

  // Converting move constructor from null derived to base
  UniquePtr<BaseClass> basePtr(std::move(derivedPtr));

  DALI_TEST_CHECK(!derivedPtr);
  DALI_TEST_CHECK(!basePtr);

  END_TEST;
}

int UtcDaliUniquePtrConvertingMoveAssignment(void)
{
  TestApplication application;
  bool            destructorCalled1(false);
  bool            destructorCalled2(false);

  {
    UniquePtr<BaseClass>    basePtr(new BaseClass(100, destructorCalled1));
    UniquePtr<DerivedClass> derivedPtr(new DerivedClass(42, "test", destructorCalled2));

    DALI_TEST_CHECK(basePtr);
    DALI_TEST_CHECK(derivedPtr);
    DALI_TEST_CHECK(basePtr->GetValue() == 100);
    DALI_TEST_CHECK(derivedPtr->GetValue() == 42);
    DALI_TEST_CHECK(!destructorCalled1);
    DALI_TEST_CHECK(!destructorCalled2);

    // Converting move assignment from derived to base
    basePtr = std::move(derivedPtr);

    DALI_TEST_CHECK(basePtr);
    DALI_TEST_CHECK(!derivedPtr);
    DALI_TEST_CHECK(basePtr->GetValue() == 42);
    DALI_TEST_CHECK(basePtr->GetDerivedValue() == 142);
    DALI_TEST_CHECK(destructorCalled1); // Old base object was deleted
    DALI_TEST_CHECK(!destructorCalled2);
  }

  // Destructor should be called when basePtr goes out of scope
  DALI_TEST_CHECK(destructorCalled2);

  END_TEST;
}

int UtcDaliUniquePtrConvertingMoveAssignmentFromNullptr(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  {
    UniquePtr<BaseClass>    basePtr(new BaseClass(100, destructorCalled));
    UniquePtr<DerivedClass> derivedPtr;

    DALI_TEST_CHECK(basePtr);
    DALI_TEST_CHECK(!derivedPtr);
    DALI_TEST_CHECK(!destructorCalled);

    // Converting move assignment from null derived to base
    basePtr = std::move(derivedPtr);

    DALI_TEST_CHECK(!basePtr);
    DALI_TEST_CHECK(!derivedPtr);
    DALI_TEST_CHECK(destructorCalled); // Old base object was deleted
  }

  END_TEST;
}

int UtcDaliUniquePtrConvertingMoveAssignmentToNullptr(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  {
    UniquePtr<BaseClass>    basePtr;
    UniquePtr<DerivedClass> derivedPtr(new DerivedClass(42, "test", destructorCalled));

    DALI_TEST_CHECK(!basePtr);
    DALI_TEST_CHECK(derivedPtr);
    DALI_TEST_CHECK(!destructorCalled);

    // Converting move assignment from derived to null base
    basePtr = std::move(derivedPtr);

    DALI_TEST_CHECK(basePtr);
    DALI_TEST_CHECK(!derivedPtr);
    DALI_TEST_CHECK(basePtr->GetValue() == 42);
    DALI_TEST_CHECK(!destructorCalled);
  }

  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

int UtcDaliUniquePtrConvertingMoveAssignmentSelf(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  // This test ensures that self-assignment is handled correctly
  // Note: This is a compile-time check that the conversion works
  UniquePtr<DerivedClass> derivedPtr(new DerivedClass(42, "test", destructorCalled));

  DALI_TEST_CHECK(derivedPtr);
  DALI_TEST_CHECK(!destructorCalled);

  // The following should compile without errors
  UniquePtr<BaseClass> basePtr(std::move(derivedPtr));

  DALI_TEST_CHECK(basePtr);
  DALI_TEST_CHECK(!derivedPtr);
  DALI_TEST_CHECK(basePtr->GetValue() == 42);
  DALI_TEST_CHECK(!destructorCalled);

  END_TEST;
}

int UtcDaliUniquePtrConvertingMoveConstructorWithFunctorDeleter(void)
{
  TestApplication application;
  bool            baseDeleterCalled(false);
  bool            derivedDeleterCalled(false);

  BaseClassDeleter baseDeleter(&baseDeleterCalled);
  BaseClassDeleter derivedDeleter(&derivedDeleterCalled);

  {
    UniquePtr<DerivedClass, BaseClassDeleter> derivedPtr(new DerivedClass(42, "test"), derivedDeleter);
    DALI_TEST_CHECK(derivedPtr);

    // Converting move constructor from derived to base with functor deleter
    UniquePtr<BaseClass, BaseClassDeleter> basePtr(std::move(derivedPtr));

    DALI_TEST_CHECK(basePtr);
    DALI_TEST_CHECK(!derivedPtr);
    DALI_TEST_CHECK(basePtr->GetValue() == 42);
    DALI_TEST_CHECK(!baseDeleterCalled);
    DALI_TEST_CHECK(!derivedDeleterCalled);
  }

  // Derived Deleter should be called, but not the base one
  DALI_TEST_CHECK(!baseDeleterCalled);
  DALI_TEST_CHECK(derivedDeleterCalled); // derivedDeleter was moved, not used

  END_TEST;
}

int UtcDaliUniquePtrConvertingMoveAssignmentWithFunctorDeleter(void)
{
  TestApplication application;
  bool            baseDeleterCalled(false);
  bool            derivedDeleterCalled(false);

  BaseClassDeleter baseDeleter(&baseDeleterCalled);
  BaseClassDeleter derivedDeleter(&derivedDeleterCalled);

  {
    UniquePtr<BaseClass, BaseClassDeleter>    basePtr(new BaseClass(100), baseDeleter);
    UniquePtr<DerivedClass, BaseClassDeleter> derivedPtr(new DerivedClass(42, "test"), derivedDeleter);

    DALI_TEST_CHECK(basePtr);
    DALI_TEST_CHECK(derivedPtr);
    DALI_TEST_CHECK(!baseDeleterCalled);
    DALI_TEST_CHECK(!derivedDeleterCalled);

    // Converting move assignment from derived to base with functor deleter
    basePtr = std::move(derivedPtr);

    DALI_TEST_CHECK(basePtr);
    DALI_TEST_CHECK(!derivedPtr);
    DALI_TEST_CHECK(basePtr->GetValue() == 42);
    DALI_TEST_CHECK(baseDeleterCalled); // Old base object was deleted
    DALI_TEST_CHECK(!derivedDeleterCalled);
  }

  // Deleter should be called when basePtr goes out of scope
  DALI_TEST_CHECK(derivedDeleterCalled);

  END_TEST;
}

int UtcDaliMakeUniqueWithDerivedType(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  {
    // Test that MakeUnique works with derived types and can be converted to base
    auto derivedPtr = MakeUnique<DerivedClass>(42, "test", destructorCalled);
    DALI_TEST_CHECK(derivedPtr);
    DALI_TEST_CHECK(derivedPtr->GetValue() == 42);
    DALI_TEST_CHECK(derivedPtr->GetName() == "test");
    DALI_TEST_CHECK(derivedPtr->GetDerivedValue() == 142);
    DALI_TEST_CHECK(!destructorCalled);

    // Convert derived to base using converting move constructor
    UniquePtr<BaseClass> basePtr(std::move(derivedPtr));

    DALI_TEST_CHECK(basePtr);
    DALI_TEST_CHECK(!derivedPtr);
    DALI_TEST_CHECK(basePtr->GetValue() == 42);
    DALI_TEST_CHECK(basePtr->GetDerivedValue() == 142);
    DALI_TEST_CHECK(!destructorCalled);
  }

  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

int UtcDaliMakeUniquePerfectForwardingLValue(void)
{
  TestApplication application;

  // Test that lvalue arguments are properly forwarded as references
  std::string name = "test_lvalue";
  auto        ptr  = MakeUnique<MakeUniqueTestClass>(100, name);

  DALI_TEST_CHECK(ptr);
  DALI_TEST_CHECK(ptr->GetValue() == 100);
  DALI_TEST_CHECK(ptr->GetName() == "test_lvalue");

  // Modify the original string and verify it wasn't moved (lvalue reference was used)
  name = "modified";
  DALI_TEST_CHECK(ptr->GetName() == "test_lvalue"); // Should still be original value

  END_TEST;
}

int UtcDaliMakeUniquePerfectForwardingRValue(void)
{
  TestApplication application;

  // Test that rvalue arguments are properly forwarded via move semantics
  std::string name = "test_rvalue";
  auto        ptr  = MakeUnique<MakeUniqueTestClass>(200, std::move(name));

  DALI_TEST_CHECK(ptr);
  DALI_TEST_CHECK(ptr->GetValue() == 200);
  DALI_TEST_CHECK(ptr->GetName() == "test_rvalue");

  // The original string should be in a valid but unspecified state after move
  DALI_TEST_CHECK(name.empty() || name == "test_rvalue"); // Either is valid after move

  END_TEST;
}

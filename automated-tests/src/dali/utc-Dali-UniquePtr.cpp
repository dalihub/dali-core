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

// Empty Custom Functor Deleter to check for EBO
struct CustomDeleterEmpty
{
  void operator()(TestClass* p) const
  {
    delete p;
  }
};

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
    UniquePtr<TestClass, void (*)(TestClass*)> ptr(new TestClass(destructorCalled), &CustomDeleterFunction);
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

int UtcDaliUniquePtrResetWithSamePtr(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  UniquePtr<TestClass> ptr(new TestClass(destructorCalled));
  DALI_TEST_CHECK(ptr);
  DALI_TEST_CHECK(!destructorCalled);

  auto rawPtr = ptr.Get();
  ptr.Reset(rawPtr);

  DALI_TEST_CHECK(!destructorCalled);

  END_TEST;
}

int UtcDaliUniquePtrResetWithSamePtrCustomDeleter(void)
{
  TestApplication application;
  bool            destructorCalled(false);
  bool            customDeleterCalled(false);
  CustomDeleter   customDeleter(customDeleterCalled);

  UniquePtr<TestClass, CustomDeleter> ptr(new TestClass(destructorCalled), customDeleter);
  DALI_TEST_CHECK(ptr);
  DALI_TEST_CHECK(!destructorCalled);

  auto rawPtr = ptr.Get();
  ptr.Reset(rawPtr);

  DALI_TEST_CHECK(!destructorCalled);
  DALI_TEST_CHECK(!customDeleterCalled);

  END_TEST;
}

int UtcDaliUniquePtrEqualityOperator(void)
{
  TestApplication application;

  UniquePtr<TestClass> ptr1(new TestClass());
  UniquePtr<TestClass> ptr2(new TestClass());
  UniquePtr<TestClass> ptr3;

  // Two different pointers should not be equal
  DALI_TEST_CHECK(!(ptr1 == ptr2));

  // A pointer and a null pointer should not be equal
  DALI_TEST_CHECK(!(ptr1 == ptr3));

  // Two null pointers should be equal
  UniquePtr<TestClass> ptr4;
  DALI_TEST_CHECK(ptr3 == ptr4);

  // Self equality
  UniquePtr<TestClass> ptr5(new TestClass());
  DALI_TEST_CHECK(ptr5 == ptr5);

  END_TEST;
}

int UtcDaliUniquePtrInequalityOperator(void)
{
  TestApplication application;

  UniquePtr<TestClass> ptr1(new TestClass());
  UniquePtr<TestClass> ptr2(new TestClass());
  UniquePtr<TestClass> ptr3;

  // Two different pointers should be unequal
  DALI_TEST_CHECK(ptr1 != ptr2);

  // A pointer and a null pointer should be unequal
  DALI_TEST_CHECK(ptr1 != ptr3);

  // Two null pointers should not be unequal
  UniquePtr<TestClass> ptr4;
  DALI_TEST_CHECK(!(ptr3 != ptr4));

  // Self inequality
  UniquePtr<TestClass> ptr5(new TestClass());
  DALI_TEST_CHECK(!(ptr5 != ptr5));

  END_TEST;
}

int UtcDaliUniquePtrLessThanOperator(void)
{
  TestApplication application;

  UniquePtr<TestClass> ptr1(new TestClass());
  UniquePtr<TestClass> ptr2(new TestClass());
  UniquePtr<TestClass> ptr3;

  // One of two different pointers must be less than the other
  DALI_TEST_CHECK((ptr1 < ptr2) || (ptr2 < ptr1));

  // A null pointer should be less than a non-null pointer (or vice versa, depending on address)
  // At minimum, they should be orderable
  bool isLess    = (ptr3 < ptr1);
  bool isGreater = (ptr1 < ptr3);
  DALI_TEST_CHECK(isLess != isGreater); // Exactly one must be true

  // A pointer should not be less than itself
  DALI_TEST_CHECK(!(ptr1 < ptr1));

  END_TEST;
}

int UtcDaliUniquePtrLessThanOrEqualOperator(void)
{
  TestApplication application;

  UniquePtr<TestClass> ptr1(new TestClass());
  UniquePtr<TestClass> ptr2(new TestClass());

  // A pointer should be <= itself
  DALI_TEST_CHECK(ptr1 <= ptr1);

  // One of two different pointers must be <= the other
  DALI_TEST_CHECK((ptr1 <= ptr2) || (ptr2 <= ptr1));

  // Both should be true only if they point to the same address (they don't)
  if(ptr1.Get() != ptr2.Get())
  {
    // Exactly one should be strictly less
    DALI_TEST_CHECK((ptr1 <= ptr2) != (ptr2 <= ptr1) || (ptr1 <= ptr2 && ptr2 <= ptr1));
  }

  END_TEST;
}

int UtcDaliUniquePtrGreaterThanOperator(void)
{
  TestApplication application;

  UniquePtr<TestClass> ptr1(new TestClass());
  UniquePtr<TestClass> ptr2(new TestClass());

  // One of two different pointers must be greater than the other
  DALI_TEST_CHECK((ptr1 > ptr2) || (ptr2 > ptr1));

  // A pointer should not be greater than itself
  DALI_TEST_CHECK(!(ptr1 > ptr1));

  // Consistency with less-than: (a > b) == (b < a)
  DALI_TEST_CHECK((ptr1 > ptr2) == (ptr2 < ptr1));

  END_TEST;
}

int UtcDaliUniquePtrGreaterThanOrEqualOperator(void)
{
  TestApplication application;

  UniquePtr<TestClass> ptr1(new TestClass());
  UniquePtr<TestClass> ptr2(new TestClass());

  // A pointer should be >= itself
  DALI_TEST_CHECK(ptr1 >= ptr1);

  // One of two different pointers must be >= the other
  DALI_TEST_CHECK((ptr1 >= ptr2) || (ptr2 >= ptr1));

  // Consistency with less-than-or-equal: (a >= b) == (b <= a)
  DALI_TEST_CHECK((ptr1 >= ptr2) == (ptr2 <= ptr1));

  END_TEST;
}

int UtcDaliUniquePtrGetDeleterFunctionPointer(void)
{
  TestApplication application;
  bool            destructorCalled(false);
  gCustomDeleterFunctionCalled = false;

  using DeleterType = void (*)(TestClass*);
  UniquePtr<TestClass, DeleterType> ptr(new TestClass(destructorCalled), &CustomDeleterFunction);

  // GetDeleter should return the function pointer we passed
  DeleterType& deleter = ptr.GetDeleter();
  DALI_TEST_CHECK(deleter == &CustomDeleterFunction);

  // Verify the deleter works by destroying the object
  ptr.Reset();
  DALI_TEST_CHECK(destructorCalled);
  DALI_TEST_CHECK(gCustomDeleterFunctionCalled);

  END_TEST;
}

int UtcDaliUniquePtrGetDeleterFunctor(void)
{
  TestApplication application;
  bool            destructorCalled(false);
  bool            customDeleterCalled(false);
  CustomDeleter   customDeleter(customDeleterCalled);

  UniquePtr<TestClass, CustomDeleter> ptr(new TestClass(destructorCalled), customDeleter);

  // GetDeleter should return a reference to the CustomDeleter
  CustomDeleter& retrievedDeleter = ptr.GetDeleter();
  DALI_TEST_CHECK(retrievedDeleter.mDeleterCalled == &customDeleterCalled);

  // Verify the deleter works by destroying the object
  ptr.Reset();
  DALI_TEST_CHECK(destructorCalled);
  DALI_TEST_CHECK(customDeleterCalled);

  END_TEST;
}

int UtcDaliUniquePtrGetDeleterConst(void)
{
  TestApplication application;

  // Test const GetDeleter on function pointer specialization
  using DeleterType = void (*)(TestClass*);
  const UniquePtr<TestClass, DeleterType> ptr1(new TestClass(), &CustomDeleterFunction);
  const DeleterType&                      deleter1 = ptr1.GetDeleter();
  DALI_TEST_CHECK(deleter1 == &CustomDeleterFunction);

  // Test const GetDeleter on functor specialization
  bool                                      customDeleterCalled(false);
  CustomDeleter                             customDeleter(customDeleterCalled);
  const UniquePtr<TestClass, CustomDeleter> ptr2(new TestClass(), customDeleter);
  const CustomDeleter&                      deleter2 = ptr2.GetDeleter();
  DALI_TEST_CHECK(deleter2.mDeleterCalled == &customDeleterCalled);

  END_TEST;
}

int UtcDaliUniquePtrDefaultDeleterConvertingConstructor(void)
{
  // Test that DefaultDeleter<Derived> can be converted to DefaultDeleter<Base>
  DefaultDeleter<DerivedClass> derivedDeleter;
  DefaultDeleter<BaseClass>    baseDeleter(derivedDeleter);

  // Verify the deleter works by deleting a derived object through base pointer
  bool       destructorCalled(false);
  BaseClass* rawPtr = new DerivedClass(42, "test", destructorCalled);
  baseDeleter(rawPtr);
  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

int UtcDaliUniquePtrFunctorDeleterDefaultConstructor(void)
{
  TestApplication application;
  {
    // Default-construct UniquePtr with functor deleter
    // The deleter should be default-constructed
    UniquePtr<TestClass, CustomDeleter> ptr;
    DALI_TEST_CHECK(!ptr);
    DALI_TEST_CHECK(ptr.Get() == nullptr);

    // Now assign a new object
    ptr.Reset(new TestClass());
    DALI_TEST_CHECK(ptr);
  }
  // After going out of scope, the default-constructed CustomDeleter should be used
  // (it uses gDiscardedBool, so we can't easily check, but the test verifies no crash)

  END_TEST;
}

int UtcDaliUniquePtrNullptrDeleterFallback(void)
{
  TestApplication application;
  bool            destructorCalled(false);

  {
    // Passing nullptr as the deleter should fall back to the default deleter (simple delete)
    using DeleterType = void (*)(TestClass*);
    UniquePtr<TestClass, DeleterType> ptr(new TestClass(destructorCalled), nullptr);
    DALI_TEST_CHECK(ptr);

    // The deleter should have been replaced with the default (non-null) deleter
    DeleterType& deleter = ptr.GetDeleter();
    DALI_TEST_CHECK(deleter != nullptr);
  }
  // The object should still be properly deleted via the default deleter
  DALI_TEST_CHECK(destructorCalled);

  END_TEST;
}

int UtcDaliUniquePtrMoveAssignmentSelfWithCustomDeleter(void)
{
  TestApplication application;
  bool            destructorCalled(false);
  bool            customDeleterCalled(false);
  CustomDeleter   customDeleter(customDeleterCalled);
  {
    UniquePtr<TestClass, CustomDeleter> ptr(new TestClass(destructorCalled), customDeleter);
    DALI_TEST_CHECK(ptr);

    // Self std::move assignment make compile warning over gcc-13. Let we ignore the warning.
#if (__GNUC__ >= 13)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
#endif
    ptr = std::move(ptr);

    DALI_TEST_CHECK(ptr);
    DALI_TEST_CHECK(!destructorCalled);
    DALI_TEST_CHECK(!customDeleterCalled);
#if (__GNUC__ >= 13)
#pragma GCC diagnostic pop
#endif
  }
  DALI_TEST_CHECK(destructorCalled);
  DALI_TEST_CHECK(customDeleterCalled);

  END_TEST;
}

int UtcDaliUniquePtrReleaseWithFunctorDeleter(void)
{
  TestApplication application;
  bool            destructorCalled(false);
  bool            customDeleterCalled(false);
  CustomDeleter   customDeleter(customDeleterCalled);

  TestClass*                          rawPtr = new TestClass(destructorCalled);
  UniquePtr<TestClass, CustomDeleter> ptr(rawPtr, customDeleter);

  DALI_TEST_CHECK(ptr);
  DALI_TEST_CHECK(ptr.Get() == rawPtr);
  DALI_TEST_CHECK(!destructorCalled);
  DALI_TEST_CHECK(!customDeleterCalled);

  TestClass* releasedPtr = ptr.Release();

  DALI_TEST_CHECK(releasedPtr == rawPtr);
  DALI_TEST_CHECK(!ptr);
  DALI_TEST_CHECK(ptr.Get() == nullptr);
  DALI_TEST_CHECK(!destructorCalled);
  DALI_TEST_CHECK(!customDeleterCalled);

  // Manually delete the released pointer
  delete releasedPtr;
  DALI_TEST_CHECK(destructorCalled);
  // Custom deleter should NOT have been called since we used plain delete
  DALI_TEST_CHECK(!customDeleterCalled);

  END_TEST;
}

int UtcDaliUniquePtrSizeComparisons(void)
{
  const auto sizeOfTestClassPtr              = sizeof(TestClass*);
  const auto sizeOfTestClassFunctionPtr      = sizeof(void (*)(TestClass*));
  const auto sizeOfTestClassCustomDeleterPtr = sizeof(CustomDeleter*);

  // UniquePtr uses Empty Base Optimization, so shouldn't add any more memory usage when using the default deleter
  DALI_TEST_CHECK(sizeof(UniquePtr<TestClass>) == sizeOfTestClassPtr);

  // Function pointer needs to store BOTH ptr and function pointer
  DALI_TEST_CHECK(sizeof(UniquePtr<TestClass, void (*)(TestClass*)>) == (sizeOfTestClassPtr + sizeOfTestClassFunctionPtr));

  // An empty custom functor deleter should also use EBO
  DALI_TEST_CHECK(sizeof(UniquePtr<TestClass, CustomDeleterEmpty>) == sizeOfTestClassPtr);

  // A custom functor deleter with data needs an extra pointer to the functor though
  DALI_TEST_CHECK(sizeof(UniquePtr<TestClass, CustomDeleter>) == (sizeOfTestClassPtr + sizeOfTestClassCustomDeleterPtr));

  END_TEST;
}

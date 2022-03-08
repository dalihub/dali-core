/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

#include <dali-test-suite-utils.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <iostream>

using namespace Dali;

namespace
{
const int REPEAT = 1000;

size_t g_creationCount             = 0;
size_t g_destructionCount          = 0;
size_t g_creationCountSubclass     = 0;
size_t g_destructionCountSubclass  = 0;
size_t g_creationCountUnrelated    = 0;
size_t g_destructionCountUnrelated = 0;

class Counted : public RefObject
{
public:
  Counted()
  {
    ++g_creationCount;
  }
  ~Counted()
  {
    ++g_destructionCount;
  }
};

class CountedSubclass : public Counted
{
public:
  CountedSubclass()
  {
    ++g_creationCountSubclass;
  }
  ~CountedSubclass()
  {
    ++g_destructionCountSubclass;
  }
};

class UnrelatedCounted : public RefObject
{
public:
  UnrelatedCounted()
  {
    ++g_creationCountUnrelated;
  }
  ~UnrelatedCounted()
  {
    ++g_destructionCountUnrelated;
  }
};

class TestObject : public RefObject
{
public:
  TestObject()
  : data(201)
  {
  }

  TestObject(const TestObject& testObject)
  : RefObject(testObject),
    data(testObject.data)
  {
  }

  TestObject& Assign(const TestObject& testObject)
  {
    RefObject::operator=(testObject);
    data               = testObject.data;
    return *this;
  }

  int data;
};

} // Anonymous namespace

/**
 * Test that a default constructed pointer is null and harmless.
 */
int UtcDaliIntrusivePtrIntrusivePtr(void)
{
  tet_infoline("Testing Dali::IntrusivePtr::IntrusivePtr()");

  g_creationCount = g_destructionCount = 0;

  IntrusivePtr<Counted> counted;
  DALI_TEST_EQUALS(g_creationCount, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(g_destructionCount, 0u, TEST_LOCATION);
  // Test the pointer is null
  DALI_TEST_EQUALS(counted.Get(), (Counted*)0, TEST_LOCATION);
  DALI_TEST_EQUALS(&(*counted), (Counted*)0, TEST_LOCATION);
  // Check destruction of the null smart pointer does nothing:
  counted = IntrusivePtr<Counted>();
  DALI_TEST_EQUALS(g_creationCount, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(g_destructionCount, 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliIntrusivePtrIntrusivePtrTP(void)
{
  tet_infoline("Testing Dali::IntrusivePtr::IntrusivePtr(T*)");

  g_creationCount = g_destructionCount = 0;

  IntrusivePtr<Counted> counted(new Counted);
  DALI_TEST_EQUALS(g_creationCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(g_destructionCount, 0u, TEST_LOCATION);
  counted = 0;
  DALI_TEST_EQUALS(g_creationCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(g_destructionCount, 1u, TEST_LOCATION);

  END_TEST;
}

// Class is too simple for a negative case to be created: int UtcDaliIntrusiveIntrusivePtrTN(void)

int UtcDaliIntrusivePtrIntrusivePtrIntrusivePtrUP(void)
{
  tet_infoline("Testing Dali::IntrusivePtr::IntrusivePtr(IntrusivePtr<U> const &)");

  g_creationCount = g_destructionCount = g_creationCountSubclass = g_destructionCountSubclass = 0;

  IntrusivePtr<CountedSubclass> countedSubclass(new CountedSubclass);
  DALI_TEST_EQUALS(g_creationCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(g_creationCountSubclass, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(g_destructionCount, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(g_destructionCountSubclass, 0u, TEST_LOCATION);

  IntrusivePtr<Counted> counted(countedSubclass);
  DALI_TEST_EQUALS(counted->ReferenceCount(), 2, TEST_LOCATION);

  // Make loads more references:
  std::vector<IntrusivePtr<Counted> > intrusivePtrs;
  for(int i = 0; i < REPEAT; ++i)
  {
    intrusivePtrs.push_back(IntrusivePtr<Counted>(countedSubclass));
  }
  DALI_TEST_EQUALS(counted->ReferenceCount(), 2 + REPEAT, TEST_LOCATION);

  DALI_TEST_EQUALS(g_creationCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(g_creationCountSubclass, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(g_destructionCount, 0u, TEST_LOCATION);

  END_TEST;
}

// The negative version of this test would fail at compile time:
// int UtcDaliIntrusivePtrIntrusivePtrIntrusivePtrUN(void)

int UtcDaliIntrusivePtrIntrusivePtrIntrusivePtrP(void)
{
  tet_infoline("Testing Dali::IntrusivePtr::IntrusivePtr(IntrusivePtr const &)");

  // Pass a pointer to a constructed second object:
  // Pass a pointer to null:

  g_creationCount = g_destructionCount = 0;

  IntrusivePtr<Counted> counted(new Counted);
  DALI_TEST_EQUALS(g_creationCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(g_destructionCount, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(counted->ReferenceCount(), 1, TEST_LOCATION);

  IntrusivePtr<Counted> counted2(counted);
  DALI_TEST_EQUALS(counted->ReferenceCount(), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(counted.Get(), counted2.Get(), TEST_LOCATION);

  // Make loads more references:
  std::vector<IntrusivePtr<Counted> > intrusivePtrs;
  for(int i = 0; i < REPEAT; ++i)
  {
    intrusivePtrs.push_back(IntrusivePtr<Counted>(counted));
  }
  DALI_TEST_EQUALS(counted->ReferenceCount(), 2 + REPEAT, TEST_LOCATION);

  DALI_TEST_EQUALS(g_creationCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(g_destructionCount, 0u, TEST_LOCATION);

  intrusivePtrs.clear();

  DALI_TEST_EQUALS(counted->ReferenceCount(), 2, TEST_LOCATION);

  DALI_TEST_EQUALS(g_creationCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(g_destructionCount, 0u, TEST_LOCATION);

  counted.Reset();
  DALI_TEST_EQUALS(counted2->ReferenceCount(), 1, TEST_LOCATION);
  counted2.Reset();

  DALI_TEST_EQUALS(g_creationCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(g_destructionCount, 1u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliIntrusivePtrGetP(void)
{
  tet_infoline("Testing Dali::IntrusivePtr::Get()");

  IntrusivePtr<Counted> counted(new Counted);
  DALI_TEST_CHECK(counted.Get() != 0);
  DALI_TEST_EQUALS(g_creationCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(counted->ReferenceCount(), 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliIntrusivePtrGetN(void)
{
  tet_infoline("Testing Dali::IntrusivePtr::Get()");

  g_creationCount = 0;

  IntrusivePtr<Counted> counted(0);
  DALI_TEST_CHECK(counted.Get() == 0);
  DALI_TEST_EQUALS(g_creationCount, 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliIntrusivePtrArrowOperatorP(void)
{
  tet_infoline("Positive Test for Dali::IntrusivePtr::operator->()");

  IntrusivePtr<Counted> counted(new Counted);
  DALI_TEST_CHECK((counted.operator->()) != 0);
  DALI_TEST_EQUALS(counted->ReferenceCount(), 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliIntrusivePtrArrowOperatorN(void)
{
  tet_infoline("Negative Test for Dali::IntrusivePtr::operator->()");

  IntrusivePtr<Counted> counted;
  DALI_TEST_CHECK((counted.operator->()) == 0);

  END_TEST;
}

int UtcDaliIntrusivePtrIndirectionOperatorP(void)
{
  tet_infoline("Positive Test for Dali::IntrusivePtr::operator*()");

  IntrusivePtr<Counted> counted(new Counted);
  DALI_TEST_CHECK(&(counted.operator*()) != 0);
  DALI_TEST_EQUALS((*counted).ReferenceCount(), 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliIntrusivePtrIndirectionOperatorN(void)
{
  tet_infoline("Negative Test for Dali::IntrusivePtr::operator*()");

  IntrusivePtr<Counted> counted;
  DALI_TEST_CHECK(&(counted.operator*()) == 0);

  END_TEST;
}

int UtcDaliIntrusivePtrResetP(void)
{
  tet_infoline("Positive Test for Dali::IntrusivePtr::Reset()");

  IntrusivePtr<Counted> counted(new Counted);
  DALI_TEST_CHECK(counted.Get() != 0);
  counted.Reset();
  DALI_TEST_CHECK(counted.Get() == 0);

  END_TEST;
}

int UtcDaliIntrusivePtrResetN(void)
{
  tet_infoline("Negative Test for Dali::IntrusivePtr::Reset()");

  IntrusivePtr<Counted> counted;
  Counted*              firstGet = counted.Get();
  counted.Reset();
  DALI_TEST_EQUALS(counted.Get(), firstGet, TEST_LOCATION);

  END_TEST;
}

int UtcDaliIntrusivePtrResetTP(void)
{
  tet_infoline("Positive Test for Dali::IntrusivePtr::Reset(T*)");

  g_creationCount = g_destructionCount = 0;

  IntrusivePtr<Counted> counted(new Counted);

  IntrusivePtr<Counted> counted2(new Counted);

  DALI_TEST_EQUALS(counted->ReferenceCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(counted2->ReferenceCount(), 1, TEST_LOCATION);

  counted.Reset(counted2.Get());

  DALI_TEST_EQUALS(counted->ReferenceCount(), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(counted2->ReferenceCount(), 2, TEST_LOCATION);

  DALI_TEST_EQUALS(counted.Get(), counted2.Get(), TEST_LOCATION);

  DALI_TEST_EQUALS(g_creationCount, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(g_destructionCount, 1u, TEST_LOCATION);

  counted2.Reset((Counted*)0);
  counted.Reset(counted2.Get());
  DALI_TEST_EQUALS(g_destructionCount, 2u, TEST_LOCATION);

  // Check that reseting nulls is harmless:
  counted2.Reset(counted.Get());
  counted.Reset(counted2.Get());

  DALI_TEST_EQUALS(g_destructionCount, 2u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliIntrusivePtrResetTN(void)
{
  tet_infoline("Negative Test for Dali::IntrusivePtr::Reset(T*)");

  g_creationCount = g_destructionCount = 0;

  IntrusivePtr<Counted> counted(new Counted);

  counted.Reset((Counted*)0);

  DALI_TEST_EQUALS(counted.Get(), (Counted*)0, TEST_LOCATION);
  DALI_TEST_EQUALS(g_creationCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(g_destructionCount, 1u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliIntrusivePtrOperatorBoolP(void)
{
  tet_infoline("Positive Test for Dali::IntrusivePtr::operator bool()");

  IntrusivePtr<Counted> counted(new Counted);
  DALI_TEST_CHECK(counted.operator bool() == true);
  DALI_TEST_CHECK(counted);

  counted.Reset();
  DALI_TEST_CHECK(counted.operator bool() == false);

  END_TEST;
}

int UtcDaliIntrusivePtrOperatorBoolN(void)
{
  tet_infoline("Negative Test for Dali::IntrusivePtr::operator bool()");

  IntrusivePtr<Counted> counted;
  DALI_TEST_CHECK(counted.operator bool() == false);
  DALI_TEST_CHECK(!counted);
  END_TEST;
}

/** Equality of two different types*/
int UtcDaliIntrusivePtrOperatorEqualTU(void)
{
  tet_infoline("Test for Dali::IntrusivePtr::operator ==(T, U)");

  IntrusivePtr<Counted>         counted1(new Counted);
  IntrusivePtr<CountedSubclass> countedSubclass1(new CountedSubclass);
  IntrusivePtr<CountedSubclass> countedSubclass2(new CountedSubclass);
  IntrusivePtr<Counted>         counted2(countedSubclass2);

  DALI_TEST_EQUALS(operator==(counted1, countedSubclass1), false, TEST_LOCATION);
  DALI_TEST_EQUALS(operator==(counted2, countedSubclass2), true, TEST_LOCATION);
  END_TEST;
}

/** Inequality of two different types*/
int UtcDaliIntrusivePtrOperatorNotEqualTU(void)
{
  tet_infoline("Test for Dali::IntrusivePtr::operator !=(T, U)");

  IntrusivePtr<Counted>         counted1(new Counted);
  IntrusivePtr<CountedSubclass> countedSubclass1(new CountedSubclass);
  IntrusivePtr<CountedSubclass> countedSubclass2(new CountedSubclass);
  IntrusivePtr<Counted>         counted2(countedSubclass2);

  DALI_TEST_EQUALS(operator!=(counted1, countedSubclass1), true, TEST_LOCATION);
  DALI_TEST_EQUALS(operator!=(counted2, countedSubclass2), false, TEST_LOCATION);
  END_TEST;
}

/** Equality of two different types where right hand side is a raw pointer */
int UtcDaliIntrusivePtrOperatorEqualRightPointerTU(void)
{
  tet_infoline("Test for Dali::IntrusivePtr::operator ==(T, U*)");

  IntrusivePtr<Counted>         counted1(new Counted);
  IntrusivePtr<CountedSubclass> countedSubclass1(new CountedSubclass);
  IntrusivePtr<CountedSubclass> countedSubclass2(new CountedSubclass);
  IntrusivePtr<Counted>         counted2(countedSubclass2);

  DALI_TEST_EQUALS(operator==(counted1, countedSubclass1.Get()), false, TEST_LOCATION);
  DALI_TEST_EQUALS(operator==(counted2, countedSubclass2.Get()), true, TEST_LOCATION);
  END_TEST;
}

/** Inequality of two different types where the right hand side is a raw pointer */
int UtcDaliIntrusivePtrOperatorNotEqualRightPointerTU(void)
{
  tet_infoline("Test for Dali::IntrusivePtr::operator !=(T, U*)");

  IntrusivePtr<Counted>         counted1(new Counted);
  IntrusivePtr<CountedSubclass> countedSubclass1(new CountedSubclass);
  IntrusivePtr<CountedSubclass> countedSubclass2(new CountedSubclass);
  IntrusivePtr<Counted>         counted2(countedSubclass2);

  DALI_TEST_EQUALS(operator!=(counted1, countedSubclass1.Get()), true, TEST_LOCATION);
  DALI_TEST_EQUALS(operator!=(counted2, countedSubclass2.Get()), false, TEST_LOCATION);
  END_TEST;
}

/** Equality of two different types where left hand side is a raw pointer */
int UtcDaliIntrusivePtrOperatorEqualLeftPointerTU(void)
{
  tet_infoline("Test for Dali::IntrusivePtr::operator ==(T*, U)");

  IntrusivePtr<Counted>         counted1(new Counted);
  IntrusivePtr<CountedSubclass> countedSubclass1(new CountedSubclass);
  IntrusivePtr<CountedSubclass> countedSubclass2(new CountedSubclass);
  IntrusivePtr<Counted>         counted2(countedSubclass2);

  DALI_TEST_EQUALS(operator==(counted1.Get(), countedSubclass1), false, TEST_LOCATION);
  DALI_TEST_EQUALS(operator==(counted2.Get(), countedSubclass2), true, TEST_LOCATION);
  END_TEST;
}

/** Inequality of two different types where the left hand side is a raw pointer */
int UtcDaliIntrusivePtrOperatorNotEqualLeftPointerTU(void)
{
  tet_infoline("Test for Dali::IntrusivePtr::operator !=(T*, U)");

  IntrusivePtr<Counted>         counted1(new Counted);
  IntrusivePtr<CountedSubclass> countedSubclass1(new CountedSubclass);
  IntrusivePtr<CountedSubclass> countedSubclass2(new CountedSubclass);
  IntrusivePtr<Counted>         counted2(countedSubclass2);

  DALI_TEST_EQUALS(operator!=(counted1.Get(), countedSubclass1), true, TEST_LOCATION);
  DALI_TEST_EQUALS(operator!=(counted2.Get(), countedSubclass2), false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRefObjectCopyConstructor(void)
{
  tet_infoline("Test for Dali::RefObject(const RefObject&)");

  {
    IntrusivePtr<TestObject> testPtr(new TestObject);
    DALI_TEST_EQUALS(testPtr->ReferenceCount(), 1, TEST_LOCATION);

    const TestObject& testObject = *testPtr.Get();
    {
      IntrusivePtr<TestObject> testPtr2(new TestObject(testObject));
      DALI_TEST_EQUALS(testPtr2->ReferenceCount(), 1, TEST_LOCATION);
    }
    DALI_TEST_EQUALS(testPtr->ReferenceCount(), 1, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliRefObjectAssignmentOperator(void)
{
  tet_infoline("Test for Dali::RefObject::operator=(const RefObject&)");

  {
    IntrusivePtr<TestObject> testPtr(new TestObject);
    DALI_TEST_EQUALS(testPtr->ReferenceCount(), 1, TEST_LOCATION);

    const TestObject& testObject = *testPtr.Get();
    {
      IntrusivePtr<TestObject> testPtr2(new TestObject());
      testPtr->data                     = 33;
      IntrusivePtr<TestObject> testPtr3 = testPtr2;
      DALI_TEST_EQUALS(testPtr2->ReferenceCount(), 2, TEST_LOCATION);
      DALI_TEST_EQUALS(testPtr2->data, 201, TEST_LOCATION);

      TestObject& testObject2 = *testPtr2.Get();
      testObject2             = testObject;

      DALI_TEST_EQUALS(testPtr->ReferenceCount(), 1, TEST_LOCATION);
      DALI_TEST_EQUALS(testPtr2->ReferenceCount(), 2, TEST_LOCATION);
    }
    DALI_TEST_EQUALS(testPtr->ReferenceCount(), 1, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliRefObjectAssignmentOperatorToNull(void)
{
  tet_infoline("Testing Dali::IntrusivePtr = nullptr");

  g_creationCount = g_destructionCount = 0;

  IntrusivePtr<Counted> counted(new Counted);

  DALI_TEST_EQUALS(g_creationCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(g_destructionCount, 0u, TEST_LOCATION);

  IntrusivePtr<Counted> counted2 = counted;
  DALI_TEST_EQUALS(g_creationCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(g_destructionCount, 0u, TEST_LOCATION);

  DALI_TEST_EQUALS(counted->ReferenceCount(), 2, TEST_LOCATION);

  counted2 = nullptr;
  DALI_TEST_EQUALS(g_destructionCount, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(counted->ReferenceCount(), 1, TEST_LOCATION);

  counted = nullptr;
  DALI_TEST_EQUALS(g_destructionCount, 1u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliIntrusivePtrMoveConstructor(void)
{
  IntrusivePtr<TestObject> testPtr(new TestObject);
  DALI_TEST_EQUALS(testPtr->ReferenceCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(testPtr->data, 201, TEST_LOCATION);

  IntrusivePtr<TestObject> movePtr = std::move(testPtr);
  DALI_TEST_EQUALS(movePtr->ReferenceCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(movePtr->data, 201, TEST_LOCATION);
  DALI_TEST_CHECK(!testPtr);

  IntrusivePtr<TestObject> anotherTestPtr(new TestObject);
  DALI_TEST_EQUALS(anotherTestPtr->ReferenceCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(anotherTestPtr->data, 201, TEST_LOCATION);
  IntrusivePtr<TestObject> anotherMovePtr = std::move(anotherTestPtr);
  DALI_TEST_EQUALS(anotherMovePtr->ReferenceCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(anotherMovePtr->data, 201, TEST_LOCATION);
  DALI_TEST_CHECK(!anotherTestPtr.Get());

  IntrusivePtr<CountedSubclass> countedSubclass(new CountedSubclass);
  DALI_TEST_EQUALS(countedSubclass->ReferenceCount(), 1, TEST_LOCATION);

  IntrusivePtr<Counted> countedMovePtr = std::move(countedSubclass);
  DALI_TEST_EQUALS(countedMovePtr->ReferenceCount(), 1, TEST_LOCATION);
  DALI_TEST_CHECK(!countedSubclass);

  END_TEST;
}

int UtcDaliIntrusivePtrMoveAssignment(void)
{
  IntrusivePtr<TestObject> testPtr(new TestObject);
  DALI_TEST_EQUALS(testPtr->ReferenceCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(testPtr->data, 201, TEST_LOCATION);

  IntrusivePtr<TestObject> secondPtr(testPtr);
  DALI_TEST_EQUALS(testPtr->ReferenceCount(), 2, TEST_LOCATION);

  IntrusivePtr<TestObject> thirdPtr;
  testPtr = std::move(thirdPtr);
  DALI_TEST_EQUALS(secondPtr->ReferenceCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(secondPtr->data, 201, TEST_LOCATION);
  DALI_TEST_CHECK(!testPtr.Get());

  IntrusivePtr<TestObject> fourthPtr(new TestObject);
  testPtr = std::move(fourthPtr);
  DALI_TEST_CHECK(!fourthPtr.Get());

  IntrusivePtr<CountedSubclass> countedSubclassPtr(new CountedSubclass);
  DALI_TEST_EQUALS(countedSubclassPtr->ReferenceCount(), 1, TEST_LOCATION);

  IntrusivePtr<Counted> countedMovePtr;
  countedMovePtr = std::move(countedSubclassPtr);
  DALI_TEST_EQUALS(countedMovePtr->ReferenceCount(), 1, TEST_LOCATION);
  DALI_TEST_CHECK(!countedSubclassPtr);

  END_TEST;
}

/** Equality with nullptr */
int UtcDaliIntrusivePtrOperatorEqualWithNullptr(void)
{
  tet_infoline("Test for Dali::IntrusivePtr::operator == nullptr");

  IntrusivePtr<Counted> counted1(new Counted);

  // counted1 is not nullptr.
  DALI_TEST_CHECK(!(counted1 == nullptr));
  DALI_TEST_CHECK(!(nullptr == counted1));
  DALI_TEST_CHECK(counted1 != nullptr);
  DALI_TEST_CHECK(nullptr != counted1);

  counted1 = nullptr;

  // counted1 is nullptr.
  DALI_TEST_CHECK(counted1 == nullptr);
  DALI_TEST_CHECK(nullptr == counted1);
  DALI_TEST_CHECK(!(counted1 != nullptr));
  DALI_TEST_CHECK(!(nullptr != counted1));

  END_TEST;
}

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <utility>

// INTERNAL INCLUDES
#include <dali-test-suite-utils.h>
#include <dali/internal/common/owner-pointer.h>

using namespace Dali::Internal;

void utc_dali_internal_owner_pointer_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_owner_pointer_cleanup(void)
{
  test_return_value = TET_PASS;
}

///////////////////////////////////////////////////////////////////////////////
namespace
{
/// Takes in a reference to a bool which is set to true when the destructor is called
class OwnedClass
{
public:
  OwnedClass(bool& destructorCalled)
  : mDestructorCalled(destructorCalled)
  {
    mDestructorCalled = false;
  }

  ~OwnedClass()
  {
    mDestructorCalled = true;
  }

private:
  bool& mDestructorCalled;
};

/// Just a simple class with a function that marks a member boolean to true if that function is called
class ClassWithFunction
{
public:
  ClassWithFunction() = default;

  void MyFunction()
  {
    functionCalled = true;
  }

  bool functionCalled{false};
};

} // namespace

///////////////////////////////////////////////////////////////////////////////

int UtcDaliOwnerPointerEnsureDeletion(void)
{
  // Ensure that the object owned by the owner-pointer is deleted.

  bool deleted = false;

  {
    OwnerPointer<OwnedClass> pointer(new OwnedClass(deleted));
    DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);
  }

  // OwnerPointer out-of-scope, object should be deleted.

  DALI_TEST_EQUALS(deleted, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOwnerPointerDefaultConstructor(void)
{
  // Ensure the default constructor is created as expected.

  OwnerPointer<OwnedClass> pointer;
  DALI_TEST_CHECK(pointer.Get() == nullptr);

  END_TEST;
}

int UtcDaliOwnerPointerCopy(void)
{
  // Call copy constructor and assignment operator

  bool        deleted = false;
  OwnedClass* owned   = new OwnedClass(deleted);

  OwnerPointer<OwnedClass> first(owned);
  DALI_TEST_CHECK(first.Get() == owned);

  {
    // Copy constructor, first should have a nullptr now, no object deletion
    OwnerPointer<OwnedClass> second(first);
    DALI_TEST_CHECK(first.Get() == nullptr);
    DALI_TEST_CHECK(second.Get() == owned);
    DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);

    // Self assignment, nothing should change or be deleted.
    first  = first;
    second = second;
    DALI_TEST_CHECK(first.Get() == nullptr);
    DALI_TEST_CHECK(second.Get() == owned);
    DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);

    // Assign second to first, no deletion, second should have a nullptr now
    first = second;
    DALI_TEST_CHECK(first.Get() == owned);
    DALI_TEST_CHECK(second.Get() == nullptr);
    DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);
  }

  // second is out-of-scope now, no object deletion
  DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);

  // Assign to an empty pointer, owned object should be deleted
  OwnerPointer<OwnedClass> empty;
  first = empty;
  DALI_TEST_EQUALS(deleted, true, TEST_LOCATION);
  DALI_TEST_CHECK(first.Get() == nullptr);
  DALI_TEST_CHECK(empty.Get() == nullptr);

  END_TEST;
}

int UtcDaliOwnerPointerMove(void)
{
  // Call move constructor and move assignment operator

  bool        deleted = false;
  OwnedClass* owned   = new OwnedClass(deleted);

  OwnerPointer<OwnedClass> first(owned);
  DALI_TEST_CHECK(first.Get() == owned);

  {
    // Move constructor, first should have a nullptr now, no object deletion
    OwnerPointer<OwnedClass> second(std::move(first));
    DALI_TEST_CHECK(first.Get() == nullptr);
    DALI_TEST_CHECK(second.Get() == owned);
    DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);

    // Self std::move assignment make compile warning over gcc-13. Let we ignore the warning.
#if (__GNUC__ >= 13)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
#endif
    // Self assignment, nothing should change or be deleted.
    first  = std::move(first);
    second = std::move(second);
    DALI_TEST_CHECK(first.Get() == nullptr);
    DALI_TEST_CHECK(second.Get() == owned);
    DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);
#if (__GNUC__ >= 13)
#pragma GCC diagnostic pop
#endif

    // Assign second to first, no deletion, second should have a nullptr now
    first = std::move(second);
    DALI_TEST_CHECK(first.Get() == owned);
    DALI_TEST_CHECK(second.Get() == nullptr);
    DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);
  }

  // second is out-of-scope now, no object deletion
  DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);

  // Assign to an empty pointer, owned object should be deleted
  OwnerPointer<OwnedClass> empty;
  first = std::move(empty);
  DALI_TEST_EQUALS(deleted, true, TEST_LOCATION);
  DALI_TEST_CHECK(first.Get() == nullptr);
  DALI_TEST_CHECK(empty.Get() == nullptr);

  END_TEST;
}

int UtcDaliOwnerPointerIndirection(void)
{
  // Check the indirection operators

  using Ptr = OwnerPointer<int>;

  {
    int* rawIntPtr(new int(200));
    Ptr  nonConstPtr(rawIntPtr);
    DALI_TEST_CHECK(rawIntPtr == &(*nonConstPtr));
    DALI_TEST_EQUALS(*nonConstPtr, 200, TEST_LOCATION);
  }

  {
    int*      rawIntPtr2(new int(300));
    const Ptr constPtr(rawIntPtr2);
    DALI_TEST_CHECK(rawIntPtr2 == &(*constPtr));
    DALI_TEST_EQUALS(*constPtr, 300, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliOwnerPointerPointerOperator(void)
{
  // Check the pointer operators

  using Ptr = OwnerPointer<ClassWithFunction>;

  // Check if function is called as expected when using a const OwnerPointer
  {
    ClassWithFunction* rawPtr(new ClassWithFunction);
    Ptr                nonConstPtr(rawPtr);
    DALI_TEST_EQUALS(rawPtr->functionCalled, false, TEST_LOCATION);
    nonConstPtr->MyFunction();
    DALI_TEST_EQUALS(rawPtr->functionCalled, true, TEST_LOCATION);
  }

  // Check if function is called as expected when using a const OwnerPointer
  {
    ClassWithFunction* rawPtr2(new ClassWithFunction);
    const Ptr          constPtr(rawPtr2);
    DALI_TEST_EQUALS(rawPtr2->functionCalled, false, TEST_LOCATION);
    constPtr->MyFunction();
    DALI_TEST_EQUALS(rawPtr2->functionCalled, true, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliOwnerPointerComparisonOperator(void)
{
  // Check the comparison operator

  using Ptr = OwnerPointer<int>;

  int* rawIntPtr(new int(200));
  Ptr  ownerPtr(rawIntPtr);
  DALI_TEST_CHECK(ownerPtr == rawIntPtr);
  DALI_TEST_CHECK(!(ownerPtr == nullptr));

  END_TEST;
}

int UtcDaliOwnerPointerReset(void)
{
  // Ensure that calling Reset deletes the object and sets the owner-pointer to NULL

  bool deleted = false;

  OwnerPointer<OwnedClass> pointer(new OwnedClass(deleted));
  DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);
  pointer.Reset();
  DALI_TEST_EQUALS(deleted, true, TEST_LOCATION);
  DALI_TEST_CHECK(pointer.Get() == nullptr);

  // Reset the empty pointer, should have no effect but there shouldn't be any crash
  pointer.Reset();
  DALI_TEST_CHECK(pointer.Get() == nullptr);

  END_TEST;
}

int UtcDaliOwnerPointerRelease(void)
{
  // Ensure that calling Release does NOT delete the object but still sets the owner-pointer to NULL

  bool deleted = false;

  OwnedClass*              rawPtr = new OwnedClass(deleted);
  OwnerPointer<OwnedClass> pointer(rawPtr);

  DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);
  DALI_TEST_CHECK(pointer.Release() == rawPtr);
  DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);
  DALI_TEST_CHECK(pointer.Get() == nullptr);

  // Release the empty pointer, should have no effect but there shouldn't be any crash
  DALI_TEST_CHECK(pointer.Release() == nullptr);
  DALI_TEST_CHECK(pointer.Get() == nullptr);

  delete rawPtr; // Manually delete the released pointer

  END_TEST;
}

int UtcDaliOwnerPointerGet(void)
{
  // Check the Get method

  using Ptr = OwnerPointer<int>;

  int* rawIntPtr(new int(200));
  Ptr  ownerPtr(rawIntPtr);
  DALI_TEST_CHECK(ownerPtr.Get() == rawIntPtr);

  END_TEST;
}

int UtcDaliOwnerPointerSwap(void)
{
  // Ensure the Swap method swaps the pointers and doesn't delete any objects

  using Ptr = OwnerPointer<OwnedClass>;

  bool firstObjectDeleted  = false;
  bool secondObjectDeleted = false;

  OwnedClass* firstRawPtr  = new OwnedClass(firstObjectDeleted);
  OwnedClass* secondRawPtr = new OwnedClass(secondObjectDeleted);

  Ptr firstPtr(firstRawPtr);
  Ptr secondPtr(secondRawPtr);

  // Check initial values
  DALI_TEST_EQUALS(firstObjectDeleted, false, TEST_LOCATION);
  DALI_TEST_EQUALS(secondObjectDeleted, false, TEST_LOCATION);
  DALI_TEST_CHECK(firstPtr == firstRawPtr);
  DALI_TEST_CHECK(secondPtr == secondRawPtr);

  // Call Swap on first and ensure swap is done and there's no deletion
  firstPtr.Swap(secondPtr);
  DALI_TEST_EQUALS(firstObjectDeleted, false, TEST_LOCATION);
  DALI_TEST_EQUALS(secondObjectDeleted, false, TEST_LOCATION);
  DALI_TEST_CHECK(firstPtr == secondRawPtr);
  DALI_TEST_CHECK(secondPtr == firstRawPtr);

  // Swap back using second, again no deletion
  secondPtr.Swap(firstPtr);
  DALI_TEST_EQUALS(firstObjectDeleted, false, TEST_LOCATION);
  DALI_TEST_EQUALS(secondObjectDeleted, false, TEST_LOCATION);
  DALI_TEST_CHECK(firstPtr == firstRawPtr);
  DALI_TEST_CHECK(secondPtr == secondRawPtr);

  // Swap with self, nothing should change or be deleted
  firstPtr.Swap(firstPtr);
  DALI_TEST_EQUALS(firstObjectDeleted, false, TEST_LOCATION);
  DALI_TEST_CHECK(firstPtr == firstRawPtr);

  // Swap with an empty OwnerPointer, no deletion but firstPtr should be empty now
  Ptr emptyPtr;
  firstPtr.Swap(emptyPtr);
  DALI_TEST_EQUALS(firstObjectDeleted, false, TEST_LOCATION);
  DALI_TEST_CHECK(firstPtr == nullptr);
  DALI_TEST_CHECK(emptyPtr == firstRawPtr);

  END_TEST;
}

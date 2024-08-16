/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <chrono>    ///< std::chrono::system_clock
#include <algorithm> ///< std::shuffle
#include <random>    ///< std::default_random_engine
#include <utility>

// INTERNAL INCLUDES
#include <dali-test-suite-utils.h>
#include <dali/integration-api/ordered-set.h>

using namespace Dali;
using namespace Dali::Integration;

void utc_dali_internal_ordered_set_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_ordered_set_cleanup(void)
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

/// Just a simple class with a member value and global refcount.
class ClassWithId
{
public:
  ClassWithId(int id)
  : mId(id)
  {
    ++gRefCount;
  }
  ~ClassWithId()
  {
    --gRefCount;
  }

  static int gRefCount;
  int        mId{};
};
int ClassWithId::gRefCount = 0;

} // namespace

///////////////////////////////////////////////////////////////////////////////

int UtcDaliOrderedSetEnsureDeletion(void)
{
  // Ensure that the object owned by the OrderedSet is deleted.

  bool deleted = false;

  {
    OrderedSet<OwnedClass> set;
    set.PushBack(new OwnedClass(deleted));
    DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);
  }

  // OrderedSet out-of-scope, object should be deleted.

  DALI_TEST_EQUALS(deleted, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOrderedSetFalseEnsureNotDeletionWhen(void)
{
  // Ensure that the object owned by the OrderedSET is deleted.

  bool deleted = false;

  OwnedClass* ptr;

  {
    // Create OrderedSet without ownership. It will not delete automatically.
    OrderedSet<OwnedClass, false> set;
    ptr = new OwnedClass(deleted);
    set.PushBack(ptr);
    DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);
  }

  // OrderedSet out-of-scope, but, object should not be deleted.

  DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);

  delete ptr;
  DALI_TEST_EQUALS(deleted, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOrderedSetDefaultConstructor(void)
{
  // Ensure the default constructor is created as expected.

  OrderedSet<OwnedClass> set;
  DALI_TEST_CHECK(set.Count() == 0u);

  END_TEST;
}

int UtcDaliOrderedSetFalseDefaultConstructor(void)
{
  // Ensure the default constructor is created as expected.

  OrderedSet<OwnedClass, false> set;
  DALI_TEST_CHECK(set.Count() == 0u);

  END_TEST;
}

int UtcDaliOrderedSetReserve(void)
{
  OrderedSet<OwnedClass> set;
  set.Reserve(100u);

  // Reserve function didn't change the count.
  DALI_TEST_CHECK(set.Count() == 0u);

  OrderedSet<OwnedClass> set2;
  set2.Reserve(100u);

  // Reserve function didn't change the count.
  DALI_TEST_CHECK(set2.Count() == 0u);

  END_TEST;
}

int UtcDaliOrderedSetMove(void)
{
  // Call move constructor and move assignment operator

  bool        deleted = false;
  OwnedClass* owned   = new OwnedClass(deleted);

  OrderedSet<OwnedClass> first;
  DALI_TEST_CHECK(first.Find(owned) == first.End());
  first.PushBack(owned);
  DALI_TEST_CHECK(first.Find(owned) != first.End());

  {
    // Move constructor, first should have a nullptr now, no object deletion
    OrderedSet<OwnedClass> second(std::move(first));
    DALI_TEST_CHECK(first.Find(owned) == first.End());
    DALI_TEST_CHECK(second.Find(owned) != second.End());
    DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);

    // Assign second to first, no deletion, second should have a nullptr now
    first = std::move(second);
    DALI_TEST_CHECK(first.Find(owned) != first.End());
    DALI_TEST_CHECK(second.Find(owned) == second.End());
    DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);
  }

  // second is out-of-scope now, no object deletion
  DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);

  // Assign to an empty pointer, owned object should be deleted
  OrderedSet<OwnedClass> empty;
  first = std::move(empty);
  DALI_TEST_EQUALS(deleted, true, TEST_LOCATION);
  DALI_TEST_CHECK(first.Find(owned) == first.End());
  DALI_TEST_CHECK(empty.Find(owned) == empty.End());

  END_TEST;
}

int UtcDaliOrderedSetFalseMove(void)
{
  // Call move constructor and move assignment operator

  bool        deleted = false;
  OwnedClass* owned   = new OwnedClass(deleted);

  OrderedSet<OwnedClass, false> first;
  DALI_TEST_CHECK(first.Find(owned) == first.End());
  first.PushBack(owned);
  DALI_TEST_CHECK(first.Find(owned) != first.End());

  {
    // Move constructor, first should have a nullptr now, no object deletion
    OrderedSet<OwnedClass, false> second(std::move(first));
    DALI_TEST_CHECK(first.Find(owned) == first.End());
    DALI_TEST_CHECK(second.Find(owned) != second.End());
    DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);

    // Assign second to first, no deletion, second should have a nullptr now
    first = std::move(second);
    DALI_TEST_CHECK(first.Find(owned) != first.End());
    DALI_TEST_CHECK(second.Find(owned) == second.End());
    DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);
  }

  // second is out-of-scope now, no object deletion
  DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);

  // Assign to an empty pointer, but owned object should not be deleted
  OrderedSet<OwnedClass, false> empty;
  first = std::move(empty);
  DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);
  DALI_TEST_CHECK(first.Find(owned) == first.End());
  DALI_TEST_CHECK(empty.Find(owned) == empty.End());

  delete owned;
  DALI_TEST_EQUALS(deleted, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOrderedSetErase(void)
{
  // Ensure that calling Reset deletes the object and sets the owner-pointer to NULL

  bool        deleted = false;
  OwnedClass* owned   = new OwnedClass(deleted);

  OrderedSet<OwnedClass> set;
  set.PushBack(owned);
  DALI_TEST_EQUALS(set.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);
  set.EraseObject(owned);
  DALI_TEST_EQUALS(deleted, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOrderedSetFalseErase(void)
{
  // Ensure that calling Reset not deletes the object and sets the owner-pointer to NULL

  bool        deleted = false;
  OwnedClass* owned   = new OwnedClass(deleted);

  OrderedSet<OwnedClass, false> set;
  set.PushBack(owned);
  DALI_TEST_EQUALS(set.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);
  set.EraseObject(owned);
  DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);

  delete owned;
  DALI_TEST_EQUALS(deleted, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOrderedSetClear(void)
{
  // Ensure that calling Reset deletes the object and sets the owner-pointer to NULL

  bool        deleted = false;
  OwnedClass* owned   = new OwnedClass(deleted);

  OrderedSet<OwnedClass> set;
  set.PushBack(owned);
  DALI_TEST_EQUALS(set.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);

  set.Clear();
  DALI_TEST_EQUALS(set.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(deleted, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOrderedSetFalseClear(void)
{
  // Ensure that calling Reset deletes the object and sets the owner-pointer to NULL

  bool        deleted = false;
  OwnedClass* owned   = new OwnedClass(deleted);

  OrderedSet<OwnedClass, false> set;
  set.PushBack(owned);
  DALI_TEST_EQUALS(set.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);

  set.Clear();
  DALI_TEST_EQUALS(set.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);

  delete owned;
  DALI_TEST_EQUALS(deleted, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOrderedSetRelease(void)
{
  // Ensure that calling Release does NOT delete the object but still sets the owner-pointer to NULL

  bool        deleted = false;
  OwnedClass* owned   = new OwnedClass(deleted);

  OrderedSet<OwnedClass> set;
  set.PushBack(owned);
  DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);

  auto iter = set.Find(owned);
  DALI_TEST_CHECK(iter != set.End());
  DALI_TEST_EQUALS(set.Count(), 1u, TEST_LOCATION);

  OwnedClass* released = set.Release(iter);
  DALI_TEST_EQUALS(deleted, false, TEST_LOCATION);
  DALI_TEST_CHECK(set.Find(owned) == set.End());
  DALI_TEST_CHECK(set.Find(released) == set.End());
  DALI_TEST_EQUALS(set.Count(), 0u, TEST_LOCATION);

  delete released;
  DALI_TEST_EQUALS(deleted, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOrderedSetIteratorOrderCheck(void)
{
  // Ensure that order of iterator is equal with Order of Insertion.

  // Reset refcount of class
  ClassWithId::gRefCount = 0;

  // To avoid lucky pass, run this test multiple time.
  int tryCnt = 3;
  while(tryCnt--)
  {
    int baseId = tryCnt; // random id
    int id     = baseId;
    int n      = 10 + 5 * (tryCnt + 1); // random count

    OrderedSet<ClassWithId> set;

    for(int i = 0; i < n; ++i)
    {
      ClassWithId* object = new ClassWithId(id++);
      set.PushBack(object);
    }

    int expectId;
    // Check by for iteration
    expectId = baseId;
    for(auto iter = set.Begin(), iterEnd = set.End(); iter != iterEnd; ++iter)
    {
      DALI_TEST_EQUALS(expectId++, (*iter)->mId, TEST_LOCATION);
    }
    DALI_TEST_EQUALS(expectId, id, TEST_LOCATION);

    // Check by range referenced iteration
    expectId = baseId;
    for(auto&& iter : set)
    {
      DALI_TEST_EQUALS(expectId++, iter->mId, TEST_LOCATION);
      // Change iter's value.
      ++(iter->mId);
    }
    DALI_TEST_EQUALS(expectId, id, TEST_LOCATION);

    // Check by range iteration. Note that class value changed.
    expectId = baseId + 1;
    for(const auto& iter : set)
    {
      DALI_TEST_EQUALS(expectId++, iter->mId, TEST_LOCATION);
    }
    DALI_TEST_EQUALS(expectId, id + 1, TEST_LOCATION);
  }

  // Check whether leak exist.
  DALI_TEST_EQUALS(ClassWithId::gRefCount, 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOrderedSetFalseIteratorOrderCheck(void)
{
  // Ensure that order of iterator is equal with Order of Insertion.

  // Reset refcount of class
  ClassWithId::gRefCount = 0;

  // Container of ownered class, to release memories after test finished.
  std::vector<ClassWithId*> objectList;

  // To avoid lucky pass, run this test multiple time.
  int tryCnt = 3;
  while(tryCnt--)
  {
    int baseId = tryCnt; // random id
    int id     = baseId;
    int n      = 10 + 5 * (tryCnt + 1); // random count

    OrderedSet<ClassWithId, false> set;

    for(int i = 0; i < n; ++i)
    {
      ClassWithId* object = new ClassWithId(id++);
      objectList.push_back(object);
      set.PushBack(object);
    }

    int expectId;
    // Check by for iteration
    expectId = baseId;
    for(auto iter = set.Begin(), iterEnd = set.End(); iter != iterEnd; ++iter)
    {
      DALI_TEST_EQUALS(expectId++, (*iter)->mId, TEST_LOCATION);
    }
    DALI_TEST_EQUALS(expectId, id, TEST_LOCATION);

    // Check by range referenced iteration
    expectId = baseId;
    for(auto&& iter : set)
    {
      DALI_TEST_EQUALS(expectId++, iter->mId, TEST_LOCATION);
      // Change iter's value.
      ++(iter->mId);
    }
    DALI_TEST_EQUALS(expectId, id, TEST_LOCATION);

    // Check by range iteration. Note that class value changed.
    expectId = baseId + 1;
    for(const auto& iter : set)
    {
      DALI_TEST_EQUALS(expectId++, iter->mId, TEST_LOCATION);
    }
    DALI_TEST_EQUALS(expectId, id + 1, TEST_LOCATION);
  }

  // Check whether leak exist.
  DALI_TEST_EQUALS(ClassWithId::gRefCount, objectList.size(), TEST_LOCATION);

  // Release memory now.
  for(auto&& iter : objectList)
  {
    delete iter;
  }
  objectList.clear();
  DALI_TEST_EQUALS(ClassWithId::gRefCount, 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOrderedSetReorderCacheMap(void)
{
  // Ensure that order of iterator is equal with Order of Insertion.
  // And also check the order is valid even if change the order by user, after we call ReorderCacheMap.

  // Reset refcount of class
  ClassWithId::gRefCount = 0;

  // To avoid lucky pass, run this test multiple time.
  int tryCnt = 3;
  while(tryCnt--)
  {
    int baseId = tryCnt; // random id
    int id     = baseId;
    int n      = 10 + 5 * (tryCnt + 1); // random count

    OrderedSet<ClassWithId> set;

    for(int i = 0; i < n; ++i)
    {
      ClassWithId* object = new ClassWithId(id++);
      set.PushBack(object);
    }

    int expectId;
    // Check by for iteration
    expectId = baseId;
    for(auto iter = set.Begin(), iterEnd = set.End(); iter != iterEnd; ++iter)
    {
      DALI_TEST_EQUALS(expectId++, (*iter)->mId, TEST_LOCATION);
    }
    DALI_TEST_EQUALS(expectId, id, TEST_LOCATION);

    // Shuffle it randomly.
    std::vector<std::pair<int, ClassWithId*>> shuffleList;
    shuffleList.reserve(n);
    for(auto iter = set.Begin(), iterEnd = set.End(); iter != iterEnd; ++iter)
    {
      shuffleList.emplace_back((*iter)->mId, (*iter));
    }
    std::shuffle(shuffleList.begin(), shuffleList.end(), std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));

    // Change the value of container as shuffled order. After then, call ReorderCacheMap().
    int shuffleIndex = 0;
    for(auto iter = set.Begin(), iterEnd = set.End(); iter != iterEnd; ++iter)
    {
      (*iter) = shuffleList[shuffleIndex++].second;
    }
    set.ReorderCacheMap();

    // Check by iterator. And randomly erase item
    shuffleIndex = 0;
    for(auto iter = set.Begin(), iterEnd = set.End(); iter != iterEnd; ++iter)
    {
      DALI_TEST_EQUALS(shuffleList[shuffleIndex++].first, (*iter)->mId, TEST_LOCATION);
    }

    // Randomly erase item, and check again until all items removed.
    while(!set.IsEmpty())
    {
      int removeIndex = rand()%set.Count();
      auto iter = set.Find(shuffleList[removeIndex].second);
      DALI_TEST_CHECK(iter != set.End());
      set.Erase(iter);
      for(int i = removeIndex + 1; i < n; ++i)
      {
        shuffleList[i - 1] = shuffleList[i];
      }
      --n;

      shuffleIndex = 0;
      for(auto iter = set.Begin(), iterEnd = set.End(); iter != iterEnd; ++iter)
      {
        DALI_TEST_EQUALS(shuffleList[shuffleIndex++].first, (*iter)->mId, TEST_LOCATION);
      }
    }
  }

  // Check whether leak exist.
  DALI_TEST_EQUALS(ClassWithId::gRefCount, 0, TEST_LOCATION);

  END_TEST;
}
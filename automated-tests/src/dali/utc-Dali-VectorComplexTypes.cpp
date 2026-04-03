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

#define ENABLE_VECTOR_ASSERTS

#include <dali-test-suite-utils.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <iostream>

using namespace Dali;

namespace
{
const Dali::VectorBase::SizeType ZERO(0);

bool gConstructorCalled = false;
bool gDestructorCalled  = false;

struct ComplexType
{
  ComplexType()
  {
    gConstructorCalled = true;
  }
  ~ComplexType()
  {
    gDestructorCalled = true;
  }
};

} // anonymous namespace

int UtcDaliVectorComplex(void)
{
  tet_infoline("Testing Dali::Vector< ComplexType > ");

  DALI_TEST_EQUALS(static_cast<bool>(Dali::Vector<ComplexType>::BaseType), false, TEST_LOCATION);

  Vector<ComplexType> classvector;
  DALI_TEST_EQUALS(ZERO, classvector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(ZERO, classvector.Capacity(), TEST_LOCATION);
  DALI_TEST_EQUALS(classvector.Begin(), classvector.End(), TEST_LOCATION);

  DALI_TEST_EQUALS(false, gConstructorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, gDestructorCalled, TEST_LOCATION);
  classvector.PushBack(ComplexType());
  DALI_TEST_EQUALS(true, gConstructorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(1u), classvector.Count(), TEST_LOCATION);
  DALI_TEST_GREATER(classvector.Capacity(), ZERO, TEST_LOCATION);
  classvector.Clear();
  DALI_TEST_EQUALS(true, gDestructorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(ZERO, classvector.Count(), TEST_LOCATION);
  DALI_TEST_GREATER(classvector.Capacity(), ZERO, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVectorBaseHandle(void)
{
  tet_infoline("Testing Dali::Vector<Dali::BaseHandle> ");
  TestApplication application;

  Dali::BaseHandle handle0 = Dali::Actor::New();
  Dali::BaseHandle handle1 = Dali::Actor::New();
  Dali::BaseHandle handle2 = Dali::Actor::New();
  Dali::BaseHandle handle3 = Dali::Actor::New();
  Dali::BaseHandle handle4 = Dali::Actor::New();
  tet_printf("handle0 : %p\n", handle0.GetObjectPtr());
  tet_printf("handle1 : %p\n", handle1.GetObjectPtr());
  tet_printf("handle2 : %p\n", handle2.GetObjectPtr());
  tet_printf("handle3 : %p\n", handle3.GetObjectPtr());
  tet_printf("handle4 : %p\n", handle4.GetObjectPtr());

  auto TestReferenceCount = [&](uint32_t expect0, uint32_t expect1, uint32_t expect2, uint32_t expect3, uint32_t expect4, const char* location)
  {
    if(handle0.GetBaseObject().ReferenceCount() != expect0)
    {
      tet_printf("Failed at handle0!\n");
    }
    if(handle1.GetBaseObject().ReferenceCount() != expect1)
    {
      tet_printf("Failed at handle1!\n");
    }
    if(handle2.GetBaseObject().ReferenceCount() != expect2)
    {
      tet_printf("Failed at handle2!\n");
    }
    if(handle3.GetBaseObject().ReferenceCount() != expect3)
    {
      tet_printf("Failed at handle3!\n");
    }
    if(handle4.GetBaseObject().ReferenceCount() != expect4)
    {
      tet_printf("Failed at handle4!\n");
    }
    DALI_TEST_EQUALS(handle0.GetBaseObject().ReferenceCount(), expect0, location);
    DALI_TEST_EQUALS(handle1.GetBaseObject().ReferenceCount(), expect1, location);
    DALI_TEST_EQUALS(handle2.GetBaseObject().ReferenceCount(), expect2, location);
    DALI_TEST_EQUALS(handle3.GetBaseObject().ReferenceCount(), expect3, location);
    DALI_TEST_EQUALS(handle4.GetBaseObject().ReferenceCount(), expect4, location);
  };

  auto TestVectorVariable = [](Dali::Vector<Dali::BaseHandle>& targetHandleVector, std::vector<Dali::BaseHandle> expectHandleVector, const char* location)
  {
    DALI_TEST_EQUALS(targetHandleVector.Count(), static_cast<Dali::VectorBase::SizeType>(expectHandleVector.size()), location);
    for(Dali::VectorBase::SizeType i = 0u; i < targetHandleVector.Count(); ++i)
    {
      if(targetHandleVector[i].GetObjectPtr() != expectHandleVector[i].GetObjectPtr())
      {
        tet_printf("Failed at [%zu]\n", i);
      }
      DALI_TEST_EQUALS(targetHandleVector[i], expectHandleVector[i], location);
    }
  };

  TestReferenceCount(1, 1, 1, 1, 1, TEST_LOCATION);

  DALI_TEST_EQUALS(static_cast<bool>(Dali::Vector<Dali::BaseHandle>::BaseType), false, TEST_LOCATION);
  Dali::Vector<Dali::BaseHandle> baseHandleVector;

  TestReferenceCount(1, 1, 1, 1, 1, TEST_LOCATION);
  baseHandleVector.PushBack(handle0);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(1u), baseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 1, 1, 1, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Front(), handle0, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Back(), handle0, TEST_LOCATION);
  baseHandleVector.PushBack(handle1);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(2u), baseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 2, 1, 1, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Front(), handle0, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Back(), handle1, TEST_LOCATION);
  baseHandleVector.PushBack(handle2);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(3u), baseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 2, 2, 1, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Front(), handle0, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Back(), handle2, TEST_LOCATION);
  baseHandleVector.PushBack(handle3);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(4u), baseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 2, 2, 2, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Front(), handle0, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Back(), handle3, TEST_LOCATION);
  baseHandleVector.PushBack(handle4);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(5u), baseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 2, 2, 2, 2, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle0, handle1, handle2, handle3, handle4}, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Front(), handle0, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Back(), handle4, TEST_LOCATION);

  // Get BaseHandle
  Dali::BaseHandle copiedHandle2 = baseHandleVector[2];
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(5u), baseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 2, 3, 2, 2, TEST_LOCATION);
  copiedHandle2.Reset();
  TestReferenceCount(2, 2, 2, 2, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Front(), handle0, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Back(), handle4, TEST_LOCATION);

  // Set BaseHandle
  baseHandleVector[2] = handle0;
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(5u), baseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(3, 2, 1, 2, 2, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle0, handle1, handle0, handle3, handle4}, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Front(), handle0, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Back(), handle4, TEST_LOCATION);
  baseHandleVector[0] = handle2;
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(5u), baseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 2, 2, 2, 2, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle2, handle1, handle0, handle3, handle4}, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Front(), handle2, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Back(), handle4, TEST_LOCATION);

  // Erase BaseHandle
  baseHandleVector.Erase(baseHandleVector.Begin());
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(4u), baseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 2, 1, 2, 2, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle1, handle0, handle3, handle4}, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Front(), handle1, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Back(), handle4, TEST_LOCATION);
  baseHandleVector.Erase(baseHandleVector.Begin() + 2);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(3u), baseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 2, 1, 1, 2, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle1, handle0, handle4}, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Front(), handle1, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Back(), handle4, TEST_LOCATION);

  // Insert BaseHandle
  baseHandleVector.Insert(baseHandleVector.Begin() + 1, handle2);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(4u), baseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 2, 2, 1, 2, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle1, handle2, handle0, handle4}, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Front(), handle1, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Back(), handle4, TEST_LOCATION);

  // Copy assign
  Dali::Vector<Dali::BaseHandle> copiedBaseHandleVector = baseHandleVector;
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(4u), baseHandleVector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(4u), copiedBaseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(3, 3, 3, 1, 3, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle1, handle2, handle0, handle4}, TEST_LOCATION);
  TestVectorVariable(copiedBaseHandleVector, {handle1, handle2, handle0, handle4}, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Front(), handle1, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Back(), handle4, TEST_LOCATION);
  DALI_TEST_EQUALS(copiedBaseHandleVector.Front(), handle1, TEST_LOCATION);
  DALI_TEST_EQUALS(copiedBaseHandleVector.Back(), handle4, TEST_LOCATION);

  baseHandleVector.PushBack(handle3);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(5u), baseHandleVector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(4u), copiedBaseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(3, 3, 3, 2, 3, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle1, handle2, handle0, handle4, handle3}, TEST_LOCATION);
  TestVectorVariable(copiedBaseHandleVector, {handle1, handle2, handle0, handle4}, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Front(), handle1, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Back(), handle3, TEST_LOCATION);
  DALI_TEST_EQUALS(copiedBaseHandleVector.Front(), handle1, TEST_LOCATION);
  DALI_TEST_EQUALS(copiedBaseHandleVector.Back(), handle4, TEST_LOCATION);

  // Insert range
  copiedBaseHandleVector.Insert(copiedBaseHandleVector.Begin() + 1, baseHandleVector.Begin(), baseHandleVector.End());
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(5u), baseHandleVector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(9u), copiedBaseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(4, 4, 4, 3, 4, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle1, handle2, handle0, handle4, handle3}, TEST_LOCATION);
  TestVectorVariable(copiedBaseHandleVector, {handle1, handle1, handle2, handle0, handle4, handle3, handle2, handle0, handle4}, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Front(), handle1, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Back(), handle3, TEST_LOCATION);
  DALI_TEST_EQUALS(copiedBaseHandleVector.Front(), handle1, TEST_LOCATION);
  DALI_TEST_EQUALS(copiedBaseHandleVector.Back(), handle4, TEST_LOCATION);

  // Move assign
  Dali::Vector<Dali::BaseHandle> movedBaseHandleVector = std::move(copiedBaseHandleVector);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(5u), baseHandleVector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(0u), copiedBaseHandleVector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(9u), movedBaseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(4, 4, 4, 3, 4, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle1, handle2, handle0, handle4, handle3}, TEST_LOCATION);
  TestVectorVariable(copiedBaseHandleVector, {}, TEST_LOCATION);
  TestVectorVariable(movedBaseHandleVector, {handle1, handle1, handle2, handle0, handle4, handle3, handle2, handle0, handle4}, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Front(), handle1, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Back(), handle3, TEST_LOCATION);
  DALI_TEST_EQUALS(movedBaseHandleVector.Front(), handle1, TEST_LOCATION);
  DALI_TEST_EQUALS(movedBaseHandleVector.Back(), handle4, TEST_LOCATION);

  // Erase range
  movedBaseHandleVector.Erase(movedBaseHandleVector.Begin() + 2, movedBaseHandleVector.End() - 1);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(5u), baseHandleVector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(3u), movedBaseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 4, 2, 2, 3, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle1, handle2, handle0, handle4, handle3}, TEST_LOCATION);
  TestVectorVariable(copiedBaseHandleVector, {}, TEST_LOCATION);
  TestVectorVariable(movedBaseHandleVector, {handle1, handle1, handle4}, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Front(), handle1, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Back(), handle3, TEST_LOCATION);
  DALI_TEST_EQUALS(movedBaseHandleVector.Front(), handle1, TEST_LOCATION);
  DALI_TEST_EQUALS(movedBaseHandleVector.Back(), handle4, TEST_LOCATION);

  baseHandleVector.Erase(baseHandleVector.End() - 1, baseHandleVector.End());
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(4u), baseHandleVector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(3u), movedBaseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 4, 2, 1, 3, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle1, handle2, handle0, handle4}, TEST_LOCATION);
  TestVectorVariable(copiedBaseHandleVector, {}, TEST_LOCATION);
  TestVectorVariable(movedBaseHandleVector, {handle1, handle1, handle4}, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Front(), handle1, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Back(), handle4, TEST_LOCATION);
  DALI_TEST_EQUALS(movedBaseHandleVector.Front(), handle1, TEST_LOCATION);
  DALI_TEST_EQUALS(movedBaseHandleVector.Back(), handle4, TEST_LOCATION);

  // Clear
  movedBaseHandleVector.Clear();
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(0u), movedBaseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 2, 2, 1, 2, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle1, handle2, handle0, handle4}, TEST_LOCATION);
  TestVectorVariable(movedBaseHandleVector, {}, TEST_LOCATION);
  DALI_TEST_GREATER(movedBaseHandleVector.Capacity(), ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Front(), handle1, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Back(), handle4, TEST_LOCATION);

  // Reserve
  movedBaseHandleVector.Reserve(128);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(0u), movedBaseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 2, 2, 1, 2, TEST_LOCATION);
  TestVectorVariable(movedBaseHandleVector, {}, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(128u), movedBaseHandleVector.Capacity(), TEST_LOCATION);

  // ShrinkToFit
  movedBaseHandleVector.Insert(movedBaseHandleVector.End(), handle3);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(1u), movedBaseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 2, 2, 2, 2, TEST_LOCATION);
  TestVectorVariable(movedBaseHandleVector, {handle3}, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(128u), movedBaseHandleVector.Capacity(), TEST_LOCATION);

  movedBaseHandleVector.ShrinkToFit();
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(1u), movedBaseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 2, 2, 2, 2, TEST_LOCATION);
  TestVectorVariable(movedBaseHandleVector, {handle3}, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(1u), movedBaseHandleVector.Capacity(), TEST_LOCATION);

  // Release
  movedBaseHandleVector.Release();
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(0u), movedBaseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 2, 2, 1, 2, TEST_LOCATION);
  TestVectorVariable(movedBaseHandleVector, {}, TEST_LOCATION);
  DALI_TEST_EQUALS(movedBaseHandleVector.Capacity(), ZERO, TEST_LOCATION);

  // Swap
  baseHandleVector.Swap(movedBaseHandleVector);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(0u), baseHandleVector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(4u), movedBaseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 2, 2, 1, 2, TEST_LOCATION);
  TestVectorVariable(movedBaseHandleVector, {handle1, handle2, handle0, handle4}, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {}, TEST_LOCATION);
  DALI_TEST_EQUALS(movedBaseHandleVector.Front(), handle1, TEST_LOCATION);
  DALI_TEST_EQUALS(movedBaseHandleVector.Back(), handle4, TEST_LOCATION);

  // Remove
  movedBaseHandleVector.Remove(movedBaseHandleVector.Begin());
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(3u), movedBaseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 1, 2, 1, 2, TEST_LOCATION);
  // Note : Below order depend on the remove behavior.
  TestVectorVariable(movedBaseHandleVector, {handle4, handle2, handle0}, TEST_LOCATION);
  DALI_TEST_EQUALS(movedBaseHandleVector.Front(), handle4, TEST_LOCATION);
  DALI_TEST_EQUALS(movedBaseHandleVector.Back(), handle0, TEST_LOCATION);

  movedBaseHandleVector.Remove(movedBaseHandleVector.End() - 1);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(2u), movedBaseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(1, 1, 2, 1, 2, TEST_LOCATION);
  // Note : Below order depend on the remove behavior.
  TestVectorVariable(movedBaseHandleVector, {handle4, handle2}, TEST_LOCATION);
  DALI_TEST_EQUALS(movedBaseHandleVector.Front(), handle4, TEST_LOCATION);
  DALI_TEST_EQUALS(movedBaseHandleVector.Back(), handle2, TEST_LOCATION);

  // Chane front element
  movedBaseHandleVector.Front() = handle3;
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(2u), movedBaseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(1, 1, 2, 2, 1, TEST_LOCATION);
  TestVectorVariable(movedBaseHandleVector, {handle3, handle2}, TEST_LOCATION);
  DALI_TEST_EQUALS(movedBaseHandleVector.Front(), handle3, TEST_LOCATION);
  DALI_TEST_EQUALS(movedBaseHandleVector.Back(), handle2, TEST_LOCATION);

  // Chane back element
  movedBaseHandleVector.Back() = handle0;
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(2u), movedBaseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 1, 1, 2, 1, TEST_LOCATION);
  TestVectorVariable(movedBaseHandleVector, {handle3, handle0}, TEST_LOCATION);
  DALI_TEST_EQUALS(movedBaseHandleVector.Front(), handle3, TEST_LOCATION);
  DALI_TEST_EQUALS(movedBaseHandleVector.Back(), handle0, TEST_LOCATION);

  END_TEST;
}

namespace
{
/// A move-only type for testing Dali::Vector with types that have deleted copy constructors.
struct MoveOnlyType
{
  explicit MoveOnlyType(int v = 0)
  : value(v)
  {
    ++sConstructCount;
  }

  ~MoveOnlyType()
  {
    ++sDestructCount;
  }

  MoveOnlyType(const MoveOnlyType&)            = delete;
  MoveOnlyType& operator=(const MoveOnlyType&) = delete;

  MoveOnlyType(MoveOnlyType&& rhs) noexcept
  : value(rhs.value)
  {
    rhs.value = -1;
    ++sConstructCount;
  }

  MoveOnlyType& operator=(MoveOnlyType&& rhs) noexcept
  {
    if(this != &rhs)
    {
      value     = rhs.value;
      rhs.value = -1;
    }
    return *this;
  }

  explicit operator bool() const noexcept
  {
    return value >= 0;
  }

  int value;

  static int sConstructCount;
  static int sDestructCount;

  static void ResetCounts()
  {
    sConstructCount = 0;
    sDestructCount  = 0;
  }
};

int MoveOnlyType::sConstructCount = 0;
int MoveOnlyType::sDestructCount  = 0;

} // anonymous namespace

int UtcDaliVectorMoveOnlyPushBack(void)
{
  tet_infoline("Testing Dali::Vector PushBack with move-only type");

  MoveOnlyType::ResetCounts();
  DALI_TEST_EQUALS(static_cast<bool>(Dali::Vector<MoveOnlyType>::BaseType), false, TEST_LOCATION);

  Vector<MoveOnlyType> vector;
  DALI_TEST_EQUALS(ZERO, vector.Count(), TEST_LOCATION);

  vector.PushBack(MoveOnlyType(10));
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(1u), vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(vector[0].value, 10, TEST_LOCATION);

  vector.PushBack(MoveOnlyType(20));
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(2u), vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(vector[0].value, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[1].value, 20, TEST_LOCATION);

  vector.PushBack(MoveOnlyType(30));
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(3u), vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(vector[0].value, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[1].value, 20, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[2].value, 30, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVectorMoveOnlyFrontBack(void)
{
  tet_infoline("Testing Dali::Vector Back with move-only type");

  MoveOnlyType::ResetCounts();

  Vector<MoveOnlyType> vector;
  DALI_TEST_EQUALS(ZERO, vector.Count(), TEST_LOCATION);

  vector.PushBack(MoveOnlyType(10));
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(1u), vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(vector[0].value, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.Front().value, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.Back().value, 10, TEST_LOCATION);

  vector.PushBack(MoveOnlyType(20));
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(2u), vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(vector[0].value, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[1].value, 20, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.Front().value, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.Back().value, 20, TEST_LOCATION);

  vector.PushBack(MoveOnlyType(30));
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(3u), vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(vector[0].value, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[1].value, 20, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[2].value, 30, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.Front().value, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.Back().value, 30, TEST_LOCATION);

  // Change value
  vector.Front().value = 40;
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(3u), vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(vector[0].value, 40, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[1].value, 20, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[2].value, 30, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.Front().value, 40, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.Back().value, 30, TEST_LOCATION);

  vector.Back().value = 50;
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(3u), vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(vector[0].value, 40, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[1].value, 20, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[2].value, 50, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.Front().value, 40, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.Back().value, 50, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVectorMoveOnlyInsert(void)
{
  tet_infoline("Testing Dali::Vector Insert with move-only type");

  Vector<MoveOnlyType> vector;

  // Insert at end (like PushBack)
  vector.PushBack(MoveOnlyType(10));
  vector.PushBack(MoveOnlyType(30));

  // Insert in the middle
  vector.Insert(vector.Begin() + 1, MoveOnlyType(20));
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(3u), vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(vector[0].value, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[1].value, 20, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[2].value, 30, TEST_LOCATION);

  // Insert at beginning
  vector.Insert(vector.Begin(), MoveOnlyType(5));
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(4u), vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(vector[0].value, 5, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[1].value, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[2].value, 20, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[3].value, 30, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVectorMoveOnlyErase(void)
{
  tet_infoline("Testing Dali::Vector Erase with move-only type");

  Vector<MoveOnlyType> vector;
  vector.PushBack(MoveOnlyType(10));
  vector.PushBack(MoveOnlyType(20));
  vector.PushBack(MoveOnlyType(30));
  vector.PushBack(MoveOnlyType(40));

  // Erase from middle
  vector.Erase(vector.Begin() + 1);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(3u), vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(vector[0].value, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[1].value, 30, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[2].value, 40, TEST_LOCATION);

  // Erase from beginning
  vector.Erase(vector.Begin());
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(2u), vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(vector[0].value, 30, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[1].value, 40, TEST_LOCATION);

  // Erase last element (goes through Remove path)
  vector.Erase(vector.End() - 1);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(1u), vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(vector[0].value, 30, TEST_LOCATION);

  // Erase sole element
  vector.Erase(vector.Begin());
  DALI_TEST_EQUALS(ZERO, vector.Count(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliVectorMoveOnlyEraseRange(void)
{
  tet_infoline("Testing Dali::Vector Erase range with move-only type");

  Vector<MoveOnlyType> vector;
  vector.PushBack(MoveOnlyType(10));
  vector.PushBack(MoveOnlyType(20));
  vector.PushBack(MoveOnlyType(30));
  vector.PushBack(MoveOnlyType(40));
  vector.PushBack(MoveOnlyType(50));

  // Erase range from middle
  vector.Erase(vector.Begin() + 1, vector.Begin() + 3);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(3u), vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(vector[0].value, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[1].value, 40, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[2].value, 50, TEST_LOCATION);

  // Erase range from end
  vector.Erase(vector.Begin() + 1, vector.End());
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(1u), vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(vector[0].value, 10, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVectorMoveOnlyRemove(void)
{
  tet_infoline("Testing Dali::Vector Remove with move-only type");

  Vector<MoveOnlyType> vector;
  vector.PushBack(MoveOnlyType(10));
  vector.PushBack(MoveOnlyType(20));
  vector.PushBack(MoveOnlyType(30));

  // Remove from beginning (swaps with last, destroys last)
  vector.Remove(vector.Begin());
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(2u), vector.Count(), TEST_LOCATION);
  // After Remove: last element (30) was swapped to position 0, then old last destroyed
  DALI_TEST_EQUALS(vector[0].value, 30, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[1].value, 20, TEST_LOCATION);

  // Remove last element (no swap needed)
  vector.Remove(vector.End() - 1);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(1u), vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(vector[0].value, 30, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVectorMoveOnlyGrowth(void)
{
  tet_infoline("Testing Dali::Vector automatic growth/reallocation with move-only type");

  MoveOnlyType::ResetCounts();

  Vector<MoveOnlyType> vector;

  // Push enough elements to trigger multiple reallocations.
  // Dali::Vector doubles capacity, so starting from 0:
  //   capacity 0 → 2 → 4 → 8 → 16 → 32
  const int count = 20;
  for(int i = 0; i < count; ++i)
  {
    vector.PushBack(MoveOnlyType(i * 10));
  }

  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(count), vector.Count(), TEST_LOCATION);
  DALI_TEST_GREATER(vector.Capacity(), static_cast<Dali::VectorBase::SizeType>(count - 1), TEST_LOCATION);

  // Verify all values survived multiple reallocations
  for(int i = 0; i < count; ++i)
  {
    DALI_TEST_EQUALS(vector[i].value, i * 10, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliVectorMoveOnlyReserveRealloc(void)
{
  tet_infoline("Testing Dali::Vector Reserve causing reallocation with existing move-only elements");

  Vector<MoveOnlyType> vector;
  vector.PushBack(MoveOnlyType(10));
  vector.PushBack(MoveOnlyType(20));
  vector.PushBack(MoveOnlyType(30));

  auto oldCapacity = vector.Capacity();

  // Force reallocation by reserving much more
  vector.Reserve(128);
  DALI_TEST_GREATER(vector.Capacity(), oldCapacity, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(3u), vector.Count(), TEST_LOCATION);

  // Values must survive the reallocation
  DALI_TEST_EQUALS(vector[0].value, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[1].value, 20, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[2].value, 30, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVectorMoveOnlyClear(void)
{
  tet_infoline("Testing Dali::Vector Clear and Release with move-only type");

  MoveOnlyType::ResetCounts();

  {
    Vector<MoveOnlyType> vector;
    vector.PushBack(MoveOnlyType(10));
    vector.PushBack(MoveOnlyType(20));
    vector.PushBack(MoveOnlyType(30));

    MoveOnlyType::ResetCounts();

    vector.Clear();
    DALI_TEST_EQUALS(ZERO, vector.Count(), TEST_LOCATION);
    DALI_TEST_GREATER(vector.Capacity(), ZERO, TEST_LOCATION);
    // All 3 elements should have been destructed
    DALI_TEST_EQUALS(MoveOnlyType::sDestructCount, 3, TEST_LOCATION);

    MoveOnlyType::ResetCounts();
  }
  // Vector destructor calls Release — no elements remain, so no additional destructions
  DALI_TEST_EQUALS(MoveOnlyType::sDestructCount, 0, TEST_LOCATION);

  {
    Vector<MoveOnlyType> vector;
    vector.PushBack(MoveOnlyType(10));
    vector.PushBack(MoveOnlyType(20));

    MoveOnlyType::ResetCounts();

    vector.Release();
    DALI_TEST_EQUALS(ZERO, vector.Count(), TEST_LOCATION);
    DALI_TEST_EQUALS(ZERO, vector.Capacity(), TEST_LOCATION);
    // All 2 elements should have been destructed
    DALI_TEST_EQUALS(MoveOnlyType::sDestructCount, 2, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliVectorMoveOnlyInsertGrowth(void)
{
  tet_infoline("Testing Dali::Vector Insert at middle causing reallocation with move-only type");

  Vector<MoveOnlyType> vector;
  // Fill to capacity
  vector.Reserve(4);
  vector.PushBack(MoveOnlyType(10));
  vector.PushBack(MoveOnlyType(20));
  vector.PushBack(MoveOnlyType(30));
  vector.PushBack(MoveOnlyType(40));
  DALI_TEST_EQUALS(vector.Count(), vector.Capacity(), TEST_LOCATION);

  // This insert must trigger reallocation AND shift elements
  vector.Insert(vector.Begin() + 2, MoveOnlyType(25));
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(5u), vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(vector[0].value, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[1].value, 20, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[2].value, 25, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[3].value, 30, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[4].value, 40, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVectorComplexMovePushBack(void)
{
  tet_infoline("Testing Dali::Vector PushBack(ItemType&&) with copyable complex type");

  TestApplication application;

  Dali::BaseHandle handle0 = Dali::Actor::New();
  DALI_TEST_EQUALS(handle0.GetBaseObject().ReferenceCount(), 1u, TEST_LOCATION);

  Vector<Dali::BaseHandle> vector;

  // Move handle into vector — reference count should NOT increase (moved, not copied)
  Dali::BaseHandle movable = handle0;
  DALI_TEST_EQUALS(handle0.GetBaseObject().ReferenceCount(), 2u, TEST_LOCATION);

  vector.PushBack(std::move(movable));
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(1u), vector.Count(), TEST_LOCATION);
  // movable was moved, so ref count stays at 2 (handle0 + vector element)
  DALI_TEST_EQUALS(handle0.GetBaseObject().ReferenceCount(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(!movable); // movable should be empty after move

  END_TEST;
}

int UtcDaliVectorComplexMoveInsert(void)
{
  tet_infoline("Testing Dali::Vector Insert(Iterator, ItemType&&) with copyable complex type");

  TestApplication application;

  Dali::BaseHandle handle0 = Dali::Actor::New();
  Dali::BaseHandle handle1 = Dali::Actor::New();
  Dali::BaseHandle handle2 = Dali::Actor::New();

  Vector<Dali::BaseHandle> vector;
  vector.PushBack(handle0);
  vector.PushBack(handle2);
  DALI_TEST_EQUALS(handle0.GetBaseObject().ReferenceCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(handle1.GetBaseObject().ReferenceCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(handle2.GetBaseObject().ReferenceCount(), 2u, TEST_LOCATION);

  // Move-insert in the middle
  Dali::BaseHandle movable1 = handle1;
  DALI_TEST_EQUALS(handle1.GetBaseObject().ReferenceCount(), 2u, TEST_LOCATION);

  vector.Insert(vector.Begin() + 1, std::move(movable1));
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(3u), vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(handle1.GetBaseObject().ReferenceCount(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(!movable1);
  DALI_TEST_EQUALS(vector[0], handle0, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[1], handle1, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[2], handle2, TEST_LOCATION);

  END_TEST;
}
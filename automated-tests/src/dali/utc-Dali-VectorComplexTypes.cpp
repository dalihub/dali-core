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

int UtcDaliVectorData(void)
{
  tet_infoline("Testing Dali::Vector::Data() method");

  // Test with trivial type
  {
    Vector<int> vector;

    // Empty vector should return nullptr
    DALI_TEST_CHECK(vector.Data() == nullptr);
    DALI_TEST_CHECK(const_cast<const Vector<int>&>(vector).Data() == nullptr);

    // Add elements
    vector.PushBack(10);
    vector.PushBack(20);
    vector.PushBack(30);

    // Data() should return valid pointer
    int* data = vector.Data();
    DALI_TEST_CHECK(data != nullptr);
    DALI_TEST_EQUALS(data[0], 10, TEST_LOCATION);
    DALI_TEST_EQUALS(data[1], 20, TEST_LOCATION);
    DALI_TEST_EQUALS(data[2], 30, TEST_LOCATION);

    // Const version
    const Vector<int>& constVector = vector;
    const int*         constData   = constVector.Data();
    DALI_TEST_CHECK(constData != nullptr);
    DALI_TEST_EQUALS(constData[0], 10, TEST_LOCATION);
    DALI_TEST_EQUALS(constData[1], 20, TEST_LOCATION);
    DALI_TEST_EQUALS(constData[2], 30, TEST_LOCATION);

    // Data() should equal Begin()
    DALI_TEST_CHECK(data == vector.Begin());
    DALI_TEST_CHECK(constData == vector.Begin());

    // After a reallocation-triggering PushBack, Data() must still return a
    // valid pointer (it will point to the new buffer, not the old one).
    // We cannot safely dereference the old `data` pointer after this call,
    // but we can confirm that Data() itself remains functional.
    vector.PushBack(40);
    vector.PushBack(50);
    int* dataAfterGrowth = vector.Data();
    DALI_TEST_CHECK(dataAfterGrowth != nullptr);
    DALI_TEST_CHECK(dataAfterGrowth == vector.Begin());
    DALI_TEST_EQUALS(dataAfterGrowth[0], 10, TEST_LOCATION);
    DALI_TEST_EQUALS(dataAfterGrowth[1], 20, TEST_LOCATION);
    DALI_TEST_EQUALS(dataAfterGrowth[2], 30, TEST_LOCATION);
    DALI_TEST_EQUALS(dataAfterGrowth[3], 40, TEST_LOCATION);
    DALI_TEST_EQUALS(dataAfterGrowth[4], 50, TEST_LOCATION);
  }

  // Test with non-trivial type
  {
    Vector<Dali::BaseHandle> vector;
    DALI_TEST_CHECK(vector.Data() == nullptr);
    DALI_TEST_CHECK(const_cast<const Vector<Dali::BaseHandle>&>(vector).Data() == nullptr);

    TestApplication  application;
    Dali::BaseHandle handle0 = Dali::Actor::New();
    Dali::BaseHandle handle1 = Dali::Actor::New();
    Dali::BaseHandle handle2 = Dali::Actor::New();

    vector.PushBack(handle0);
    vector.PushBack(handle1);
    vector.PushBack(handle2);

    Dali::BaseHandle* data = vector.Data();
    DALI_TEST_CHECK(data != nullptr);
    DALI_TEST_EQUALS(data[0], handle0, TEST_LOCATION);
    DALI_TEST_EQUALS(data[1], handle1, TEST_LOCATION);
    DALI_TEST_EQUALS(data[2], handle2, TEST_LOCATION);

    // Data() must equal Begin() for non-trivial types too
    DALI_TEST_CHECK(data == vector.Begin());

    const Vector<Dali::BaseHandle>& constVec  = vector;
    const Dali::BaseHandle*         constData = constVec.Data();
    DALI_TEST_CHECK(constData != nullptr);
    DALI_TEST_CHECK(constData == vector.Begin());
    DALI_TEST_EQUALS(constData[0], handle0, TEST_LOCATION);
    DALI_TEST_EQUALS(constData[1], handle1, TEST_LOCATION);
    DALI_TEST_EQUALS(constData[2], handle2, TEST_LOCATION);
  }

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

  // Self insert range within same vector.
  baseHandleVector.Clear();
  TestReferenceCount(3, 3, 3, 2, 3, TEST_LOCATION);

  baseHandleVector.PushBack(handle0);
  baseHandleVector.PushBack(handle1);
  baseHandleVector.PushBack(handle2);
  baseHandleVector.PushBack(handle3);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(4u), baseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(4, 4, 4, 3, 3, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle0, handle1, handle2, handle3}, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Front(), handle0, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Back(), handle3, TEST_LOCATION);

  baseHandleVector.Insert(baseHandleVector.Begin() + 1, baseHandleVector.Begin(), baseHandleVector.End());
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(8u), baseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(5, 5, 5, 4, 3, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle0, handle0, handle1, handle2, handle3, handle1, handle2, handle3}, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Front(), handle0, TEST_LOCATION);
  DALI_TEST_EQUALS(baseHandleVector.Back(), handle3, TEST_LOCATION);

  // Revive the original vector for further tests (Since the above self-insert test made after below test case prepared. The number of references is not updated).
  baseHandleVector.Clear();
  baseHandleVector.PushBack(handle1);
  baseHandleVector.PushBack(handle2);
  baseHandleVector.PushBack(handle0);
  baseHandleVector.PushBack(handle4);
  baseHandleVector.PushBack(handle3);

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

int UtcDaliVectorMoveOnlyShrinkToFit(void)
{
  tet_infoline("Testing Dali::Vector ShrinkToFit with move-only type");

  MoveOnlyType::ResetCounts();

  Vector<MoveOnlyType> vector;
  vector.Reserve(8);
  DALI_TEST_EQUALS(ZERO, vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(8u), vector.Capacity(), TEST_LOCATION);

  vector.PushBack(MoveOnlyType(10));
  vector.PushBack(MoveOnlyType(20));

  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(2u), vector.Count(), TEST_LOCATION);
  DALI_TEST_GREATER(vector.Capacity(), static_cast<Dali::VectorBase::SizeType>(2u), TEST_LOCATION);
  DALI_TEST_EQUALS(vector[0].value, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[1].value, 20, TEST_LOCATION);

  DALI_TEST_EQUALS(MoveOnlyType::sConstructCount, 4, TEST_LOCATION);
  DALI_TEST_EQUALS(MoveOnlyType::sDestructCount, 2, TEST_LOCATION);

  vector.ShrinkToFit();

  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(2u), vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(vector.Capacity(), static_cast<Dali::VectorBase::SizeType>(2u), TEST_LOCATION);
  DALI_TEST_EQUALS(vector[0].value, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[1].value, 20, TEST_LOCATION);

  DALI_TEST_EQUALS(MoveOnlyType::sConstructCount, 6, TEST_LOCATION);
  DALI_TEST_EQUALS(MoveOnlyType::sDestructCount, 4, TEST_LOCATION);

  vector.Clear();

  DALI_TEST_EQUALS(MoveOnlyType::sDestructCount, 6, TEST_LOCATION);
  DALI_TEST_EQUALS(ZERO, vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(vector.Capacity(), static_cast<Dali::VectorBase::SizeType>(2u), TEST_LOCATION);

  vector.ShrinkToFit();

  DALI_TEST_EQUALS(ZERO, vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(ZERO, vector.Capacity(), TEST_LOCATION);

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

int UtcDaliVectorMoveOnlyInsertMove(void)
{
  tet_infoline("Testing Dali::Vector InsertMove with move-only type");

  MoveOnlyType::ResetCounts();

  Vector<MoveOnlyType> source;
  source.PushBack(MoveOnlyType(10));
  source.PushBack(MoveOnlyType(20));

  Vector<MoveOnlyType> vector;
  vector.PushBack(MoveOnlyType(5));
  vector.PushBack(MoveOnlyType(15));

  vector.InsertMove(vector.End(), source.Begin(), source.End());
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(4u), vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(vector[0].value, 5, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[1].value, 15, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[2].value, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[3].value, 20, TEST_LOCATION);

  // Note that we don't touch the original container size.
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(2u), source.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(source[0].value, -1, TEST_LOCATION); // moved
  DALI_TEST_EQUALS(source[1].value, -1, TEST_LOCATION); // moved

  // InsertMove self
  vector.InsertMove(vector.Begin() + 1, vector.Begin(), vector.Begin() + 2);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(6u), vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(vector[0].value, -1, TEST_LOCATION); // moved
  DALI_TEST_EQUALS(vector[1].value, 5, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[2].value, 15, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[3].value, -1, TEST_LOCATION); // moved
  DALI_TEST_EQUALS(vector[4].value, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[5].value, 20, TEST_LOCATION);

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

int UtcDaliVectorResizeNonTrivial(void)
{
  tet_infoline("Testing Dali::Vector::Resize with non-trivial types - destructors must be called when shrinking");

  // Shrink
  {
    Vector<MoveOnlyType> vector;

    // Add 5 elements
    vector.PushBack(MoveOnlyType(1));
    vector.PushBack(MoveOnlyType(2));
    vector.PushBack(MoveOnlyType(3));
    vector.PushBack(MoveOnlyType(4));
    vector.PushBack(MoveOnlyType(5));

    DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(5u), vector.Count(), TEST_LOCATION);

    // Reset counters before shrink test
    MoveOnlyType::sConstructCount = 0;
    MoveOnlyType::sDestructCount  = 0;

    // Shrink from 5 to 2 using the rvalue overload (required for move-only types).
    // The temporary MoveOnlyType(0) argument is bound by rvalue reference and is
    // NOT moved into the vector on the shrink path — it is simply destroyed at the
    // end of the statement.
    // Destruction tally:
    //   3 in-vector elements (indices 2, 3, 4) destroyed by Erase()
    //   1 temporary MoveOnlyType(0) argument destroyed at end of statement
    //   Total: 4
    vector.Resize(2u, MoveOnlyType(0));

    DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(2u), vector.Count(), TEST_LOCATION);
    DALI_TEST_EQUALS(MoveOnlyType::sDestructCount, 4, TEST_LOCATION);
    DALI_TEST_EQUALS(vector[0].value, 1, TEST_LOCATION);
    DALI_TEST_EQUALS(vector[1].value, 2, TEST_LOCATION);

    // Reset counters before next shrink
    MoveOnlyType::sConstructCount = 0;
    MoveOnlyType::sDestructCount  = 0;

    // Shrink from 2 to 0.
    // Destruction tally:
    //   2 in-vector elements (indices 0, 1) destroyed by Erase()
    //   1 temporary MoveOnlyType(0) argument destroyed at end of statement
    //   Total: 3
    vector.Resize(0u, MoveOnlyType(0));

    DALI_TEST_EQUALS(ZERO, vector.Count(), TEST_LOCATION);
    DALI_TEST_EQUALS(MoveOnlyType::sDestructCount, 3, TEST_LOCATION);
  }

  // Resize to same size
  {
    Vector<MoveOnlyType> vector;
    vector.PushBack(MoveOnlyType(10));
    vector.PushBack(MoveOnlyType(20));

    MoveOnlyType::ResetCounts();

    // Resize to current size — no construction, no destruction (other than the
    // temporary argument itself).
    vector.Resize(2u, MoveOnlyType(0));

    DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(2u), vector.Count(), TEST_LOCATION);
    // Only the temporary argument was destroyed; vector elements are untouched.
    DALI_TEST_EQUALS(MoveOnlyType::sDestructCount, 1, TEST_LOCATION);
    DALI_TEST_EQUALS(vector[0].value, 10, TEST_LOCATION);
    DALI_TEST_EQUALS(vector[1].value, 20, TEST_LOCATION);
  }

  // Grow by one
  {
    Vector<MoveOnlyType> vector;
    vector.PushBack(MoveOnlyType(10));
    vector.PushBack(MoveOnlyType(20));

    // Reserve the target capacity before resetting counters so that the
    // Reserve() call inside Resize() does not trigger a reallocation that
    // would move existing elements and pollute sConstructCount/sDestructCount.
    vector.Reserve(3u);
    MoveOnlyType::ResetCounts();

    // Grow from 2 to 3 by moving a single item.
    // Construction tally:
    //   1 temporary MoveOnlyType(99) constructed at call site
    //   1 move-construction into the first (and only) new vector slot
    //   Total constructions: 2
    // Destruction tally:
    //   1 moved-from temporary destroyed at end of full-expression
    //   Total destructions: 1
    vector.Resize(3u, MoveOnlyType(99));

    DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(3u), vector.Count(), TEST_LOCATION);
    DALI_TEST_EQUALS(MoveOnlyType::sConstructCount, 2, TEST_LOCATION);
    DALI_TEST_EQUALS(MoveOnlyType::sDestructCount, 1, TEST_LOCATION);
    DALI_TEST_EQUALS(vector[0].value, 10, TEST_LOCATION);
    DALI_TEST_EQUALS(vector[1].value, 20, TEST_LOCATION);
    DALI_TEST_EQUALS(vector[2].value, 99, TEST_LOCATION);
  }

  // Grow by many
  // item is moved into the first new slot; additional slots are default-constructed.
  {
    Vector<MoveOnlyType> vector;
    vector.PushBack(MoveOnlyType(10));
    vector.PushBack(MoveOnlyType(20));

    // Reserve the target capacity before resetting counters so that the
    // Reserve() call inside Resize() does not trigger a reallocation and
    // pollute the construct/destruct counts with element-move operations.
    vector.Reserve(5u);
    MoveOnlyType::ResetCounts();

    // Grow from 2 to 5 (3 new slots).
    //
    // Construction tally:
    //   1  temporary MoveOnlyType(99) constructed at call site
    //   1  move-construction of that temporary into new slot [2]  (via InsertMove)
    //   1  temporary MoveOnlyType(0) for PushBack(ItemType()) → slot [3]
    //   1  move-construction of that temporary into new slot [3]  (via InsertMove)
    //   1  temporary MoveOnlyType(0) for PushBack(ItemType()) → slot [4]
    //   1  move-construction of that temporary into new slot [4]  (via InsertMove)
    //   Total constructions: 6
    //
    // Destruction tally:
    //   1  temporary MoveOnlyType(99) (now moved-from) destroyed at end of
    //      full-expression after Resize() returns
    //   1  temporary MoveOnlyType(0) for slot [3] destroyed at end of its
    //      enclosing PushBack(ItemType()) full-expression
    //   1  temporary MoveOnlyType(0) for slot [4] destroyed at end of its
    //      enclosing PushBack(ItemType()) full-expression
    //   Note: InsertMove moves the elements into the vector but does NOT call
    //         the destructor on the source bytes itself; the temporaries are
    //         destroyed by normal C++ scope rules.
    //   Total destructions: 3
    vector.Resize(5u, MoveOnlyType(99));

    DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(5u), vector.Count(), TEST_LOCATION);
    DALI_TEST_EQUALS(MoveOnlyType::sConstructCount, 6, TEST_LOCATION);
    DALI_TEST_EQUALS(MoveOnlyType::sDestructCount, 3, TEST_LOCATION);

    // Original elements preserved
    DALI_TEST_EQUALS(vector[0].value, 10, TEST_LOCATION);
    DALI_TEST_EQUALS(vector[1].value, 20, TEST_LOCATION);
    // First new slot received the moved item
    DALI_TEST_EQUALS(vector[2].value, 99, TEST_LOCATION);
    // Remaining new slots are default-constructed (MoveOnlyType() → value==0)
    DALI_TEST_EQUALS(vector[3].value, 0, TEST_LOCATION);
    DALI_TEST_EQUALS(vector[4].value, 0, TEST_LOCATION);
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

int UtcDaliVectorComplexMoveAssignmentDestruct(void)
{
  tet_infoline("Testing Dali::Vector move assignment destructs existing non-trivial elements");

  // Regression test for the bug where operator=(Vector&&) called Replace() without
  // first calling Clear(), silently skipping destructors of existing elements.
  //
  // Before the fix:  existing elements were overwritten without destruction -> resource leak.
  // After the fix:   Clear() is called first, so every existing element is properly destroyed.

  TestApplication application;

  Dali::BaseHandle handle0 = Dali::Actor::New();
  Dali::BaseHandle handle1 = Dali::Actor::New();
  Dali::BaseHandle handle2 = Dali::Actor::New();
  Dali::BaseHandle handle3 = Dali::Actor::New();

  // dst holds handle0 and handle1 copies (refcount 2 each).
  Vector<Dali::BaseHandle> dst;
  dst.PushBack(handle0);
  dst.PushBack(handle1);
  DALI_TEST_EQUALS(handle0.GetBaseObject().ReferenceCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(handle1.GetBaseObject().ReferenceCount(), 2u, TEST_LOCATION);

  // src holds handle2 and handle3 copies (refcount 2 each).
  Vector<Dali::BaseHandle> src;
  src.PushBack(handle2);
  src.PushBack(handle3);
  DALI_TEST_EQUALS(handle2.GetBaseObject().ReferenceCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(handle3.GetBaseObject().ReferenceCount(), 2u, TEST_LOCATION);

  // Move-assign src into dst.
  // Correct behaviour:
  //   1. dst's existing elements (handle0, handle1 copies) are destroyed -> refcounts drop to 1.
  //   2. dst takes ownership of src's buffer (handle2, handle3 copies).
  //   3. src becomes empty.
#if (__GNUC__ >= 13)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
#endif
  dst = std::move(src);
#if (__GNUC__ >= 13)
#pragma GCC diagnostic pop
#endif

  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(2u), dst.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(ZERO, src.Count(), TEST_LOCATION);

  // handle0 and handle1 copies that lived in dst must have been destructed.
  DALI_TEST_EQUALS(handle0.GetBaseObject().ReferenceCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(handle1.GetBaseObject().ReferenceCount(), 1u, TEST_LOCATION);

  // dst now owns what was in src.
  DALI_TEST_EQUALS(handle2.GetBaseObject().ReferenceCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(handle3.GetBaseObject().ReferenceCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(dst[0], handle2, TEST_LOCATION);
  DALI_TEST_EQUALS(dst[1], handle3, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVectorComplexMoveAssignmentSelf(void)
{
  tet_infoline("Testing Dali::Vector move self-assignment is a no-op");

  TestApplication application;

  Dali::BaseHandle handle0 = Dali::Actor::New();
  Dali::BaseHandle handle1 = Dali::Actor::New();

  Vector<Dali::BaseHandle> v;
  v.PushBack(handle0);
  v.PushBack(handle1);
  DALI_TEST_EQUALS(handle0.GetBaseObject().ReferenceCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(handle1.GetBaseObject().ReferenceCount(), 2u, TEST_LOCATION);

  // Self-assign: guarded by the (this != &vector) check; must be a no-op.
#if (__GNUC__ >= 13)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
#endif
  v = std::move(v);
#if (__GNUC__ >= 13)
#pragma GCC diagnostic pop
#endif

  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(2u), v.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(handle0.GetBaseObject().ReferenceCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(handle1.GetBaseObject().ReferenceCount(), 2u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVectorMoveOnlyMoveAssignment(void)
{
  tet_infoline("Testing Dali::Vector move assignment with move-only type properly destructs existing elements");

  MoveOnlyType::ResetCounts();

  Vector<MoveOnlyType> dst;
  dst.PushBack(MoveOnlyType(1));
  dst.PushBack(MoveOnlyType(2));
  dst.PushBack(MoveOnlyType(3));

  Vector<MoveOnlyType> src;
  src.PushBack(MoveOnlyType(10));
  src.PushBack(MoveOnlyType(20));

  MoveOnlyType::ResetCounts();

  // Move src into dst. dst's 3 existing elements must be destroyed.
#if (__GNUC__ >= 13)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
#endif
  dst = std::move(src);
#if (__GNUC__ >= 13)
#pragma GCC diagnostic pop
#endif

  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(2u), dst.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(ZERO, src.Count(), TEST_LOCATION);

  // Exactly 3 destructors must have fired for the old dst elements.
  DALI_TEST_EQUALS(MoveOnlyType::sDestructCount, 3, TEST_LOCATION);

  DALI_TEST_EQUALS(dst[0].value, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(dst[1].value, 20, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVectorResizeTrivialShrinkP(void)
{
  tet_infoline("Testing Dali::Vector::Resize shrink with trivial type (const-ref overload)");

  // Verifies the trivial-type Resize(count, const ItemType&) shrink path.
  // For trivial types no destructors are called, but count must update correctly.
  Vector<int> vector;
  vector.PushBack(1);
  vector.PushBack(2);
  vector.PushBack(3);
  vector.PushBack(4);
  vector.PushBack(5);

  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(5u), vector.Count(), TEST_LOCATION);

  // Shrink from 5 to 2.
  vector.Resize(2u, 0);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(2u), vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(vector[0], 1, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[1], 2, TEST_LOCATION);

  // Shrink to 0.
  vector.Resize(0u, 0);
  DALI_TEST_EQUALS(ZERO, vector.Count(), TEST_LOCATION);

  // Grow back to 3 (fill value 7).
  vector.Resize(3u, 7);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(3u), vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(vector[0], 7, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[1], 7, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[2], 7, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVectorMoveOnlyInsertSelf(void)
{
  tet_infoline("Testing Dali::Vector Insert(at, element&&) where element is already inside the vector (self-insert)");
  // Regression test: Insert(Iterator, ItemType&&) used to have a use-after-move bug.
  // MoveItemInternal shifts existing elements right (moving and destroying originals),
  // then the loop move-constructs from 'element'.  When 'element' lived in [at, End()),
  // it was already destroyed before the loop read from it.
  // The fix: if &element is inside [Begin(), End()) and at <= &element, move element
  // to a local variable first so the source is valid throughout InsertMove.

  // Case 1: insert last element before a middle element.
  // vector=[10,20,30], insert vector[2](30) at position 1 → [10,30,20,-1]
  {
    Vector<MoveOnlyType> vector;
    vector.Reserve(5);
    vector.PushBack(MoveOnlyType(10));
    vector.PushBack(MoveOnlyType(20));
    vector.PushBack(MoveOnlyType(30));

    vector.Insert(vector.Begin() + 1, std::move(vector[2]));

    DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(4u), vector.Count(), TEST_LOCATION);
    DALI_TEST_EQUALS(vector[0].value, 10, TEST_LOCATION);
    DALI_TEST_EQUALS(vector[1].value, 30, TEST_LOCATION); // inserted value
    DALI_TEST_EQUALS(vector[2].value, 20, TEST_LOCATION); // shifted
    DALI_TEST_EQUALS(vector[3].value, -1, TEST_LOCATION); // moved-from original
  }

  // Case 2: insert last element at the front.
  // vector=[10,20,30], insert vector[2](30) at Begin() → [30,10,20,-1]
  {
    Vector<MoveOnlyType> vector;
    vector.Reserve(5);
    vector.PushBack(MoveOnlyType(10));
    vector.PushBack(MoveOnlyType(20));
    vector.PushBack(MoveOnlyType(30));

    vector.Insert(vector.Begin(), std::move(vector[2]));

    DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(4u), vector.Count(), TEST_LOCATION);
    DALI_TEST_EQUALS(vector[0].value, 30, TEST_LOCATION); // inserted value
    DALI_TEST_EQUALS(vector[1].value, 10, TEST_LOCATION); // shifted
    DALI_TEST_EQUALS(vector[2].value, 20, TEST_LOCATION); // shifted
    DALI_TEST_EQUALS(vector[3].value, -1, TEST_LOCATION); // moved-from original
  }

  // Case 3: at == &element (inserting an element at its own position).
  // vector=[10,20,30], insert vector[1](20) at position 1 → [10,20,-1,30]
  {
    Vector<MoveOnlyType> vector;
    vector.Reserve(5);
    vector.PushBack(MoveOnlyType(10));
    vector.PushBack(MoveOnlyType(20));
    vector.PushBack(MoveOnlyType(30));

    vector.Insert(vector.Begin() + 1, std::move(vector[1]));

    DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(4u), vector.Count(), TEST_LOCATION);
    DALI_TEST_EQUALS(vector[0].value, 10, TEST_LOCATION);
    DALI_TEST_EQUALS(vector[1].value, 20, TEST_LOCATION); // inserted value
    DALI_TEST_EQUALS(vector[2].value, -1, TEST_LOCATION); // moved-from original (shifted to pos 2)
    DALI_TEST_EQUALS(vector[3].value, 30, TEST_LOCATION); // shifted
  }

  END_TEST;
}

int UtcDaliVectorComplexMoveInsertSelf(void)
{
  tet_infoline("Testing Dali::Vector Insert(at, element&&) self-insert with BaseHandle (verifies reference counts)");
  // Same regression as UtcDaliVectorMoveOnlyInsertSelf but uses BaseHandle so we can
  // confirm that reference counts are correct (no double-release, no leak).

  TestApplication application;

  Dali::BaseHandle handle0 = Dali::Actor::New();
  Dali::BaseHandle handle1 = Dali::Actor::New();
  Dali::BaseHandle handle2 = Dali::Actor::New();

  Vector<Dali::BaseHandle> vector;
  vector.Reserve(4);
  vector.PushBack(handle0);
  vector.PushBack(handle1);
  vector.PushBack(handle2);

  DALI_TEST_EQUALS(handle0.GetBaseObject().ReferenceCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(handle1.GetBaseObject().ReferenceCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(handle2.GetBaseObject().ReferenceCount(), 2u, TEST_LOCATION);

  // Self-insert: move vector[2] (a copy of handle2) to position 1.
  // Expected order:  [handle0, handle2, handle1, <empty>]
  // Reference counts must not change for handle0/handle1/handle2.
  vector.Insert(vector.Begin() + 1, std::move(vector[2]));

  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(4u), vector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(vector[0], handle0, TEST_LOCATION);
  DALI_TEST_EQUALS(vector[1], handle2, TEST_LOCATION); // inserted
  DALI_TEST_EQUALS(vector[2], handle1, TEST_LOCATION); // shifted
  DALI_TEST_CHECK(!vector[3]);                         // moved-from: empty handle

  // No extra AddRef or Release should have happened.
  DALI_TEST_EQUALS(handle0.GetBaseObject().ReferenceCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(handle1.GetBaseObject().ReferenceCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(handle2.GetBaseObject().ReferenceCount(), 2u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVectorComplexTypeTraitsP(void)
{
  tet_infoline("Testing VectorIsTrivial / TypeTraits gives correct trivial classification");

  // int is trivial -> BaseType == true (uses trivial algorithm)
  DALI_TEST_EQUALS(static_cast<bool>(Dali::Vector<int>::BaseType), true, TEST_LOCATION);

  // float is trivial
  DALI_TEST_EQUALS(static_cast<bool>(Dali::Vector<float>::BaseType), true, TEST_LOCATION);

  // ComplexType has user-defined destructor -> non-trivial
  DALI_TEST_EQUALS(static_cast<bool>(Dali::Vector<ComplexType>::BaseType), false, TEST_LOCATION);

  // MoveOnlyType has user-defined destructor and deleted copy -> non-trivial
  DALI_TEST_EQUALS(static_cast<bool>(Dali::Vector<MoveOnlyType>::BaseType), false, TEST_LOCATION);

  // BaseHandle is a complex handle type -> non-trivial
  DALI_TEST_EQUALS(static_cast<bool>(Dali::Vector<Dali::BaseHandle>::BaseType), false, TEST_LOCATION);

  END_TEST;
}

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

  Dali::Vector<Dali::BaseHandle> baseHandleVector;

  TestReferenceCount(1, 1, 1, 1, 1, TEST_LOCATION);
  baseHandleVector.PushBack(handle0);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(1u), baseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 1, 1, 1, 1, TEST_LOCATION);
  baseHandleVector.PushBack(handle1);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(2u), baseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 2, 1, 1, 1, TEST_LOCATION);
  baseHandleVector.PushBack(handle2);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(3u), baseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 2, 2, 1, 1, TEST_LOCATION);
  baseHandleVector.PushBack(handle3);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(4u), baseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 2, 2, 2, 1, TEST_LOCATION);
  baseHandleVector.PushBack(handle4);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(5u), baseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 2, 2, 2, 2, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle0, handle1, handle2, handle3, handle4}, TEST_LOCATION);

  // Get BaseHandle
  Dali::BaseHandle copiedHandle2 = baseHandleVector[2];
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(5u), baseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 2, 3, 2, 2, TEST_LOCATION);
  copiedHandle2.Reset();
  TestReferenceCount(2, 2, 2, 2, 2, TEST_LOCATION);

  // Set BaseHandle
  baseHandleVector[2] = handle0;
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(5u), baseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(3, 2, 1, 2, 2, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle0, handle1, handle0, handle3, handle4}, TEST_LOCATION);
  baseHandleVector[0] = handle2;
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(5u), baseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 2, 2, 2, 2, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle2, handle1, handle0, handle3, handle4}, TEST_LOCATION);

  // Erase BaseHandle
  baseHandleVector.Erase(baseHandleVector.Begin());
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(4u), baseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 2, 1, 2, 2, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle1, handle0, handle3, handle4}, TEST_LOCATION);
  baseHandleVector.Erase(baseHandleVector.Begin() + 2);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(3u), baseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 2, 1, 1, 2, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle1, handle0, handle4}, TEST_LOCATION);

  // Insert BaseHandle
  baseHandleVector.Insert(baseHandleVector.Begin() + 1, handle2);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(4u), baseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 2, 2, 1, 2, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle1, handle2, handle0, handle4}, TEST_LOCATION);

  // Copy assign
  Dali::Vector<Dali::BaseHandle> copiedBaseHandleVector = baseHandleVector;
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(4u), baseHandleVector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(4u), copiedBaseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(3, 3, 3, 1, 3, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle1, handle2, handle0, handle4}, TEST_LOCATION);
  TestVectorVariable(copiedBaseHandleVector, {handle1, handle2, handle0, handle4}, TEST_LOCATION);

  baseHandleVector.PushBack(handle3);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(5u), baseHandleVector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(4u), copiedBaseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(3, 3, 3, 2, 3, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle1, handle2, handle0, handle4, handle3}, TEST_LOCATION);
  TestVectorVariable(copiedBaseHandleVector, {handle1, handle2, handle0, handle4}, TEST_LOCATION);

  // Insert range
  copiedBaseHandleVector.Insert(copiedBaseHandleVector.Begin() + 1, baseHandleVector.Begin(), baseHandleVector.End());
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(5u), baseHandleVector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(9u), copiedBaseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(4, 4, 4, 3, 4, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle1, handle2, handle0, handle4, handle3}, TEST_LOCATION);
  TestVectorVariable(copiedBaseHandleVector, {handle1, handle1, handle2, handle0, handle4, handle3, handle2, handle0, handle4}, TEST_LOCATION);

  // Move assign
  Dali::Vector<Dali::BaseHandle> movedBaseHandleVector = std::move(copiedBaseHandleVector);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(5u), baseHandleVector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(0u), copiedBaseHandleVector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(9u), movedBaseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(4, 4, 4, 3, 4, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle1, handle2, handle0, handle4, handle3}, TEST_LOCATION);
  TestVectorVariable(copiedBaseHandleVector, {}, TEST_LOCATION);
  TestVectorVariable(movedBaseHandleVector, {handle1, handle1, handle2, handle0, handle4, handle3, handle2, handle0, handle4}, TEST_LOCATION);

  // Erase range
  movedBaseHandleVector.Erase(movedBaseHandleVector.Begin() + 2, movedBaseHandleVector.End() - 1);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(5u), baseHandleVector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(3u), movedBaseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 4, 2, 2, 3, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle1, handle2, handle0, handle4, handle3}, TEST_LOCATION);
  TestVectorVariable(copiedBaseHandleVector, {}, TEST_LOCATION);
  TestVectorVariable(movedBaseHandleVector, {handle1, handle1, handle4}, TEST_LOCATION);

  baseHandleVector.Erase(baseHandleVector.End() - 1, baseHandleVector.End());
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(4u), baseHandleVector.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(3u), movedBaseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 4, 2, 1, 3, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle1, handle2, handle0, handle4}, TEST_LOCATION);
  TestVectorVariable(copiedBaseHandleVector, {}, TEST_LOCATION);
  TestVectorVariable(movedBaseHandleVector, {handle1, handle1, handle4}, TEST_LOCATION);

  // Clear
  movedBaseHandleVector.Clear();
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(0u), movedBaseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 2, 2, 1, 2, TEST_LOCATION);
  TestVectorVariable(baseHandleVector, {handle1, handle2, handle0, handle4}, TEST_LOCATION);
  TestVectorVariable(movedBaseHandleVector, {}, TEST_LOCATION);
  DALI_TEST_GREATER(movedBaseHandleVector.Capacity(), ZERO, TEST_LOCATION);

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

  // Remove
  movedBaseHandleVector.Remove(movedBaseHandleVector.Begin());
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(3u), movedBaseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(2, 1, 2, 1, 2, TEST_LOCATION);
  // Note : Below order depend on the remove behavior.
  TestVectorVariable(movedBaseHandleVector, {handle4, handle2, handle0}, TEST_LOCATION);

  movedBaseHandleVector.Remove(movedBaseHandleVector.End() - 1);
  DALI_TEST_EQUALS(static_cast<Dali::VectorBase::SizeType>(2u), movedBaseHandleVector.Count(), TEST_LOCATION);
  TestReferenceCount(1, 1, 2, 1, 2, TEST_LOCATION);
  // Note : Below order depend on the remove behavior.
  TestVectorVariable(movedBaseHandleVector, {handle4, handle2}, TEST_LOCATION);

  END_TEST;
}
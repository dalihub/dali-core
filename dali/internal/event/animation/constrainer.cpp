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

// CLASS HEADER
#include <dali/internal/event/animation/constrainer.h>

// INTERNAL INCLUDES
#include <dali/devel-api/common/free-list.h>
#include <dali/integration-api/constraint-integ.h>
#include <dali/internal/event/animation/constraint-source-impl.h>
#include <dali/public-api/animation/constraint.h>

namespace Dali
{
namespace Internal
{
namespace
{
Dali::FreeList& GetFreeList()
{
  static Dali::FreeList gConstrainerFreeList;
  return gConstrainerFreeList;
}

uint32_t AcquireConstrainerTag()
{
  uint32_t tag = GetFreeList().Add(0);
  DALI_ASSERT_ALWAYS(tag < Dali::ConstraintTagRanges::INTERNAL_TAG_MAX_COUNT_PER_DERIVATION && "To many constrainer applied!");
  return tag + Dali::ConstraintTagRanges::CORE_CONSTRAINT_TAG_START;
}

void ReleaseConstrainerTag(uint32_t tag)
{
  if(DALI_LIKELY(tag >= Dali::ConstraintTagRanges::CORE_CONSTRAINT_TAG_START))
  {
    GetFreeList().Remove(tag - Dali::ConstraintTagRanges::CORE_CONSTRAINT_TAG_START);
  }
}

/**
 * @brief Special tag number if we never apply the constraints before.
 */
constexpr uint32_t NOT_APPLIED_TAG_NUMBER = 0u;
static_assert(NOT_APPLIED_TAG_NUMBER < Dali::ConstraintTagRanges::CORE_CONSTRAINT_TAG_START);
static_assert(NOT_APPLIED_TAG_NUMBER < Dali::ConstraintTagRanges::INTERNAL_CONSTRAINT_TAG_START);
} // namespace

Constrainer::Constrainer()
: Object(nullptr), // we don't have our own scene object
  mObservedObjects(),
  mTag(NOT_APPLIED_TAG_NUMBER)
{
}

Constrainer::~Constrainer()
{
  //Remove all the constraints created by the object
  uint32_t         tag = mTag;
  const ObjectIter end = mObservedObjects.End();
  for(ObjectIter iter = mObservedObjects.Begin(); iter != end; ++iter)
  {
    //Remove Constrainer from the observers list of the object
    (*iter)->RemoveObserver(*this);

    //Remove constraints
    if(tag != NOT_APPLIED_TAG_NUMBER)
    {
      (*iter)->RemoveConstraints(tag);
    }
  }

  if(mTag != NOT_APPLIED_TAG_NUMBER)
  {
    ReleaseConstrainerTag(mTag);
  }
}

void Constrainer::ObjectDestroyed(Object& object)
{
  //Remove object from the list of observed
  const ObjectIter end = mObservedObjects.End();
  for(ObjectIter iter = mObservedObjects.Begin(); iter != end; ++iter)
  {
    if(*iter == &object)
    {
      mObservedObjects.Erase(iter);
      return;
    }
  }
}

void Constrainer::Remove(Dali::Handle& target)
{
  uint32_t         tag    = mTag;
  Object&          object = GetImplementation(target);
  const ObjectIter end    = mObservedObjects.End();
  for(ObjectIter iter = mObservedObjects.Begin(); iter != end; ++iter)
  {
    if(*iter == &object)
    {
      //Stop observing the object
      (*iter)->RemoveObserver(*this);

      //Remove constraints created in the object
      if(tag != NOT_APPLIED_TAG_NUMBER)
      {
        Dali::Integration::HandleRemoveConstraints(target, tag);
      }

      //Remove object from the vector of observed objects
      mObservedObjects.Erase(iter);
    }
  }
}

void Constrainer::Observe(Dali::Handle& handle)
{
  Object& object = GetImplementation(handle);

  //Add the object to the list of observed objects if it is not in it already
  const ObjectIter end  = mObservedObjects.End();
  ObjectIter       iter = mObservedObjects.Begin();
  for(; iter != end; ++iter)
  {
    if(*iter == &object)
    {
      break;
    }
  }

  if(iter == end)
  {
    //Start observing the object
    object.AddObserver(*this);

    //Add object in the observed objects vector
    mObservedObjects.PushBack(&object);
  }
}

uint32_t Constrainer::GetTag()
{
  if(mTag == NOT_APPLIED_TAG_NUMBER)
  {
    mTag = AcquireConstrainerTag();
  }
  return mTag;
}

} // namespace Internal

} // namespace Dali

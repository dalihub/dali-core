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

// CLASS HEADER
#include <dali/internal/event/common/base-object-impl.h>

namespace Dali
{
BaseObject::Impl::Impl(BaseObject& baseObject)
: mBaseObject(baseObject),
  mObservers(),
  mObserverNotifying(false)
{
}

BaseObject::Impl::~Impl()
{
  // Guard Add/Remove observer during observer notifying.
  mObserverNotifying = true;

  // Notification for observers
  for(auto&& item : mObservers)
  {
    item->ObjectDestroyed(mBaseObject);
  }

  // Note : We don't need to restore mObserverNotifying to false as we are in delete the object.
  // If someone call AddObserver / RemoveObserver after this, assert.

  // Remove all observers
  mObservers.Clear();
}

BaseObject::Impl& BaseObject::Impl::Get(BaseObject& baseObject)
{
  return *baseObject.mImpl;
}

const BaseObject::Impl& BaseObject::Impl::Get(const BaseObject& baseObject)
{
  return *baseObject.mImpl;
}

void BaseObject::Impl::AddObserver(Observer& observer)
{
  DALI_ASSERT_ALWAYS(!mObserverNotifying && "Cannot add observer while notifying BaseObject::Impl::Observers");

  // make sure an observer doesn't observe the same object twice
  // otherwise it will get multiple calls to ObjectDestroyed()
  DALI_ASSERT_DEBUG(mObservers.End() == std::find(mObservers.Begin(), mObservers.End(), &observer));

  mObservers.PushBack(&observer);
}

void BaseObject::Impl::RemoveObserver(Observer& observer)
{
  DALI_ASSERT_ALWAYS(!mObserverNotifying && "Cannot remove observer while notifying BaseObject::Impl::Observers");

  // Find the observer...
  const auto endIter = mObservers.End();
  for(auto iter = mObservers.Begin(); iter != endIter; ++iter)
  {
    if((*iter) == &observer)
    {
      mObservers.Erase(iter);
      break;
    }
  }
  DALI_ASSERT_DEBUG(endIter != mObservers.End());
}

} // namespace Dali

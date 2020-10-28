/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/signals/callback.h>

namespace Dali
{
// CallbackBase

CallbackBase::CallbackBase()
: mFunction(nullptr)
{
}

CallbackBase::~CallbackBase()
{
  Reset();
}

CallbackBase::CallbackBase(Function function)
: mFunction(function)
{
}

CallbackBase::CallbackBase(void* object, MemberFunction function, Dispatcher dispatcher)
: mMemberFunction(function)
{
    mImpl.mObjectPointer            = object;
    mImpl.mMemberFunctionDispatcher = dispatcher;
    mImpl.mDestructorDispatcher     = nullptr; // object is not owned
}

CallbackBase::CallbackBase(void* object, MemberFunction function, Dispatcher dispatcher, Destructor destructor)
: mMemberFunction(function)
{
    mImpl.mObjectPointer            = object;
    mImpl.mMemberFunctionDispatcher = dispatcher;
    mImpl.mDestructorDispatcher     = destructor; // object is owned
}

void CallbackBase::Reset()
{
  // if destructor function is set it means we own this object
  if(mImpl.mObjectPointer &&
     mImpl.mDestructorDispatcher)
  {
    // call the destructor dispatcher
    (*mImpl.mDestructorDispatcher)(mImpl.mObjectPointer);
  }

  mImpl = {};

  mFunction = nullptr;
}

} // namespace Dali

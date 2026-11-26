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

// CLASS HEADER
#include <dali/public-api/signals/callback.h>

namespace DALI_NAMESPACE
{
// The layout of CallbackBase is compiled into applications (see the WARNING in callback.h).
// The Windows build compiles with /vmg, so under the Microsoft ABI the pointer-to-member in the
// union takes its most general form (24 bytes on x64, 16 bytes with 8-byte alignment on x86).
#if defined(_MSC_VER)
static_assert(sizeof(void*) != 8 || sizeof(CallbackBase) == 56, "CallbackBase layout is ABI-frozen");
static_assert(sizeof(void*) != 4 || sizeof(CallbackBase) == 40, "CallbackBase layout is ABI-frozen");
#else
static_assert(sizeof(void*) != 8 || sizeof(CallbackBase) == 48, "CallbackBase layout is ABI-frozen");
static_assert(sizeof(void*) != 4 || sizeof(CallbackBase) == 24, "CallbackBase layout is ABI-frozen");
#endif

// CallbackBase

CallbackBase::CallbackBase()
: mStaticFunction(nullptr)
{
}

CallbackBase::~CallbackBase()
{
  Reset();
}

CallbackBase::CallbackBase(StaticFunction function)
: mStaticFunction(function)
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

  mStaticFunction = nullptr;
}

} //namespace DALI_NAMESPACE

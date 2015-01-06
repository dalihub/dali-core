/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/signals/functor-delegate.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/compile-time-assert.h>

namespace Dali
{
namespace
{
/**
 * our implementation currently relies on C function pointer to be the size of void*
 * in FunctorDispatcher we pass the C function as void* as the code is common between member
 * functions and regular functions.
 * If this assert fails, please implement the template specialisation for C functions.
 */
#if !defined(EMSCRIPTEN)
void Function() __attribute__((unused));
void Function() { }
DALI_COMPILE_TIME_ASSERT( sizeof(void*) == sizeof(&Function) );
#endif
}

FunctorDelegate::~FunctorDelegate()
{
  if( mFunctorPointer )
  {
    (*mDestructorDispatcher)( mFunctorPointer );
  }
}

void FunctorDelegate::Execute()
{
  if( mFunctorPointer )
  {
    Dispatcher dispatcher = mMemberFunctionDispatcher;
    (*dispatcher)( mFunctorPointer );
  }
}

FunctorDelegate::FunctorDelegate( void* objectPtr, Dispatcher dispatcher, Destructor destructor )
: mFunctorPointer( objectPtr ),
  mMemberFunctionDispatcher( dispatcher ),
  mDestructorDispatcher( destructor )
{
}

} // namespace Dali

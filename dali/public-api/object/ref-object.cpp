/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/ref-object.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

// EXTERNAL INCLUDES
#include <stdint.h>

namespace Dali
{

RefObject::RefObject()
: mCount(0)
{
}

RefObject::RefObject(const RefObject&)
: mCount(0) // Do not copy the reference count
{
}

RefObject::~RefObject()
{
#ifdef ENABLE_DEBUG
  if(mCount)
  {
    DALI_LOG_ERROR("mCount should be zero, deleting referenced object!");
  }
#endif // ENABLE_DEBUG
}

RefObject& RefObject::operator=(const RefObject&)
{
  // Do not copy the reference count
  return *this;
}

void RefObject::Reference()
{
  // The inline assembly below was tested on an ARMv8 64 bit platform on
  // 2015-02-06 and found to run in 11.8 nanoseconds, whereas
  // __sync_add_and_fetch( address, 1 ) required 18.8 nanoseconds.
  // Including the assembly here produced one fewer assembly instruction than if
  // it was wrapped in a function and inlined here by the compiler.
#if defined __aarch64__

  asm volatile(
  "1:\tldxr  w1, %[address] \n\t"
      "add   w1, w1, %[one] \n\t"
      "stxr  w2,  w1, %[address] \n\t"
      "cbnz  w2,  1b \n\t"
      // Outputs:
    : // Q = A memory address with no offset
      // ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
      [address] "+Q" (mCount)
      // Inputs:
    : [one] "Ir" (1)
      // Clobbers: (explicitly clobber w1 register to hold the loaded value and
      // register w2 to hold success/fail):
    : "w1", "w2"
  );

  // 32 bit ARMv7 version of above:
  // Better than the code emitted by GCC for __sync_add_and_fetch(), as that
  // includes two dmb memory barrier instructions: one before and one after the
  // loop.
#elif defined __arm__

  asm volatile(
    "1:\tldrex r1, %[address] \n\t"
        "add   r1, r1, %[one] \n\t"
        "strex r2,  r1, %[address] \n\t"
        "teq   r2,  %[zero] \n\t"
        "bne   1b \n\t"
        // Outputs:
      : [address] "+Q" (mCount)
        // Inputs:
      : [zero] "Ir" (0),
        [one]  "Ir" (1)
        // Clobbers: (modified registers):
      : "r1", "r2"
    );

#else

  // gcc > 4.1 builtin atomic add and fetch:
  __sync_add_and_fetch( &mCount, 1 );

#endif
}

void RefObject::Unreference()
{
  // The output register:
  int32_t newValue;

#if defined __aarch64__

  asm volatile(
  "1:\tldxr   %w[newValue], %[address] \n\t"
      "sub   %w[newValue], %w[newValue], %[one] \n\t"
      "stxr  w2,  %w[newValue], %[address] \n\t"
      "cbnz  w2,  1b \n\t"
    // Outputs:
  : [newValue] "=&r" (newValue),
    // Q = A memory address with no offset
    // ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
    [address] "+Q" (mCount)
    // Inputs:
  : [one]  "Ir" (1)
    // Clobbered: I.e., stuff that is modified.
  : "w2"
  );

#elif defined __arm__

  asm volatile(
  "1:\tldrex %[newValue], %[address] \n\t"
      "sub   %[newValue], %[newValue], %[one] \n\t"
      "strex r2,  %[newValue], %[address] \n\t"
      "teq   r2, %[zero] \n\t"
      "bne   1b \n\t"
    // Outputs:
  : [newValue] "=&r" (newValue),
    [address] "+Q" (mCount)
    // Inputs:
    : [zero] "Ir" (0),
      [one]  "Ir" (1)
    // Clobbered:
  : "r2"
  );

#else

  // gcc > 4.1 builtin atomic subtract and fetch (--mCount; return mCount)
  newValue = __sync_sub_and_fetch( &mCount, 1 );

#endif

  if( newValue == 0 )
  {
    delete this;
  }
}

int RefObject::ReferenceCount()
{
  return mCount;
}

} // namespace Dali



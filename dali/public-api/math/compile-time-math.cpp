//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali/public-api/math/compile-time-math.h>

// INTERNAL INCLUDES
#include <dali/public-api/math/compile-time-assert.h>

namespace Dali
{

namespace
{
  // verification for static asserts, these get removed from final binary by the linker
  const unsigned int value0 = Power<10,0>::value;
  DALI_COMPILE_TIME_ASSERT( value0 == 1 ); // cannot have template with , inside macro... macros suck
  const unsigned int value1 = Power<10,1>::value;
  DALI_COMPILE_TIME_ASSERT( value1 == 10 );
  const unsigned int value2 = Power<10,2>::value;
  DALI_COMPILE_TIME_ASSERT( value2 == 100 );
  const unsigned int value3 = Power<10,3>::value;
  DALI_COMPILE_TIME_ASSERT( value3 == 1000 );
  const unsigned int value4 = Power<4,4>::value;
  DALI_COMPILE_TIME_ASSERT( value4 == 256 );

  const unsigned int log0 = Log<0, 10>::value;
  DALI_COMPILE_TIME_ASSERT( log0 == 0 );
  const unsigned int log1 = Log<1, 10>::value;
  DALI_COMPILE_TIME_ASSERT( log1 == 0 );
  const unsigned int log2 = Log<2, 10>::value;
  DALI_COMPILE_TIME_ASSERT( log2 == 1 );
  const unsigned int log3 = Log<10, 10>::value;
  DALI_COMPILE_TIME_ASSERT( log3 == 1 );
  const unsigned int log4 = Log<100, 10>::value;
  DALI_COMPILE_TIME_ASSERT( log4 == 2 );
  const unsigned int log5 = Log<1000, 10>::value;
  DALI_COMPILE_TIME_ASSERT( log5 == 3 );

  const unsigned int logpow0 = Log<Power<10,0>::value, 10 >::value;
  DALI_COMPILE_TIME_ASSERT( logpow0 == 0 );
  const unsigned int logpow1 = Log<Power<2,0>::value, 2 >::value;
  DALI_COMPILE_TIME_ASSERT( logpow1 == 0 );
  const unsigned int logpow2 = Log<Power<10,2>::value, 10 >::value;
  DALI_COMPILE_TIME_ASSERT( logpow2 == 2 );
  const unsigned int logpow3 = Log<Power<2,2>::value, 2 >::value;
  DALI_COMPILE_TIME_ASSERT( logpow3 == 2 );

  const unsigned int powlog0 = Power<10, Log<10,10>::value >::value;
  DALI_COMPILE_TIME_ASSERT( powlog0 == 10 );
  const unsigned int powlog1 = Power<10, Log<100,10>::value >::value;
  DALI_COMPILE_TIME_ASSERT( powlog1 == 100 );

  // TODO unfortunately cannot static assert floats so cannot test EPSILON here...

} // namespace

} // namespace Dali

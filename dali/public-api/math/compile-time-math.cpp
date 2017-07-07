/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/math/compile-time-math.h>

// EXTERNAL INCLUDES
#include <type_traits>

namespace Dali
{

// verification for static asserts, these get removed from final binary by the linker
static_assert( Power<10,0>::value == 1, "" );
static_assert( Power<10,1>::value == 10, "" );
static_assert( Power<10,2>::value == 100, "" );
static_assert( Power<10,3>::value == 1000, "" );
static_assert( Power<4,4>::value == 256, "" );

static_assert( Log<0, 10>::value == 0, "" );
static_assert( Log<1, 10>::value == 0, "" );
static_assert( Log<2, 10>::value == 1, "" );
static_assert( Log<10, 10>::value == 1, "" );
static_assert( Log<100, 10>::value == 2, "" );
static_assert( Log<1000, 10>::value == 3, "" );

static_assert( Log<Power<10,0>::value, 10 >::value == 0, "" );
static_assert( Log<Power<2,0>::value, 2 >::value == 0, "" );
static_assert( Log<Power<10,2>::value, 10 >::value == 2, "" );
static_assert( Log<Power<2,2>::value, 2 >::value == 2, "" );

static_assert( Power<10, Log<10,10>::value >::value == 10, "" );
static_assert( Power<10, Log<100,10>::value >::value == 100, "" );

// TODO unfortunately cannot static assert floats so cannot test EPSILON here...

} // namespace Dali

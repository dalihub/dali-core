#ifndef __DALI_COMPILE_TIME_ASSERT_H__
#define __DALI_COMPILE_TIME_ASSERT_H__

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

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

// EXTERNAL INCLUDES
#include <cstddef>

namespace Dali DALI_IMPORT_API
{

template <bool x> struct CompileTimeAssertBool;
template <> struct CompileTimeAssertBool<true> {};

template<int x> struct CompileTimeAssertInt {};

/**
 * Use DALI_COMPILE_TIME_ASSERT to test expressions at compile time.
 * If x is false, then 'sizeof' will be attempted with incomplete type.
 */
#define DALI_COMPILE_TIME_ASSERT( x ) typedef CompileTimeAssertInt< sizeof( CompileTimeAssertBool< ( x ) > ) > CompileTimeAssertType

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_COMPILE_TIME_ASSERT_H__

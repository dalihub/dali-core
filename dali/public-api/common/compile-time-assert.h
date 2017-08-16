#ifndef __DALI_COMPILE_TIME_ASSERT_H__
#define __DALI_COMPILE_TIME_ASSERT_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

// EXTERNAL INCLUDES
#include <cstddef>

namespace Dali
{
/**
 * @addtogroup dali_core_common
 * @{
 */

/**
 * @brief Use DALI_COMPILE_TIME_ASSERT to test expressions at compile time.
 *
 * @SINCE_1_0.0
 */
#define DALI_COMPILE_TIME_ASSERT( x ) static_assert( x, #x );

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_COMPILE_TIME_ASSERT_H__

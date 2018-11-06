#ifndef __DALI_VIEWPORT_H__
#define __DALI_VIEWPORT_H__

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <cstdint> // int32_t

// INTERNAL INCLUDES
#include <dali/public-api/math/rect.h>

namespace Dali
{
/**
 * @addtogroup dali_core_math
 * @{
 */

/**
 * @brief Typedef for a viewport ( a rectangle representing a screen area ).
 * @SINCE_1_0.0
 */
typedef Rect<int32_t> Viewport;

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_VIEWPORT_H__

#ifndef DALI_UTILITY_H
#define DALI_UTILITY_H

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

// EXTERNAL INCLUDES
#include <utility> ///< for std::move()

// INTERNAL INCLUDES
#include <dali/public-api/common/type-traits.h>

namespace Dali
{
/**
 * @addtogroup dali_core_common
 * @{
 */

/**
 * @brief Get the smaller value.
 *
 * @SINCE_2_5.8
 * @param[in] lhs The first value.
 * @param[in] rhs The second value.
 * @return T the smaller value.
 */
template<typename T>
constexpr const T& Min(const T& lhs, const T& rhs)
{
  return lhs < rhs ? lhs : rhs;
}

/**
 * @brief Get the bigger value.
 *
 * @SINCE_2_5.8
 * @param[in] lhs The first value.
 * @param[in] rhs The second value.
 * @return T the bigger value.
 */
template<typename T>
constexpr const T& Max(const T& lhs, const T& rhs)
{
  return lhs > rhs ? lhs : rhs;
}

/**
 * @brief Swap the value between left-hand-side and right-hand-side.
 *        It need move constructor and move assign.
 *
 * @SINCE_2_5.10
 * @param[in] lhs The first value.
 * @param[in] rhs The second value.
 */
template<typename T>
constexpr void Swap(T& lhs, T& rhs)
{
  T temperary = std::move(lhs);
  lhs         = std::move(rhs);
  rhs         = std::move(temperary);
}

/**
 * @}
 */
} // namespace Dali

#endif // DALI_UTILITY_H

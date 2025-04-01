#ifndef DALI_CONSTRAINT_TAG_RANGES_H
#define DALI_CONSTRAINT_TAG_RANGES_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

namespace Dali
{
/**
 * @addtogroup dali_core_animation
 * @{
 */

/**
 * @brief Enumeration for the constraint tag.
 *
 * Enumerations are being used here rather than static constants so that switch statements can be
 * used to compare property indices.
 * @SINCE_2_4.14
 */
// clang-format off
enum ConstraintTagRanges
{
  DEFAULT_TAG = 0, ///< Special tag that we don't set any tags @SINCE_2_4.14

  CUSTOM_CONSTRAINT_TAG_START = 1,         ///< Minimum tag for application @SINCE_2_4.14
  CUSTOM_CONSTRAINT_TAG_MAX   = 999999999, ///< Maximum tag for application @SINCE_2_4.14

  INTERNAL_CONSTRAINT_TAG_START = CUSTOM_CONSTRAINT_TAG_MAX + 1, ///< Minimum tag for internal @SINCE_2_4.14

  INTERNAL_TAG_MAX_COUNT_PER_DERIVATION = 1000000, ///< The range of tag number for each sub-repository could be used @SINCE_2_4.14

  CORE_CONSTRAINT_TAG_START = INTERNAL_CONSTRAINT_TAG_START,                                          ///< Minimum tag that Core can uses up to @SINCE_2_4.14
  CORE_CONSTRAINT_TAG_MAX   = CORE_CONSTRAINT_TAG_START + INTERNAL_TAG_MAX_COUNT_PER_DERIVATION - 1u, ///< Maximum tag that Core can uses up to @SINCE_2_4.14

  INTERNAL_CONSTRAINT_TAG_MAX = INTERNAL_CONSTRAINT_TAG_START + 999999999,  ///< Maximum tag for internal @SINCE_2_4.14
};
// clang-format on

/**
 * @}
 */
} // namespace Dali

#endif // DALI_CONSTRAINT_TAG_RANGES_H

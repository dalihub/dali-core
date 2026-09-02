#ifndef DALI_INSETS_H
#define DALI_INSETS_H

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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
/**
 * @addtogroup dali_core_common
 * @{
 */

/**
 * @brief Structure describing four logical edge insets with floating-point precision.
 *
 * Values are ordered as start, end, top, and bottom.
 * @SINCE_2_5.38
 */
struct DALI_CORE_API Insets
{
  /**
   * @brief Default constructor which provides an initialized Dali::Insets( 0.f, 0.f, 0.f, 0.f ).
   * @SINCE_2_5.38
   */
  Insets();

  /**
   * @brief Copy constructor.
   * @SINCE_2_5.38
   * @param[in] copy A reference to the copied Insets
   */
  Insets(const Insets& copy) = default;

  /**
   * @brief Move constructor.
   * @SINCE_2_5.38
   * @param[in] move A reference to the moved Insets
   */
  Insets(Insets&& move) noexcept = default;

  /**
   * @brief Constructor.
   *
   * @SINCE_2_5.38
   * @param[in] start  Start inset
   * @param[in] end    End inset
   * @param[in] top    Top inset
   * @param[in] bottom Bottom inset
   */
  Insets(float start, float end, float top, float bottom);

  /**
   * @brief Creates insets with symmetric horizontal and vertical values.
   *
   * @SINCE_2_5.38
   * @param[in] horizontal The value assigned to the start and end edges
   * @param[in] vertical   The value assigned to the top and bottom edges
   */
  Insets(float horizontal, float vertical);

  /**
   * @brief Copy Assignment operator.
   * @SINCE_2_5.38
   * @param[in] copy A reference to the copied Insets
   * @return Itself
   */
  Insets& operator=(const Insets& copy) = default;

  /**
   * @brief Move Assignment operator.
   * @SINCE_2_5.38
   * @param[in] move A reference to the moved Insets
   * @return Itself
   */
  Insets& operator=(Insets&& move) noexcept = default;

  /**
   * @brief Assignment operator.
   *
   * @SINCE_2_5.38
   * @param[in] array Array of float
   * @return Itself
   */
  Insets& operator=(const float* array);

  /**
   * @brief Equality operator.
   *
   * @SINCE_2_5.38
   * @param[in] rhs The Insets to test against
   * @return True if the insets are equal
   */
  bool operator==(const Insets& rhs) const;

  /**
   * @brief Inequality operator.
   *
   * @SINCE_2_5.38
   * @param[in] rhs The Insets to test against
   * @return True if the insets are not equal
   */
  bool operator!=(const Insets& rhs) const;

public:
  float start;  ///< The start inset.  @SINCE_2_5.38
  float end;    ///< The end inset.    @SINCE_2_5.38
  float top;    ///< The top inset.    @SINCE_2_5.38
  float bottom; ///< The bottom inset. @SINCE_2_5.38
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_INSETS_H

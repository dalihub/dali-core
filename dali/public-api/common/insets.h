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
#include <dali/public-api/math/math-utils.h>

namespace DALI_NAMESPACE
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
  constexpr Insets()
  : start(0.0f),
    end(0.0f),
    top(0.0f),
    bottom(0.0f)
  {
  }

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
   * @param[in] initStart  Start inset
   * @param[in] initEnd    End inset
   * @param[in] initTop    Top inset
   * @param[in] initBottom Bottom inset
   */
  constexpr Insets(float initStart, float initEnd, float initTop, float initBottom)
  : start(initStart),
    end(initEnd),
    top(initTop),
    bottom(initBottom)
  {
  }

  /**
   * @brief Creates insets with symmetric horizontal and vertical values.
   *
   * @SINCE_2_5.38
   * @param[in] horizontal The value assigned to the start and end edges
   * @param[in] vertical   The value assigned to the top and bottom edges
   */
  constexpr Insets(float horizontal, float vertical)
  : start(horizontal),
    end(horizontal),
    top(vertical),
    bottom(vertical)
  {
  }

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
  constexpr Insets& operator=(const float* array)
  {
    start  = array[0];
    end    = array[1];
    top    = array[2];
    bottom = array[3];

    return *this;
  }

  /**
   * @brief Equality operator.
   *
   * @SINCE_2_5.38
   * @param[in] rhs The Insets to test against
   * @return True if the insets are equal
   */
  constexpr bool operator==(const Insets& rhs) const
  {
    return (Internal::MathUtils::Abs(start - rhs.start) <= GetRangedEpsilon(start, rhs.start)) &&
           (Internal::MathUtils::Abs(end - rhs.end) <= GetRangedEpsilon(end, rhs.end)) &&
           (Internal::MathUtils::Abs(top - rhs.top) <= GetRangedEpsilon(top, rhs.top)) &&
           (Internal::MathUtils::Abs(bottom - rhs.bottom) <= GetRangedEpsilon(bottom, rhs.bottom));
  }

  /**
   * @brief Inequality operator.
   *
   * @SINCE_2_5.38
   * @param[in] rhs The Insets to test against
   * @return True if the insets are not equal
   */
  constexpr bool operator!=(const Insets& rhs) const
  {
    return !(*this == rhs);
  }

public:
  float start;  ///< The start inset.  @SINCE_2_5.38
  float end;    ///< The end inset.    @SINCE_2_5.38
  float top;    ///< The top inset.    @SINCE_2_5.38
  float bottom; ///< The bottom inset. @SINCE_2_5.38
};

/**
 * @}
 */
} //namespace DALI_NAMESPACE

#endif // DALI_INSETS_H

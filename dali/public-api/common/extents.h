#ifndef DALI_EXTENTS_H
#define DALI_EXTENTS_H

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

// EXTERNAL INCLUDES
#include <cstdint>
#include <ostream>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
/**
 * @addtogroup dali_core_common
 * @{
 */

/**
 * @brief Structure describing the a collection of uint16_t.
 * @SINCE_1_2.62
 */
struct DALI_EXPORT_API Extents
{
  /**
   * @brief Default constructor which provides an initialized Dali::Extents( 0u, 0u, 0u, 0u ).
   * @SINCE_1_2.62
   */
  Extents();

  /**
   * @brief Copy constructor.
   * @SINCE_1_2.62
   * @param[in] copy A reference to the copied Extents
   */
  Extents( const Extents& copy ) = default;

  /**
   * @brief Constructor.
   *
   * @SINCE_1_2.62
   * @param[in] start     Start extent
   * @param[in] end       End extent
   * @param[in] top       Top extent
   * @param[in] bottom    Bottom extent
   */
  Extents( uint16_t start, uint16_t end, uint16_t top, uint16_t bottom );

  /**
   * @brief Copy Assignment operator.
   * @SINCE_1_2.62
   * @param[in] copy A reference to the copied Extents
   * @return Itself
   */
  Extents& operator=( const Extents& copy ) = default;

  /**
   * @brief Assignment operator.
   *
   * @SINCE_1_2.62
   * @param[in] array Array of uint16_t
   * @return Itself
   */
  Extents& operator=( const uint16_t* array );

  /**
   * @brief Equality operator.
   *
   * @SINCE_1_2.62
   * @param[in] rhs The Extents to test against
   * @return True if the extents are equal
   */
  bool operator==( const Extents &rhs ) const;

  /**
   * @brief Inequality operator.
   *
   * @SINCE_1_2.62
   * @param[in] rhs The Extents to test against
   * @return True if the extents are not equal
   */
  bool operator!=( const Extents &rhs ) const;

public:
  uint16_t start;   ///< The start extent.  @SINCE_1_2.62
  uint16_t end;     ///< The end extent.    @SINCE_1_2.62
  uint16_t top;     ///< The top extent.    @SINCE_1_2.62
  uint16_t bottom;  ///< The bottom extent. @SINCE_1_2.62

};

/**
 * @brief Converts the value of the extents into a string and insert in to an output stream.
 *
 * @SINCE_1_2.62
 * @param[in] stream The output stream operator
 * @param[in] extents The Extents to output
 * @return The output stream operator
 */
DALI_EXPORT_API std::ostream& operator<<( std::ostream& stream, const Extents& extents );

/**
 * @}
 */
} // namespace Dali

#endif // DALI_EXTENTS_H

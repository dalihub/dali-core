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
 */

#ifndef DALI_PAIR_H
#define DALI_PAIR_H

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-utility.h>
#include <dali/public-api/common/type-traits.h>

namespace Dali
{
/**
 * @addtogroup dali_core_common
 * @{
 */

/**
 * @brief A pair of values
 *
 * This template class provides a simple container for two heterogeneous objects.
 *
 * @tparam T1 The type of the first element
 * @tparam T2 The type of the second element
 * @SINCE_2_5.16
 */
template<typename T1, typename T2>
class Pair
{
public:
  /**
   * @brief Default constructor.
   * @note T1 and T2 must be default constructible.
   * @SINCE_2_5.16
   */
  Pair()
  : first{},
    second{}
  {
  }

  /**
   * @brief Constructor from two values.
   * @SINCE_2_5.16
   * @param[in] t1 The first value
   * @param[in] t2 The second value
   */
  Pair(const T1& t1, const T2& t2)
  : first(t1),
    second(t2)
  {
  }

  /**
   * @brief Move constructor from two values.
   * @SINCE_2_5.16
   * @param[in] t1 The first value
   * @param[in] t2 The second value
   */
  Pair(T1&& t1, T2&& t2)
  : first(static_cast<T1&&>(t1)),
    second(static_cast<T2&&>(t2))
  {
  }

  /**
   * @brief Copy constructor.
   * @SINCE_2_5.16
   * @param[in] other The pair to copy from
   */
  Pair(const Pair& other) = default;

  /**
   * @brief Move constructor.
   * @SINCE_2_5.16
   * @param[in] other The pair to move from
   */
  Pair(Pair&& other) noexcept = default;

  /**
   * @brief Converting constructor from another pair type.
   *
   * Constructs this pair from another pair type where U1 is convertible to T1 and U2 is convertible to T2.
   * This enables implicit type conversions.
   *
   * @tparam U1 The source first element type
   * @tparam U2 The source second element type
   * @param[in] other The pair to copy from
   * @SINCE_2_5.16
   */
  template<typename U1, typename U2>
  Pair(const Pair<U1, U2>& other)
  : first(other.first),
    second(other.second)
  {
  }

  /**
   * @brief Converting move constructor from another pair type.
   *
   * Constructs this pair by moving from another pair type where U1 is convertible to T1 and U2 is convertible to T2.
   *
   * @tparam U1 The source first element type
   * @tparam U2 The source second element type
   * @param[in] other The pair to move from
   * @SINCE_2_5.16
   */
  template<typename U1, typename U2>
  Pair(Pair<U1, U2>&& other)
  : first(static_cast<U1&&>(other.first)),
    second(static_cast<U2&&>(other.second))
  {
  }

  /**
   * @brief Copy assignment operator.
   * @SINCE_2_5.16
   * @param[in] other The pair to copy from
   * @return Reference to this pair
   */
  Pair& operator=(const Pair& other) = default;

  /**
   * @brief Move assignment operator.
   * @SINCE_2_5.16
   * @param[in] other The pair to move from
   * @return Reference to this pair
   */
  Pair& operator=(Pair&& other) noexcept = default;

  /**
   * @brief Converting copy assignment operator.
   *
   * Assigns this pair from another pair type where U1 is convertible to T1 and U2 is convertible to T2.
   *
   * @tparam U1 The source first element type
   * @tparam U2 The source second element type
   * @param[in] other The pair to copy from
   * @return Reference to this pair
   * @SINCE_2_5.16
   */
  template<typename U1, typename U2>
  Pair& operator=(const Pair<U1, U2>& other)
  {
    first  = other.first;
    second = other.second;
    return *this;
  }

  /**
   * @brief Converting move assignment operator.
   *
   * Move-assigns this pair from another pair type where U1 is convertible to T1 and U2 is convertible to T2.
   *
   * @tparam U1 The source first element type
   * @tparam U2 The source second element type
   * @param[in] other The pair to move from
   * @return Reference to this pair
   * @SINCE_2_5.16
   */
  template<typename U1, typename U2>
  Pair& operator=(Pair<U1, U2>&& other)
  {
    first  = static_cast<U1&&>(other.first);
    second = static_cast<U2&&>(other.second);
    return *this;
  }

  /**
   * @brief Equality comparison operator.
   * @SINCE_2_5.16
   * @param[in] other The pair to compare with
   * @return true if both first and second elements are equal
   */
  bool operator==(const Pair& other) const
  {
    return (first == other.first) && (second == other.second);
  }

  /**
   * @brief Inequality comparison operator.
   * @SINCE_2_5.16
   * @param[in] other The pair to compare with
   * @return true if first or second elements differ
   */
  bool operator!=(const Pair& other) const
  {
    return !(*this == other);
  }

  /**
   * @brief Less than comparison operator (lexicographical comparison).
   * @SINCE_2_5.16
   * @param[in] other The pair to compare with
   * @return true if this pair is lexicographically less than other
   */
  bool operator<(const Pair& other) const
  {
    if(first < other.first)
    {
      return true;
    }
    if(other.first < first)
    {
      return false;
    }
    return second < other.second;
  }

  /**
   * @brief Less than or equal comparison operator.
   * @SINCE_2_5.16
   * @param[in] other The pair to compare with
   * @return true if this pair is less than or equal to other
   */
  bool operator<=(const Pair& other) const
  {
    return !(other < *this);
  }

  /**
   * @brief Greater than comparison operator.
   * @SINCE_2_5.16
   * @param[in] other The pair to compare with
   * @return true if this pair is greater than other
   */
  bool operator>(const Pair& other) const
  {
    return other < *this;
  }

  /**
   * @brief Greater than or equal comparison operator.
   * @SINCE_2_5.16
   * @param[in] other The pair to compare with
   * @return true if this pair is greater than or equal to other
   */
  bool operator>=(const Pair& other) const
  {
    return !(*this < other);
  }

  /**
   * @brief Swap the contents with another pair.
   * @SINCE_2_5.16
   * @param[in] other The pair to swap with
   */
  void Swap(Pair& other)
  {
    Dali::Swap(first, other.first);
    Dali::Swap(second, other.second);
  }

public:
  using FirstType  = T1;
  using SecondType = T2;

  T1 first;  ///< The first element
  T2 second; ///< The second element
};

/**
 * @brief TypeTraits specialization for Dali::Pair.
 *
 * IS_TRIVIAL_TYPE is true only if both TypeTraits<T1>::IS_TRIVIAL_TYPE
 * and TypeTraits<T2>::IS_TRIVIAL_TYPE are true.
 *
 * @SINCE_2_5.16
 */
template<typename T1, typename T2>
struct TypeTraits<Pair<T1, T2>> : public BasicTypes<Pair<T1, T2>>
{
  enum
  {
    IS_TRIVIAL_TYPE = TypeTraits<T1>::IS_TRIVIAL_TYPE && TypeTraits<T2>::IS_TRIVIAL_TYPE
  };
};

/**
 * @brief Creates a Pair from two values.
 *
 * Constructs a Pair with decayed types (removes references, const, volatile)
 * while preserving the value category of each argument via perfect forwarding.
 * Rvalue arguments are moved into the Pair; lvalue arguments are copied.
 *
 * @tparam T1 The type of the first value
 * @tparam T2 The type of the second value
 * @param[in] t1 The first value
 * @param[in] t2 The second value
 * @return A Pair containing the two values with decayed types
 * @SINCE_2_5.16
 */
template<typename T1, typename T2>
inline Pair<typename DecayType<T1>::type, typename DecayType<T2>::type>
MakePair(T1&& t1, T2&& t2)
{
  return Pair<typename DecayType<T1>::type, typename DecayType<T2>::type>(Dali::Forward<T1>(t1), Dali::Forward<T2>(t2));
}

/**
 * @brief Swap function for Pair (enables ADL lookup).
 * @SINCE_2_5.16
 * @param[in] lhs First pair to swap
 * @param[in] rhs Second pair to swap
 */
template<typename T1, typename T2>
inline void Swap(Pair<T1, T2>& lhs, Pair<T1, T2>& rhs)
{
  lhs.Swap(rhs);
}

/**
 * @}
 */

} // namespace Dali

#endif // DALI_PAIR_H

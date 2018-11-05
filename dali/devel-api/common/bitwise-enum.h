#ifndef DALI_BITWISE_ENUM_H
#define DALI_BITWISE_ENUM_H

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
 */

#include <type_traits> // std::enable_if, std::underlying_type

namespace Dali
{

/**
 * Type traits and methods to enable type safe bit field operators for an enum.
 * usage:
 *   template<> struct EnableBitMaskOperators< MyEnumType > { static const bool ENABLE = true; };
 * after this one can set bitfields with | and |=, like
 *   MyEnumType value = FLAG1 | FLAG2;
 * and test them with &, like:
 *   if( myFlag & FLAG2 )
 *    // do something
 */
template< typename Enum >
struct EnableBitMaskOperators
{
  static const bool ENABLE = false; // can't be constexpr as it's a data member
};

/**
 * Combine two bitfields
 * @param lhs bitfield to or
 * @param rhs bitfield to or
 * @return EnumType with both flags set
 */
template < typename EnumType >
inline typename std::enable_if< EnableBitMaskOperators< EnumType >::ENABLE, EnumType >::type operator|( EnumType lhs, EnumType rhs )
{
  using UnderlyingType = typename std::underlying_type<EnumType>::type;
  return static_cast<EnumType>( static_cast<UnderlyingType>( lhs ) | static_cast<UnderlyingType>(rhs ) );
}

/**
 * Combine two bitfields
 * @param lhs bitfield to or
 * @param rhs bitfield to or
 * @return reference to lhs with both flags set
 */
template < typename EnumType >
inline typename std::enable_if< EnableBitMaskOperators< EnumType >::ENABLE, EnumType& >::type operator|=( EnumType& lhs, EnumType rhs )
{
  using UnderlyingType = typename std::underlying_type<EnumType>::type;
  lhs = static_cast<EnumType>(static_cast<UnderlyingType>( lhs ) | static_cast<UnderlyingType>( rhs ) );
  return lhs;
}

/**
 * Test two bitfields.
 * @param lhs bitfield to AND
 * @param rhs bitfield to AND
 * @return true if at least one flag is same in both
 */
template < typename EnumType >
inline typename std::enable_if< EnableBitMaskOperators< EnumType >::ENABLE, bool >::type operator&( EnumType lhs, EnumType rhs )
{
  using UnderlyingType = typename std::underlying_type<EnumType>::type;
  return static_cast<bool>( static_cast<UnderlyingType>( lhs ) & static_cast<UnderlyingType>(rhs ) );
}

} // namespace Dali

#endif // DALI_BITWISE_ENUM_H

#ifndef __DALI_TYPE_TRAITS_H__
#define __DALI_TYPE_TRAITS_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

namespace Dali
{
/**
 * @addtogroup dali_core_common
 * @{
 */

/**
 * @brief Basic type traits that every type has by default.
 *
 * This allows specializations to not have to repeat all flags
 * @SINCE_1_0.0
 */
template <typename Type>
struct BasicTypes
{
  /**
   * @brief This flag tells Dali if a class can be considered POD.
   *
   * If it declares copy constructor and/or destructor, its not considered trivial
   * and cannot be copied by using memcpy etc.
   * @SINCE_1_0.0
   */
  enum { IS_TRIVIAL_TYPE = __has_trivial_destructor(Type) && __has_trivial_copy(Type) };
};

/**
 * @brief Type traits.
 *
 * An example of overriding a traits flag for a custom type can be done by:
 *
 * @code
 *
 * namespace Dali
 * {
 *   /// Tell DALi that Dali::Matrix is POD, even though it has a copy constructor
 *   template <> struct TypeTraits< Dali::Matrix > : public BasicTypes< Dali::Matrix > { enum { IS_TRIVIAL_TYPE = true }; };
 * }
 *
 * @endcode
 *
 * @SINCE_1_0.0
 */
template <typename Type>
struct TypeTraits : public BasicTypes< Type >
{
};

/**
 * @}
 */
} // namespace Dali

#endif /* __DALI_TYPE_TRAITS_H__ */

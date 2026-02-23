#ifndef DALI_TYPE_TRAITS_H
#define DALI_TYPE_TRAITS_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
 * This allows specializations to not have to repeat all flags.
 * @SINCE_1_0.0
 */
template<typename Type>
struct BasicTypes
{
  /**
   * @brief This flag tells Dali if a class can be considered POD.
   *
   * If it declares copy constructor and/or destructor, its not considered trivial
   * and cannot be copied by using memcpy etc.
   * @SINCE_1_0.0
   */
  enum
  {
    IS_TRIVIAL_TYPE = __has_trivial_destructor(Type) && __has_trivial_copy(Type)
  };
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
template<typename Type>
struct TypeTraits : public BasicTypes<Type>
{
};

/**
 * @brief Helper to detect if a type is a function pointer.
 *
 * This is a type trait used for compile-time type detection. It allows templates
 * to behave differently based on whether a type is a function pointer or not.
 *
 * The primary template returns false for all non-function-pointer types.
 * Specializations can be added to return true for specific function pointer types.
 *
 * @SINCE_2_5.11
 *
 * Example usage:
 * @code
 * template<typename T>
 * struct IsFunctionPointer<void (*)(T*)> {
 *     static const bool value = true;  // True for function pointers
 * };
 *
 * // Usage:
 * IsFunctionPointer<void (*)(int*)>::value;    // true
 * IsFunctionPointer<CustomDeleter>::value;     // false
 * @endcode
 */
template<typename Deleter>
struct IsFunctionPointer
{
  static const bool value = false;
};

/**
 * @brief Specialization for function pointer types.
 *
 * This specialization returns true when the type is a function pointer of the
 * form `void (*)(T*)`, which is the common signature for deleter functions.
 *
 * @SINCE_2_5.11
 */
template<typename T>
struct IsFunctionPointer<void (*)(T*)>
{
  static const bool value = true;
};

/**
 * @brief SFINAE helper for conditional template instantiation.
 *
 * SFINAE stands for "Substitution Failure Is Not An Error". It's a C++ template
 * metaprogramming technique that allows the compiler to select different template
 * implementations based on type properties at compile time.
 *
 * When the compiler encounters template code, it tries to substitute template
 * parameters. If substitution fails (e.g., accessing a non-existent type member),
 * the compiler doesn't throw an error - it simply removes that template from
 * consideration and tries other alternatives.
 *
 * This template enables conditional compilation:
 * - Primary template (B=false): No 'type' member exists, causing substitution failure
 * - Specialization (B=true): Provides 'type' member, allowing substitution to succeed
 *
 * @SINCE_2_5.11
 */
template<bool B, typename T = void>
struct EnableIf
{
  // No 'type' member when B is false - this causes substitution failure
};

/**
 * @brief Specialization that enables the template when B is true.
 *
 * When the boolean condition is true, this specialization provides the 'type'
 * member, allowing the template substitution to succeed.
 *
 * @SINCE_2_5.11
 */
template<typename T>
struct EnableIf<true, T>
{
  typedef T type; // 'type' member exists only when B is true
};

// Internal namespace for type trait implementation details
namespace Detail
{
/**
 * @brief A type that can be constructed from any type.
 *
 * Used in SFINAE-based type detection. The templated constructor allows
 * this type to accept any argument through implicit conversion.
 */
struct AnyType
{
  template<typename T>
  AnyType(T&&);
};

/**
 * @brief Helper struct to test convertibility.
 *
 * Contains two test functions:
 * - Test(From): Accepts the source type and returns the target type
 * - Test(AnyType): Accepts anything via the AnyType constructor and returns char
 *
 * The compiler will select the best match based on overload resolution,
 * which allows us to detect implicit convertibility.
 */
template<typename To>
struct ConvertibleHelper
{
  /**
   * @brief Test function that accepts a convertible type.
   *
   * If From is implicitly convertible to To, this function will be selected
   * by overload resolution because it's a better match than the AnyType constructor.
   *
   * @tparam from The source type to test
   * @return A value of type To
   */
  template<typename From>
  static To Test(From from);

  /**
   * @brief Fallback test function for non-convertible types.
   *
   * If From cannot be implicitly converted to To, the first Test function
   * fails SFINAE and this fallback is selected instead.
   *
   * @param anyType that accepts any type
   * @return A char value
   */
  static char Test(AnyType anyType);
};

/**
 * @brief Implementation of IsConvertible.
 *
 * Uses sizeof on the return type of Test() to determine which overload
 * was selected, which indicates whether the conversion is possible.
 *
 * @tparam From The source type
 * @tparam To The target type
 */
template<typename From, typename To>
struct IsConvertibleImpl
{
  static From& Create();

  /**
   * @brief Static constant indicating if From is convertible to To.
   *
   * If From is implicitly convertible to To:
   * - Test(Create()) returns type To
   * - sizeof(To) == sizeof(To) evaluates to true
   *
   * If From is NOT implicitly convertible to To:
   * - The Test(From) overload causes substitution failure (SFINAE)
   * - Test(AnyType) is selected instead, returning char
   * - sizeof(char) == sizeof(To) evaluates to false (assuming To != char)
   */
  static const bool value = sizeof(ConvertibleHelper<To>::Test(Create())) == sizeof(To);
};

// Specialization for conversion to same type
template<typename From>
struct IsConvertibleImpl<From, From>
{
  static const bool value = true;
};

} // namespace Detail

/**
 * @brief Type trait to check if one type is implicitly convertible to another.
 *
 * Determines whether an imaginary rvalue of type From can be used to
 * initialize an object or reference of type To.
 *
 * @tparam From The source type to convert from
 * @tparam To The target type to convert to
 *
 * @SINCE_2_5.11
 *
 * Example usage:
 * @code
 * IsConvertible<int, float>::value;        // true - int converts to float
 * IsConvertible<Base*, Derived*>::value;   // false - cannot convert derived to base pointer
 * IsConvertible<Derived*, Base*>::value;   // true - derived pointer converts to base pointer
 * IsConvertible<int*, float*>::value;      // false - pointer types are unrelated
 * @endcode
 */
template<typename From, typename To>
struct IsConvertible : Detail::IsConvertibleImpl<From, To>
{
};

/**
 * @brief Integral constant type trait helper.
 *
 * Provides a compile-time constant value of type bool. Used as a base
 * class for other type traits that need to expose a boolean value.
 *
 * @tparam Value The boolean value
 * @tparam Type The type of the value (default: bool)
 *
 * @SINCE_2_5.11
 */
template<bool Value, typename Type = bool>
struct IntegralConstant
{
  static const bool                     value = Value;
  typedef bool                          value_type;
  typedef IntegralConstant<Value, Type> type;
};

/**
 * @brief Internal namespace for implementation details of IsLValueReference.
 */
namespace Detail
{
/**
 * @brief Helper to detect if a type is an lvalue reference.
 *
 * Primary template returns false for non-reference types.
 */
template<typename Type>
struct IsLValueReferenceImpl : public IntegralConstant<false>
{
};

/**
 * @brief Specialization that returns true for lvalue references.
 */
template<typename Type>
struct IsLValueReferenceImpl<Type&> : public IntegralConstant<true>
{
};

} // namespace Detail

/**
 * @brief Type trait to check if a type is an lvalue reference.
 *
 * @tparam Type The type to check
 *
 * @SINCE_2_5.11
 *
 * Example usage:
 * @code
 * IsLValueReference<int>::value;     // false
 * IsLValueReference<int&>::value;    // true
 * IsLValueReference<int&&>::value;   // false
 * @endcode
 */
template<typename Type>
struct IsLValueReference : public Detail::IsLValueReferenceImpl<Type>
{
};

/**
 * @brief Type trait to remove reference qualifiers from a type.
 *
 * Removes both lvalue and rvalue reference qualifiers from a type,
 * returning the underlying unqualified type.
 *
 * @tparam Type The type to process
 *
 * @SINCE_2_5.11
 *
 * Example usage:
 * @code
 * RemoveReference<int>::type;          // int
 * RemoveReference<int&>::type;         // int
 * RemoveReference<int&&>::type;        // int
 * RemoveReference<const int&>::type;   // const int
 * @endcode
 */
template<typename Type>
struct RemoveReference
{
  typedef Type type;
};

/**
 * @brief Specialization to remove lvalue reference.
 *
 * @tparam Type The referenced type
 */
template<typename Type>
struct RemoveReference<Type&>
{
  typedef Type type;
};

/**
 * @brief Specialization to remove rvalue reference.
 *
 * @tparam Type The referenced type
 */
template<typename Type>
struct RemoveReference<Type&&>
{
  typedef Type type;
};

/**
 * @brief Type trait to add lvalue reference to a type.
 *
 * Adds an lvalue reference qualifier to a type. If the type is already
 * a reference, it returns the type unchanged.
 *
 * @tparam Type The type to add reference to
 *
 * @SINCE_2_5.11
 *
 * Example usage:
 * @code
 * AddLValueReference<int>::type;    // int&
 * AddLValueReference<int&>::type;   // int&
 * AddLValueReference<void>::type;   // void (no reference added)
 * @endcode
 */
template<typename Type>
struct AddLValueReference
{
  typedef Type& type;
};

/**
 * @brief Specialization that doesn't add reference to void.
 *
 * Void cannot have a reference type, so it remains void.
 */
template<>
struct AddLValueReference<void>
{
  typedef void type;
};

/**
 * @brief Specialization that doesn't add reference to const void.
 *
 * Const void cannot have a reference type.
 */
template<>
struct AddLValueReference<const void>
{
  typedef const void type;
};

/**
 * @brief Perfect forwarding helper function.
 *
 * Preserves the value category (lvalue/rvalue) of the argument when
 * forwarding it to another function. This is a custom implementation
 * of std::forward that doesn't depend on the standard library.
 *
 * For lvalue references (Type&), the argument is forwarded as an lvalue.
 * For rvalue references (Type&&), the argument is forwarded as an rvalue.
 *
 * @tparam Type The type of the forwarding reference
 * @param[in] arg The argument to forward
 * @return The argument with its value category preserved
 *
 * @SINCE_2_5.11
 *
 * Example usage:
 * @code
 * template<typename T>
 * void Wrapper(T&& arg) {
 *     // Forward arg to Process, preserving whether it's an lvalue or rvalue
 *     Process(Forward<T>(arg));
 * }
 *
 * int x = 10;
 * Wrapper(x);      // Forward<int&>(x) - forwards as lvalue
 * Wrapper(20);     // Forward<int>(20) - forwards as rvalue
 * @endcode
 */
template<typename Type>
inline Type&& Forward(typename RemoveReference<Type>::type& arg) noexcept
{
  return static_cast<Type&&>(arg);
}

/**
 * @brief Perfect forwarding helper function for rvalue references.
 *
 * Overload for rvalue references. When called with an rvalue argument,
 * this overload is selected and forwards it as an rvalue.
 *
 * @tparam Type The type of the forwarding reference
 * @param[in] arg The argument to forward
 * @return The argument forwarded as an rvalue reference
 *
 * @SINCE_2_5.11
 */
template<typename Type>
inline Type&& Forward(typename RemoveReference<Type>::type&& arg) noexcept
{
  static_assert(!IsLValueReference<Type>::value, "Cannot forward an rvalue as an lvalue");
  return static_cast<Type&&>(arg);
}

/**
 * @}
 */

} // namespace Dali

#endif // DALI_TYPE_TRAITS_H

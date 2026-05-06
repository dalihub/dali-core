#ifndef DALI_TYPE_INFO_ID_H
#define DALI_TYPE_INFO_ID_H

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
#include <cstdint>
#include <cstring>

namespace Dali
{

/**
 * @addtogroup dali_core_object
 * @{
 */

/**
 * @brief Compile-time FNV-1a hash function for string literals.
 *
 * @SINCE_2_5.21
 * @param[in] str The null-terminated string to hash
 * @param[in] hash The running hash value (used for recursion; default is the FNV offset basis)
 * @return The 32-bit FNV-1a hash of the string
 *
 * @note The algorithm and constants used here (offset basis 0x811c9dc5,
 * prime 0x01000193) are part of the ABI for any TypeInfoId value that
 * is serialised or persisted outside of a single process lifetime.
 * Do not change these constants in a future release without a
 * corresponding version bump and migration guide.
 * Within a single process, the hash is used only as a fast lookup key;
 * the strcmp fallback in TypeInfoId::operator== ensures correctness
 * regardless of the hash values.
 */
constexpr uint32_t HashFNV1a(const char* str, uint32_t hash = 0x811c9dc5)
{
  return (*str) ? HashFNV1a(str + 1, (hash ^ static_cast<uint32_t>(static_cast<unsigned char>(*str))) * 0x01000193) : hash;
}

/**
 * @brief A lightweight type identifier that replaces std::type_info in public APIs.
 *
 * TypeInfoId uses a 32-bit FNV-1a hash of the type name as the primary
 * identity, with the original name retained for human-readable display
 * and collision detection.
 *
 * TypeInfoId is an immutable value type (uint32_t + const char*).
 * It is safe to copy and compare across threads without synchronisation.
 * The const char* member always points to a string with static storage
 * duration — either a string literal (from DALI_TYPE_ID stringification)
 * or a static Dali::String buffer (from the Any::TypeInfoIdFromTypeid helper).
 *
 * Examples of use:
 * \code{.cpp}
 * TypeInfoId voidType;                                       // default-constructed (void)
 * TypeInfoId intType = DALI_TYPE_ID(int);                    // from macro
 * TypeInfoId actorType = DALI_TYPE_ID(Dali::Actor);          // from macro with namespace
 * if( intType == actorType )
 * {
 *   // never true
 * }
 * const char* name = intType.GetName(); // "int"
 * \endcode*
 * @SINCE_2_5.21
 */

class TypeInfoId
{
public:
  /**
   * @brief The underlying integer type used for the type identifier.
   * @SINCE_2_5.21
   */
  using IdType = uint32_t;

  /**
   * @brief Default constructor representing the "void" type (no type).
   *
   * @SINCE_2_5.21
   */
  constexpr TypeInfoId(): mId(0), mName(nullptr)
  {
  }

  /**
   * @brief Constructs a TypeInfoId from a hash and name.
   *
   * @SINCE_2_5.21
   * @param[in] id The FNV-1a hash of the type name
   * @param[in] name The human-readable type name (must point to static-duration storage)
   */
  constexpr TypeInfoId(IdType id, const char* name): mId(id), mName(name)
  {
  }

  /**
   * @brief Equality operator.
   *
   * Uses hash comparison as the fast path, with a string comparison
   * fallback to guard against hash collisions.
   *
   * @SINCE_2_5.21
   * @param[in] other The TypeInfoId to compare with
   * @return true if both TypeInfoId values represent the same type, false otherwise
   */
  bool operator==(const TypeInfoId& other) const
  {
    // Fast path: integer comparison
    if(mId != other.mId)
    {
      return false;
    }
    // Safety: guard against hash collision with string comparison
    if(mName == other.mName)
    {
      return true;
    }
    // same pointer (includes both-null case)
    if(!mName || !other.mName)
    {
      return false;
    }
    // one is null, the other is not — cannot be equal
    return strcmp(mName, other.mName) == 0;
  }

  /**
   * @brief Inequality operator.
   *
   * @SINCE_2_5.21
   * @param[in] other The TypeInfoId to compare with
   * @return true if the TypeInfoId values represent different types, false otherwise
   */
  bool operator!=(const TypeInfoId& other) const
  {
    return !(*this == other);
  }

  /**
   * @brief Gets the human-readable type name.
   *
   * @SINCE_2_5.21
   * @return The type name string, or "void" if no type is set.
   * @note Returns "void" for both the default-constructed (empty) state and
   * for an explicitly void-typed value. Use IsValid() to distinguish the two.
   */
  const char* GetName() const
  {
    return mName ? mName : "void";
  }

  /**
   * @brief Gets the numeric type identifier.
   *
   * @SINCE_2_5.21
   * @return The FNV-1a hash of the type name
   */
  IdType GetId() const
  {
    return mId;
  }

  /**
   * @brief Checks whether this TypeInfoId represents a valid type.
   *
   * Returns false only for default-constructed (empty) TypeInfoId values.
   * A TypeInfoId constructed via DALI_TYPE_ID or TypeInfoIdFromTypeid is always valid.
   *
   * @SINCE_2_5.21
   * @return true if a type is set, false if default-constructed (void/empty)
   */
  bool IsValid() const
  {
    return mName != nullptr;
  }

private:
  IdType      mId;   ///< FNV-1a hash of the type name (fast comparison)
  const char* mName; ///< Human-readable type name (for display and collision detection)
};

/**
 * @}
 */

} // namespace Dali

/**
 * @brief Macro to create a TypeInfoId from a C++ type at call sites.
 *
 * Uses preprocessor stringification (\#Type) to obtain the type name and
 * FNV-1a to hash it at compile time. The type must be a complete, named
 * type — not a template parameter token.
 *
 * Examples of use:
 * \code{.cpp}
 * DALI_TYPE_ID(int)         -> TypeInfoId(HashFNV1a("int"), "int")
 * DALI_TYPE_ID(Dali::Actor) -> TypeInfoId(HashFNV1a("Dali::Actor"), "Dali::Actor")
 * \endcode
 *
 * @note Do NOT use this macro inside a template method body where the
 * argument is itself a template parameter (e.g., DALI_TYPE_ID(T) where T
 * is a typename). Use Dali::Any::IsType\<T\>() for type queries on Dali::Any
 * values, or refer to the Dali::Any class internals which use TypeInfoIdFromTypeid\<T\>()
 * directly.  * The macro uses preprocessor stringification (\#Type) to obtain
 * the type name.
 *
 * @return A TypeInfoId representing the given type
 * @SINCE_2_5.21
 */
#define DALI_TYPE_ID(Type) Dali::TypeInfoId(Dali::HashFNV1a(#Type), #Type)

/**
 * @brief Macro to create a TypeInfoId from an explicit string alias.
 *
 * Use this variant when the type name cannot be stringified directly by the
 * preprocessor — for example, template types containing commas, or when a
 * stable alias name is preferred over the C++ identifier.
 *
 * The alias string MUST be unique within the DALi type registry, and MUST
 * be used consistently at both registration and lookup sites. Unlike
 * DALI_TYPE_ID, this macro provides no compile-time type checking — the
 * compiler does not validate that the alias string matches the type.
 * Prefer a typedef/using alias with DALI_TYPE_ID over this macro.
 *
 * Examples of use:
 * \code{.cpp}
 * using IntVector = std::vector<int>;
 * DALI_TYPE_ID(IntVector)  // preferred
 * // Or if a typedef is not possible:
 * DALI_TYPE_ID_ALIAS(std::vector<int>, "std::vector<int>")
 * \endcode
 *
 * @return A TypeInfoId representing the given type via its alias
 * @SINCE_2_5.21
 */
#define DALI_TYPE_ID_ALIAS(Type, Alias) (static_cast<void>(sizeof(Type)), Dali::TypeInfoId(Dali::HashFNV1a(Alias), Alias))

#endif // DALI_TYPE_INFO_ID_H

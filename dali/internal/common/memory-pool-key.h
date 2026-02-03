#ifndef DALI_INTERNAL_MEMORY_POOL_KEY_H
#define DALI_INTERNAL_MEMORY_POOL_KEY_H

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

#include <dali/internal/common/fixed-size-memory-pool.h>

namespace Dali::Internal
{
/**
 * MemoryPoolKey is a 32bit replacement for pointers to objects stored
 * within MemoryPools. In a 32 bit environment, it is an actual ptr.
 *
 * @tparam Class The class of object stored within a memory pool. The
 * class grants limited access to the actual memory pool.
 *
 * The key has pointer semantics.
 *
 * As this has a copy constructor, it's not a trivial type, however because
 * it encapsulates an integer, specialized types can use Dali::Vector by
 * overriding the type traits using:
 *
 * template<>
 * struct TypeTraits<SpecializedClass> : public BasicTypes<SpecializedClass>
 * {
 *   enum
 *   {
 *     IS_TRIVIAL_TYPE = true
 *   };
 * };
 */
template<typename Class>
class MemoryPoolKey
{
public:
  using KeyType = FixedSizeMemoryPool::KeyType;

  /**
   * Default Constructor - generates an empty key.
   */
  MemoryPoolKey()
  : key(INVALID)
  {
  }

  /**
   * Constructor - Construct a key object from an int key.
   * @param[in] aKey A key of an object in the pool
   */
  explicit MemoryPoolKey(KeyType aKey)
  : key(aKey)
  {
  }

  /**
   * Copy constructor.
   * @param[in] rhs Key to copy
   */
  MemoryPoolKey(const MemoryPoolKey<Class>& rhs)
  {
    key = rhs.key;
  }

  /**
   * Assignment operator
   * @param[in] rhs Key to copy
   */
  MemoryPoolKey& operator=(MemoryPoolKey& rhs)
  {
    key = rhs.key;
    return *this;
  }

  /**
   * Assignment operator
   * @param[in] rhs Key to copy
   */
  const MemoryPoolKey& operator=(const MemoryPoolKey& rhs)
  {
    key = rhs.key;
    return *this;
  }

  /**
   * Member operator
   * @return A pointer to the object that the key references, or nullptr
   */
  Class* operator->() const
  {
#if defined(__LP64__)
    return Class::Get(key);
#else
    return static_cast<Class*>(key);
#endif
  }

  /**
   * Method to get a pointer to the object referenced by the key
   * @return A pointer to the referenced object, or nullptr if not in the memory pool
   */
  Class* Get() const
  {
#if defined(__LP64__)
    return Class::Get(key);
#else
    return static_cast<Class*>(key);
#endif
  }

  /**
   * Boolean operator (tests against INVALID, not 0)
   * @return true if the key is not invalid
   */
  explicit operator bool() const
  {
    return key != INVALID;
  }

  /**
   * Equality operator
   * @param[in] rhs The key to test against
   * @return true if the keys match
   */
  bool operator==(const MemoryPoolKey<Class>& rhs) const
  {
    return key == rhs.key;
  }

  /**
   * Inequality operator
   * @param[in] rhs The key to test against
   * @return true if the keys don't match
   */
  bool operator!=(const MemoryPoolKey<Class>& rhs) const
  {
    return key != rhs.key;
  }

  /**
   * Equality operator for nullptr
   * @param[in] np nullptr
   * @return true if the key is invalid
   */
  bool operator==(decltype(nullptr) np) const
  {
    return key == INVALID;
  }

  /**
   * Inequality operator for nullptr
   * @param[in] np nullptr
   * @return true if the keys is valid
   */
  bool operator!=(decltype(nullptr) np) const
  {
    return key != INVALID;
  }

  uint32_t Value() const
  {
    return reinterpret_cast<uint32_t>(key);
  }

private:
  // Ensure that INVALID constant can't be used directly.
#if defined(__LP64__)
  static const KeyType INVALID{0xffffffff}; ///< Null or Invalid constant.
#else
  static constexpr KeyType INVALID{nullptr};
#endif

  KeyType key{INVALID}; ///< The actual key.
};

} // namespace Dali::Internal

#endif // DALI_INTERNAL_MEMORY_POOL_KEY_H

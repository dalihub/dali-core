#ifndef DALI_INTERNAL_OWNER_KEY_TYPE_H
#define DALI_INTERNAL_OWNER_KEY_TYPE_H

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
#include <dali/internal/common/memory-pool-key.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
namespace Internal
{
/**
 * OwnerKeyType is a struct which is responsible for killing memory-pool allocated objects.
 */
template<class ObjectType>
class OwnerKeyType
{
public:
  using KeyType = MemoryPoolKey<ObjectType>;

  /**
   * Default constructor. Creates an OwnerKeyType that does not own any object.
   * @note This does not protect against two different OwnerKeyType pointing to the same object.
   *       Both OwnerKeyType will try to release the memory of the same object in that case which
   *       could lead to a crash.
   */
  OwnerKeyType()
  : mKey()
  {
  }

  /**
   * Constructor. Creates an OwnerKeyType that owns the object.
   * @param[in] key A key type to a memory pool allocated object.
   */
  OwnerKeyType(KeyType key)
  : mKey(key)
  {
  }

  /**
   * Copy constructor. Passes the ownership of a pointer to another.
   * @param[in] other The pointer that gives away the ownership.
   */
  OwnerKeyType(const OwnerKeyType& other)
  : OwnerKeyType(static_cast<OwnerKeyType&&>(const_cast<OwnerKeyType&>(other))) // Remove constness & cast to rvalue to use the move constructor
  {
    // other needs to be const for compiler to pick up this as copy constructor;
    // though we are using this as move as there can only be one owner
  }

  /**
   * Move constructor. Passes the ownership of a pointer to another.
   * @param[in] other The pointer that gives away the ownership.
   */
  OwnerKeyType(OwnerKeyType&& other) noexcept
  : mKey()
  {
    Swap(other);
  }

  /**
   * Assignment operator. Passes the ownership of a pointer to another.
   * @param[in] other The pointer that gives away the ownership.
   */
  OwnerKeyType& operator=(OwnerKeyType& other)
  {
    if(this != &other) // no self-assignment
    {
      Delete(mKey);
      mKey       = other.mKey;
      other.mKey = KeyType();
    }

    // return self
    return *this;
  }

  /**
   * Move assignment operator. Passes the ownership of a pointer to another.
   * @param[in] other The pointer that gives away the ownership.
   */
  OwnerKeyType& operator=(OwnerKeyType&& other) noexcept
  {
    // Reuse operator=
    return operator=(other);
  }

  /**
   * Assignment operator. Takes the ownership of the object.
   * If it owns an object already, it will be deleted.
   * @param[in] pointer A pointer to a heap allocated object.
   */
  OwnerKeyType& operator=(KeyType key)
  {
    if(mKey != key)
    {
      Reset();
      mKey = key;
    }

    return *this;
  }

  /**
   * Destructor.
   */
  ~OwnerKeyType()
  {
    Reset();
  }

  /**
   * Compare with a key.
   * @return true if the key matches the one owned by this object.
   */
  bool operator==(const KeyType key)
  {
    return (mKey == key);
  }

  /**
   * Reset the pointer, deleting any owned object.
   */
  void Reset()
  {
    Delete(mKey);
    mKey = KeyType();
  }

  /**
   * Release the ownership, it does not delete the object.
   * @return a pointer to the object.
   */
  KeyType Release()
  {
    KeyType tmp = mKey;
    mKey        = KeyType();
    return tmp;
  }

  /**
   * Swap owned objects
   * @param[in] other The pointer to swap the owned objects with.
   */
  void Swap(OwnerKeyType& other)
  {
    if(this != &other)
    {
      KeyType tmp = mKey;
      mKey        = other.mKey;
      other.mKey  = tmp;
    }
  }

private:
  /**
   * @brief delete the contents of the key
   * Function provided to allow classes to provide a custom destructor through template specialisation
   * @param key value to the object
   */
  void Delete(KeyType key)
  {
    delete key.Get();
  }

private:
  // data
  KeyType mKey; ///< Raw key value to the object
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_OWNER_KEY_TYPE_H

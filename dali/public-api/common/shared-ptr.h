#pragma once
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
#include <cstdint> // for uint32_t

// INTERNAL INCLUDES
#include <dali/public-api/common/type-traits.h>

namespace Dali
{
/**
 * @addtogroup dali_core_common
 * @{
 */

/**
 * @brief Templated shared pointer class with a default deleter.
 *
 * @SINCE_2_5.16
 * @tparam Type The type of the pointer stored
 */
template<typename Type>
class SharedPtr
{
public:
  // Allow converting constructors to access private members from other template instantiations
  template<typename>
  friend class SharedPtr;

  /**
   * @brief Default constructor.
   * @SINCE_2_5.16
   * @param[in] ptr Raw pointer to take ownership of
   */
  SharedPtr()
  : mPtr(nullptr),
    mRefCount(nullptr)
  {
  }

  /**
   * @brief Constructor that manages the given pointer.
   * @SINCE_2_5.16
   * @param[in] ptr Raw pointer to take ownership of
   */
  explicit SharedPtr(Type* ptr)
  : mPtr(ptr),
    mRefCount(ptr ? new uint32_t(1) : nullptr)
  {
  }

  /**
   * @brief Copy constructor.
   * @SINCE_2_5.16
   * @param[in] other The object to be copied
   */
  SharedPtr(const SharedPtr& other) noexcept
  : mPtr(other.mPtr),
    mRefCount(other.mRefCount)
  {
    if(mRefCount)
    {
      __sync_fetch_and_add(mRefCount, 1); // Atomic increment
    }
  }

  /**
   * @brief Move constructor.
   * @SINCE_2_5.16
   * @param[in] other The object to be moved
   */
  SharedPtr(SharedPtr&& other) noexcept
  : mPtr(other.mPtr),
    mRefCount(other.mRefCount)
  {
    other.mPtr      = nullptr;
    other.mRefCount = nullptr;
  }

  /**
   * @brief Destructor.
   * @SINCE_2_5.16
   */
  ~SharedPtr()
  {
    ResetInternal();
  }

  /**
   * @brief Copy assignment operator.
   * @SINCE_2_5.16
   * @param[in] other The object to be copied
   */
  SharedPtr& operator=(const SharedPtr& other) noexcept
  {
    if(this != &other)
    {
      ResetInternal();
      mPtr      = other.mPtr;
      mRefCount = other.mRefCount;
      if(mRefCount)
      {
        __sync_fetch_and_add(mRefCount, 1); // Atomic increment
      }
    }
    return *this;
  }

  /**
   * @brief Move assignment operator.
   * @SINCE_2_5.16
   * @param[in] other The object to be moved
   */
  SharedPtr& operator=(SharedPtr&& other) noexcept
  {
    if(this != &other)
    {
      ResetInternal();
      mPtr            = other.mPtr;
      mRefCount       = other.mRefCount;
      other.mPtr      = nullptr;
      other.mRefCount = nullptr;
    }
    return *this;
  }

  /**
   * @brief Converting copy constructor from derived type.
   *
   * Constructs a SharedPtr<Base> from a SharedPtr<Derived>.
   *
   * @SINCE_2_5.16
   * @tparam U The derived type
   * @param[in] other The derived SharedPtr to move from
   * @note Enabled only if U* is convertible to Type*
   */
  template<typename U, typename = typename EnableIf<IsConvertible<U*, Type*>::value>::type>
  SharedPtr(const SharedPtr<U>& other) noexcept
  : mPtr(other.mPtr),
    mRefCount(other.mRefCount)
  {
    if(mRefCount)
    {
      __sync_fetch_and_add(mRefCount, 1); // Atomic increment
    }
  }

  /**
   * @brief Converting move constructor from derived type.
   *
   * Constructs a SharedPtr<Base> from a SharedPtr<Derived>.
   *
   * @SINCE_2_5.16
   * @tparam U The derived type
   * @param[in] other The derived SharedPtr to move from
   * @note Enabled only if U* is convertible to Type*
   */
  template<typename U, typename = typename EnableIf<IsConvertible<U*, Type*>::value>::type>
  SharedPtr(SharedPtr<U>&& other) noexcept
  : mPtr(other.mPtr),
    mRefCount(other.mRefCount)
  {
    other.mPtr      = nullptr;
    other.mRefCount = nullptr;
  }

  /**
   * @brief Converting copy assignment operator from derived type.
   *
   * Assigns a SharedPtr<Base> from a SharedPtr<Derived>.
   *
   * @SINCE_2_5.16
   * @tparam U The derived type
   * @param[in] other The derived SharedPtr to move from
   * @return Reference to this object
   * @note Enabled only if U* is convertible to Type*
   */
  template<typename U, typename = typename EnableIf<IsConvertible<U*, Type*>::value>::type>
  SharedPtr& operator=(const SharedPtr<U>& other) noexcept
  {
    ResetInternal();
    mPtr      = other.mPtr;
    mRefCount = other.mRefCount;
    if(mRefCount)
    {
      __sync_fetch_and_add(mRefCount, 1); // Atomic increment
    }
    return *this;
  }

  /**
   * @brief Converting move assignment operator from derived type.
   *
   * Assigns a SharedPtr<Base> from a SharedPtr<Derived>.
   *
   * @SINCE_2_5.11
   * @tparam U The derived type
   * @param[in] other The derived SharedPtr to move from
   * @return Reference to this object
   * @note Enabled only if U* is convertible to Type*
   */
  template<typename U, typename = typename EnableIf<IsConvertible<U*, Type*>::value>::type>
  SharedPtr& operator=(SharedPtr<U>&& other) noexcept
  {
    ResetInternal();
    mPtr            = other.mPtr;
    mRefCount       = other.mRefCount;
    other.mPtr      = nullptr;
    other.mRefCount = nullptr;
    return *this;
  }

  /**
   * @brief Overloaded dereference operator.
   * @SINCE_2_5.16
   * @return A reference to the object
   */
  Type& operator*()
  {
    return *mPtr;
  }

  /**
   * @brief Overloaded const dereference operator.
   * @SINCE_2_5.16
   * @return A const reference to the object
   */
  const Type& operator*() const
  {
    return *mPtr;
  }

  /**
   * @brief Overloaded pointer operator.
   * @SINCE_2_5.16
   * @return A pointer to the object
   */
  Type* operator->()
  {
    return mPtr;
  }

  /**
   * @brief Overloaded pointer to const operator.
   * @SINCE_2_5.16
   * @return A pointer to a const object
   */
  const Type* operator->() const
  {
    return mPtr;
  }

  /**
   * @brief Explicit conversion to bool.
   * @SINCE_2_5.16
   */
  explicit operator bool() const
  {
    return mPtr != nullptr;
  }

  /**
   * @brief Get the raw pointer to the managed object.
   * @SINCE_2_5.16
   * @return The raw pointer to the object
   */
  Type* Get()
  {
    return mPtr;
  }

  /**
   * @brief Get the raw pointer to const object.
   * @SINCE_2_5.16
   * @return The raw pointer to the const object
   */
  const Type* Get() const
  {
    return mPtr;
  }

  /**
   * @brief Get the number of SharedPtr objects referring to the same managed object.
   * @SINCE_2_5.16
   * @return The use count
   */
  uint32_t UseCount() const noexcept
  {
    return mRefCount ? *mRefCount : 0;
  }

  /**
   * @brief Reset the managed object.
   * @SINCE_2_5.16
   * @param[in] ptr The object to take ownership of
   */
  void Reset(Type* ptr = nullptr) noexcept
  {
    if(ptr != mPtr)
    {
      ResetInternal();
      mPtr = ptr;
      if(ptr)
      {
        mRefCount = new uint32_t(1);
      }
      else
      {
        mRefCount = nullptr;
      }
    }
  }

  /**
   * @brief Swap two SharedPtr instances.
   * @SINCE_2_5.16
   */
  void Swap(SharedPtr& other) noexcept
  {
    Type*     tempPtr      = mPtr;
    uint32_t* tempRefCount = mRefCount;

    mPtr      = other.mPtr;
    mRefCount = other.mRefCount;

    other.mPtr      = tempPtr;
    other.mRefCount = tempRefCount;
  }

private:
  Type*     mPtr;      ///< Pointer to the managed object
  uint32_t* mRefCount; ///< Pointer to the reference counter

  /// Helper function to reset the internal state
  void ResetInternal()
  {
    if(mRefCount)
    {
      if(__sync_fetch_and_sub(mRefCount, 1) == 1)
      { // Atomic decrement
        delete mPtr;
        delete mRefCount;
      }
      mPtr      = nullptr;
      mRefCount = nullptr;
    }
  }
};

/**
 * @brief Creates a SharedPtr that manages a new object.
 * @SINCE_2_5.16
 * @tparam Type The type of the object to construct
 * @tparam Args The types of arguments to forward to the constructor
 * @param[in] args The arguments to forward to the constructor of Type
 * @return SharedPtr<Type> managing the newly created object
 */
template<typename Type, typename... Args>
SharedPtr<Type> MakeShared(Args&&... args)
{
  return SharedPtr<Type>(new Type(Forward<Args>(args)...));
}

/**
 * @brief Equality comparison operator for SharedPtr objects.
 *
 * @SINCE_2_5.16
 * @tparam Type The type of object managed by the SharedPtr
 * @param lhs The left-hand side SharedPtr to compare
 * @param rhs The right-hand side SharedPtr to compare
 * @return true if both SharedPtr objects are null or own the same pointer, false otherwise
 * @note This comparison is based on pointer address equality, not value equality of the managed objects.
 */
template<typename Type>
bool operator==(const SharedPtr<Type>& lhs, const SharedPtr<Type>& rhs) noexcept
{
  return lhs.Get() == rhs.Get();
}

/**
 * @brief Inequality comparison operator for SharedPtr objects.
 *
 * @SINCE_2_5.16
 * @tparam Type The type of object managed by the SharedPtr
 * @param lhs The left-hand side SharedPtr to compare
 * @param rhs The right-hand side SharedPtr to compare
 * @return true if the SharedPtr objects manage different pointers, false otherwise
 */
template<typename Type>
bool operator!=(const SharedPtr<Type>& lhs, const SharedPtr<Type>& rhs) noexcept
{
  return lhs.Get() != rhs.Get();
}

/**
 * @brief Less-than comparison operator for SharedPtr objects.
 *
 * Enables use of SharedPtr in ordered containers (e.g., map, set).
 *
 * @SINCE_2_5.16
 * @tparam Type The type of object managed by the SharedPtr
 * @param lhs The left-hand side SharedPtr to compare
 * @param rhs The right-hand side SharedPtr to compare
 * @return true if lhs.Get() < rhs.Get(), false otherwise
 */
template<typename Type>
bool operator<(const SharedPtr<Type>& lhs, const SharedPtr<Type>& rhs) noexcept
{
  return lhs.Get() < rhs.Get();
}

/**
 * @brief Less-than-or-equal comparison operator for SharedPtr objects.
 *
 * @SINCE_2_5.16
 * @tparam Type The type of object managed by the SharedPtr
 * @param lhs The left-hand side SharedPtr to compare
 * @param rhs The right-hand side SharedPtr to compare
 * @return true if lhs.Get() <= rhs.Get(), false otherwise
 */
template<typename Type>
bool operator<=(const SharedPtr<Type>& lhs, const SharedPtr<Type>& rhs) noexcept
{
  return lhs.Get() <= rhs.Get();
}

/**
 * @brief Greater-than comparison operator for SharedPtr objects
 *
 * @SINCE_2_5.16
 * @tparam Type The type of object managed by the SharedPtr.
 * @param lhs The left-hand side SharedPtr to compare.
 * @param rhs The right-hand side SharedPtr to compare.
 * @return true if lhs.Get() > rhs.Get(), false otherwise.
 */
template<typename Type>
bool operator>(const SharedPtr<Type>& lhs, const SharedPtr<Type>& rhs) noexcept
{
  return lhs.Get() > rhs.Get();
}

/**
 * @brief Greater-than-or-equal comparison operator for SharedPtr objects.
 *
 * @SINCE_2_5.16
 * @tparam Type The type of object managed by the SharedPtr.
 * @param lhs The left-hand side SharedPtr to compare.
 * @param rhs The right-hand side SharedPtr to compare.
 * @return true if lhs.Get() >= rhs.Get(), false otherwise.
 */
template<typename Type>
bool operator>=(const SharedPtr<Type>& lhs, const SharedPtr<Type>& rhs) noexcept
{
  return lhs.Get() >= rhs.Get();
}

/**
 * @}
 */

} // namespace Dali
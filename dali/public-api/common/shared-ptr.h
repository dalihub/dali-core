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

// Forward declarations
template<typename Type>
class SharedPtr;

template<typename Type>
class WeakPtr;

template<typename Type>
class EnableSharedFromThis;

/**
 * @brief Control block for reference counting.
 *
 * This structure holds both strong (shared) and weak reference counts.
 * It is used internally by SharedPtr and WeakPtr.
 * @SINCE_2_5.22
 */
struct RefCountControlBlock
{
  uint32_t strongCount; ///< Number of SharedPtr references
  uint32_t weakCount;   ///< Number of WeakPtr references

  RefCountControlBlock()
  : strongCount(1),
    weakCount(1)
  {
  }
};

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

  // Allow EnableSharedFromThis to access private constructor
  template<typename>
  friend class EnableSharedFromThis;

  // Allow WeakPtr to access private members
  template<typename>
  friend class WeakPtr;

  // Allow DynamicPointerCast to access private constructor and mControlBlock
  template<typename To, typename From>
  friend SharedPtr<To> DynamicPointerCast(const SharedPtr<From>& from) noexcept;

  /**
   * @brief Default constructor.
   * @SINCE_2_5.16
   */
  SharedPtr()
  : mPtr(nullptr),
    mControlBlock(nullptr)
  {
  }

  /**
   * @brief Constructor that manages the given pointer.
   * @SINCE_2_5.16
   * @param[in] ptr Raw pointer to take ownership of
   */
  explicit SharedPtr(Type* ptr)
  : mPtr(ptr),
    mControlBlock(ptr ? new RefCountControlBlock() : nullptr)
  {
    // Initialize EnableSharedFromThis if applicable
    if(mPtr)
    {
      InitializeEnableSharedFromThis(mPtr);
    }
  }

  /**
   * @brief Copy constructor.
   * @SINCE_2_5.16
   * @param[in] other The object to be copied
   */
  SharedPtr(const SharedPtr& other) noexcept
  : mPtr(other.mPtr),
    mControlBlock(other.mControlBlock)
  {
    if(mControlBlock)
    {
      __sync_fetch_and_add(&mControlBlock->strongCount, 1);
    }
  }

  /**
   * @brief Move constructor.
   * @SINCE_2_5.16
   * @param[in] other The object to be moved
   */
  SharedPtr(SharedPtr&& other) noexcept
  : mPtr(other.mPtr),
    mControlBlock(other.mControlBlock)
  {
    other.mPtr          = nullptr;
    other.mControlBlock = nullptr;
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
      mPtr          = other.mPtr;
      mControlBlock = other.mControlBlock;
      if(mControlBlock)
      {
        __sync_fetch_and_add(&mControlBlock->strongCount, 1);
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
      mPtr                = other.mPtr;
      mControlBlock       = other.mControlBlock;
      other.mPtr          = nullptr;
      other.mControlBlock = nullptr;
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
    mControlBlock(other.mControlBlock)
  {
    if(mControlBlock)
    {
      __sync_fetch_and_add(&mControlBlock->strongCount, 1);
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
    mControlBlock(other.mControlBlock)
  {
    other.mPtr          = nullptr;
    other.mControlBlock = nullptr;
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
    mPtr          = other.mPtr;
    mControlBlock = other.mControlBlock;
    if(mControlBlock)
    {
      __sync_fetch_and_add(&mControlBlock->strongCount, 1);
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
    mPtr                = other.mPtr;
    mControlBlock       = other.mControlBlock;
    other.mPtr          = nullptr;
    other.mControlBlock = nullptr;
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
   * @brief Check if the SharedPtr is empty (null).
   * @SINCE_2_5.16
   * @return true if the SharedPtr is empty
   */
  bool IsEmpty() const
  {
    return mPtr == nullptr;
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
    return mControlBlock ? mControlBlock->strongCount : 0;
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
        mControlBlock = new RefCountControlBlock();
        InitializeEnableSharedFromThis(mPtr);
      }
      else
      {
        mControlBlock = nullptr;
      }
    }
  }

  /**
   * @brief Swap two SharedPtr instances.
   * @SINCE_2_5.16
   */
  void Swap(SharedPtr& other) noexcept
  {
    Type*                 tempPtr          = mPtr;
    RefCountControlBlock* tempControlBlock = mControlBlock;

    mPtr          = other.mPtr;
    mControlBlock = other.mControlBlock;

    other.mPtr          = tempPtr;
    other.mControlBlock = tempControlBlock;
  }

private:
  Type*                 mPtr;          ///< Pointer to the managed object
  RefCountControlBlock* mControlBlock; ///< Pointer to the control block

  /**
   * @brief Constructor for sharing control block from another SharedPtr.
   * @param ptr The cast pointer to manage
   * @param controlBlock The control block to share
   * @param incrementCount If true, increment the strong count
   */
  SharedPtr(Type* ptr, RefCountControlBlock* controlBlock, bool incrementCount = true) noexcept
  : mPtr(ptr),
    mControlBlock(controlBlock)
  {
    if(mControlBlock && incrementCount)
    {
      __sync_fetch_and_add(&mControlBlock->strongCount, 1);
    }
  }

  /// Helper to initialize EnableSharedFromThis base class
  template<typename U>
  void InitializeEnableSharedFromThis(EnableSharedFromThis<U>* ptr);

  /// Overload for non-EnableSharedFromThis types (no-op)
  void InitializeEnableSharedFromThis(...)
  {
  }

  /// Helper function to reset the internal state
  void ResetInternal()
  {
    if(mControlBlock)
    {
      if(__sync_fetch_and_sub(&mControlBlock->strongCount, 1) == 1)
      {
        delete mPtr;
        // Decrement weak count (for the "virtual" weak reference from strong)
        if(__sync_fetch_and_sub(&mControlBlock->weakCount, 1) == 1)
        {
          delete mControlBlock;
        }
      }
      mPtr          = nullptr;
      mControlBlock = nullptr;
    }
  }
};

// Note: Circular Reference Issues if we put WeakPtr in a different header

/**
 * @brief Templated weak pointer class.
 *
 * WeakPtr holds a non-owning reference to an object managed by SharedPtr.
 * It must be converted to SharedPtr using Lock() to access the object.
 *
 * @SINCE_2_5.22
 * @tparam Type The type of the pointer stored
 */
template<typename Type>
class WeakPtr
{
public:
  // Allow converting constructors to access private members from other template instantiations
  template<typename>
  friend class WeakPtr;

  // Allow SharedPtr to access private members
  template<typename>
  friend class SharedPtr;

  // Allow EnableSharedFromThis to access private members
  template<typename>
  friend class EnableSharedFromThis;

  /**
   * @brief Default constructor.
   * @SINCE_2_5.22
   */
  WeakPtr() noexcept
  : mPtr(nullptr),
    mControlBlock(nullptr)
  {
  }

  /**
   * @brief Construct from SharedPtr.
   * @SINCE_2_5.22
   * @param[in] shared The SharedPtr to create a weak reference to
   */
  WeakPtr(const SharedPtr<Type>& shared) noexcept
  : mPtr(shared.mPtr),
    mControlBlock(shared.mControlBlock)
  {
    if(mControlBlock)
    {
      __sync_fetch_and_add(&mControlBlock->weakCount, 1);
    }
  }

  /**
   * @brief Copy constructor.
   * @SINCE_2_5.22
   * @param[in] other The WeakPtr to copy
   */
  WeakPtr(const WeakPtr& other) noexcept
  : mPtr(other.mPtr),
    mControlBlock(other.mControlBlock)
  {
    if(mControlBlock)
    {
      __sync_fetch_and_add(&mControlBlock->weakCount, 1);
    }
  }

  /**
   * @brief Move constructor.
   * @SINCE_2_5.22
   * @param[in] other The WeakPtr to move
   */
  WeakPtr(WeakPtr&& other) noexcept
  : mPtr(other.mPtr),
    mControlBlock(other.mControlBlock)
  {
    other.mPtr          = nullptr;
    other.mControlBlock = nullptr;
  }

  /**
   * @brief Converting copy constructor from derived type.
   * @SINCE_2_5.22
   * @tparam U The derived type
   * @param[in] other The derived WeakPtr to copy from
   * @note Enabled only if U* is convertible to Type*
   */
  template<typename U, typename = typename EnableIf<IsConvertible<U*, Type*>::value>::type>
  WeakPtr(const WeakPtr<U>& other) noexcept
  : mPtr(other.mPtr),
    mControlBlock(other.mControlBlock)
  {
    if(mControlBlock)
    {
      __sync_fetch_and_add(&mControlBlock->weakCount, 1);
    }
  }

  /**
   * @brief Converting move constructor from derived type.
   * @SINCE_2_5.22
   * @tparam U The derived type
   * @param[in] other The derived WeakPtr to move from
   * @note Enabled only if U* is convertible to Type*
   */
  template<typename U, typename = typename EnableIf<IsConvertible<U*, Type*>::value>::type>
  WeakPtr(WeakPtr<U>&& other) noexcept
  : mPtr(other.mPtr),
    mControlBlock(other.mControlBlock)
  {
    other.mPtr          = nullptr;
    other.mControlBlock = nullptr;
  }

  /**
   * @brief Destructor.
   * @SINCE_2_5.22
   */
  ~WeakPtr()
  {
    Reset();
  }

  /**
   * @brief Copy assignment operator.
   * @SINCE_2_5.22
   * @param[in] other The WeakPtr to assign
   */
  WeakPtr& operator=(const WeakPtr& other) noexcept
  {
    if(this != &other)
    {
      Reset();
      mPtr          = other.mPtr;
      mControlBlock = other.mControlBlock;
      if(mControlBlock)
      {
        __sync_fetch_and_add(&mControlBlock->weakCount, 1);
      }
    }
    return *this;
  }

  /**
   * @brief Move assignment operator.
   * @SINCE_2_5.22
   * @param[in] other The WeakPtr to move
   */
  WeakPtr& operator=(WeakPtr&& other) noexcept
  {
    if(this != &other)
    {
      Reset();
      mPtr                = other.mPtr;
      mControlBlock       = other.mControlBlock;
      other.mPtr          = nullptr;
      other.mControlBlock = nullptr;
    }
    return *this;
  }

  /**
   * @brief Assign from SharedPtr.
   * @SINCE_2_5.22
   * @param[in] shared The SharedPtr to create a weak reference to
   */
  WeakPtr& operator=(const SharedPtr<Type>& shared) noexcept
  {
    Reset();
    mPtr          = shared.mPtr;
    mControlBlock = shared.mControlBlock;
    if(mControlBlock)
    {
      __sync_fetch_and_add(&mControlBlock->weakCount, 1);
    }
    return *this;
  }

  /**
   * @brief Reset to empty.
   * @SINCE_2_5.22
   */
  void Reset() noexcept
  {
    if(mControlBlock)
    {
      // Decrement weak count
      if(__sync_fetch_and_sub(&mControlBlock->weakCount, 1) == 1)
      {
        // If weak count reaches 0 and strong count is also 0, delete control block
        if(mControlBlock->strongCount == 0)
        {
          delete mControlBlock;
        }
      }
      mPtr          = nullptr;
      mControlBlock = nullptr;
    }
  }

  /**
   * @brief Check if the managed object has been deleted.
   * @SINCE_2_5.22
   * @return true if the managed object has been deleted
   */
  bool Expired() const noexcept
  {
    return !mControlBlock || mControlBlock->strongCount == 0;
  }

  /**
   * @brief Get a SharedPtr to the managed object.
   *
   * Returns an empty SharedPtr if the object has been deleted.
   *
   * @SINCE_2_5.22
   * @return A SharedPtr to the managed object, or empty if expired
   */
  SharedPtr<Type> Lock() const noexcept
  {
    if(!mControlBlock || mControlBlock->strongCount == 0)
    {
      return SharedPtr<Type>();
    }

    // Increment strong count atomically
    uint32_t count;
    do
    {
      count = mControlBlock->strongCount;
      if(count == 0)
      {
        return SharedPtr<Type>();
      }
    } while(!__sync_bool_compare_and_swap(&mControlBlock->strongCount, count, count + 1));

    // Pass false to not increment again - we already incremented above
    return SharedPtr<Type>(mPtr, mControlBlock, false);
  }

  /**
   * @brief Get the number of SharedPtr objects referring to the same managed object.
   * @SINCE_2_5.22
   * @return The use count
   */
  uint32_t UseCount() const noexcept
  {
    return mControlBlock ? mControlBlock->strongCount : 0;
  }

private:
  Type*                 mPtr;          ///< Pointer to the managed object
  RefCountControlBlock* mControlBlock; ///< Pointer to the control block

  /**
   * @brief Private constructor for internal use.
   * @param ptr The pointer to manage
   * @param controlBlock The control block to share
   */
  WeakPtr(Type* ptr, RefCountControlBlock* controlBlock) noexcept
  : mPtr(ptr),
    mControlBlock(controlBlock)
  {
    if(mControlBlock)
    {
      __sync_fetch_and_add(&mControlBlock->weakCount, 1);
    }
  }
};

/**
 * @brief Base class that enables shared_from_this for Dali::SharedPtr.
 *
 * Derive from this class to be able to call SharedFromThis() on objects
 * managed by Dali::SharedPtr.
 *
 * @SINCE_2_5.22
 * @tparam Type The derived class type (CRTP pattern)
 */
template<typename Type>
class EnableSharedFromThis
{
public:
  /**
   * @brief Get a SharedPtr that shares ownership with existing SharedPtrs.
   * @SINCE_2_5.22
   * @return A SharedPtr that manages the same object
   * @note This method will return an empty SharedPtr if the object is not
   * managed by a SharedPtr (i.e., not created via MakeShared or similar)
   */
  SharedPtr<Type> SharedFromThis()
  {
    return mWeakPtr.Lock();
  }

  /**
   * @brief Get a const SharedPtr that shares ownership with existing SharedPtrs.
   * @SINCE_2_5.22
   * @return A const SharedPtr that manages the same object
   */
  SharedPtr<const Type> SharedFromThis() const
  {
    return mWeakPtr.Lock();
  }

  /**
   * @brief Get a WeakPtr to this object.
   * @SINCE_2_5.22
   * @return A WeakPtr that references this object
   */
  WeakPtr<Type> WeakFromThis()
  {
    return mWeakPtr;
  }

  /**
   * @brief Get a const WeakPtr to this object.
   * @SINCE_2_5.22
   * @return A const WeakPtr that references this object
   */
  WeakPtr<const Type> WeakFromThis() const
  {
    return mWeakPtr;
  }

protected:
  EnableSharedFromThis() = default;
  EnableSharedFromThis(const EnableSharedFromThis&)
  {
  }
  EnableSharedFromThis& operator=(const EnableSharedFromThis&)
  {
    return *this;
  }
  ~EnableSharedFromThis() = default;

private:
  template<typename U>
  friend class SharedPtr;

  WeakPtr<Type> mWeakPtr;
};

// Definition of SharedPtr::InitializeEnableSharedFromThis (must come after EnableSharedFromThis)
template<typename Type>
template<typename U>
void SharedPtr<Type>::InitializeEnableSharedFromThis(EnableSharedFromThis<U>* ptr)
{
  ptr->mWeakPtr = WeakPtr<U>(static_cast<U*>(ptr), mControlBlock);
}

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
 * @brief Performs a dynamic cast on a SharedPtr.
 *
 * Creates a new SharedPtr that manages the same object but with a different
 * pointer type obtained via dynamic_cast. The returned SharedPtr shares
 * ownership with the original.
 *
 * @SINCE_2_5.22
 * @tparam To The target pointer type
 * @tparam From The source pointer type
 * @param[in] from The source SharedPtr
 * @return A SharedPtr<To> managing the cast pointer, or empty SharedPtr if cast fails
 */
template<typename To, typename From>
SharedPtr<To> DynamicPointerCast(const SharedPtr<From>& from) noexcept
{
  // Note: We need to const_cast because Get() returns const pointer,
  // but the dynamic_cast target type is non-const. This is safe because
  // we're creating a new non-const SharedPtr from the same managed object.
  if(auto* castPtr = dynamic_cast<To*>(const_cast<From*>(from.Get())))
  {
    return SharedPtr<To>(castPtr, from.mControlBlock);
  }
  return {};
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
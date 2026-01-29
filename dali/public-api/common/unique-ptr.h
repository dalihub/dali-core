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

// INTERNAL INCLUDES
#include <dali/public-api/common/type-traits.h>

namespace Dali
{
/**
 * @addtogroup dali_core_common
 * @{
 */

/**
 * @brief Templated unique pointer class with function pointer deleters.
 *
 * @tparam Type The type of the pointer stored
 * @tparam Deleter The function to delete the object
 *
 * @SINCE_2_5.11
 */
template<typename Type, typename Deleter = void (*)(Type*), typename = void>
class UniquePtr
{
public:
  // Allow converting constructors to access private members from other template instantiations
  template<typename, typename, typename>
  friend class UniquePtr;

  static constexpr Deleter DefaultDeleter = [](Type* p)
  {
    delete p;
  }; ///< Default deleter

  /**
   * @brief Default constructor.
   * @SINCE_2_5.11
   */
  UniquePtr()
  : mPtr(nullptr),
    mDeleter(DefaultDeleter)
  {
  }

  /**
   * @brief Constructor that takes ownership of a raw pointer.
   * @SINCE_2_5.11
   * @param[in] ptr Raw pointer to take ownership of
   * @param[in] deleter The function to delete the *ptr*
   * @note If *deleter* is nullptr, then the \ref DefaultDeleter will be used (i.e. simple delete)
   */
  explicit UniquePtr(Type* ptr, Deleter deleter = DefaultDeleter)
  : mPtr(ptr),
    mDeleter(deleter ? deleter : DefaultDeleter)
  {
  }

  /**
   * @brief Destructor.
   * @SINCE_2_5.11
   */
  ~UniquePtr()
  {
    if(mPtr && mDeleter)
    {
      mDeleter(mPtr);
    }
  }

  /**
   * @brief Move constructor.
   * @SINCE_2_5.11
   * @param[in] other The object to be moved
   * @note *other* deleter is not changed
   */
  UniquePtr(UniquePtr&& other) noexcept
  : mPtr(other.mPtr),
    mDeleter(other.mDeleter ? other.mDeleter : DefaultDeleter)
  {
    other.mPtr = nullptr;
  }

  /**
   * @brief Move assignment operator.
   * @SINCE_2_5.11
   * @param[in] other The object to be moved
   * @note *other* deleter is not changed
   */
  UniquePtr& operator=(UniquePtr&& other) noexcept
  {
    if(this != &other)
    {
      // Release current ownership
      if(mPtr && mDeleter)
      {
        mDeleter(mPtr);
      }
      // Take ownership of the new object
      mPtr       = other.mPtr;
      mDeleter   = other.mDeleter ? other.mDeleter : DefaultDeleter;
      other.mPtr = nullptr;
    }
    return *this;
  }

  /**
   * @brief Converting move constructor from derived type.
   *
   * Constructs a UniquePtr<Base> from a UniquePtr<Derived>.
   *
   * @tparam U The derived type
   * @tparam E The deleter type of the source
   * @param[in] other The derived UniquePtr to move from
   * @note Enabled only if U* is convertible to Type*
   */
  template<typename U, typename E, typename = typename EnableIf<IsConvertible<U*, Type*>::value>::type>
  UniquePtr(UniquePtr<U, E>&& other) noexcept
  : mPtr(other.Release()),
    mDeleter(Deleter(other.mDeleter) ? Deleter(other.mDeleter) : DefaultDeleter)
  {
  }

  /**
   * @brief Converting move assignment operator from derived type.
   *
   * Assigns a UniquePtr<Base> from a UniquePtr<Derived>.
   *
   * @tparam U The derived type
   * @tparam E The deleter type of the source
   * @param[in] other The derived UniquePtr to move from
   * @return Reference to this object
   * @note Enabled only if U* is convertible to Type*
   */
  template<typename U, typename E, typename = typename EnableIf<IsConvertible<U*, Type*>::value>::type>
  UniquePtr& operator=(UniquePtr<U, E>&& other) noexcept
  {
    // Release current ownership
    if(mPtr && mDeleter)
    {
      mDeleter(mPtr);
    }
    // Take ownership of the new object
    mPtr     = other.Release();
    mDeleter = Deleter(other.mDeleter) ? Deleter(other.mDeleter) : DefaultDeleter;
    return *this;
  }

  UniquePtr(const UniquePtr&)            = delete; ///< Deleted copy constructor
  UniquePtr& operator=(const UniquePtr&) = delete; ///< Deleted copy assignment operator

  /**
   * @brief Overloaded dereference operator.
   * @SINCE_2_5.11
   * @return A reference to the object
   */
  Type& operator*()
  {
    return *mPtr;
  }

  /**
   * @brief Overloaded const dereference operator.
   * @SINCE_2_5.11
   * @return A const reference to the object
   */
  const Type& operator*() const
  {
    return *mPtr;
  }

  /**
   * @brief Overloaded pointer operator.
   * @SINCE_2_5.11
   * @return A pointer to the object
   */
  Type* operator->()
  {
    return mPtr;
  }

  /**
   * @brief Overloaded pointer to const operator.
   * @SINCE_2_5.11
   * @return A pointer to a const object
   */
  const Type* operator->() const
  {
    return mPtr;
  }

  /**
   * @brief Explicit conversion to bool.
   * @SINCE_2_5.11
   */
  explicit operator bool() const
  {
    return mPtr != nullptr;
  }

  /**
   * @brief Get the raw pointer.
   * @SINCE_2_5.11
   * @return The raw pointer to the object
   */
  Type* Get()
  {
    return mPtr;
  }

  /**
   * @brief Get the raw pointer to const object.
   * @SINCE_2_5.11
   * @return The raw pointer to the const object
   */
  const Type* Get() const
  {
    return mPtr;
  }

  /**
   * @brief Release ownership of the managed object.
   * @SINCE_2_5.11
   * @return A pointer to the object that has been released
   * @note The deleter remains the same as before
   */
  Type* Release()
  {
    Type* temp = mPtr;
    mPtr       = nullptr;
    return temp;
  }

  /**
   * @brief Reset the managed object.
   * @SINCE_2_5.11
   * @param[in] ptr The object to take ownership of
   * @note The deleter remains the same as before
   */
  void Reset(Type* ptr = nullptr)
  {
    // Delete the current object
    if(mPtr && mDeleter)
    {
      mDeleter(mPtr);
    }
    mPtr = ptr; // Take ownership of the new object
  }

private:
  Type*   mPtr;     ///< The managed object
  Deleter mDeleter; ///< The custom deleter
};

/**
 * @brief Templated unique pointer partial specialization with functor deleters (non-function-pointer types).
 *
 * @tparam Type The type of the pointer stored
 * @tparam Deleter The functor to delete the object
 *
 * @SINCE_2_5.11
 */
// Partial specialization for functor deleters (non-function-pointer types)
template<typename Type, typename Deleter>
class UniquePtr<Type, Deleter, typename EnableIf<!IsFunctionPointer<Deleter>::value>::type>
{
public:
  // Allow converting constructors to access private members from other template instantiations
  template<typename, typename, typename>
  friend class UniquePtr;

  /**
   * @brief Constructor that takes ownership of a raw pointer.
   * @SINCE_2_5.11
   * @param[in] ptr Raw pointer to take ownership of
   * @param[in] deleter The functor to delete the *ptr*
   * @note For functor deleters, the deleter parameter is required
   */
  explicit UniquePtr(Type* ptr, Deleter deleter)
  : mPtr(ptr),
    mDeleter(deleter)
  {
  }

  /**
   * @brief Destructor.
   * @SINCE_2_5.11
   */
  ~UniquePtr()
  {
    if(mPtr)
    {
      mDeleter(mPtr);
    }
  }

  /**
   * @brief Move constructor.
   * @SINCE_2_5.11
   * @param[in] other The object to be moved
   * @note *other* deleter is not changed but is copied
   */
  UniquePtr(UniquePtr&& other) noexcept
  : mPtr(other.mPtr),
    mDeleter(other.mDeleter)
  {
    other.mPtr = nullptr;
  }

  /**
   * @brief Move assignment operator.
   * @SINCE_2_5.11
   * @param[in] other The object to be moved
   * @note *other* deleter is not changed
   */
  UniquePtr& operator=(UniquePtr&& other) noexcept
  {
    if(this != &other)
    {
      // Release current ownership
      if(mPtr)
      {
        mDeleter(mPtr);
      }
      // Take ownership of the new object
      mPtr       = other.mPtr;
      mDeleter   = other.mDeleter;
      other.mPtr = nullptr;
    }
    return *this;
  }

  /**
   * @brief Converting move constructor from derived type.
   *
   * Constructs a UniquePtr<Base> from a UniquePtr<Derived>.
   *
   * @tparam U The derived type
   * @tparam E The deleter type of the source
   * @param[in] other The derived UniquePtr to move from
   * @note Enabled only if U* is convertible to Type*
   */
  template<typename U, typename E, typename = typename EnableIf<IsConvertible<U*, Type*>::value>::type>
  UniquePtr(UniquePtr<U, E>&& other) noexcept
  : mPtr(other.Release()),
    mDeleter(other.mDeleter)
  {
  }

  /**
   * @brief Converting move assignment operator from derived type.
   *
   * Assigns a UniquePtr<Base> from a UniquePtr<Derived>.
   *
   * @tparam U The derived type
   * @tparam E The deleter type of the source
   * @param[in] other The derived UniquePtr to move from
   * @return Reference to this object
   * @note Enabled only if U* is convertible to Type*
   */
  template<typename U, typename E, typename = typename EnableIf<IsConvertible<U*, Type*>::value>::type>
  UniquePtr& operator=(UniquePtr<U, E>&& other) noexcept
  {
    // Release current ownership
    if(mPtr)
    {
      mDeleter(mPtr);
    }
    // Take ownership of the new object
    mPtr     = other.Release();
    mDeleter = other.mDeleter;
    return *this;
  }

  UniquePtr()                            = delete; ///< Deleted default constructor
  UniquePtr(const UniquePtr&)            = delete; ///< Deleted copy constructor
  UniquePtr& operator=(const UniquePtr&) = delete; ///< Deleted copy assignment operator

  /**
   * @brief Overloaded dereference operator.
   * @SINCE_2_5.11
   * @return A reference to the object
   */
  Type& operator*()
  {
    return *mPtr;
  }

  /**
   * @brief Overloaded const dereference operator.
   * @SINCE_2_5.11
   * @return A const reference to the object
   */
  const Type& operator*() const
  {
    return *mPtr;
  }

  /**
   * @brief Overloaded pointer operator.
   * @SINCE_2_5.11
   * @return A pointer to the object
   */
  Type* operator->()
  {
    return mPtr;
  }

  /**
   * @brief Overloaded pointer to const operator.
   * @SINCE_2_5.11
   * @return A pointer to a const object
   */
  const Type* operator->() const
  {
    return mPtr;
  }

  /**
   * @brief Explicit conversion to bool.
   * @SINCE_2_5.11
   */
  explicit operator bool() const
  {
    return mPtr != nullptr;
  }

  /**
   * @brief Get the raw pointer.
   * @SINCE_2_5.11
   * @return The raw pointer to the object
   */
  Type* Get()
  {
    return mPtr;
  }

  /**
   * @brief Get the raw pointer to const object.
   * @SINCE_2_5.11
   * @return The raw pointer to the const object
   */
  const Type* Get() const
  {
    return mPtr;
  }

  /**
   * @brief Release ownership of the managed object.
   * @SINCE_2_5.11
   * @return A pointer to the object that has been released
   * @note The deleter remains the same as before
   */
  Type* Release()
  {
    Type* temp = mPtr;
    mPtr       = nullptr;
    return temp;
  }

  /**
   * @brief Reset the managed object.
   * @SINCE_2_5.11
   * @param[in] ptr The object to take ownership of
   * @note The deleter remains the same as before
   */
  void Reset(Type* ptr = nullptr)
  {
    // Delete the current object
    if(mPtr)
    {
      mDeleter(mPtr);
    }
    mPtr = ptr; // Take ownership of the new object
  }

private:
  Type*   mPtr;     ///< The managed object
  Deleter mDeleter; ///< The custom deleter
};

/**
 * @brief Creates a UniquePtr that manages a new object.
 *
 * @tparam Type The type of the object to construct
 * @tparam Args The types of arguments to forward to the constructor
 * @param[in] args The arguments to forward to the constructor of Type
 * @return UniquePtr<Type> managing the newly created object
 *
 * @SINCE_2_5.11
 */
template<typename Type, typename... Args>
UniquePtr<Type> MakeUnique(Args&&... args)
{
  return UniquePtr<Type>(new Type(Forward<Args>(args)...));
}

/**
 * @}
 */

} // namespace Dali

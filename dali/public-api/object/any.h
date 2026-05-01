#ifndef DALI_ANY_TYPE_H
#define DALI_ANY_TYPE_H

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
#include <cstddef>  // NULL
#include <typeinfo> // operator typeid

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/object/type-info-id.h>

namespace Dali
{
/**
 * @addtogroup dali_core_object
 * @{
 */

/**
 * @brief Stores a value of any type.
 *
 * Examples of use:
 * \code{.cpp}
 * Any uintVariable = 5u;
 * Any floatVariable( 4.5f );
 * Any strVariable( Dali::String( "Hello world" ) );
 * uintVariable = 1u;
 * uint32_t variable = AnyCast< uint32_t >( uintVariable );
 * if ( uintVariable.IsType< int >() )
 * \endcode
 * @SINCE_1_0.0
 */
class Any
{
public:
  /**
   * @brief Default constructor.
   * @SINCE_1_0.0
   */
  DALI_CORE_API Any();

  /**
   * @brief Destructor. Free resources.
   * @SINCE_1_0.0
   */
  DALI_CORE_API ~Any();

  /**
   * @brief Passes Assert message.
   *
   * @SINCE_1_0.0
   * @param[in] assertMessage Assert message to report
   */
  DALI_CORE_API static void AssertAlways(const char* assertMessage);

  /**
   * @brief Constructs a Any type with the given value.
   *
   * @SINCE_1_0.0
   * @param[in] value The given value
   */
  template<typename Type>
  Any(const Type& value)
  : mContainer(new AnyContainerImpl<Type>(value))
  {
  }

  /**
   * @brief Copy Constructor.
   * @SINCE_1_0.0
   * @param[in] any Any to be copied
   */
  Any(const Any& any)
  {
    // If container isn't empty then copy the container?
    if(nullptr != any.mContainer)
    {
      mContainer = any.mContainer->mCloneFunc(*any.mContainer);
    }
    else
    {
      // Otherwise mark new container as empty
      mContainer = nullptr;
    }
  }

  /**
   * @brief Move Constructor.
   * @SINCE_2_1.44
   * @param[in] any Any to be moved
   */
  Any(Any&& any) noexcept
  {
    mContainer = any.mContainer;

    // Remove input value's container.
    any.mContainer = nullptr;
  }

  /**
   * @brief Assigns a given value to the Any type.
   *
   * @SINCE_1_0.0
   * @param[in] value The given value
   * @return A reference to this
   * @note If the types are different, then the current container will be re-created.
   */
  template<typename Type>
  Any& operator=(const Type& value)
  {
    // If the container is empty then assign the new value
    if(nullptr == mContainer)
    {
      mContainer = new AnyContainerImpl<Type>(value);
    }
    else
    {
      // Check to see if this type is compatible with current container?
      if(mContainer->GetType() == TypeInfoIdFromTypeid<Type>())
      {
        // Same type so just set the new value
        static_cast<AnyContainerImpl<Type>*>(mContainer)->SetValue(value);
      }
      else
      {
        // Incompatible types, so delete old container and assign a new one with this type and value
        mContainer->mDeleteFunc(mContainer);
        mContainer = new AnyContainerImpl<Type>(value);
      }
    }
    return *this;
  }

  /**
   * @brief Assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] any Any to be assigned which contains a value of identical type to current contents.
   * @return A reference to this
   * @exception DaliException If parameter any is of a different type.
   */
  DALI_CORE_API Any& operator=(const Any& any);

  /**
   * @brief Move assignment operator.
   *
   * @SINCE_2_1.44
   * @param[in] any Any to be moved which contains a value of identical type to current contents.
   * @return A reference to this
   */
  DALI_CORE_API Any& operator=(Any&& any) noexcept;

  /**
   * @brief Gets a value of type Type from container.
   *
   * @SINCE_1_0.0
   * @param[in] type destination of type Type to write to
   */
  template<typename Type>
  void Get(Type& type) const
  {
    type = Get<Type>();
  }

  /**
   * @brief Returns the type identifier of the stored value.
   *
   * @SINCE_1_0.0
   * @return The TypeInfoId of the stored value, or a default-constructed
   * TypeInfoId (representing void) if there is no value stored
   */
  DALI_CORE_API const TypeInfoId& GetType() const;

  /**
   * @brief Checks if the stored value is of type T.
   *
   * This method provides a type-safe way to query the type of the stored value
   * without using RTTI directly. It is the recommended way to check types
   * before calling Get\<T\>() or AnyCast\<T\>() to avoid exceptions.
   *
   * @note Use this method instead of comparing GetType() with DALI_TYPE_ID(T).
   * The IsType\<T\>() method handles the type name resolution internally and
   * works correctly even when T is a template parameter.
   *
   * Typical use cases:
   * - Checking if an Any value can be safely retrieved as a specific type
   * - Implementing type-safe visitors for Any values
   * - Conditional processing based on the stored type
   *
   * Example:
   * \code{.cpp}
   * Any value = 42;
   * if(value.IsType<int>())
   * {
   *   int intValue = AnyCast<int>(value);  // Safe, no exception
   * }
   * else if(value.IsType<float>())
   * {
   *   float floatValue = AnyCast<float>(value);
   * }
   * \endcode
   *
   * @SINCE_2_5.21
   * @tparam T The type to check against
   * @return true if the stored value is of type T, false otherwise (including if empty)
   */
  template<typename T>
  bool IsType() const
  {
    if(!mContainer) return false;
    return mContainer->GetType() == TypeInfoIdFromTypeid<T>();
  }

  /**
   * @brief Retrieves the stored value in the Any type.
   *
   * @SINCE_1_0.0
   * @return The stored value
   */
  template<typename Type>
  const Type& Get() const
  {
    if(nullptr == mContainer)
    {
      AssertAlways("Any::Get(). mContainer is NULL");
    }
    else if(mContainer->GetType() != TypeInfoIdFromTypeid<Type>()) // Check if the value has the same value than the Any type.
    {
      AssertAlways("Any::Get(). Trying to retrieve a value of a different type than the template one.");
    }
    return static_cast<AnyContainerImpl<Type>*>(mContainer)->GetValue();
  }

  /**
   * @brief Returns pointer of Type to the value stored.
   *
   * @SINCE_1_0.0
   * @return pointer to the value, or NULL if no value is contained
   */
  template<typename Type>
  Type* GetPointer()
  {
    if(nullptr == mContainer)
    {
      return NULL;
    }
    // Check if the value has the same value than the Any type.
    if(mContainer->GetType() != TypeInfoIdFromTypeid<Type>())
    {
      AssertAlways("Any::GetPointer(). Trying to retrieve a pointer to a value of a different type than the template one.");
    }
    return static_cast<AnyContainerImpl<Type>*>(mContainer)->GetPointerToValue();
  }

  /**
   * @brief Returns pointer of Type to the value stored.
   *
   * @SINCE_1_0.0
   * @return pointer to the value, or NULL if no value is contained
   */
  template<typename Type>
  const Type* GetPointer() const
  {
    if(nullptr == mContainer)
    {
      return NULL;
    }
    // Check if the value has the same value than the Any type.
    if(mContainer->GetType() != TypeInfoIdFromTypeid<Type>())
    {
      AssertAlways("Any::GetPointer(). Trying to retrieve a pointer to a value of a different type than the template one.");
    }
    return static_cast<AnyContainerImpl<Type>*>(mContainer)->GetPointerToValue();
  }

  /**
   * @brief Returns whether container holds a value.
   *
   * @SINCE_1_0.0
   * @return @c true if the container is empty, else @c false
   */
  bool Empty() const
  {
    return (nullptr == mContainer) ? true : false;
  }

  struct AnyContainerBase; // Forward declaration for typedef
  using CloneFunc = AnyContainerBase* (*)(const AnyContainerBase&);

  using DeleteFunc = void (*)(const AnyContainerBase*);

  /**
   * @brief Base container to hold type for match verification and instance cloning function.
   *
   * @SINCE_1_0.0
   */
  struct AnyContainerBase
  {
    /**
     * @brief Constructor of base container.
     *
     * @SINCE_1_0.0
     * @param[in] type TypeInfoId of container
     * @param[in] cloneFunc Cloning function to replicate this container type
     * @param[in] deleteFunc Deleting function to destroy this container type
     */
    AnyContainerBase(TypeInfoId type, CloneFunc cloneFunc, DeleteFunc deleteFunc)
    : mType(type),
      mCloneFunc(cloneFunc),
      mDeleteFunc(deleteFunc)
    {
    }

    /**
     * @brief Gets the TypeInfoId of this container.
     *
     * @SINCE_1_0.0
     * @return Type
     */
    const TypeInfoId& GetType() const
    {
      return mType;
    }

    TypeInfoId mType;       // typeID
    CloneFunc  mCloneFunc;  // cloning function for this container
    DeleteFunc mDeleteFunc; // deleting function for this container
  };

  /**
   * @brief Templated Clone function from container base.
   *
   * @SINCE_1_0.0
   * @param[in] base The reference to container
   */
  template<typename Type>
  struct AnyContainerImplCloner
  {
    static AnyContainerBase* Clone(const AnyContainerBase& base)
    {
      return new AnyContainerImpl<Type>(static_cast<AnyContainerImpl<Type> >(base));
    }
  };

  /**
   * @brief Templated Delete function from container base.
   *
   * @SINCE_1_0.0
   * @param[in] base The pointer to container
   */
  template<typename Type>
  struct AnyContainerImplDelete
  {
    static void Delete(const AnyContainerBase* base)
    {
      delete(static_cast<const AnyContainerImpl<Type>*>(base));
    }
  };

  /**
   * @brief Templated class to hold value for type.
   *
   * @SINCE_1_0.0
   */
  template<typename Type>
  class AnyContainerImpl : public AnyContainerBase
  {
  public:
    /**
     * @brief Constructor to create container holding value of type Type.
     *
     * @SINCE_1_0.0
     * @param[in] value Value of Type
     */
    AnyContainerImpl(const Type& value)
    : AnyContainerBase(TypeInfoIdFromTypeid<Type>(),
                       static_cast<CloneFunc>(&AnyContainerImplCloner<Type>::Clone),
                       static_cast<DeleteFunc>(&AnyContainerImplDelete<Type>::Delete)),
      mValue(value)
    {
    }

    /**
     * @brief Constructor to create new container of type from and existing container (cloning).
     *
     * @SINCE_1_0.0
     * @param[in] base The reference to base container to copy from
     */
    AnyContainerImpl(const AnyContainerBase& base)
    : AnyContainerBase(TypeInfoIdFromTypeid<Type>(),
                       static_cast<CloneFunc>(&AnyContainerImplCloner<Type>::Clone),
                       static_cast<DeleteFunc>(&AnyContainerImplDelete<Type>::Delete))
    {
      mValue = static_cast<const AnyContainerImpl&>(base).GetValue();
    }

    /**
     * @brief Gets the container's stored value.
     *
     * @SINCE_1_0.0
     * @return Value of type Type
     */
    const Type& GetValue() const
    {
      return mValue;
    }

    /**
     * @brief Sets the container's stored value.
     *
     * @SINCE_1_0.0
     * @param[in] value Value of type Type
     */
    void SetValue(const Type& value)
    {
      mValue = value;
    }

    /**
     * @brief Gets a pointer to the value held.
     *
     * @SINCE_1_0.0
     * @return Pointer to the value of type Type
     */
    Type* GetPointerToValue()
    {
      return static_cast<Type*>(&mValue);
    }

    /**
     * @brief Gets a pointer to the value held.
     *
     * @SINCE_1_0.0
     * @return Pointer to the value of type Type
     */
    const Type* GetPointerToValue() const
    {
      return static_cast<const Type*>(&mValue);
    }

  private:
    Type mValue;
  };

private:
  /// @cond internal
  /**
   * @brief Demangles a typeid name to a human-readable class name.
   *
   * @note DALI_CORE_API is required because this is called by the inline
   * template TypeInfoIdFromTypeid<T>(), which is instantiated in user code.
   * On Windows DLLs, the symbol must be exported for the linker to find it.
   * Without DALI_CORE_API, user code calling IsType<T>() or AnyCast<T>()
   * would fail to link on Windows.
   *
   * @param[in] mangledName The mangled type name (e.g., from typeid(T).name())
   * @return The demangled, human-readable type name
   */
  static DALI_CORE_API Dali::String DemangleTypeName(const char* mangledName);

  /**
   * @brief Constructs a TypeInfoId from typeid(T), with proper demangling.
   *
   * This helper is used by the Any class template methods where Type is a
   * template parameter and DALI_TYPE_ID(Type) cannot be used (because the
   * preprocessor cannot stringify a template parameter).
   *
   * The demangled name is cached per-type per-translation-unit (static local)
   * to avoid repeated demangling calls. The cost is paid once on the first
   * Any<T> construction in each translation unit; subsequent calls in that
   * TU are a single load of a static reference. The const char* stored in
   * the returned TypeInfoId points into the static Dali::String buffer,
   * which has program-lifetime storage duration.
   *
   * Demangling consistency: on GCC, typeid(int).name() returns "i" (mangled).
   * DemangleTypeName converts this to "int", which matches what
   * DALI_TYPE_ID(int) produces via \#Type stringification. This ensures
   * that TypeInfoIdFromTypeid\<int\>() == DALI_TYPE_ID(int).
   *
   * @tparam T The C++ type to create a TypeInfoId for
   * @return A const reference to a cached TypeInfoId representing type T
   */
  template<typename T>
  static const TypeInfoId& TypeInfoIdFromTypeid()
  {
    static const Dali::String demangledName = DemangleTypeName(typeid(T).name());
    static const TypeInfoId   cached(Dali::HashFNV1a(demangledName.CStr()), demangledName.CStr());
    return cached;
  }
  /// @endcond

  AnyContainerBase* mContainer;
};

/**
 * AnyCast helper functions
 */

/**
 * @brief Extracts a pointer to the held type of an Any object from a pointer to that Any object (NULL if empty).
 *
 * @SINCE_1_0.0
 * @param[in] any Pointer to an Any object
 *
 * @return Pointer to the Type held
 */
template<typename Type>
inline Type* AnyCast(Any* any)
{
  return any->GetPointer<Type>();
}

/**
 * @brief Extracts a const pointer to the held type of an Any object from a pointer to that Any object (NULL if empty).
 *
 * @SINCE_1_0.0
 * @param[in] any const Pointer to an Any object
 *
 * @return const Pointer to the Type held
 */
template<typename Type>
inline const Type* AnyCast(const Any* any)
{
  return any->GetPointer<Type>();
}

/**
 * @brief Extracts a held value of type Type from an Any object from a reference to that Any object.
 *
 * @SINCE_1_0.0
 * @param[in] any The reference to an Any object
 *
 * @return Type value of type Type
 */
template<typename Type>
inline Type AnyCast(Any& any)
{
  return any.Get<Type>();
}

/**
 * @brief Extracts a held value of type Type from an Any object from a const reference to that Any object.
 *
 * @SINCE_1_0.0
 * @param[in] any The reference to an Any object
 *
 * @return Type value of type Type
 */
template<typename Type>
inline Type AnyCast(const Any& any)
{
  return any.Get<Type>();
}

/**
 * @brief Extracts a reference to the held value of type Type from an Any object from a reference to that Any object
 *
 * @SINCE_1_0.0
 * @param[in] any The reference to an Any object
 *
 * @return A reference to the Type value of type Type
 */
template<typename Type>
inline Type& AnyCastReference(Any& any)
{
  return any.Get<Type>();
}

/**
 * @brief Extracts a const reference to the held value of type Type from an Any object from a const reference to that Any object.
 *
 * @SINCE_1_0.0
 * @param[in] any The reference to an Any object
 *
 * @return A const reference to the Type value of type Type
 */
template<typename Type>
inline const Type& AnyCastReference(const Any& any)
{
  return any.Get<Type>();
}

/**
 * @}
 */
} // namespace Dali

#endif // DALI_ANY_TYPE_H

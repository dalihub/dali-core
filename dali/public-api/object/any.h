#ifndef __DALI_ANY_TYPE_H__
#define __DALI_ANY_TYPE_H__

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

// EXTERNAL INCLUDES
#include <typeinfo>   // operator typeid
#include <cstddef>    // NULL

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

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
 * Any strVariable( std::string( "Hello world" ) );
 * uintVariable = 1u;
 * unsigned int variable = AnyCast< unsigned int >( uintVariable );
 * if ( typeid( int ) == uintVariable.GetType() )
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
  DALI_IMPORT_API Any();

  /**
   * @brief Destructor. Free resources.
   * @SINCE_1_0.0
   */
  DALI_IMPORT_API ~Any();

  /**
   * @brief Pass Assert message
   *
   * @SINCE_1_0.0
   * @param assertMessage Assert message to report
   */
  DALI_IMPORT_API static void AssertAlways( const char* assertMessage );

  /**
   * @brief Constructs a Any type with the given value.
   *
   * @SINCE_1_0.0
   * @param[in] value The given value.
   */
  template<typename Type>
  Any( const Type& value )
  : mContainer( new AnyContainerImpl<Type>( value ) )
  {
  }

  /**
   * @brief Copy Constructor.
   * @SINCE_1_0.0
   * @param [in] any Any to be copied.
   */
  Any( const Any& any )
  {
    // If container isn't empty then copy the container?
    if ( NULL != any.mContainer )
    {
      mContainer = any.mContainer->mCloneFunc( *any.mContainer );
    }
    else
    {
      // Otherwise mark new container as empty
      mContainer = NULL;
    }
  }

  /**
   * @brief Assigns a given value to the Any type.
   *
   * @SINCE_1_0.0
   * @param[in] value The given value.
   * @note If the types are different, then the current container will be re-created.
   *
   */
  template<typename Type>
  Any& operator=( const Type& value )
  {
    // If the container is empty then assign the new value
    if ( NULL == mContainer )
    {
      mContainer = new AnyContainerImpl< Type >( value );
    }
    else
    {
      // Check to see if this type is compatible with current container?
      if ( mContainer->GetType() == typeid( Type ) )
      {
        // Same type so just set the new value
        static_cast< AnyContainerImpl< Type >* >( mContainer )->SetValue( value );
      }
      else
      {
        // Incompatible types, so delete old container and assign a new one with this type and value
        mContainer->mDeleteFunc( mContainer );
        mContainer = new AnyContainerImpl< Type >( value );
      }
    }
    return *this;
  }

  /**
   * @brief Assignment operator.
   *
   * @SINCE_1_0.0
   * @param [in] any Any to be assigned which contains a value of identical type to current contents.
   * @exception DaliException If parameter any is of a different type.
   *
   */
  DALI_IMPORT_API Any& operator=( const Any& any );

  /**
   * @brief Get a value of type Type from container
   *
   * @SINCE_1_0.0
   * @param type destination of type Type to write to
   */
  template<typename Type>
  void Get( Type& type ) const
  {
    type = Get<Type>();
  }

  /**
   * @brief Returns the type info of the stored value.
   *
   * @SINCE_1_0.0
   * @return The std::type_info of the stored value or the type info of the void
   * type if there is no value stored.
   */
  DALI_IMPORT_API const std::type_info& GetType() const;

  /**
   * @brief Retrieves the stored value in the Any type.
   *
   * @SINCE_1_0.0
   * @return The stored value.
   */
  template<typename Type>
  const Type& Get() const
  {

    if ( NULL == mContainer )
    {
      AssertAlways( "Any::Get(). mContainer is NULL" );
    }

    // Check if the value has the same value than the Any type.
    if( mContainer->GetType() != typeid( Type ) )
    {
      AssertAlways( "Any::Get(). Trying to retrieve a value of a different type than the template one." );
    }
    return static_cast< AnyContainerImpl< Type >* >( mContainer )->GetValue();
  }

  /**
   * @brief Return pointer of Type to the value stored
   *
   * @SINCE_1_0.0
   * @return pointer to the value or NULL if no value is contained
   */
  template<typename Type>
  Type* GetPointer()
  {
    if( NULL == mContainer )
    {
      return NULL;
    }
     // Check if the value has the same value than the Any type.
    if( mContainer->GetType() != typeid( Type ) )
    {
      AssertAlways( "Any::GetPointer(). Trying to retrieve a pointer to a value of a different type than the template one." );
    }
    return static_cast< AnyContainerImpl< Type >* >( mContainer )->GetPointerToValue();
  }

  /**
   * @brief Return pointer of Type to the value stored
   *
   * @SINCE_1_0.0
   * @return pointer to the value or NULL if no value is contained
   */
  template<typename Type>
  const Type* GetPointer() const
  {
    if( NULL == mContainer )
    {
      return NULL;
    }
     // Check if the value has the same value than the Any type.
    if( mContainer->GetType() != typeid( Type ) )
    {
      AssertAlways( "Any::GetPointer(). Trying to retrieve a pointer to a value of a different type than the template one." );
    }
    return static_cast< AnyContainerImpl< Type >* >( mContainer )->GetPointerToValue();
  }

  /**
   * @brief Returns whether container holds a value
   *
   * @SINCE_1_0.0
   * @return true if the container is empty, else false.
   */
  bool Empty() const
  {
    return ( NULL == mContainer ) ? true : false;
  }

  struct AnyContainerBase;    // Forward declaration for typedef
  typedef AnyContainerBase* (*CloneFunc)( const AnyContainerBase& base );
  typedef void (*DeleteFunc)( const AnyContainerBase* base );

  /**
   * @brief Base container to hold type for match verification and instance cloning function
   *
   * @SINCE_1_0.0
   */
  struct AnyContainerBase
  {
    /**
     * @brief Constructor of base container
     *
     * @SINCE_1_0.0
     * @param type typeid of container
     * @param cloneFunc Cloning function to replicate this container type
     * @param deleteFunc Deleting function to destroy this container type
     */
    AnyContainerBase( const std::type_info& type, CloneFunc cloneFunc, DeleteFunc deleteFunc )
    : mType( type ),
      mCloneFunc( cloneFunc ),
      mDeleteFunc( deleteFunc )
    {}

    /**
     * @brief Get the typeid of this container
     *
     * @SINCE_1_0.0
     * @return type
     */
    const std::type_info& GetType() const
    {
      return mType;
    }

    const::std::type_info& mType;       // typeID
    CloneFunc mCloneFunc;               // cloning function for this container
    DeleteFunc mDeleteFunc;             // deleting function for this container
  };


  /**
   * @brief Templated Clone function from container base
   *
   * @SINCE_1_0.0
   * @param base reference to container
   */
  template<typename Type>
  struct AnyContainerImplCloner
  {
    static AnyContainerBase* Clone( const AnyContainerBase& base )
    {
      return new AnyContainerImpl< Type >( static_cast< AnyContainerImpl< Type > >( base ) );
    }
  };

  /**
   * @brief Templated Delete function from container base
   *
   * @SINCE_1_0.0
   * @param base pointer to container
   */
  template<typename Type>
  struct AnyContainerImplDelete
  {
    static void Delete( const AnyContainerBase* base )
    {
      delete ( static_cast< const AnyContainerImpl< Type >* > ( base ) );
    }
  };

  /**
   * @brief Templated class to hold value for type
   *
   * @SINCE_1_0.0
   */
  template<typename Type>
  class AnyContainerImpl : public AnyContainerBase
  {
  public:

    /**
     * @brief Constructor to create container holding value of type Type
     *
     * @SINCE_1_0.0
     * @param value Value of Type
     */
    AnyContainerImpl( const Type& value )
    : AnyContainerBase( typeid( Type ),
                        static_cast< CloneFunc >( &AnyContainerImplCloner< Type >::Clone ),
                        static_cast< DeleteFunc >( &AnyContainerImplDelete< Type >::Delete ) ),
                        mValue( value )
    {}

    /**
     * @brief Constructor to create new container of type from and existing container (cloning)
     *
     * @SINCE_1_0.0
     * @param base reference to base container to copy from
     */
    AnyContainerImpl( const AnyContainerBase& base )
    : AnyContainerBase( typeid( Type ),
                        static_cast< CloneFunc >( &AnyContainerImplCloner< Type >::Clone ),
                        static_cast< DeleteFunc >( &AnyContainerImplDelete< Type >::Delete ) )
    {
      mValue = static_cast< const AnyContainerImpl& >( base ).GetValue();
    }

    /**
     * @brief Get the container's stored value
     *
     * @SINCE_1_0.0
     * @return value of type Type
     */
    const Type& GetValue() const
    {
      return mValue;
    }

    /**
     * @brief Set the container's stored value
     *
     * @SINCE_1_0.0
     * @param value of type Type
     */
    void SetValue( const Type& value )
    {
      mValue = value;
    }

    /**
     * @brief Get a pointer to the value held
     *
     * @SINCE_1_0.0
     * @return pointer to the value of type Type
     */
    Type* GetPointerToValue()
    {
      return static_cast< Type* >( &mValue );
    }

    /**
     * @brief Get a pointer to the value held
     *
     * @SINCE_1_0.0
     * @return pointer to the value of type Type
     */
    const Type* GetPointerToValue() const
    {
      return static_cast< const Type* >( &mValue );
    }

    private:
      Type mValue;
  };

  AnyContainerBase* mContainer;

};

/**
 * AnyCast helper functions
 */

/**
 * @brief Extract a pointer to the held type of an Any object from a pointer to that Any object (NULL if empty )
 *
 * @SINCE_1_0.0
 * @param any Pointer to an Any object
 *
 * @return Pointer to the Type held
 */
template<typename Type>
inline Type* AnyCast( Any* any )
{
  return any->GetPointer<Type>();
}

/**
 * @brief Extract a const pointer to the held type of an Any object from a pointer to that Any object (NULL if empty )
 *
 * @SINCE_1_0.0
 * @param any const Pointer to an Any object
 *
 * @return const Pointer to the Type held
 */
template<typename Type>
inline const Type* AnyCast( const Any* any )
{
  return any->GetPointer<Type>();
}

/**
 * @brief Extract a held value of type Type from an Any object from a reference to that Any object
 *
 * @SINCE_1_0.0
 * @param any reference to an Any object
 *
 * @return Type value of type Type
 */
template<typename Type>
inline Type AnyCast( Any& any )
{
  return any.Get<Type>();
}

/**
 * @brief Extract a held value of type Type from an Any object from a const reference to that Any object
 *
 * @SINCE_1_0.0
 * @param any reference to an Any object
 *
 * @return Type value of type Type
 */
template<typename Type>
inline Type AnyCast( const Any& any )
{
  return any.Get<Type>();
}

/**
 * @brief Extract a reference to the held value of type Type from an Any object from a reference to that Any object
 *
 * @SINCE_1_0.0
 * @param any reference to an Any object
 *
 * @return A reference to the Type value of type Type
 */
template<typename Type>
inline Type& AnyCastReference( Any& any )
{
  return any.Get<Type>();
}

/**
 * @brief Extract a const reference to the held value of type Type from an Any object from a const reference to that Any object
 *
 * @SINCE_1_0.0
 * @param any reference to an Any object
 *
 * @return A const reference to the Type value of type Type
 */
template<typename Type>
inline const Type& AnyCastReference( const Any& any )
{
  return any.Get<Type>();
}

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_ANY_TYPE_H__

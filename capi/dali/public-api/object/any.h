#ifndef __DALI_ANY_TYPE_H__
#define __DALI_ANY_TYPE_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/**
 * @addtogroup CAPI_DALI_OBJECT_MODULE
 * @{
 */

// EXTERNAL INCLUDES
#include <typeinfo>   // operator typeid


// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali DALI_IMPORT_API
{

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
 */
class Any
{
public:
  /**
   * Default constructor.
   */
  Any();

  /**
   * Destructor. Free resources.
   */
  ~Any();

/**
   * @brief Pass Assert message
   *
   * @param assertMessage Assert message to report
   */
  static void AssertAlways( const char* assertMessage );

  /**
   * @brief Constructs a Any type with the given value.
   *
   * @param[in] value The given value.
   */
  template<typename Type>
  Any( const Type& value )
  : mContainer( new AnyContainerImpl<Type>( value ) )
  {
  }

  /**
   * Copy Constructor.
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
   * @note If the types are different, then the current container will be re-created.
   *
   * @param[in] value The given value.
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
   * @exception DaliException If parameter any is of a different type.
   *
   * @param [in] any Any to be assigned which contains a value of identical type to current contents.
   */
  Any& operator=( const Any& any );

  /**
   * @brief Get a value of type Type from container
   *
   * @param type destination of type Type to write to
   */
  template<typename Type>
  void Get( Type& type )
  {
    type = Get<Type>();
  }

  /**
   * @brief Returns the type info of the stored value.
   *
   * @return The std::type_info of the stored value or the type info of the void
   * type if there is no value stored.
   */
  const std::type_info& GetType() const;

  /**
   * @brief Retrieves the stored value in the Any type.
   *
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
   * @brief Returns whether container holds a value
   *
   * @return true if the container is empty, else false.
   */
  bool Empty()
  {
    return ( NULL == mContainer ) ? true : false;
  }

  struct AnyContainerBase;    // Forward declaration for typedef
  typedef AnyContainerBase* (*CloneFunc)( const AnyContainerBase& base );
  typedef void (*DeleteFunc)( const AnyContainerBase* base );

  /**
   * Base container to hold type for match verification and instance cloning function
   *
   */
  struct AnyContainerBase
  {
    /**
     * @brief Constructor of base container
     *
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
   */
  template<typename Type>
  class AnyContainerImpl : public AnyContainerBase
  {
  public:

    /**
     * @brief Constructor to create container holding value of type Type
     *
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
     * @return value of type Type
     */
    const Type& GetValue() const
    {
      return mValue;
    }

    /**
     * @brief Set the container's stored value
     *
     * @param value of type Type
     */
    void SetValue( const Type& value )
    {
      mValue = value;
    }

    /**
     * @brief Get a pointer to the value held
     *
     * @return pointer to the value of type Type
     */
    Type* GetPointerToValue()
    {
      return static_cast< Type* >( &mValue );
    }

    private:
      Type mValue;
  };

  AnyContainerBase* mContainer;

};

/**
 * AnyCast helper functions ( replicates boost functionality, but without exception generation )
 */

/**
 * @brief Extract a pointer to the held type of an Any object from a pointer to that Any object (NULL if empty )
 *
 * @param any Pointer to an Any object
 *
 * @return Pointer to the Type held
 */
template<typename Type>inline Type* AnyCast( Any* any )
{
  return any->GetPointer<Type>();
}

/**
 * @brief Extract a const pointer to the held type of an Any object from a pointer to that Any object (NULL if empty )
 *
 * @param any const Pointer to an Any object
 *
 * @return const Pointer to the Type held
 */
template<typename Type>inline const Type* AnyCast( const Any* any )
{
  return any->GetPointer<Type>();
}

/**
 * @brief Extract a held value of type Type from an Any object from a reference to that Any object
 *
 * @param any reference to an Any object
 *
 * @return Type value of type Type
 */
template<typename Type>inline Type AnyCast( Any& any )
{
  return any.Get<Type>();
}

/**
 * @brief Extract a held value of type Type from an Any object from a const reference to that Any object
 *
 * @param any reference to an Any object
 *
 * @return Type value of type Type
 */
template<typename Type>inline Type AnyCast( const Any& any )
{
  return any.Get<Type>();
}

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_ANY_TYPE_H__

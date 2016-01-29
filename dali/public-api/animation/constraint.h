#ifndef __DALI_CONSTRAINT_H__
#define __DALI_CONSTRAINT_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/animation/constraint-source.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/object/property-input.h>
#include <dali/public-api/signals/callback.h>

namespace Dali
{
/**
 * @addtogroup dali_core_animation
 * @{
 */

class Handle;

namespace Internal DALI_INTERNAL
{
class ConstraintBase;
}

typedef Vector< PropertyInput* > PropertyInputContainer;

/**
 * @brief An abstract base class for Constraints.
 *
 * This can be used to constrain a property of an object, after animations have been applied.
 * Constraints are applied in the following order:
 *   - Constraints are applied to on-stage actors in a depth-first traversal.
 *   - For each actor, the constraints are applied in the same order as the calls to Apply().
 *   - Constraints are not applied to off-stage actors.
 *
 * Create a constraint using one of the New methods depending on the type of callback function used.
 * Try to use a C function unless some data needs to be stored, otherwise functors and class methods
 * are also supported.
 *
 * A constraint can be applied to an object in the following manner:
 *
 * @code
 * Handle handle = CreateMyObject();
 * Constraint constraint = Constraint::New< Vector3 >( handle, CONSTRAINING_PROPERTY_INDEX, &MyFunction );
 * constraint.AddSource( LocalSource( INPUT_PROPERTY_INDEX ) );
 * constraint.Apply();
 * @endcode
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API Constraint : public BaseHandle
{
public:

  /**
   * @brief Template for the Function that is called by the Constraint system.
   *
   * Supports:
   *  - C style functions
   *  - Static member methods of an object
   *  - Member functions of a particular class
   *  - Functors of a particular class
   *  - If a functor or method is provided, then a copy of the object is made.
   *
   * The expected signature of the callback should be:
   * @code
   *   void Function( P&, const PropertyInputContainer& );
   * @endcode
   *
   * The P& parameter is an in,out parameter which stores the current value of the property. The callback
   * should change this value to the desired one. The PropertyInputContainer is a const reference to the property inputs
   * added to the Constraint in the order they were added via AddSource().
   *
   * @tparam  P  The property type to constrain.
   * @SINCE_1_0.0
   */
  template< typename P >
  class DALI_INTERNAL Function : public CallbackBase
  {
  public:

    /**
     * @brief Constructor which connects to the provided C function (or a static member function).
     *
     * The expected signature of the function is:
     * @code
     *   void MyFunction( P&, const PropertyInputContainer& );
     * @endcode
     *
     * @SINCE_1_0.0
     * @param[in]  function  The function to call.
     */
    Function( void( *function )( P&, const PropertyInputContainer& ) )
    : CallbackBase( reinterpret_cast< CallbackBase::Function >( function ) ),
      mCopyConstructorDispatcher( NULL )
    {
    }

    /**
     * @brief Constructor which copies a function object and connects to the functor of that object.
     *
     * The function object should have a functor with the following signature:
     * @code
     *   void operator()( P&, const PropertyInputContainer& );
     * @endcode
     *
     * @SINCE_1_0.0
     * @param[in]  object  The object to copy.
     *
     * @tparam  T  The type of the object.
     */
    template< class T >
    Function( const T& object )
    : CallbackBase( reinterpret_cast< void* >( new T( object ) ), // copy the object
                    NULL, // uses operator() instead of member function
                    reinterpret_cast< CallbackBase::Dispatcher >( &FunctorDispatcher2< T, P&, const PropertyInputContainer& >::Dispatch ),
                    reinterpret_cast< CallbackBase::Destructor >( &Destroyer< T >::Delete ) ),
      mCopyConstructorDispatcher( reinterpret_cast< CopyConstructorDispatcher >( &ObjectCopyConstructorDispatcher< T >::Copy ) )
    {
    }

    /**
     * @brief Constructor which copies a function object and allows a connection to a member method.
     *
     * The object should have a method with the signature:
     * @code
     *   void MyObject::MyMethod( P&, const PropertyInputContainer& );
     * @endcode
     *
     * @SINCE_1_0.0
     * @param[in]  object          The object to copy.
     * @param[in]  memberFunction  The member function to call. This has to be a member of the same class.
     *
     * @tparam  T  The type of the object.
     */
    template< class T >
    Function( const T& object, void ( T::*memberFunction ) ( P&, const PropertyInputContainer& ) )
    : CallbackBase( reinterpret_cast< void* >( new T( object ) ), // copy the object
                    reinterpret_cast< CallbackBase::MemberFunction >( memberFunction ),
                    reinterpret_cast< CallbackBase::Dispatcher >( &Dispatcher2< T, P&, const PropertyInputContainer& >::Dispatch ),
                    reinterpret_cast< CallbackBase::Destructor >( &Destroyer< T >::Delete ) ),
      mCopyConstructorDispatcher( reinterpret_cast< CopyConstructorDispatcher >( &ObjectCopyConstructorDispatcher< T >::Copy ) )
    {
    }

    /**
     * @brief Clones the Function object.
     *
     * The object, if held by this object, is also copied.
     *
     * @SINCE_1_0.0
     * @return A pointer to a newly-allocation Function.
     */
    CallbackBase* Clone()
    {
      CallbackBase* callback = NULL;
      if ( mImpl && mImpl->mObjectPointer && mCopyConstructorDispatcher )
      {
        callback = new Function( mCopyConstructorDispatcher( reinterpret_cast< UndefinedClass* >( mImpl->mObjectPointer ) ) /* Copy the object */,
                                 mMemberFunction,
                                 mImpl->mMemberFunctionDispatcher,
                                 mImpl->mDestructorDispatcher,
                                 mCopyConstructorDispatcher );
      }
      else
      {
        callback = new Function( mFunction );
      }
      return callback;
    }

  private:

    /**
     * @brief Must not be declared.
     *
     * This is used so that no optimisations are done by the compiler when using void*.
     */
    class UndefinedClass;

    /**
     * @brief Used to call the function to copy the stored object
     * @SINCE_1_0.0
     */
    typedef UndefinedClass* (*CopyConstructorDispatcher) ( UndefinedClass* object );

    /**
     * @brief Copies the actual object in Constraint::Function.
     *
     * @tparam  T  The type of the object.
     * @SINCE_1_0.0
     */
    template< class T >
    struct ObjectCopyConstructorDispatcher
    {
      /**
       * @brief Copy the object stored in Constraint::Function.
       *
       * @SINCE_1_0.0
       * @param[in]  object  The object to copy.
       *
       * @return Newly allocated clone of the object.
       */
      static UndefinedClass* Copy( const UndefinedClass* object )
      {
        T* copy = new T( *( reinterpret_cast< const T* >( object ) ) );
        return reinterpret_cast< UndefinedClass* >( copy );
      }
    };

    /**
     * @brief Undefined copy constructor
     * @SINCE_1_0.0
     */
    Function( const Function& );

    /**
     * @brief Undefined assignment operator
     * @SINCE_1_0.0
     */
    Function& operator=( const Function& );

    /**
     * @brief Constructor used when copying the stored object.
     *
     * @SINCE_1_0.0
     * @param[in]  object                     A newly copied object
     * @param[in]  memberFunction             The member function of the object.
     * @param[in]  dispatcher                 Used to call the actual object.
     * @param[in]  destructor                 Used to delete the owned object.
     * @param[in]  copyConstructorDispatcher  Used to create a copy of the owned object.
     */
    Function( void* object,
              CallbackBase::MemberFunction memberFunction,
              CallbackBase::Dispatcher dispatcher,
              CallbackBase::Destructor destructor,
              CopyConstructorDispatcher copyConstructorDispatcher )
    : CallbackBase( object, memberFunction, dispatcher, destructor ),
      mCopyConstructorDispatcher( copyConstructorDispatcher )
    {
    }

    /**
     * @brief Constructor used when copying a simple stored function.
     *
     * @SINCE_1_0.0
     * @param[in]  function   The function to call.
     */
    Function( CallbackBase::Function function )
    : CallbackBase( function ),
      mCopyConstructorDispatcher( NULL )
    {
    }

    // Data

    CopyConstructorDispatcher mCopyConstructorDispatcher; ///< Function to call to copy the stored object
  };

  /**
   * @brief The action that will happen when the constraint is removed.
   *
   * The final value may be "baked" i.e. saved permanently.
   * Alternatively the constrained value may be discarded when the constraint is removed.
   * @SINCE_1_0.0
   */
  enum RemoveAction
  {
    Bake,   ///< When the constraint is fully-applied, the constrained value is saved. @SINCE_1_0.0
    Discard ///< When the constraint is removed, the constrained value is discarded. @SINCE_1_0.0
  };

  static const RemoveAction  DEFAULT_REMOVE_ACTION;  ///< Bake

  /**
   * @brief Create an uninitialized Constraint; this can be initialized with Constraint::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   * @SINCE_1_0.0
   */
  Constraint();

  /**
   * @brief Create a constraint which targets a property using a function or a static class member.
   *
   * The expected signature, for a Vector3 type for example, of the function is:
   * @code
   *   void MyFunction( Vector3&, const PropertyInputContainer& );
   * @endcode
   *
   * Create the constraint with this function as follows:
   * @code
   *   Constraint constraint = Constraint::New< Vector3 >( handle, CONSTRAINING_PROPERTY_INDEX, &MyFunction );
   * @endcode
   *
   * @SINCE_1_0.0
   * @param[in]  handle       The handle to the property-owning object.
   * @param[in]  targetIndex  The index of the property to constrain.
   * @param[in]  function     The function to call to set the constrained property value.
   * @return The new constraint.
   *
   * @tparam P The type of the property to constrain.
   */
  template< class P >
  static Constraint New( Handle handle, Property::Index targetIndex, void( *function )( P&, const PropertyInputContainer& ) )
  {
    CallbackBase* callback = new Constraint::Function< P >( function );
    return New( handle, targetIndex, PropertyTypes::Get< P >(), callback );
  }

  /**
   * @brief Create a constraint which targets a property using a functor object.
   *
   * The expected structure, for a Vector3 type for example, of the functor object is:
   * @code
   *   struct MyObject
   *   {
   *     void operator() ( Vector3&, const PropertyInputContainer& );
   *   };
   * @endcode
   *
   * Create the constraint with this object as follows:
   * @code
   *   Constraint constraint = Constraint::New< Vector3 >( handle, CONSTRAINING_PROPERTY_INDEX, MyObject() );
   * @endcode
   *
   * @SINCE_1_0.0
   * @param[in]  handle       The handle to the property-owning object.
   * @param[in]  targetIndex  The index of the property to constrain.
   * @param[in]  object       The functor object whose functor is called to set the constrained property value.
   * @return The new constraint.
   *
   * @tparam P The type of the property to constrain.
   * @tparam T The type of the object.
   */
  template< class P, class T >
  static Constraint New( Handle handle, Property::Index targetIndex, const T& object )
  {
    CallbackBase* function = new Constraint::Function< P >( object );
    return New( handle, targetIndex, PropertyTypes::Get< P >(), function );
  }

  /**
   * @brief Create a constraint which targets a property using an object method.
   *
   * The expected structure, for a Vector3 type for example, of the object is:
   * @code
   *   struct MyObject
   *   {
   *     void MyMethod( Vector3&, const PropertyInputContainer& );
   *   };
   * @endcode
   *
   * Create the constraint with this object as follows:
   * @code
   *   Constraint constraint = Constraint::New< Vector3 >( handle, CONSTRAINING_PROPERTY_INDEX, MyObject(), &MyObject::MyMethod );
   * @endcode
   *
   * @SINCE_1_0.0
   * @param[in]  handle          The handle to the property-owning object.
   * @param[in]  targetIndex     The index of the property to constrain.
   * @param[in]  object          The object whose member function is called to set the constrained property value.
   * @param[in]  memberFunction  The member function to call to set the constrained property value.
   * @return The new constraint.
   *
   * @tparam P The type of the property to constrain.
   * @tparam T The type of the object.
   */
  template< class P, class T >
  static Constraint New( Handle handle, Property::Index targetIndex, const T& object, void ( T::*memberFunction ) ( P&, const PropertyInputContainer& ) )
  {
    CallbackBase* function = new Constraint::Function< P >( object, memberFunction );
    return New( handle, targetIndex, PropertyTypes::Get< P >(), function );
  }

  /**
   * @brief Creates a clones of this constraint for another object.
   *
   * @SINCE_1_0.0
   * @param[in]  handle  The handle to the property-owning object this constraint is to be cloned for.
   *
   * @return The new constraint.
   */
  Constraint Clone( Handle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~Constraint();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param [in]  constraint  A reference to the copied handle
   */
  Constraint( const Constraint& constraint );

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  Constraint& operator=( const Constraint& rhs );

  /**
   * @brief Downcast an Object handle to Constraint handle.
   *
   * If handle points to a Constraint object the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @SINCE_1_0.0
   * @param[in]  baseHandle  to An object
   * @return handle to a Constraint object or an uninitialized handle
   */
  static Constraint DownCast( BaseHandle baseHandle );

  /**
   * @brief Adds a constraint source to the constraint
   *
   * @SINCE_1_0.0
   * @param[in] source The constraint source input to add
   */
  void AddSource( ConstraintSource source );

  /**
   * @brief Applies this constraint.
   *
   * @SINCE_1_0.0
   * @pre The constraint must be initialized
   * @pre The target object must still be alive
   * @pre The source inputs should not have been destroyed
   */
  void Apply();

  /**
   * @brief Removes this constraint.
   * @SINCE_1_0.0
   */
  void Remove();

  /**
   * @brief Retrieve the object which this constraint is targeting.
   *
   * @SINCE_1_0.0
   * @return The target object.
   */
  Handle GetTargetObject();

  /**
   * @brief Retrieve the property which this constraint is targeting.
   *
   * @SINCE_1_0.0
   * @return The target property.
   */
  Dali::Property::Index GetTargetProperty();

  /**
   * @brief Set whether the constraint will "bake" a value when fully-applied.
   *
   * Otherwise the constrained value will be discarded, when the constraint is removed.
   * The default value is Constraint::Bake.
   * @SINCE_1_0.0
   * @param[in] action The remove-action.
   */
  void SetRemoveAction( RemoveAction action );

  /**
   * @brief Query whether the constraint will "bake" a value when fully-applied.
   *
   * Otherwise the constrained value will be discarded, when the constraint is removed.
   * @SINCE_1_0.0
   * @return The apply-action.
   */
  RemoveAction GetRemoveAction() const;

  /**
   * @brief Set a tag for the constraint so it can be identified later
   *
   * @SINCE_1_0.0
   * @param[in] tag An integer to identify the constraint
   */
  void SetTag( const unsigned int tag );

  /**
   * @brief Get the tag
   *
   * @SINCE_1_0.0
   * @return The tag
   */
  unsigned int GetTag() const;

public: // Not intended for use by Application developers

  /**
   * @brief This constructor is used by Dali New() methods
   * @SINCE_1_0.0
   * @param [in] constraint A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL Constraint( Internal::ConstraintBase* constraint );

private: // Not intended for use by Application developers

  /**
   * @brief Construct a new constraint which targets a property.
   *
   * @SINCE_1_0.0
   * @param[in]  handle       The handle to the property-owning object.
   * @param[in]  targetIndex  The index of the property to constrain.
   * @param[in]  targetType   Type The type of the constrained property.
   * @param[in]  function     The constraint function.
   * @return The new constraint.
   */
  static Constraint New( Handle handle, Property::Index targetIndex, Property::Type targetType, CallbackBase* function );
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_CONSTRAINT_H__

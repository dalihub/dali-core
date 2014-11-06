#ifndef __DALI_PROPERTY_CONSTRAINT_H__
#define __DALI_PROPERTY_CONSTRAINT_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/animation/property-input-accessor.h>
#include <dali/internal/event/animation/property-input-indexer.h>
#include <dali/internal/event/common/property-input-impl.h>

namespace Dali
{

namespace Internal
{

/**
 * A base class for connecting properties to a constraint function.
 */
template < typename PropertyType >
class PropertyConstraintBase
{
public:

  /**
   * Create a property constraint.
   */
  PropertyConstraintBase()
  : mInputsInitialized( false )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~PropertyConstraintBase()
  {
  }

  /**
   * Clone a property constraint.
   */
  virtual PropertyConstraintBase< PropertyType >* Clone() = 0;

  /**
   * Clone a property constraint supporting component indices.
   */
  virtual PropertyConstraintBase< PropertyType >* CloneComponentFunc() = 0;

  /**
   * Set the input for one of the property constraint parameters.
   * @param [in] index The parameter index.
   * @param [in] input The interface for receiving a property value.
   */
  virtual void SetInput( unsigned int index, int componentIndex, const PropertyInputImpl& input ) = 0;

  /**
   * Retrieve the input for one of the property constraint parameters.
   * @param [in] index The parameter index.
   * @return The property input, or NULL if no input exists with this index.
   */
  virtual const PropertyInputImpl* GetInput( unsigned int index ) const = 0;

  /**
   * Query whether all of the inputs have been initialized.
   * @return True if all of the inputs have been initialized.
   */
  bool InputsInitialized()
  {
    if ( !mInputsInitialized )
    {
      // Check whether the inputs are initialized yet
      unsigned int index( 0u );
      for ( const PropertyInputImpl* input = GetInput( index );
            NULL != input;
            input = GetInput( ++index ) )
      {
        if ( !input->InputInitialized() )
        {
          return false;
        }
      }

      // All inputs are now initialized
      mInputsInitialized = true;
    }

    return true;
  }

  /**
   * Query whether any of the inputs have changed
   * @return True if any of the inputs have changed.
   */
  bool InputsChanged()
  {
    unsigned int index( 0u );
    for ( const PropertyInputImpl* input = GetInput( index );
          NULL != input;
          input = GetInput( ++index ) )
    {
      if ( input->InputChanged() )
      {
        // At least one of the inputs has changed
        return true;
      }
    }

    return false;
  }

  /**
   * Apply the constraint.
   * @param [in] bufferIndex The current update buffer index.
   * @param [in] current The current property value.
   * @return The constrained property value.
   */
  virtual PropertyType Apply( BufferIndex bufferIndex, const PropertyType& current ) = 0;

private:

  bool mInputsInitialized;
};

/**
 * Connects a constraint function with a target property & zero input properties.
 */
template < typename PropertyType >
class PropertyConstraint0 : public PropertyConstraintBase< PropertyType >
{
public:

  typedef boost::function< PropertyType (const PropertyType&)> ConstraintFunction;

  /**
   * Constructor.
   * @param [in] func A constraint function.
   */
  PropertyConstraint0( const ConstraintFunction& func )
  : mFunction( func )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~PropertyConstraint0()
  {
  }

  /**
   * @copydoc PropertyConstraintBase::Clone()
   */
  virtual PropertyConstraintBase< PropertyType >* Clone()
  {
    return new PropertyConstraint0< PropertyType >( mFunction );
  }

  /**
   * @copydoc PropertyConstraintBase::CloneComponentFunc()
   */
  virtual PropertyConstraintBase< PropertyType >* CloneComponentFunc()
  {
    DALI_ASSERT_ALWAYS( false && "PropertyConstraint0 does not support (property component) inputs" );
    return NULL;
  }

  /**
   * @copydoc PropertyConstraintBase::SetInput()
   */
  virtual void SetInput( unsigned int index, int componentIndex, const PropertyInputImpl& input )
  {
    DALI_ASSERT_ALWAYS( false && "PropertyConstraintBase::SetInput() needs overriding" ); // No additional inputs
  }

  /**
   * @copydoc PropertyConstraintBase::GetInput()
   */
  virtual const PropertyInputImpl* GetInput( unsigned int index ) const
  {
    return NULL; // No additional inputs
  }

  /**
   * @copydoc PropertyConstraintBase::Apply()
   */
  virtual PropertyType Apply( BufferIndex bufferIndex, const PropertyType& current )
  {
    return mFunction( current );
  }

private:

  // Undefined
  PropertyConstraint0(const PropertyConstraint0&);

  // Undefined
  PropertyConstraint0& operator=(const PropertyConstraint0& rhs);

private:

  ConstraintFunction mFunction;
};

/**
 * Connects a constraint function with a target property & 1 input property.
 */
template < typename PropertyType, typename PropertyInputAccessorType >
class PropertyConstraint1 : public PropertyConstraintBase< PropertyType >
{
public:

  typedef boost::function< PropertyType (const PropertyType&, const PropertyInput&) > ConstraintFunction;

  /**
   * Constructor.
   * @param [in] func A constraint function.
   */
  PropertyConstraint1( const ConstraintFunction& func )
  : mFunction( func )
  {
  }

  /**
   * Constructor.
   * @param [in] func A constraint function.
   * @param [in] input1 A property input.
   */
  PropertyConstraint1( const ConstraintFunction& func,
                       const PropertyInputAccessorType& input1 )
  : mFunction( func ),
    mInput1( input1 )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~PropertyConstraint1()
  {
  }

  /**
   * @copydoc PropertyConstraintBase::Clone()
   */
  virtual PropertyConstraintBase< PropertyType >* Clone()
  {
    return new PropertyConstraint1< PropertyType, PropertyInputAccessorType >( mFunction, mInput1 );
  }

  /**
   * @copydoc PropertyConstraintBase::CloneComponentFunc()
   */
  virtual PropertyConstraintBase< PropertyType >* CloneComponentFunc()
  {
    return new PropertyConstraint1< PropertyType, PropertyInputComponentAccessor >( mFunction, mInput1 );
  }

  /**
   * @copydoc PropertyConstraintBase::SetInput()
   */
  virtual void SetInput( unsigned int index, int componentIndex, const PropertyInputImpl& input )
  {
    DALI_ASSERT_ALWAYS( 0u == index && "Constraint only has one input property" );

    mInput1.SetInput( input, componentIndex );
  }

  /**
   * @copydoc PropertyConstraintBase::GetInput()
   */
  virtual const PropertyInputImpl* GetInput( unsigned int index ) const
  {
    if ( 0u == index )
    {
      return mInput1.GetInput();
    }

    return NULL;
  }

  /**
   * @copydoc PropertyConstraintBase::Apply()
   */
  virtual PropertyType Apply( BufferIndex bufferIndex, const PropertyType& current )
  {
    DALI_ASSERT_DEBUG( NULL != mInput1.GetInput() );

    const PropertyInputIndexer< PropertyInputAccessorType > input1( bufferIndex, &mInput1 );

    return mFunction( current, input1 );
  }

private:

  // Undefined
  PropertyConstraint1(const PropertyConstraint1&);

  // Undefined
  PropertyConstraint1& operator=(const PropertyConstraint1& rhs);

private:

  ConstraintFunction mFunction;

  PropertyInputAccessorType mInput1;
};

/**
 * Connects a constraint function with a target property & 2 input properties.
 */
template < typename PropertyType, typename PropertyInputAccessorType >
class PropertyConstraint2 : public PropertyConstraintBase< PropertyType >
{
public:

  typedef boost::function< PropertyType (const PropertyType&, const PropertyInput&, const PropertyInput&) > ConstraintFunction;

  /**
   * Constructor.
   * @param [in] func A constraint function.
   */
  PropertyConstraint2( const ConstraintFunction& func )
  : mFunction( func )
  {
  }

  /**
   * Constructor.
   * @param [in] func A constraint function.
   * @param [in] input1 A property input.
   * @param [in] input2 A 2nd property input.
   */
  PropertyConstraint2( const ConstraintFunction& func,
                       const PropertyInputAccessorType& input1,
                       const PropertyInputAccessorType& input2 )
  : mFunction( func ),
    mInput1( input1 ),
    mInput2( input2 )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~PropertyConstraint2()
  {
  }

  /**
   * @copydoc PropertyConstraintBase::Clone()
   */
  virtual PropertyConstraintBase< PropertyType >* Clone()
  {
    return new PropertyConstraint2< PropertyType, PropertyInputAccessorType >( mFunction, mInput1, mInput2 );
  }

  /**
   * @copydoc PropertyConstraintBase::CloneComponentFunc()
   */
  virtual PropertyConstraintBase< PropertyType >* CloneComponentFunc()
  {
    return new PropertyConstraint2< PropertyType, PropertyInputComponentAccessor >( mFunction, mInput1, mInput2 );
  }

  /**
   * @copydoc PropertyConstraintBase::SetInput()
   */
  virtual void SetInput( unsigned int index, int componentIndex, const PropertyInputImpl& input )
  {
    DALI_ASSERT_ALWAYS( 2u > index && "Constraint only has 2 properties" );

    if ( 0u == index )
    {
      mInput1.SetInput( input, componentIndex );
    }
    else
    {
      mInput2.SetInput( input, componentIndex );
    }
  }

  /**
   * @copydoc PropertyConstraintBase::GetInput()
   */
  virtual const PropertyInputImpl* GetInput( unsigned int index ) const
  {
    if ( 0u == index )
    {
      return mInput1.GetInput();
    }
    else if ( 1u == index )
    {
      return mInput2.GetInput();
    }

    return NULL;
  }

  /**
   * @copydoc PropertyConstraintBase::Apply()
   */
  virtual PropertyType Apply( BufferIndex bufferIndex, const PropertyType& current )
  {
    DALI_ASSERT_DEBUG( NULL != mInput1.GetInput() );
    DALI_ASSERT_DEBUG( NULL != mInput2.GetInput() );

    const PropertyInputIndexer< PropertyInputAccessorType > input1( bufferIndex, &mInput1 );
    const PropertyInputIndexer< PropertyInputAccessorType > input2( bufferIndex, &mInput2 );

    return mFunction( current,
                      input1,
                      input2 );
  }

private:

  // Undefined
  PropertyConstraint2(const PropertyConstraint2&);

  // Undefined
  PropertyConstraint2& operator=(const PropertyConstraint2& rhs);

private:

  ConstraintFunction mFunction;

  PropertyInputAccessorType mInput1;
  PropertyInputAccessorType mInput2;
};

/**
 * Connects a constraint function with a target property & 3 input properties.
 */
template < class PropertyType, typename PropertyInputAccessorType >
class PropertyConstraint3 : public PropertyConstraintBase< PropertyType >
{
public:

  typedef boost::function< PropertyType (const PropertyType&, const PropertyInput&, const PropertyInput&, const PropertyInput&)> ConstraintFunction;

  /**
   * Constructor.
   * @param [in] func A constraint function.
   */
  PropertyConstraint3( const ConstraintFunction& func )
  : mFunction( func )
  {
  }

  /**
   * Constructor.
   * @param [in] func A constraint function.
   * @param [in] input1 A property input.
   * @param [in] input2 A 2nd property input.
   * @param [in] input3 A 3rd property input.
   */
  PropertyConstraint3( const ConstraintFunction& func,
                       const PropertyInputAccessorType& input1,
                       const PropertyInputAccessorType& input2,
                       const PropertyInputAccessorType& input3 )
  : mFunction( func ),
    mInput1( input1 ),
    mInput2( input2 ),
    mInput3( input3 )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~PropertyConstraint3()
  {
  }

  /**
   * @copydoc PropertyConstraintBase::Clone()
   */
  virtual PropertyConstraintBase< PropertyType >* Clone()
  {
    return new PropertyConstraint3< PropertyType, PropertyInputAccessorType >( mFunction, mInput1, mInput2, mInput3 );
  }

  /**
   * @copydoc PropertyConstraintBase::CloneComponentFunc()
   */
  virtual PropertyConstraintBase< PropertyType >* CloneComponentFunc()
  {
    return new PropertyConstraint3< PropertyType, PropertyInputComponentAccessor >( mFunction, mInput1, mInput2, mInput3 );
  }

  /**
   * @copydoc PropertyConstraintBase::SetInput()
   */
  virtual void SetInput( unsigned int index, int componentIndex, const PropertyInputImpl& input )
  {
    DALI_ASSERT_ALWAYS( 3u > index && "Constraint only has 3 properties" );

    if ( 0u == index )
    {
      mInput1.SetInput( input, componentIndex );
    }
    else if ( 1u == index )
    {
      mInput2.SetInput( input, componentIndex );
    }
    else
    {
      mInput3.SetInput( input, componentIndex );
    }
  }

  /**
   * @copydoc PropertyConstraintBase::GetInput()
   */
  virtual const PropertyInputImpl* GetInput( unsigned int index ) const
  {
    if ( 0u == index )
    {
      return mInput1.GetInput();
    }
    else if ( 1u == index )
    {
      return mInput2.GetInput();
    }
    else if ( 2u == index )
    {
      return mInput3.GetInput();
    }

    return NULL;
  }

  /**
   * @copydoc PropertyConstraintBase::Apply()
   */
  virtual PropertyType Apply( BufferIndex bufferIndex, const PropertyType& current )
  {
    DALI_ASSERT_DEBUG( NULL != mInput1.GetInput() );
    DALI_ASSERT_DEBUG( NULL != mInput2.GetInput() );
    DALI_ASSERT_DEBUG( NULL != mInput3.GetInput() );

    const PropertyInputIndexer< PropertyInputAccessorType > input1( bufferIndex, &mInput1 );
    const PropertyInputIndexer< PropertyInputAccessorType > input2( bufferIndex, &mInput2 );
    const PropertyInputIndexer< PropertyInputAccessorType > input3( bufferIndex, &mInput3 );

    return mFunction( current,
                      input1,
                      input2,
                      input3 );
  }

private:

  // Undefined
  PropertyConstraint3(const PropertyConstraint3&);

  // Undefined
  PropertyConstraint3& operator=(const PropertyConstraint3& rhs);

private:

  ConstraintFunction mFunction;

  PropertyInputAccessorType mInput1;
  PropertyInputAccessorType mInput2;
  PropertyInputAccessorType mInput3;
};

/**
 * Connects a constraint function with a target property & 4 input properties.
 */
template < class PropertyType, typename PropertyInputAccessorType >
class PropertyConstraint4 : public PropertyConstraintBase< PropertyType >
{
public:

  typedef boost::function< PropertyType (const PropertyType&, const PropertyInput&, const PropertyInput&, const PropertyInput&, const PropertyInput&) > ConstraintFunction;

  /**
   * Constructor.
   * @param [in] func A constraint function.
   */
  PropertyConstraint4( const ConstraintFunction& func )
  : mFunction( func )
  {
  }

  /**
   * Constructor.
   * @param [in] func A constraint function.
   * @param [in] input1 A property input.
   * @param [in] input2 A 2nd property input.
   * @param [in] input3 A 3rd property input.
   * @param [in] input4 A 4th property input.
   */
  PropertyConstraint4( const ConstraintFunction& func,
                       const PropertyInputAccessorType& input1,
                       const PropertyInputAccessorType& input2,
                       const PropertyInputAccessorType& input3,
                       const PropertyInputAccessorType& input4 )
  : mFunction( func ),
    mInput1( input1 ),
    mInput2( input2 ),
    mInput3( input3 ),
    mInput4( input4 )
  {
  }

  /**
   * Create a PropertyConstraint4 instance.
   * @param [in] func A constraint function.
   * @return A newly heap-allocated object.
   */
  static PropertyConstraintBase< PropertyType >* New( ConstraintFunction func )
  {
    return new PropertyConstraint4< PropertyType, PropertyInputAccessorType >( func );
  }

  /**
   * Virtual destructor.
   */
  virtual ~PropertyConstraint4()
  {
  }

  /**
   * @copydoc PropertyConstraintBase::Clone()
   */
  virtual PropertyConstraintBase< PropertyType >* Clone()
  {
    return new PropertyConstraint4< PropertyType, PropertyInputAccessorType >( mFunction, mInput1, mInput2, mInput3, mInput4 );
  }

  /**
   * @copydoc PropertyConstraintBase::CloneComponentFunc()
   */
  virtual PropertyConstraintBase< PropertyType >* CloneComponentFunc()
  {
    return new PropertyConstraint4< PropertyType, PropertyInputComponentAccessor >( mFunction, mInput1, mInput2, mInput3, mInput4 );
  }

  /**
   * @copydoc PropertyConstraintBase::SetInput()
   */
  virtual void SetInput( unsigned int index, int componentIndex, const PropertyInputImpl& input )
  {
    DALI_ASSERT_ALWAYS( 4u > index && "Constraint only has 4 properties" );

    if ( 0u == index )
    {
      mInput1.SetInput( input, componentIndex );
    }
    else if ( 1u == index )
    {
      mInput2.SetInput( input, componentIndex );
    }
    else if ( 2u == index )
    {
      mInput3.SetInput( input, componentIndex );
    }
    else
    {
      mInput4.SetInput( input, componentIndex );
    }
  }

  /**
   * @copydoc PropertyConstraintBase::GetInput()
   */
  virtual const PropertyInputImpl* GetInput( unsigned int index ) const
  {
    if ( 0u == index )
    {
      return mInput1.GetInput();
    }
    else if ( 1u == index )
    {
      return mInput2.GetInput();
    }
    else if ( 2u == index )
    {
      return mInput3.GetInput();
    }
    else if ( 3u == index )
    {
      return mInput4.GetInput();
    }

    return NULL;
  }

  /**
   * @copydoc PropertyConstraintBase::Apply()
   */
  virtual PropertyType Apply( BufferIndex bufferIndex, const PropertyType& current )
  {
    DALI_ASSERT_DEBUG( NULL != mInput1.GetInput() );
    DALI_ASSERT_DEBUG( NULL != mInput2.GetInput() );
    DALI_ASSERT_DEBUG( NULL != mInput3.GetInput() );
    DALI_ASSERT_DEBUG( NULL != mInput4.GetInput() );

    const PropertyInputIndexer< PropertyInputAccessorType > input1( bufferIndex, &mInput1 );
    const PropertyInputIndexer< PropertyInputAccessorType > input2( bufferIndex, &mInput2 );
    const PropertyInputIndexer< PropertyInputAccessorType > input3( bufferIndex, &mInput3 );
    const PropertyInputIndexer< PropertyInputAccessorType > input4( bufferIndex, &mInput4 );

    return mFunction( current,
                      input1,
                      input2,
                      input3,
                      input4 );
  }

private:

  // Undefined
  PropertyConstraint4(const PropertyConstraint4&);

  // Undefined
  PropertyConstraint4& operator=(const PropertyConstraint4& rhs);

private:

  ConstraintFunction mFunction;

  PropertyInputAccessorType mInput1;
  PropertyInputAccessorType mInput2;
  PropertyInputAccessorType mInput3;
  PropertyInputAccessorType mInput4;
};

/**
 * Connects a constraint function with a target property & 5 input properties.
 */
template < class PropertyType, typename PropertyInputAccessorType >
class PropertyConstraint5 : public PropertyConstraintBase< PropertyType >
{
public:

  typedef boost::function< PropertyType (const PropertyType&, const PropertyInput&, const PropertyInput&, const PropertyInput&, const PropertyInput&, const PropertyInput&) > ConstraintFunction;

  /**
   * Constructor.
   * @param [in] func A constraint function.
   */
  PropertyConstraint5( const ConstraintFunction& func )
  : mFunction( func )
  {
  }

  /**
   * Constructor.
   * @param [in] func A constraint function.
   * @param [in] input1 A property input.
   * @param [in] input2 A 2nd property input.
   * @param [in] input3 A 3rd property input.
   * @param [in] input4 A 4th property input.
   * @param [in] input5 A 5th property input.
   */
  PropertyConstraint5( const ConstraintFunction& func,
                       const PropertyInputAccessorType& input1,
                       const PropertyInputAccessorType& input2,
                       const PropertyInputAccessorType& input3,
                       const PropertyInputAccessorType& input4,
                       const PropertyInputAccessorType& input5 )
  : mFunction( func ),
    mInput1( input1 ),
    mInput2( input2 ),
    mInput3( input3 ),
    mInput4( input4 ),
    mInput5( input5 )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~PropertyConstraint5()
  {
  }

  /**
   * @copydoc PropertyConstraintBase::Clone()
   */
  virtual PropertyConstraintBase< PropertyType >* Clone()
  {
    return new PropertyConstraint5< PropertyType, PropertyInputAccessorType >( mFunction, mInput1, mInput2, mInput3, mInput4, mInput5 );
  }

  /**
   * @copydoc PropertyConstraintBase::CloneComponentFunc()
   */
  virtual PropertyConstraintBase< PropertyType >* CloneComponentFunc()
  {
    return new PropertyConstraint5< PropertyType, PropertyInputComponentAccessor >( mFunction, mInput1, mInput2, mInput3, mInput4, mInput5 );
  }

  /**
   * @copydoc PropertyConstraintBase::SetInput()
   */
  virtual void SetInput( unsigned int index, int componentIndex, const PropertyInputImpl& input )
  {
    DALI_ASSERT_ALWAYS( 5u > index && "Constraint only has 5 properties");

    if ( 0u == index )
    {
      mInput1.SetInput( input, componentIndex );
    }
    else if ( 1u == index )
    {
      mInput2.SetInput( input, componentIndex );
    }
    else if ( 2u == index )
    {
      mInput3.SetInput( input, componentIndex );
    }
    else if ( 3u == index )
    {
      mInput4.SetInput( input, componentIndex );
    }
    else
    {
      mInput5.SetInput( input, componentIndex );
    }
  }

  /**
   * @copydoc PropertyConstraintBase::GetInput()
   */
  virtual const PropertyInputImpl* GetInput( unsigned int index ) const
  {
    if ( 0u == index )
    {
      return mInput1.GetInput();
    }
    else if ( 1u == index )
    {
      return mInput2.GetInput();
    }
    else if ( 2u == index )
    {
      return mInput3.GetInput();
    }
    else if ( 3u == index )
    {
      return mInput4.GetInput();
    }
    else if ( 4u == index )
    {
      return mInput5.GetInput();
    }

    return NULL;
  }

  /**
   * @copydoc PropertyConstraintBase::Apply()
   */
  virtual PropertyType Apply( BufferIndex bufferIndex, const PropertyType& current )
  {
    DALI_ASSERT_DEBUG( NULL != mInput1.GetInput() );
    DALI_ASSERT_DEBUG( NULL != mInput2.GetInput() );
    DALI_ASSERT_DEBUG( NULL != mInput3.GetInput() );
    DALI_ASSERT_DEBUG( NULL != mInput4.GetInput() );
    DALI_ASSERT_DEBUG( NULL != mInput5.GetInput() );

    const PropertyInputIndexer< PropertyInputAccessorType > input1( bufferIndex, &mInput1 );
    const PropertyInputIndexer< PropertyInputAccessorType > input2( bufferIndex, &mInput2 );
    const PropertyInputIndexer< PropertyInputAccessorType > input3( bufferIndex, &mInput3 );
    const PropertyInputIndexer< PropertyInputAccessorType > input4( bufferIndex, &mInput4 );
    const PropertyInputIndexer< PropertyInputAccessorType > input5( bufferIndex, &mInput5 );

    return mFunction( current,
                      input1,
                      input2,
                      input3,
                      input4,
                      input5 );
  }

private:

  // Undefined
  PropertyConstraint5(const PropertyConstraint5&);

  // Undefined
  PropertyConstraint5& operator=(const PropertyConstraint5& rhs);

private:

  ConstraintFunction mFunction;

  PropertyInputAccessorType mInput1;
  PropertyInputAccessorType mInput2;
  PropertyInputAccessorType mInput3;
  PropertyInputAccessorType mInput4;
  PropertyInputAccessorType mInput5;
};

/**
 * Connects a constraint function with a target property & 6 input properties.
 */
template < class PropertyType, typename PropertyInputAccessorType >
class PropertyConstraint6 : public PropertyConstraintBase< PropertyType >
{
public:

  typedef boost::function< PropertyType (const PropertyType&, const PropertyInput&, const PropertyInput&, const PropertyInput&, const PropertyInput&, const PropertyInput&, const PropertyInput&)> ConstraintFunction;

  /**
   * Constructor.
   * @param [in] func A constraint function.
   */
  PropertyConstraint6( const ConstraintFunction& func )
  : mFunction( func )
  {
  }

  /**
   * Constructor.
   * @param [in] func A constraint function.
   * @param [in] input1 A property input.
   * @param [in] input2 A 2nd property input.
   * @param [in] input3 A 3rd property input.
   * @param [in] input4 A 4th property input.
   * @param [in] input5 A 5th property input.
   * @param [in] input6 A 6th property input.
   */
  PropertyConstraint6( const ConstraintFunction& func,
                       const PropertyInputAccessorType& input1,
                       const PropertyInputAccessorType& input2,
                       const PropertyInputAccessorType& input3,
                       const PropertyInputAccessorType& input4,
                       const PropertyInputAccessorType& input5,
                       const PropertyInputAccessorType& input6 )
  : mFunction( func ),
    mInput1( input1 ),
    mInput2( input2 ),
    mInput3( input3 ),
    mInput4( input4 ),
    mInput5( input5 ),
    mInput6( input6 )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~PropertyConstraint6()
  {
  }

  /**
   * @copydoc PropertyConstraintBase::Clone()
   */
  virtual PropertyConstraintBase< PropertyType >* Clone()
  {
    return new PropertyConstraint6< PropertyType, PropertyInputAccessorType >( mFunction, mInput1, mInput2, mInput3, mInput4, mInput5, mInput6 );
  }

  /**
   * @copydoc PropertyConstraintBase::CloneComponentFunc()
   */
  virtual PropertyConstraintBase< PropertyType >* CloneComponentFunc()
  {
    return new PropertyConstraint6< PropertyType, PropertyInputComponentAccessor >( mFunction, mInput1, mInput2, mInput3, mInput4, mInput5, mInput6 );
  }

  /**
   * @copydoc PropertyConstraintBase::SetInput()
   */
  virtual void SetInput( unsigned int index, int componentIndex, const PropertyInputImpl& input )
  {
    DALI_ASSERT_ALWAYS( 6u > index && "Constraint only has 6 properties" );

    if ( 0u == index )
    {
      mInput1.SetInput( input, componentIndex );
    }
    else if ( 1u == index )
    {
      mInput2.SetInput( input, componentIndex );
    }
    else if ( 2u == index )
    {
      mInput3.SetInput( input, componentIndex );
    }
    else if ( 3u == index )
    {
      mInput4.SetInput( input, componentIndex );
    }
    else if ( 4u == index )
    {
      mInput5.SetInput( input, componentIndex );
    }
    else
    {
      mInput6.SetInput( input, componentIndex );
    }
  }

  /**
   * @copydoc PropertyConstraintBase::GetInput()
   */
  virtual const PropertyInputImpl* GetInput( unsigned int index ) const
  {
    if ( 0u == index )
    {
      return mInput1.GetInput();
    }
    else if ( 1u == index )
    {
      return mInput2.GetInput();
    }
    else if ( 2u == index )
    {
      return mInput3.GetInput();
    }
    else if ( 3u == index )
    {
      return mInput4.GetInput();
    }
    else if ( 4u == index )
    {
      return mInput5.GetInput();
    }
    else if ( 5u == index )
    {
      return mInput6.GetInput();
    }

    return NULL;
  }

  /**
   * @copydoc PropertyConstraintBase::Apply()
   */
  virtual PropertyType Apply( BufferIndex bufferIndex, const PropertyType& current )
  {
    DALI_ASSERT_DEBUG( NULL != mInput1.GetInput() );
    DALI_ASSERT_DEBUG( NULL != mInput2.GetInput() );
    DALI_ASSERT_DEBUG( NULL != mInput3.GetInput() );
    DALI_ASSERT_DEBUG( NULL != mInput4.GetInput() );
    DALI_ASSERT_DEBUG( NULL != mInput5.GetInput() );
    DALI_ASSERT_DEBUG( NULL != mInput6.GetInput() );

    const PropertyInputIndexer< PropertyInputAccessorType > input1( bufferIndex, &mInput1 );
    const PropertyInputIndexer< PropertyInputAccessorType > input2( bufferIndex, &mInput2 );
    const PropertyInputIndexer< PropertyInputAccessorType > input3( bufferIndex, &mInput3 );
    const PropertyInputIndexer< PropertyInputAccessorType > input4( bufferIndex, &mInput4 );
    const PropertyInputIndexer< PropertyInputAccessorType > input5( bufferIndex, &mInput5 );
    const PropertyInputIndexer< PropertyInputAccessorType > input6( bufferIndex, &mInput6 );

    return mFunction( current,
                      input1,
                      input2,
                      input3,
                      input4,
                      input5,
                      input6 );
  }

private:

  // Undefined
  PropertyConstraint6(const PropertyConstraint6&);

  // Undefined
  PropertyConstraint6& operator=(const PropertyConstraint6& rhs);

private:

  ConstraintFunction mFunction;

  PropertyInputAccessorType mInput1;
  PropertyInputAccessorType mInput2;
  PropertyInputAccessorType mInput3;
  PropertyInputAccessorType mInput4;
  PropertyInputAccessorType mInput5;
  PropertyInputAccessorType mInput6;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_PROPERTY_CONSTRAINT_H__

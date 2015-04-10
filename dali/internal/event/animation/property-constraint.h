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
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/internal/event/animation/property-input-accessor.h>
#include <dali/internal/event/animation/property-input-indexer.h>
#include <dali/internal/event/common/property-input-impl.h>

namespace Dali
{

namespace Internal
{

/**
 * A class for connecting properties to a constraint function.
 */
template < typename PropertyType >
class PropertyConstraint
{
public:

  typedef std::vector < PropertyInputAccessor > InputContainer;
  typedef typename InputContainer::iterator InputContainerIter;
  typedef typename InputContainer::const_iterator InputContainerConstIter;

  typedef std::vector< PropertyInputIndexer< PropertyInputAccessor > > InputIndexerContainer;

  typedef Dali::Constraint::Function< PropertyType > ConstraintFunction;

  /**
   * Create a property constraint.
   *
   * @param[in]  func  A constraint function. Ownership of this callback-function is passed to this object.
   */
  PropertyConstraint( Dali::Constraint::Function< PropertyType >* func )
  : mInputsInitialized( false ),
    mFunction( func ),
    mInputs()
  {
  }

  /**
   * Constructor.
   * @param [in]  func    A constraint function. Ownership of this callback-function is passed to this object.
   * @param [in]  inputs  Property inputs.
   */
  PropertyConstraint( Dali::Constraint::Function< PropertyType >* func,
                      const InputContainer& inputs )
  : mInputsInitialized( false ),
    mFunction( func ),
    mInputs( inputs )
  {
  }

  /**
   * Non virtual destructor.
   */
  ~PropertyConstraint()
  {
    delete mFunction;
  }

  /**
   * Clone a property constraint.
   *
   * @return The clone of the property-constraint.
   *
   * @note This function will create a copy of the stored constraint function for the clone.
   */
  PropertyConstraint< PropertyType >* Clone()
  {
    return new PropertyConstraint< PropertyType >( reinterpret_cast< ConstraintFunction* >( mFunction->Clone() ), mInputs );
  }

  /**
   * Set the input for one of the property constraint parameters.
   * @param [in] index The parameter index.
   * @param [in] input The interface for receiving a property value.
   */
  void SetInput( unsigned int index, int componentIndex, const PropertyInputImpl& input )
  {
    if ( index >= mInputs.size() )
    {
      mInputs.push_back( PropertyInputAccessor() );
    }

    mInputs[ index ].SetInput( input, componentIndex );
  }

  /**
   * Retrieve the input for one of the property constraint parameters.
   * @param [in] index The parameter index.
   * @return The property input, or NULL if no input exists with this index.
   */
  const PropertyInputImpl* GetInput( unsigned int index ) const
  {
    if ( index < mInputs.size() )
    {
      return mInputs[ index ].GetInput();
    }

    return NULL;
  }

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
   * @param [in,out] current The current property value, will be set to the constrained value upon return.
   */
  void Apply( BufferIndex bufferIndex, PropertyType& current )
  {
    InputIndexerContainer mInputIndices;
    PropertyInputContainer mIndices;
    const unsigned int noOfInputs = mInputs.size();

    mInputIndices.reserve( noOfInputs );
    mIndices.Reserve( noOfInputs );

    const InputContainerConstIter endIter = mInputs.end();
    unsigned int index = 0;
    for ( InputContainerConstIter iter = mInputs.begin(); iter != endIter; ++iter, ++index )
    {
      DALI_ASSERT_DEBUG( NULL != iter->GetInput() );
      mInputIndices.push_back( PropertyInputIndexer< PropertyInputAccessor >( bufferIndex, &*iter ) );
      mIndices.PushBack( &mInputIndices[ index ] );
    }

    CallbackBase::Execute< PropertyType&, const PropertyInputContainer& >( *mFunction, current, mIndices );
  }

private:

  // Undefined
  PropertyConstraint( const PropertyConstraint& );

  // Undefined
  PropertyConstraint& operator=( const PropertyConstraint& rhs );

private:

  bool mInputsInitialized;

  ConstraintFunction* mFunction;

  InputContainer mInputs;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_PROPERTY_CONSTRAINT_H__

#ifndef DALI_PROPERTY_CONSTRAINT_H
#define DALI_PROPERTY_CONSTRAINT_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

  using ConstraintFunction = Dali::Constraint::Function< PropertyType >;
  using InputContainer = std::vector < PropertyInputAccessor >;
  using InputIndexerContainer = std::vector< PropertyInputIndexer< PropertyInputAccessor > >;

  /**
   * Create a property constraint.
   *
   * @param[in]  func  A constraint function. Ownership of this callback-function is passed to this object.
   */
  PropertyConstraint( ConstraintFunction* func )
  : mFunction( func ),
    mInputs(),
    mInputsInitialized( false )
  {
  }

  /**
   * Constructor.
   * @param [in]  func    A constraint function. Ownership of this callback-function is passed to this object.
   * @param [in]  inputs  Property inputs.
   */
  PropertyConstraint( ConstraintFunction* func,
                      const InputContainer& inputs )
  : mFunction( func ),
    mInputs( inputs ),
    mInputsInitialized( false )
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
   * @param [in] input The interface for receiving a property value.
   * @param [in] componentIndex Component index.
   */
  void AddInput( const PropertyInputImpl* input, int32_t componentIndex )
  {
    mInputs.push_back( PropertyInputAccessor{ input, componentIndex } );
  }

  /**
   * Retrieve the input for one of the property constraint parameters.
   * @param [in] index The parameter index.
   * @return The property input, or nullptr if no input exists with this index.
   */
  const PropertyInputImpl* GetInput( uint32_t index ) const
  {
    if ( index < mInputs.size() )
    {
      return mInputs[ index ].GetInput();
    }

    return nullptr;
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
      uint32_t index( 0u );
      for ( const PropertyInputImpl* input = GetInput( index );
            nullptr != input;
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
    uint32_t index( 0u );
    for ( const PropertyInputImpl* input = GetInput( index );
          nullptr != input;
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
    InputIndexerContainer inputIndices;
    PropertyInputContainer indices;
    const uint32_t noOfInputs = static_cast<uint32_t>( mInputs.size() );

    inputIndices.reserve( noOfInputs );
    indices.Reserve( noOfInputs );

    const auto&& endIter = mInputs.end();
    uint32_t index = 0;
    for ( auto&& iter = mInputs.begin(); iter != endIter; ++iter, ++index )
    {
      DALI_ASSERT_DEBUG( nullptr != iter->GetInput() );
      inputIndices.push_back( PropertyInputIndexer< PropertyInputAccessor >( bufferIndex, &*iter ) );
      indices.PushBack( &inputIndices[ index ] );
    }

    CallbackBase::Execute< PropertyType&, const PropertyInputContainer& >( *mFunction, current, indices );
  }

private:

  // Undefined
  PropertyConstraint() = delete;
  PropertyConstraint( const PropertyConstraint& ) = delete;
  PropertyConstraint& operator=( const PropertyConstraint& rhs ) = delete;

private:

  ConstraintFunction* mFunction;
  InputContainer mInputs;
  bool mInputsInitialized;

};

} // namespace Internal

} // namespace Dali

#endif // DALI_PROPERTY_CONSTRAINT_H

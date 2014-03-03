#ifndef __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_ACCESSOR_H__
#define __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_ACCESSOR_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/update/common/animatable-property.h>

namespace Dali
{

namespace Internal
{

/**
 * A wrapper class for getting/setting a property.
 * Animators use this instead of accessing properties directly.
 */
template < typename PropertyType >
class PropertyAccessor
{
public:

  /**
   * Create a property component.
   * @param [in] property The property to access.
   */
  PropertyAccessor( SceneGraph::PropertyBase* property )
  : mProperty( dynamic_cast< SceneGraph::AnimatableProperty<PropertyType>* >(property) )
  {
  }

  /**
   * Non-virtual destructor; PropertyAccessor is not suitable as a base class.
   */
  ~PropertyAccessor()
  {
  }

  /**
   * Query whether the accessor is set.
   * @return True if set.
   */
  bool IsSet() const
  {
    return mProperty != NULL;
  }

  /**
   * Reset the property accessor
   * @post Calling any other PropertyAccessor is invalid.
   */
  void Reset()
  {
    mProperty = NULL;
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::IsClean()
   */
  bool IsClean() const
  {
    return mProperty->IsClean();
  }

  /**
   * Read access to the property.
   * @param [in] bufferIndex The current update buffer index.
   */
  const PropertyType& Get( BufferIndex bufferIndex ) const
  {
    DALI_ASSERT_DEBUG( NULL != mProperty && "PropertyAccessor::Get() mProperty was NULL" );
    return mProperty->Get( bufferIndex );
  }

  /**
   * @copydoc AnimatableProperty<float>::Set()
   */
  void Set( BufferIndex bufferIndex, const PropertyType& value ) const
  {
    DALI_ASSERT_DEBUG( NULL != mProperty && "PropertyAccessor::Set() mProperty was NULL" );
    mProperty->Set( bufferIndex, value );
  }

  /**
   * @copydoc AnimatableProperty<float>::Bake()
   */
  void Bake( BufferIndex bufferIndex, const PropertyType& value ) const
  {
    DALI_ASSERT_DEBUG( NULL != mProperty && "PropertyAccessor::Bake() mProperty was NULL" );
    mProperty->Bake( bufferIndex, value );
  }

private:

  // Undefined
  PropertyAccessor(const PropertyAccessor& property);

  // Undefined
  PropertyAccessor& operator=(const PropertyAccessor& rhs);

private:

  SceneGraph::AnimatableProperty<PropertyType>* mProperty; ///< The real property
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_ACCESSOR_H__

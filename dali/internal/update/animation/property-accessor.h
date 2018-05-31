#ifndef __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_ACCESSOR_H__
#define __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_ACCESSOR_H__

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/manager/transform-manager-property.h>

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



/**
 * A wrapper class for getting/setting a property.
 * Animators use this instead of accessing properties directly.
 */
template <typename T>
class TransformManagerPropertyAccessor
{
public:

  /**
   * Create a property component.
   * @param [in] property The property to access.
   */
  TransformManagerPropertyAccessor( SceneGraph::PropertyBase* property )
  : mProperty( dynamic_cast< SceneGraph::TransformManagerPropertyHandler<T>* >(property) )
  {
  }

  /**
   * Non-virtual destructor; PropertyAccessor is not suitable as a base class.
   */
  ~TransformManagerPropertyAccessor()
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
   * @return The value of the property
   */
  const T& Get( BufferIndex bufferIndex ) const
  {
    DALI_ASSERT_DEBUG( NULL != mProperty && "PropertyAccessor::Get() mProperty was NULL" );
    return mProperty->Get( bufferIndex );
  }

  /**
   * @copydoc AnimatableProperty<float>::Set()
   */
  void Set( BufferIndex bufferIndex, const T& value ) const
  {
    DALI_ASSERT_DEBUG( NULL != mProperty && "PropertyAccessor::Set() mProperty was NULL" );
    mProperty->Set( bufferIndex, value );
  }

  /**
   * @copydoc AnimatableProperty<float>::Bake()
   */
  void Bake( BufferIndex bufferIndex, const T& value ) const
  {
    DALI_ASSERT_DEBUG( NULL != mProperty && "PropertyAccessor::Bake() mProperty was NULL" );
    mProperty->Bake( bufferIndex, value );
  }

private:

  // Undefined
  TransformManagerPropertyAccessor(const TransformManagerPropertyAccessor& property);

  // Undefined
  TransformManagerPropertyAccessor& operator=(const TransformManagerPropertyAccessor& rhs);

private:

  SceneGraph::TransformManagerPropertyHandler<T>* mProperty; ///< The real property
};


template <typename T, unsigned int COMPONENT>
class TransformManagerPropertyComponentAccessor
{
public:

  /**
   * Create a property component.
   * @param [in] property The property to access.
   */
  TransformManagerPropertyComponentAccessor( SceneGraph::PropertyBase* property )
  : mProperty( dynamic_cast< SceneGraph::TransformManagerPropertyHandler<T>* >(property) )
  {
  }

  /**
   * Non-virtual destructor; PropertyAccessor is not suitable as a base class.
   */
  ~TransformManagerPropertyComponentAccessor()
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
   * @return The value of the component of the property
   */
  const float& Get( BufferIndex bufferIndex ) const
  {
    DALI_ASSERT_DEBUG( NULL != mProperty && "PropertyAccessor::Get() mProperty was NULL" );
    return mProperty->GetFloatComponent( COMPONENT );
  }

  /**
   * @copydoc AnimatableProperty<float>::Set()
   */
  void Set( BufferIndex bufferIndex, const float& value ) const
  {
    DALI_ASSERT_DEBUG( NULL != mProperty && "PropertyAccessor::Set() mProperty was NULL" );
    mProperty->SetFloatComponent( value, COMPONENT );
  }

  /**
   * @copydoc AnimatableProperty<float>::Bake()
   */
  void Bake( BufferIndex bufferIndex, const float& value ) const
  {
    DALI_ASSERT_DEBUG( NULL != mProperty && "PropertyAccessor::Bake() mProperty was NULL" );
    mProperty->BakeFloatComponent( value, COMPONENT );
  }

private:

  // Undefined
  TransformManagerPropertyComponentAccessor(const TransformManagerPropertyComponentAccessor& property);

  // Undefined
  TransformManagerPropertyComponentAccessor& operator=(const TransformManagerPropertyComponentAccessor& rhs);

private:

  SceneGraph::TransformManagerPropertyHandler<T>* mProperty; ///< The real property
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_ACCESSOR_H__

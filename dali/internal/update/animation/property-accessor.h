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
  : mProperty( static_cast< SceneGraph::AnimatableProperty<PropertyType>* >(property) ) // we know the type
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
    return mProperty != nullptr;
  }

  /**
   * Reset the property accessor
   * @post Calling any other PropertyAccessor is invalid.
   */
  void Reset()
  {
    mProperty = nullptr;
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
    DALI_ASSERT_DEBUG( nullptr != mProperty && "PropertyAccessor::Get() mProperty was nullptr" );
    return mProperty->Get( bufferIndex );
  }

  /**
   * @copydoc AnimatableProperty<float>::Set()
   */
  void Set( BufferIndex bufferIndex, const PropertyType& value ) const
  {
    DALI_ASSERT_DEBUG( nullptr != mProperty && "PropertyAccessor::Set() mProperty was nullptr" );
    mProperty->Set( bufferIndex, value );
  }

  /**
   * @copydoc AnimatableProperty<float>::Bake()
   */
  void Bake( BufferIndex bufferIndex, const PropertyType& value ) const
  {
    DALI_ASSERT_DEBUG( nullptr != mProperty && "PropertyAccessor::Bake() mProperty was nullptr" );
    mProperty->Bake( bufferIndex, value );
  }

private:

  // Undefined
  PropertyAccessor() = delete;
  PropertyAccessor(const PropertyAccessor& property) = delete;
  PropertyAccessor& operator=(const PropertyAccessor& rhs) = delete;

private:

  SceneGraph::AnimatableProperty<PropertyType>* mProperty; ///< The real property

};

/**
 * A wrapper class for getting/setting a transform manager property
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
  : mProperty( static_cast< SceneGraph::TransformManagerPropertyHandler<T>* >(property) ) // we know the type
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
    return mProperty != nullptr;
  }

  /**
   * Reset the property accessor
   * @post Calling any other PropertyAccessor is invalid.
   */
  void Reset()
  {
    mProperty = nullptr;
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
    DALI_ASSERT_DEBUG( nullptr != mProperty && "PropertyAccessor::Get() mProperty was nullptr" );
    return mProperty->Get( bufferIndex );
  }

  /**
   * @copydoc AnimatableProperty<float>::Set()
   */
  void Set( BufferIndex bufferIndex, const T& value ) const
  {
    DALI_ASSERT_DEBUG( nullptr != mProperty && "PropertyAccessor::Set() mProperty was nullptr" );
    mProperty->Set( bufferIndex, value );
  }

  /**
   * @copydoc AnimatableProperty<float>::Bake()
   */
  void Bake( BufferIndex bufferIndex, const T& value ) const
  {
    DALI_ASSERT_DEBUG( nullptr != mProperty && "PropertyAccessor::Bake() mProperty was nullptr" );
    mProperty->Bake( bufferIndex, value );
  }

private:

  // Undefined
  TransformManagerPropertyAccessor() = delete;
  TransformManagerPropertyAccessor(const TransformManagerPropertyAccessor& property) = delete;
  TransformManagerPropertyAccessor& operator=(const TransformManagerPropertyAccessor& rhs) = delete;

private:

  SceneGraph::TransformManagerPropertyHandler<T>* mProperty; ///< The real property

};

/**
 * A wrapper class for getting/setting a transform manager property component
 * Animators use this instead of accessing properties directly.
 */
template <typename T, uint32_t COMPONENT>
class TransformManagerPropertyComponentAccessor
{
public:

  /**
   * Create a property component.
   * @param [in] property The property to access.
   */
  TransformManagerPropertyComponentAccessor( SceneGraph::PropertyBase* property )
  : mProperty( static_cast< SceneGraph::TransformManagerPropertyHandler<T>* >(property) ) // we know the type
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
    return mProperty != nullptr;
  }

  /**
   * Reset the property accessor
   * @post Calling any other PropertyAccessor is invalid.
   */
  void Reset()
  {
    mProperty = nullptr;
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
  float Get( BufferIndex bufferIndex ) const
  {
    DALI_ASSERT_DEBUG( nullptr != mProperty && "PropertyAccessor::Get() mProperty was nullptr" );
    return mProperty->GetFloatComponent( COMPONENT );
  }

  /**
   * @copydoc AnimatableProperty<float>::Set()
   */
  void Set( BufferIndex bufferIndex, float value ) const
  {
    DALI_ASSERT_DEBUG( nullptr != mProperty && "PropertyAccessor::Set() mProperty was nullptr" );
    mProperty->SetFloatComponent( value, COMPONENT );
  }

  /**
   * @copydoc AnimatableProperty<float>::Bake()
   */
  void Bake( BufferIndex bufferIndex, float value ) const
  {
    DALI_ASSERT_DEBUG( nullptr != mProperty && "PropertyAccessor::Bake() mProperty was nullptr" );
    mProperty->BakeFloatComponent( value, COMPONENT );
  }

private:

  // Undefined
  TransformManagerPropertyComponentAccessor() = delete;
  TransformManagerPropertyComponentAccessor(const TransformManagerPropertyComponentAccessor& property) = delete;
  TransformManagerPropertyComponentAccessor& operator=(const TransformManagerPropertyComponentAccessor& rhs) = delete;

private:

  SceneGraph::TransformManagerPropertyHandler<T>* mProperty; ///< The real property

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_ACCESSOR_H__

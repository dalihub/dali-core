#ifndef __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_COMPONENT_ACCESSOR_H__
#define __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_COMPONENT_ACCESSOR_H__

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

namespace Dali
{

namespace Internal
{

/**
 * A wrapper class for getting/setting a float component of another property.
 * Animators use this instead of accessing properties directly.
 */
template < typename PropertyType >
class PropertyComponentAccessorX
{
public:

  /**
   * Create a property component.
   * @param [in] property The property which holds a float component.
   */
  PropertyComponentAccessorX( SceneGraph::PropertyBase* property )
  : mProperty( static_cast< SceneGraph::AnimatableProperty<PropertyType>* >(property) ) // we know the type
  {
  }

  /**
   * Non-virtual destructor; PropertyComponentAccessorX is not suitable as a base class.
   */
  ~PropertyComponentAccessorX()
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
   * @post Calling any other PropertyComponentAccessorX is invalid.
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
  float Get( BufferIndex bufferIndex ) const
  {
    DALI_ASSERT_DEBUG( nullptr != mProperty && "PropertyComponentAccessorX::Get() mProperty was nullptr" );
    return mProperty->Get( bufferIndex ).x; // X Component only!
  }

  /**
   * @copydoc SceneGraph::AnimatableProperty<float>::Set()
   */
  void Set( BufferIndex bufferIndex, float value ) const
  {
    DALI_ASSERT_DEBUG( nullptr != mProperty && "PropertyComponentAccessorX::Set() mProperty was nullptr" );
    mProperty->SetX( bufferIndex, value );
  }

  /**
   * @copydoc SceneGraph::AnimatableProperty<float>::Bake()
   */
  void Bake( BufferIndex bufferIndex, float value ) const
  {
    DALI_ASSERT_DEBUG( nullptr != mProperty && "PropertyComponentAccessorX::Bake() mProperty was nullptr" );
    mProperty->BakeX( bufferIndex, value );
  }

private:

  // Undefined
  PropertyComponentAccessorX() = delete;
  PropertyComponentAccessorX(const PropertyComponentAccessorX& property) = delete;
  PropertyComponentAccessorX& operator=(const PropertyComponentAccessorX& rhs) = delete;

private:

  SceneGraph::AnimatableProperty<PropertyType>* mProperty; ///< The real property

};

/**
 * A wrapper class for getting/setting a float component of another property.
 * Animators use this instead of accessing properties directly.
 */
template < typename PropertyType >
class PropertyComponentAccessorY
{
public:

  /**
   * Create a property component.
   * @param [in] property The property which holds a float component.
   */
  PropertyComponentAccessorY( SceneGraph::PropertyBase* property )
  : mProperty( static_cast< SceneGraph::AnimatableProperty<PropertyType>* >(property) ) // we know the type
  {
  }

  /**
   * Non-virtual destructor; PropertyComponentAccessorY is not suitable as a base class.
   */
  ~PropertyComponentAccessorY()
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
   * @post Calling any other PropertyComponentAccessorY is invalid.
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
  float Get( BufferIndex bufferIndex ) const
  {
    DALI_ASSERT_DEBUG( nullptr != mProperty && "PropertyComponentAccessorY::Get() mProperty was nullptr" );
    return mProperty->Get( bufferIndex ).y; // Y Component only!
  }

  /**
   * @copydoc SceneGraph::AnimatableProperty<float>::Set()
   */
  void Set( BufferIndex bufferIndex, float value ) const
  {
    DALI_ASSERT_DEBUG( nullptr != mProperty && "PropertyComponentAccessorY::Set() mProperty was nullptr" );
    mProperty->SetY( bufferIndex, value );
  }

  /**
   * @copydoc SceneGraph::AnimatableProperty<float>::Bake()
   */
  void Bake( BufferIndex bufferIndex, float value ) const
  {
    DALI_ASSERT_DEBUG( nullptr != mProperty && "PropertyComponentAccessorY::Bake() mProperty was nullptr" );
    mProperty->BakeY( bufferIndex, value );
  }

private:

  // Undefined
  PropertyComponentAccessorY() = delete;
  PropertyComponentAccessorY(const PropertyComponentAccessorY& property) = delete;
  PropertyComponentAccessorY& operator=(const PropertyComponentAccessorY& rhs) = delete;

private:

  SceneGraph::AnimatableProperty<PropertyType>* mProperty; ///< The real property
};

/**
 * A wrapper class for getting/setting a float component of another property.
 * Animators use this instead of accessing properties directly.
 */
template < typename PropertyType >
class PropertyComponentAccessorZ
{
public:

  /**
   * Create a property component.
   * @param [in] property The property which holds a float component.
   */
  PropertyComponentAccessorZ( SceneGraph::PropertyBase* property )
  : mProperty( static_cast< SceneGraph::AnimatableProperty<PropertyType>* >(property) ) // we know the type
  {
  }

  /**
   * Non-virtual destructor; PropertyComponentAccessorZ is not suitable as a base class.
   */
  ~PropertyComponentAccessorZ()
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
   * @post Calling any other PropertyComponentAccessorZ is invalid.
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
  float Get( BufferIndex bufferIndex ) const
  {
    DALI_ASSERT_DEBUG( nullptr != mProperty && "PropertyComponentAccessorZ::Get() mProperty was nullptr" );
    return mProperty->Get( bufferIndex ).z; // Z Component only!
  }

  /**
   * @copydoc SceneGraph::AnimatableProperty<float>::Set()
   */
  void Set( BufferIndex bufferIndex, float value ) const
  {
    DALI_ASSERT_DEBUG( nullptr != mProperty && "PropertyComponentAccessorZ::Set() mProperty was nullptr" );
    mProperty->SetZ( bufferIndex, value );
  }

  /**
   * @copydoc SceneGraph::AnimatableProperty<float>::Bake()
   */
  void Bake( BufferIndex bufferIndex, float value ) const
  {
    DALI_ASSERT_DEBUG( nullptr != mProperty && "PropertyComponentAccessorZ::Bake() mProperty was nullptr" );
    mProperty->BakeZ( bufferIndex, value );
  }

private:

  // Undefined
  PropertyComponentAccessorZ() = delete;
  PropertyComponentAccessorZ(const PropertyComponentAccessorZ& property) = delete;
  PropertyComponentAccessorZ& operator=(const PropertyComponentAccessorZ& rhs) = delete;

private:

  SceneGraph::AnimatableProperty<PropertyType>* mProperty; ///< The real property
};

/**
 * A wrapper class for getting/setting a float component of another property.
 * Animators use this instead of accessing properties directly.
 */
template < typename PropertyType >
class PropertyComponentAccessorW
{
public:

  /**
   * Create a property component.
   * @param [in] property The property which holds a float component.
   */
  PropertyComponentAccessorW( SceneGraph::PropertyBase* property )
  : mProperty( static_cast< SceneGraph::AnimatableProperty<PropertyType>* >(property) ) // we know the type
  {
  }

  /**
   * Non-virtual destructor; PropertyComponentAccessorW is not suitable as a base class.
   */
  ~PropertyComponentAccessorW()
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
   * @post Calling any other PropertyComponentAccessorW is invalid.
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
  float Get( BufferIndex bufferIndex ) const
  {
    DALI_ASSERT_DEBUG( nullptr != mProperty && "PropertyComponentAccessorW::Get() mProperty was nullptr" );
    return mProperty->Get( bufferIndex ).w; // W Component only!
  }

  /**
   * @copydoc SceneGraph::AnimatableProperty<float>::Set()
   */
  void Set( BufferIndex bufferIndex, float value ) const
  {
    DALI_ASSERT_DEBUG( nullptr != mProperty && "PropertyComponentAccessorW::Set() mProperty was nullptr" );
    mProperty->SetW( bufferIndex, value );
  }

  /**
   * @copydoc SceneGraph::AnimatableProperty<float>::Bake()
   */
  void Bake( BufferIndex bufferIndex, float value ) const
  {
    DALI_ASSERT_DEBUG( nullptr != mProperty && "PropertyComponentAccessorW::Bake() mProperty was nullptr" );
    mProperty->BakeW( bufferIndex, value );
  }

private:

  // Undefined
  PropertyComponentAccessorW() = delete;
  PropertyComponentAccessorW(const PropertyComponentAccessorW& property) = delete;
  PropertyComponentAccessorW& operator=(const PropertyComponentAccessorW& rhs) = delete;

private:

  SceneGraph::AnimatableProperty<PropertyType>* mProperty; ///< The real property

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_COMPONENT_ACCESSOR_H__

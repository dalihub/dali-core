#ifndef __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_COMPONENT_ACCESSOR_H__
#define __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_COMPONENT_ACCESSOR_H__

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
  : mProperty( dynamic_cast< SceneGraph::AnimatableProperty<PropertyType>* >(property) )
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
    return mProperty != NULL;
  }

  /**
   * Reset the property accessor
   * @post Calling any other PropertyComponentAccessorX is invalid.
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
  const float& Get( BufferIndex bufferIndex ) const
  {
    DALI_ASSERT_DEBUG( NULL != mProperty && "PropertyComponentAccessorX::Get() mProperty was NULL" );
    return mProperty->Get( bufferIndex ).x; // X Component only!
  }

  /**
   * @copydoc SceneGraph::AnimatableProperty<float>::Set()
   */
  void Set( BufferIndex bufferIndex, float value ) const
  {
    DALI_ASSERT_DEBUG( NULL != mProperty && "PropertyComponentAccessorX::Set() mProperty was NULL" );
    mProperty->SetX( bufferIndex, value );
  }

  /**
   * @copydoc SceneGraph::AnimatableProperty<float>::Bake()
   */
  void Bake( BufferIndex bufferIndex, float value ) const
  {
    DALI_ASSERT_DEBUG( NULL != mProperty && "PropertyComponentAccessorX::Bake() mProperty was NULL" );
    mProperty->BakeX( bufferIndex, value );
  }

private:

  // Undefined
  PropertyComponentAccessorX(const PropertyComponentAccessorX& property);

  // Undefined
  PropertyComponentAccessorX& operator=(const PropertyComponentAccessorX& rhs);

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
  : mProperty( dynamic_cast< SceneGraph::AnimatableProperty<PropertyType>* >(property) )
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
    return mProperty != NULL;
  }

  /**
   * Reset the property accessor
   * @post Calling any other PropertyComponentAccessorY is invalid.
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
  const float& Get( BufferIndex bufferIndex ) const
  {
    DALI_ASSERT_DEBUG( NULL != mProperty && "PropertyComponentAccessorY::Get() mProperty was NULL" );
    return mProperty->Get( bufferIndex ).y; // Y Component only!
  }

  /**
   * @copydoc SceneGraph::AnimatableProperty<float>::Set()
   */
  void Set( BufferIndex bufferIndex, float value ) const
  {
    DALI_ASSERT_DEBUG( NULL != mProperty && "PropertyComponentAccessorY::Set() mProperty was NULL" );
    mProperty->SetY( bufferIndex, value );
  }

  /**
   * @copydoc SceneGraph::AnimatableProperty<float>::Bake()
   */
  void Bake( BufferIndex bufferIndex, float value ) const
  {
    DALI_ASSERT_DEBUG( NULL != mProperty && "PropertyComponentAccessorY::Bake() mProperty was NULL" );
    mProperty->BakeY( bufferIndex, value );
  }

private:

  // Undefined
  PropertyComponentAccessorY(const PropertyComponentAccessorY& property);

  // Undefined
  PropertyComponentAccessorY& operator=(const PropertyComponentAccessorY& rhs);

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
  : mProperty( dynamic_cast< SceneGraph::AnimatableProperty<PropertyType>* >(property) )
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
    return mProperty != NULL;
  }

  /**
   * Reset the property accessor
   * @post Calling any other PropertyComponentAccessorZ is invalid.
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
  const float& Get( BufferIndex bufferIndex ) const
  {
    DALI_ASSERT_DEBUG( NULL != mProperty && "PropertyComponentAccessorZ::Get() mProperty was NULL" );
    return mProperty->Get( bufferIndex ).z; // Z Component only!
  }

  /**
   * @copydoc SceneGraph::AnimatableProperty<float>::Set()
   */
  void Set( BufferIndex bufferIndex, float value ) const
  {
    DALI_ASSERT_DEBUG( NULL != mProperty && "PropertyComponentAccessorZ::Set() mProperty was NULL" );
    mProperty->SetZ( bufferIndex, value );
  }

  /**
   * @copydoc SceneGraph::AnimatableProperty<float>::Bake()
   */
  void Bake( BufferIndex bufferIndex, float value ) const
  {
    DALI_ASSERT_DEBUG( NULL != mProperty && "PropertyComponentAccessorZ::Bake() mProperty was NULL" );
    mProperty->BakeZ( bufferIndex, value );
  }

private:

  // Undefined
  PropertyComponentAccessorZ(const PropertyComponentAccessorZ& property);

  // Undefined
  PropertyComponentAccessorZ& operator=(const PropertyComponentAccessorZ& rhs);

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
  : mProperty( dynamic_cast< SceneGraph::AnimatableProperty<PropertyType>* >(property) )
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
    return mProperty != NULL;
  }

  /**
   * Reset the property accessor
   * @post Calling any other PropertyComponentAccessorW is invalid.
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
  const float& Get( BufferIndex bufferIndex ) const
  {
    DALI_ASSERT_DEBUG( NULL != mProperty && "PropertyComponentAccessorW::Get() mProperty was NULL" );
    return mProperty->Get( bufferIndex ).w; // W Component only!
  }

  /**
   * @copydoc SceneGraph::AnimatableProperty<float>::Set()
   */
  void Set( BufferIndex bufferIndex, float value ) const
  {
    DALI_ASSERT_DEBUG( NULL != mProperty && "PropertyComponentAccessorW::Set() mProperty was NULL" );
    mProperty->SetW( bufferIndex, value );
  }

  /**
   * @copydoc SceneGraph::AnimatableProperty<float>::Bake()
   */
  void Bake( BufferIndex bufferIndex, float value ) const
  {
    DALI_ASSERT_DEBUG( NULL != mProperty && "PropertyComponentAccessorW::Bake() mProperty was NULL" );
    mProperty->BakeW( bufferIndex, value );
  }

private:

  // Undefined
  PropertyComponentAccessorW(const PropertyComponentAccessorW& property);

  // Undefined
  PropertyComponentAccessorW& operator=(const PropertyComponentAccessorW& rhs);

private:

  SceneGraph::AnimatableProperty<PropertyType>* mProperty; ///< The real property
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_COMPONENT_ACCESSOR_H__

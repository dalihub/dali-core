#ifndef __DALI_INTERNAL_PROPERTY_METADATA_H__
#define __DALI_INTERNAL_PROPERTY_METADATA_H__

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <algorithm>
#include <utility>

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/object/property-value.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{
class PropertyBase;
}

/**
 * @brief Base class for the Metadata required by custom and registered animatable properties.
 *
 * The value type field should be queried, before accessing the scene-graph property:
 *
 * @code
 * void Example(PropertyEntry entry)
 * {
 *   if (entry.value.GetType() == Property::VECTOR3)
 *   {
 *     SceneGraph::AnimatableProperty<Vector3>* property = dynamic_cast< SceneGraph::AnimatableProperty<Vector3>* >( entry.property );
 *     ...
 *   }
 * @endcode
 *
 */
class PropertyMetadata
{
public:

  /**
   * @brief Virtual Destructor.
   */
  virtual ~PropertyMetadata()
  {
  }

  /**
   * @brief Returns whether the property is animatable (i.e. if its a scene graph property).
   * @return True if animatable, false otherwise
   */
  bool IsAnimatable( void ) const
  {
    return NULL != mSceneGraphProperty;
  }

  /**
   * @brief Whether the property can be written to.
   * @return True if the property can be written to, false otherwise
   */
  bool IsWritable( void ) const
  {
    return mWritable;
  }

  /**
   * @brief Retrieves the scene-graph property.
   * @return The scene graph property
   *
   * @note Should only be called if the scene-graph property was passed in originally. Will debug assert if the stored property is NULL.
   */
  const SceneGraph::PropertyBase* GetSceneGraphProperty() const
  {
    DALI_ASSERT_DEBUG( mSceneGraphProperty && "Accessing uninitialized SceneGraph property" );
    return mSceneGraphProperty;
  }

  /**
   * @brief Retrieve the type of the property.
   * @return Type of the held property value
   */
  inline Property::Type GetType() const
  {
    return value.GetType();
  }

  /**
   * Set the cached value of the property.
   * @param[in] value The propertyValue to set.
   */
  void SetPropertyValue( const Property::Value& propertyValue );

  /**
   * Get the cached value of a the property.
   * @return The cached value of the property.
   */
  Property::Value GetPropertyValue() const;

  /**
   * Modifies the stored value by the relativeValue.
   * @param[in] relativeValue The value to change by.
   */
  void AdjustPropertyValueBy( const Property::Value& relativeValue );

protected:

  /**
   * @brief Constructor to create Metadata for a property.
   * @param[in] propertyValue       The value of the property (this is used by the event thread)
   * @param[in] sceneGraphProperty  A pointer to the scene-graph owned property
   * @param[in] writable            Whether the property is writable
   */
  PropertyMetadata( const Property::Value& propertyValue,
                    const SceneGraph::PropertyBase* sceneGraphProperty,
                    bool writable )
  : value( mStoredValue ),
    componentIndex( Property::INVALID_COMPONENT_INDEX ),
    mStoredValue( propertyValue ),
    mSceneGraphProperty( sceneGraphProperty ),
    mWritable( writable )
  {
  }

  /**
   * @brief Constructor to create Metadata for a component of another property.
   * @param[in] sceneGraphProperty      A pointer to the scene-graph owned property
   * @param[in] writable                Whether the property is writable
   * @param[in] baseValueRef            A reference to the metadata of the base animatable property
   * @param[in] propertyComponentIndex  The component index of the property
   */
  PropertyMetadata( const SceneGraph::PropertyBase* sceneGraphProperty, bool writable, Property::Value& baseValueRef, int propertyComponentIndex )
  : value( baseValueRef ),
    componentIndex( propertyComponentIndex ),
    mStoredValue(),
    mSceneGraphProperty( sceneGraphProperty ),
    mWritable( writable )
  {
  }

private:

  // Not implemented
  PropertyMetadata( const PropertyMetadata& );
  PropertyMetadata& operator=( const PropertyMetadata& );

public: // Data

  /**
   * @brief The value of this property used to read/write by the event thread.
   *
   * If a component index, then refers to the value in the PropertyMetatdata of the base property
   * to ensure the components are kept in sync with the overall value on the event thread.
   * Otherwise, this just refers to the storedValue.
   */
  Property::Value& value;

  /**
   * @brief The index of the property component.
   */
  int componentIndex;

private:

  Property::Value mStoredValue;                         ///< The stored property value used to read/write by the event thread
  const SceneGraph::PropertyBase* mSceneGraphProperty;  ///< A pointer to a scene-graph property; should not be modified from actor-thread
  bool mWritable:1;                                     ///< Whether the property is writable
};

/**
 * @brief Metadata for a registered animatable property.
 */
class AnimatablePropertyMetadata : public PropertyMetadata
{
public:

  /**
   * @brief Constructs metadata for a registered animatable property.
   * @param[in] propertyIndex           The index of the animatable property
   * @param[in] propertyComponentIndex  The component index of the animatable property
   * @param[in] propertyValue           The value of the property (this is used by the event thread)
   * @param[in] sceneGraphProperty      A pointer to the scene-graph owned property
   *
   * @note The base animatable property MUST be created before the component animatable property.
   */
  AnimatablePropertyMetadata( Property::Index propertyIndex,
                              const Property::Value& propertyValue,
                              const SceneGraph::PropertyBase* sceneGraphProperty )
  : PropertyMetadata( propertyValue, sceneGraphProperty, true ),
    index( propertyIndex )
  {
    DALI_ASSERT_DEBUG( sceneGraphProperty && "Uninitialized scene-graph property" );
  }

  /**
   * @brief Constructs metadata for a registered animatable component of another property.
   * @param[in] propertyIndex           The index of the animatable property
   * @param[in] propertyComponentIndex  The component index of the animatable property
   * @param[in] baseValueRef            A reference to the metadata of the base animatable property
   * @param[in] sceneGraphProperty      A pointer to the scene-graph owned property
   *
   * @note The base animatable property MUST be created before the component animatable property.
   */
  AnimatablePropertyMetadata( Property::Index propertyIndex,
                              int propertyComponentIndex,
                              Property::Value& baseValueRef,
                              const SceneGraph::PropertyBase* sceneGraphProperty )
  : PropertyMetadata( sceneGraphProperty, true, baseValueRef, propertyComponentIndex ),
    index( propertyIndex )
  {
    DALI_ASSERT_DEBUG( sceneGraphProperty && "Uninitialized scene-graph property" );
  }

  /**
   * @brief Destructor.
   */
  virtual ~AnimatablePropertyMetadata()
  {
  }

private:

  // Not implemented
  AnimatablePropertyMetadata();
  AnimatablePropertyMetadata( const AnimatablePropertyMetadata& );
  AnimatablePropertyMetadata& operator=( const AnimatablePropertyMetadata& );

public: // Data

  Property::Index   index;    ///< The index of the property.
};

class CustomPropertyMetadata : public PropertyMetadata
{
public:

  /**
   * Constructs Metadata for scene-graph-based custom properties, i.e. animatable custom properties.
   * @param[in] propertyName        The name of the custom property
   * @param[in] propertyKey         The key of the custom property
   * @param[in] propertyValue       The value of the property (this is used by the event thread)
   * @param[in] sceneGraphProperty  A pointer to the scene-graph owned property
   *
   * @note A valid sceneGraphProperty is mandatory otherwise this will debug assert.
   */
  CustomPropertyMetadata( const std::string& propertyName,
                          Property::Index propertyKey,
                          const Property::Value& propertyValue,
                          const SceneGraph::PropertyBase* sceneGraphProperty )
  : PropertyMetadata( propertyValue, sceneGraphProperty, true ),
    name( propertyName ),
    key( propertyKey ),
    childPropertyIndex( Property::INVALID_INDEX )
  {
    DALI_ASSERT_DEBUG( sceneGraphProperty && "Uninitialized scene-graph property" );
  }

  /**
   * Constructs metadata for event side only custom properties.
   * @param[in] propertyName   The name of the custom property
   * @param[in] propertyValue  The value of the property (this is used by the event thread)
   * @param[in] accessMode     The access mode of the custom property (writable, animatable etc)
   *
   * @note The access mode MUST NOT be animatable otherwise this will debug assert.
   */
  CustomPropertyMetadata( const std::string& propertyName,
                          const Property::Value& propertyValue,
                          Property::AccessMode accessMode )
  : PropertyMetadata( propertyValue, NULL, ( accessMode != Property::READ_ONLY ) ),
    name( propertyName ),
    key( Property::INVALID_KEY ),
    childPropertyIndex( Property::INVALID_INDEX )
  {
    DALI_ASSERT_DEBUG( accessMode != Property::ANIMATABLE && "Event side only properties should not be animatable" );
  }

  /**
   * @brief Destructor.
   */
  virtual ~CustomPropertyMetadata()
  {
  }

private:

  // Not implemented
  CustomPropertyMetadata();
  CustomPropertyMetadata( const CustomPropertyMetadata& );
  CustomPropertyMetadata& operator=( const CustomPropertyMetadata& );

public: // Data

  std::string       name;                 ///< The name of the property.
  Property::Index   key;                  ///< The key of the property.
  Property::Index   childPropertyIndex;   ///< The index as a child property.
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_PROPERTY_METADATA_H__

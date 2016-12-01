#ifndef __DALI_INTERNAL_PROPERTY_METADATA_H__
#define __DALI_INTERNAL_PROPERTY_METADATA_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
 * An entry in a property metadata lookup.
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
   * Constructor for an uninitalized property metadata
   */
  PropertyMetadata()
  : value(),
    componentIndex(Property::INVALID_COMPONENT_INDEX),
    mProperty(NULL)
  {
  }

  /**
   * Constructor for property metadata
   * @param [in] newProperty A pointer to the property metadata.
   */
  PropertyMetadata(const SceneGraph::PropertyBase* newProperty)
  : value(), // value is held by newProperty
    componentIndex(Property::INVALID_COMPONENT_INDEX),
    mProperty(newProperty)
  {
    DALI_ASSERT_DEBUG(mProperty && "Uninitialized scenegraph property");
  }

  /**
   * Constructor for property metadata
   * @param [in] newValue The value of the scene-graph owned property.
   */
  PropertyMetadata(Property::Value newValue)
  : value(newValue),
    componentIndex(Property::INVALID_COMPONENT_INDEX),
    mProperty(NULL)
  {
  }

  /**
   * Destructor for property metadata
   */
  virtual ~PropertyMetadata()
  {
  }

  /**
   * @return true if the property is animatable (i.e. if its a scene graph property)
   */
  bool IsAnimatable(void) const
  {
    return NULL != mProperty;
  }

  /**
   * @return true if the property can be written to
   */
  virtual bool IsWritable(void) const = 0;

  /**
   * @return the scene graph property
   */
  const SceneGraph::PropertyBase* GetSceneGraphProperty() const
  {
    DALI_ASSERT_DEBUG(mProperty && "Accessing uninitialized SceneGraph property");
    return mProperty;
  }

  /*
   * @return type of the held property value
   */
  inline Property::Type GetType() const { return value.GetType(); }

  Property::Value value;  ///< The property value for a non animatable and custom property
  int componentIndex;     ///< The index of the property component

protected:

  // Not implemented
  PropertyMetadata( const PropertyMetadata& );
  PropertyMetadata& operator=( const PropertyMetadata& );

  const SceneGraph::PropertyBase* mProperty; ///< A pointer to a scene-graph property; should not be modified from actor-thread.
};


/**
 * An entry in an animatable property metadata lookup.
 * The value type field should be queried, before accessing the animatable property:
 */
class AnimatablePropertyMetadata : public PropertyMetadata
{
public:

  /**
   * Constructor for metadata of animatable property
   * @param [in] newIndex The index of the animatable property.
   * @param [in] newType The type ID of the animatable property.
   * @param [in] newProperty A pointer to the scene-graph owned property.
   */
  AnimatablePropertyMetadata( Property::Index newIndex,
                        int newComponentIndex,
                        Property::Type newType,
                        const SceneGraph::PropertyBase* newProperty )
  : index(newIndex)
  {
    componentIndex = newComponentIndex;
    value = Property::Value(newType);
    mProperty = newProperty;
    DALI_ASSERT_DEBUG(mProperty && "Uninitialized scenegraph property");
  }

  /**
   * Constructor for metadata of animatable property
   * @param [in] newIndex The index of the animatable property.
   * @param [in] newValue The value of the scene-graph owned property.
   */
  AnimatablePropertyMetadata( Property::Index newIndex,
                        int newComponentIndex,
                        Property::Value newValue )
  : index(newIndex)
  {
    componentIndex = newComponentIndex;
    value = newValue;
  }

  /**
   * @return true if the property can be written to
   */
  virtual bool IsWritable(void) const
  {
    return true;
  }

  Property::Index index;       ///< The index of the property

private:

  // Not implemented
  AnimatablePropertyMetadata();
  AnimatablePropertyMetadata( const AnimatablePropertyMetadata& );
  AnimatablePropertyMetadata& operator=( const AnimatablePropertyMetadata& );
};

class CustomPropertyMetadata : public PropertyMetadata
{
public:

  /**
   * Constructor for metadata of scene graph based properties
   * @param [in] newName The name of the custom property.
   * @param [in] newKey  The key of the custom property.
   * @param [in] newType The type ID of the custom property.
   * @param [in] newProperty A pointer to the scene-graph owned property.
   */
  CustomPropertyMetadata( const std::string& newName,
                          Property::Index newKey,
                          Property::Type newType,
                          const SceneGraph::PropertyBase* newProperty)
  : name(newName),
    key(newKey),
    childPropertyIndex(Property::INVALID_INDEX),
    mAccessMode(Property::ANIMATABLE)
  {
    value = Property::Value(newType);
    mProperty = newProperty;
    DALI_ASSERT_DEBUG(mProperty && "Uninitialized scenegraph property");
  }

  /**
   * Constructor for metadata of event side only properties
   * @param [in] newName The name of the custom property.
   * @param [in] newValue The value of the custom property.
   * @param [in] accessMode The access mode of the custom property (writable, animatable etc).
   */
  CustomPropertyMetadata( const std::string& newName,
                          Property::Value newValue,
                          Property::AccessMode accessMode )
  : name(newName),
    key(Property::INVALID_KEY),
    childPropertyIndex(Property::INVALID_INDEX),
    mAccessMode(accessMode)
  {
    value = newValue;
    DALI_ASSERT_DEBUG(accessMode != Property::ANIMATABLE && "Animatable must have scenegraph property");
  }

  /**
   * @return true if the property can be written to
   */
  virtual bool IsWritable(void) const
  {
    return (mAccessMode == Property::ANIMATABLE) || (mAccessMode == Property::READ_WRITE);
  }

  std::string name;       ///< The name of the property
  Property::Index key;    ///< The key of the property
  Property::Index childPropertyIndex; ///< The index as a child property

private:

  // Not implemented
  CustomPropertyMetadata();
  CustomPropertyMetadata( const CustomPropertyMetadata& );
  CustomPropertyMetadata& operator=( const CustomPropertyMetadata& );

private:
  Property::AccessMode mAccessMode; ///< The mode of the property
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_PROPERTY_METADATA_H__

#ifndef __DALI_INTERNAL_SCENE_GRAPH_CUSTOM_PROPERTY_H__
#define __DALI_INTERNAL_SCENE_GRAPH_CUSTOM_PROPERTY_H__

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

// EXTERNAL INCLUDES
#include <algorithm>
#include <utility>

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <dali/public-api/object/property.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{
class PropertyBase;
}

/**
 * An entry in a scene-graph property lookup.
 * The type field should be queried, before accessing the scene-graph property:
 *
 * @code
 * void Example(PropertyEntry entry)
 * {
 *   if (entry.type == Property::VECTOR3)
 *   {
 *     SceneGraph::AnimatableProperty<Vector3>* property = dynamic_cast< SceneGraph::AnimatableProperty<Vector3>* >( entry.property );
 *     ...
 *   }
 * @endcode
 *
 */
class CustomProperty
{
public:

  /**
   * Constructor for scene graph based properties
   * @param [in] newName The name of the custom property.
   * @param [in] newType The type ID of the custom property.
   * @param [in] newProperty A pointer to the scene-graph owned property.
   */
  CustomProperty( const std::string& newName,
                  Property::Type newType,
                  const SceneGraph::PropertyBase* newProperty)
  : name(newName),
    type(newType),
    value(), // value is held by newProperty
    mProperty(newProperty),
    mAccessMode(Property::ANIMATABLE)
  {
    DALI_ASSERT_DEBUG(mProperty && "Uninitialized scenegraph property") ;
  }

  /**
   * Constructor for event side only properties
   * @param [in] newName The name of the custom property.
   * @param [in] newIndex The index of the custom property.
   * @param [in] newType The type ID of the custom property.
   * @param [in] newProperty A pointer to the scene-graph owned property.
   */
  CustomProperty( const std::string& newName,
                  Property::Value newValue,
                  Property::AccessMode accessMode )
  : name(newName),
    type(newValue.GetType()),
    value(newValue),
    mProperty(NULL),
    mAccessMode(accessMode)
  {
    DALI_ASSERT_DEBUG(accessMode != Property::ANIMATABLE && "Animatable must have scenegraph property") ;
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
  bool IsWritable(void) const
  {
    return (mAccessMode == Property::ANIMATABLE) || (mAccessMode == Property::READ_WRITE) ;
  }

  /**
   * @return the scene graph property
   */
  const SceneGraph::PropertyBase* GetSceneGraphProperty() const
  {
    DALI_ASSERT_DEBUG(mProperty && "Accessing uninitialized SceneGraph property") ;
    return mProperty;
  }

  std::string name;       ///< The name of the property
  Property::Type type;    ///< The type of the property
  Property::Value value;  ///< The property value for a non animatable and custom property

private:

  // Not implemented
  CustomProperty();
  CustomProperty( const CustomProperty& );
  CustomProperty& operator=( const CustomProperty& );

private:
  const SceneGraph::PropertyBase* mProperty; ///< A pointer to a scene-graph property; should not be modified from actor-thread.
  Property::AccessMode mAccessMode; ///< The mode of the property
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_CUSTOM_PROPERTY_H__

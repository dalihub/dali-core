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
#include <dali/public-api/common/map-wrapper.h>
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
   * Default constructor.
   */
  CustomProperty()
  : type(Property::NONE),
    mProperty(NULL),
    mAccessMode(Property::READ_ONLY)
  {
  }

  /**
   * Convenience constructor.
   * @param [in] newName The name of the custom property.
   * @param [in] newType The type ID of the custom property.
   * @param [in] newProperty A pointer to the scene-graph owned property.
   */
  CustomProperty(const std::string& newName,
                  Property::Type newType,
                  const SceneGraph::PropertyBase* newProperty)
  : name(newName),
    type(newType),
    mProperty(newProperty),
    mAccessMode(Property::ANIMATABLE)
  {
    DALI_ASSERT_DEBUG(mProperty && "Uninitialized scenegraph property") ;
  }

  CustomProperty(const std::string& newName,
                  Property::Value newValue,
                  Property::AccessMode accessMode)
  : name(newName),
    value(newValue),
    mProperty(NULL),
    mAccessMode(accessMode)
  {
    type = value.GetType() ;
    DALI_ASSERT_DEBUG(accessMode != Property::ANIMATABLE && "Animatable must have scenegraph property") ;
  }

  bool IsAnimatable(void) const
  {
    return NULL != mProperty ;
  }

  bool IsWritable(void) const
  {
    return (mAccessMode == Property::ANIMATABLE) || (mAccessMode == Property::READ_WRITE) ;
  }

  std::string name; ///< The name of the property

  Property::Type type; ///< The type of the property

  Property::Value value ; ///< The property value for a non animatable and custom property

  const SceneGraph::PropertyBase* GetSceneGraphProperty() const
  {
    DALI_ASSERT_DEBUG(mProperty && "Get on uninitialized SceneGraph property") ;
    return mProperty ;
  }

private:
  const SceneGraph::PropertyBase* mProperty; ///< A pointer to a scene-graph property; should not be modified from actor-thread.
  Property::AccessMode mAccessMode; ///< The mode of the property
};

/**
 * Used for accessing scene-graph properties by property index
 */
typedef std::map<Property::Index, CustomProperty> CustomPropertyLookup;

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_CUSTOM_PROPERTY_H__

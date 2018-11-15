#ifndef DALI_INTERNAL_OBJECT_IMPL_HELPER_H
#define DALI_INTERNAL_OBJECT_IMPL_HELPER_H

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

// EXTERNAL INCLUDEs
#include <cstring>

// INTERNAL INCLUDES
#include <dali/public-api/object/property.h> // Dali::Property
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/property-owner-messages.h>
#include <dali/internal/update/manager/update-manager.h>

namespace Dali
{
namespace Internal
{
class PropertyMetadata;
class AnimatablePropertyMetadata;
class CustomPropertyMetadata;
class PropertyInputImpl;

namespace SceneGraph
{

class PropertyBase;
class PropertyOwner;

} // namespace SceneGraph

// methods needed as parameters
using FindAnimatablePropertyMethod = AnimatablePropertyMetadata* (Object::*)(Property::Index index) const;
using FindCustomPropertyMethod = CustomPropertyMetadata* (Object::*)(Property::Index index) const;

/**
 * Helper utilities to be used by class that implement Object
 */
namespace ObjectImplHelper
{
  // Get the (animatable) scene graph property. (All registered scene graph properties are animatable)
  inline const SceneGraph::PropertyBase* GetRegisteredSceneGraphProperty( const Object* object,
                                                                          FindAnimatablePropertyMethod findAnimatablePropertyMethod,
                                                                          FindCustomPropertyMethod findCustomPropertyMethod,
                                                                          Property::Index index )
  {
    const SceneGraph::PropertyBase* property = NULL;
    if ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX && index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX )
    {
      AnimatablePropertyMetadata* animatable = (object->*findAnimatablePropertyMethod)( index );
      DALI_ASSERT_ALWAYS( animatable && "Property index is invalid" );
      property = animatable->GetSceneGraphProperty();
    }
    else if ( ( index > CHILD_PROPERTY_REGISTRATION_START_INDEX ) && // Child properties are also stored as custom properties
              ( index <= PROPERTY_CUSTOM_MAX_INDEX ) )
    {
      CustomPropertyMetadata* custom = (object->*findCustomPropertyMethod)( index );
      DALI_ASSERT_ALWAYS( custom && "Property index is invalid" );
      property = custom->GetSceneGraphProperty();
    }
    return property;
  }

  inline void SetSceneGraphProperty( EventThreadServices& eventThreadServices,
                                     const Object* object,
                                     Property::Index index,
                                     const PropertyMetadata& entry,
                                     const Property::Value& value )
  {
    const SceneGraph::PropertyOwner* sceneObject = object->GetSceneObject();

    switch ( entry.GetType() )
    {
      case Property::BOOLEAN:
      {
        const SceneGraph::AnimatableProperty<bool>* property = dynamic_cast< const SceneGraph::AnimatableProperty<bool>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        // property is being used in a separate thread; queue a message to set the property
        SceneGraph::AnimatablePropertyMessage<bool>::Send( eventThreadServices, sceneObject, property, &SceneGraph::AnimatableProperty<bool>::Bake, value.Get<bool>() );

        break;
      }

      case Property::FLOAT:
      {
        const SceneGraph::AnimatableProperty<float>* property = dynamic_cast< const SceneGraph::AnimatableProperty<float>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        // property is being used in a separate thread; queue a message to set the property
        SceneGraph::AnimatablePropertyMessage<float>::Send( eventThreadServices, sceneObject, property, &SceneGraph::AnimatableProperty<float>::Bake, value.Get<float>() );

        break;
      }

      case Property::INTEGER:
      {
        const SceneGraph::AnimatableProperty<int>* property = dynamic_cast< const SceneGraph::AnimatableProperty<int>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        // property is being used in a separate thread; queue a message to set the property
        SceneGraph::AnimatablePropertyMessage<int>::Send( eventThreadServices, sceneObject, property, &SceneGraph::AnimatableProperty<int>::Bake, value.Get<int>() );

        break;
      }

      case Property::VECTOR2:
      {
        const SceneGraph::AnimatableProperty<Vector2>* property = dynamic_cast< const SceneGraph::AnimatableProperty<Vector2>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        // property is being used in a separate thread; queue a message to set the property
        SceneGraph::AnimatablePropertyMessage<Vector2>::Send( eventThreadServices, sceneObject, property, &SceneGraph::AnimatableProperty<Vector2>::Bake, value.Get<Vector2>() );

        break;
      }

      case Property::VECTOR3:
      {
        const SceneGraph::AnimatableProperty<Vector3>* property = dynamic_cast< const SceneGraph::AnimatableProperty<Vector3>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        // property is being used in a separate thread; queue a message to set the property
        SceneGraph::AnimatablePropertyMessage<Vector3>::Send( eventThreadServices, sceneObject, property, &SceneGraph::AnimatableProperty<Vector3>::Bake, value.Get<Vector3>() );

        break;
      }

      case Property::VECTOR4:
      {
        const SceneGraph::AnimatableProperty<Vector4>* property = dynamic_cast< const SceneGraph::AnimatableProperty<Vector4>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        // property is being used in a separate thread; queue a message to set the property
        SceneGraph::AnimatablePropertyMessage<Vector4>::Send( eventThreadServices, sceneObject, property, &SceneGraph::AnimatableProperty<Vector4>::Bake, value.Get<Vector4>() );

        break;
      }

      case Property::ROTATION:
      {
        const SceneGraph::AnimatableProperty<Quaternion>* property = dynamic_cast< const SceneGraph::AnimatableProperty<Quaternion>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        // property is being used in a separate thread; queue a message to set the property
        SceneGraph::AnimatablePropertyMessage<Quaternion>::Send( eventThreadServices, sceneObject, property,&SceneGraph::AnimatableProperty<Quaternion>::Bake,  value.Get<Quaternion>() );

        break;
      }

      case Property::MATRIX:
      {
        const SceneGraph::AnimatableProperty<Matrix>* property = dynamic_cast< const SceneGraph::AnimatableProperty<Matrix>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        // property is being used in a separate thread; queue a message to set the property
        SceneGraph::AnimatablePropertyMessage<Matrix>::Send( eventThreadServices, sceneObject, property,&SceneGraph::AnimatableProperty<Matrix>::Bake,  value.Get<Matrix>() );

        break;
      }

      case Property::MATRIX3:
      {
        const SceneGraph::AnimatableProperty<Matrix3>* property = dynamic_cast< const SceneGraph::AnimatableProperty<Matrix3>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        // property is being used in a separate thread; queue a message to set the property
        SceneGraph::AnimatablePropertyMessage<Matrix3>::Send( eventThreadServices, sceneObject, property, &SceneGraph::AnimatableProperty<Matrix3>::Bake,  value.Get<Matrix3>() );

        break;
      }

      default:
      {
        // ignore non-scene-graph types
      }
    }
  }
};



} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_OBJECT_IMPL_HELPER_H

#ifndef __DALI_INTERNAL_CUSTOM_OBJECT_H__
#define __DALI_INTERNAL_CUSTOM_OBJECT_H__

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
#include <dali/internal/event/common/object-impl.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{
class PropertyOwner;
}

class CustomObject : public Object
{
public:

  /**
   * Create a new custom object.
   * @return A pointer to the newly allocated object.
   */
  static CustomObject* New();

  /**
   * @copydoc Dali::Internal::Object::GetSceneObject()
   */
  virtual const SceneGraph::PropertyOwner* GetSceneObject() const;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectAnimatableProperty()
   */
  virtual const SceneGraph::PropertyBase* GetSceneObjectAnimatableProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectInputProperty()
   */
  virtual const PropertyInputImpl* GetSceneObjectInputProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::SetDefaultProperty()
   */
  virtual void SetDefaultProperty(Property::Index index, const Property::Value& propertyValue);

  /**
   * @copydoc Dali::Internal::Object::GetDefaultProperty()
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyCurrentValue()
   */
  virtual Property::Value GetDefaultPropertyCurrentValue( Property::Index index ) const;

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~CustomObject();

  /**
   * Private constructor; see also CustomObject::New()
   */
  CustomObject();

  // Undefined
  CustomObject(const CustomObject&);

  // Undefined
  CustomObject& operator=(const CustomObject& rhs);

protected:

  SceneGraph::PropertyOwner* mUpdateObject;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_CUSTOM_OBJECT_H__

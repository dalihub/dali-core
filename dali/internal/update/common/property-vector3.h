#ifndef __DALI_INTERNAL_SCENE_GRAPH_VECTOR3_PROPERTY_H__
#define __DALI_INTERNAL_SCENE_GRAPH_VECTOR3_PROPERTY_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/object/property-input.h>
#include <dali/public-api/object/property-types.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/event/common/property-input-impl.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

/**
 * A Vector3 non-animatable property.
 */
class PropertyVector3 : public PropertyInputImpl
{
public:

  /**
   * Create an non-animatable property.
   * @param [in] initialValue The initial value of the property.
   */
  PropertyVector3( Vector3 initialValue )
  : mValue( initialValue ),
    mDirtyFlag( true )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~PropertyVector3()
  {
  }

  /**
   * Clear the dirty flag
   */
  void Clear()
  {
    mDirtyFlag = false;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetType()
   */
  virtual Dali::Property::Type GetType() const
  {
    return Dali::PropertyTypes::Get<Vector3>();
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputInitialized()
   */
  virtual bool InputInitialized() const
  {
    return true;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputChanged()
   */
  virtual bool InputChanged() const
  {
    return mDirtyFlag;
  }

  /**
   * @copydoc Dali::PropertyInput::GetVector3()
   */
  virtual const Vector3& GetVector3( BufferIndex bufferIndex ) const
  {
    return mValue;
  }

  /**
   * Flag that the property has been Set during the current frame.
   */
  void OnSet()
  {
    mDirtyFlag = true;
  }

private:

  // Undefined
  PropertyVector3(const PropertyVector3& property);

  // Undefined
  PropertyVector3& operator=(const PropertyVector3& rhs);

public:

  Vector3 mValue; ///< The property value

private:

  bool mDirtyFlag;

};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_VECTOR3_PROPERTY_H__

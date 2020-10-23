#ifndef DALI_INTERNAL_SCENE_GRAPH_GESTURE_PROPERTIES_H
#define DALI_INTERNAL_SCENE_GRAPH_GESTURE_PROPERTIES_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
 * A template for a read only properties used by Gestures.
 */
template < class T >
class GestureProperty : public PropertyInputImpl
{
public:

  /**
   * Create a read-only gesture property.
   * @param [in] initialValue The initial value of the property.
   */
  GestureProperty( const T& initialValue )
  : mValue( initialValue ),
    mInputChanged( false )
  {
  }

  /**
   * Create a read-only gesture property.
   */
  GestureProperty()
  : mValue(),
    mInputChanged( false )
  {
  }

  /**
   * Virtual destructor.
   */
  ~GestureProperty() override = default;

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  Dali::Property::Type GetType() const override
  {
    return Dali::PropertyTypes::Get< T >();
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::IsClean()
   */
  virtual bool IsClean() const
  {
    return !InputChanged();
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputInitialized()
   */
  bool InputInitialized() const override
  {
    // A constraint cannot use the property until it has been inherited (at least once).
    return true;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputChanged()
   * @note A constraint can only receive the inherited property from the previous frame.
   */
  bool InputChanged() const override
  {
    return mInputChanged;
  }

  /**
   * @brief Resets mInputChanged back to false
   */
  void Reset()
  {
    mInputChanged = false;
  }

  /**
   * Set the property value.
   * @param[in] value The new property value.
   */
  void Set(const T& value)
  {
    mValue = value;
    mInputChanged = true;
  }

  /**
   * Get the property value.
   * @return The property value.
   */
  const T& Get() const
  {
    return mValue;
  }

private:

  // Undefined
  GestureProperty(const GestureProperty& property);

  // Undefined
  GestureProperty& operator=(const GestureProperty& rhs);

protected:

  T mValue;             ///< The property value
  bool mInputChanged:1; ///< Whether the property has been modified
};

/**
 * A read only Vector2 property used by Gestures.
 */
class GesturePropertyVector2 : public GestureProperty< Vector2 >
{
public:

  /**
   * Virtual destructor.
   */
  ~GesturePropertyVector2() override = default;

  /**
   * @copydoc Dali::PropertyInput::GetVector2()
   */
  const Vector2& GetVector2( BufferIndex bufferIndex ) const override
  {
    return mValue;
  }
};

/**
 * A read only bool property used by Gestures.
 */
class GesturePropertyBool : public GestureProperty< bool >
{
public:

  /**
   * Virtual destructor.
   */
  ~GesturePropertyBool() override = default;

  /**
   * @copydoc Dali::PropertyInput::GetBoolean()
   */
  const bool& GetBoolean( BufferIndex bufferIndex ) const override
  {
    return mValue;
  }
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_GESTURE_PROPERTIES_H

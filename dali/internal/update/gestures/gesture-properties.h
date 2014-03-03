#ifndef __DALI_INTERNAL_SCENE_GRAPH_GESTURE_PROPERTIES_H__
#define __DALI_INTERNAL_SCENE_GRAPH_GESTURE_PROPERTIES_H__

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
  virtual ~GestureProperty()
  {
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  virtual Dali::Property::Type GetType() const
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
  virtual bool InputInitialized() const
  {
    // A constraint cannot use the property until it has been inherited (at least once).
    return true;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputChanged()
   * @note A constraint can only receive the inherited property from the previous frame.
   */
  virtual bool InputChanged() const
  {
    return mInputChanged;
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
  virtual ~GesturePropertyVector2()
  {
  }

  /**
   * @copydoc Dali::PropertyInput::GetVector2()
   */
  virtual const Vector2& GetVector2( BufferIndex bufferIndex ) const
  {
    return mValue;
  }
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_GESTURE_PROPERTIES_H__

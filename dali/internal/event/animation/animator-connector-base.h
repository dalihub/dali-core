#ifndef __DALI_INTERNAL_ANIMATOR_CONNECTOR_BASE_H__
#define __DALI_INTERNAL_ANIMATOR_CONNECTOR_BASE_H__

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
#include <dali/internal/event/common/object-impl.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/devel-api/common/owner-container.h>
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

namespace Internal
{

class Animation;
class AnimatorConnectorBase;

typedef OwnerPointer<AnimatorConnectorBase> AnimatorConnectorPtr;

typedef OwnerContainer< AnimatorConnectorBase* > AnimatorConnectorContainer;

typedef AnimatorConnectorContainer::Iterator AnimatorConnectorIter;
typedef AnimatorConnectorContainer::ConstIterator AnimatorConnectorConstIter;

/**
 * An abstract base class for animator connectors.
 *
 * The scene-graph objects are created by a Object e.g. Actor is a proxy for SceneGraph::Node.
 * AnimatorConnectorBase observes the proxy object, in order to detect when a scene-graph object is created.
 */
class AnimatorConnectorBase: public Object::Observer
{
public:

  /**
   * Constructor.
   */
  AnimatorConnectorBase(
      Object& object,
      Property::Index propertyIndex,
      int componentIndex,
      AlphaFunction alpha,
      const TimePeriod& period)
  : mParent( NULL ),
    mObject( &object ),
    mAlphaFunction(alpha),
    mTimePeriod(period),
    mPropertyIndex( propertyIndex ),
    mComponentIndex( componentIndex )
  {
    object.AddObserver( *this );
  }

  /**
   * Virtual destructor.
   */
  virtual ~AnimatorConnectorBase()
  {
  }

  /**
   * Set the parent of the AnimatorConnector.
   * @pre The connector does not already have a parent.
   * @param [in] parent The parent object.
   */
  virtual void SetParent(Animation& parent) = 0;

  /**
   * Retrieve the parent of the AnimatorConnector.
   * @return The parent object, or NULL.
   */
  Animation* GetParent()
  {
    return mParent;
  }

  Object* GetObject() const
  {
    return mObject;
  }

  Property::Index GetPropertyIndex() const
  {
    return mPropertyIndex;
  }

  int GetComponentIndex() const
  {
    return mComponentIndex;
  }

private:

  /**
   * From Object::Observer
   */
  virtual void SceneObjectAdded( Object& object )
  {
  }

  /**
   * From Object::Observer
   */
  virtual void SceneObjectRemoved( Object& object )
  {
  }

  /**
   * From Object::Observer
   */
  virtual void ObjectDestroyed( Object& object )
  {
    mObject = NULL;
  }

protected:

  Animation* mParent; ///< The parent owns the connector.
  Object* mObject; ///< Not owned by the animator connector. Valid until ObjectDestroyed() is called.

  AlphaFunction mAlphaFunction;
  TimePeriod mTimePeriod;

  Property::Index mPropertyIndex;
  int mComponentIndex;

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ANIMATOR_CONNECTOR_BASE_H__

#ifndef DALI_BASE_OBJECT_IMPL_H
#define DALI_BASE_OBJECT_IMPL_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/base-object.h>

namespace Dali
{

/**
 * @brief Holds the Implementation for the BaseObject class
 */
class BaseObject::Impl
{

public:

  /**
   * @brief Retrieves the implementation of the internal BaseObject class.
   * @param[in] internalBaseObject A ref to the BaseObject whose internal implementation is required
   * @return The internal implementation
   */
  static BaseObject::Impl& Get( BaseObject& baseObject );

  /**
   * @copydoc Get( BaseObject& )
   */
  static const BaseObject::Impl& Get( const BaseObject& baseObject );

  /**
   * @brief Constructor.
   * @param[in] baseObject The base object which owns this implementation
   */
  Impl( BaseObject& baseObject );

  /**
   * @brief Destructor.
   */
  ~Impl();

  class Observer
  {
  public:

    /**
     * Called shortly before the object itself is destroyed; no further callbacks will be received.
     * @param[in] object The base object.
     */
    virtual void ObjectDestroyed( BaseObject& object ) = 0;

  protected:

    /**
     * Virtual destructor
     */
    virtual ~Observer() = default;
  };

  /**
   * Add an observer to the object.
   * @param[in] observer The observer to add.
   */
  void AddObserver( Observer& observer );

  /**
   * Remove an observer from the object
   * @pre The observer has already been added.
   * @param[in] observer The observer to remove.
   */
  void RemoveObserver( Observer& observer );

private:

  BaseObject& mBaseObject;
  Dali::Vector<Observer*> mObservers;
};

} // namespace Dali

#endif // DALI_BASE_OBJECT_IMPL_H

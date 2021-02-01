#ifndef DALI_INTERNAL_CONSTRAINER_H
#define DALI_INTERNAL_CONSTRAINER_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{
namespace Internal
{
typedef Dali::Vector<Object*> ObjectContainer;
using ObjectIter = ObjectContainer::Iterator;

/**
 * An abstract base class for constrainers.
 * Constrainer base class is responsible or observing constrained objects and remove all the constraints created
 * when it is destroyed
 */
class Constrainer : public Object, public Object::Observer
{
public:
  /**
   * Constructor.
   */
  Constrainer();

  /**
   * Virtual destructor.
   */
  ~Constrainer() override;

public: // Object::Observer methods
  /**
   * @copydoc Object::Observer::SceneObjectAdded()
   */
  void SceneObjectAdded(Object& object) override
  {
  }

  /**
   * @copydoc Object::Observer::SceneObjectRemoved()
   */
  void SceneObjectRemoved(Object& object) override
  {
  }

  /**
   * @copydoc Object::Observer::ObjectDestroyed()
   */
  void ObjectDestroyed(Object& object) override;

public:
  /**
   * @brief Applies the constraint to the target property

   * @param[in] target Property to be constrained
   * @param[in] source Property used as parameter for the path
   * @param[in] range The range of values in the source property which will be mapped to [0,1]
   * @param[in] wrap Wrapping domain. Source property will be wrapped in the domain [wrap.x,wrap.y] before mapping to [0,1]
   */
  virtual void Apply(Property target, Property source, const Vector2& range, const Vector2& wrap) = 0;

  /**
   * @brief Removes the constraint in the target object
   *
   * @param[in] target A handle to an object constrained by the Constrainer
   */
  void Remove(Dali::Handle& target);

protected:
  /**
   * @brief Adds an object to the list of observed objects
   *
   * @param[in] handle A handle to the object to be observed
   */
  void Observe(Dali::Handle& handle);

private:
  ObjectContainer mObservedObjects; ///< The list of object which have been constrained by the Constrainer
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_CONSTRAINER_H

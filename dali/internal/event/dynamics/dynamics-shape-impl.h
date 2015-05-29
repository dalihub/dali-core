#ifndef __DYNAMICS_SHAPE_IMPL_H__
#define __DYNAMICS_SHAPE_IMPL_H__

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

// EXTERNAL HEADERS
#include <string>

// BASE CLASS HEADERS
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/object/base-object.h>

// INTERNAL HEADERS
#include <dali/internal/event/dynamics/dynamics-declarations.h>
#include <dali/devel-api/dynamics/dynamics-shape.h>
#include <dali/public-api/math/vector3.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{
class DynamicsShape;
} // namespace SceneGraph

class DynamicsShape;
typedef IntrusivePtr< DynamicsShape > DynamicsShapePtr;

/// @copydoc Dali::DynamicsShape
class DynamicsShape : public BaseObject
{
public:
  /**
   * Constructor
   * @param[in] type The type of shape. One of Dali::DynamicsShape::ShapeType enumeration.
   */
  DynamicsShape(Dali::DynamicsShape::ShapeType type);

protected:
  /**
   * Destructor
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~DynamicsShape();

public:
  /**
   * Get an axis aligned bounding box for this shape
   * @return An axis aligned bounding box for this shape
   */
  virtual Vector3 GetAABB() const = 0;

  /// @copydoc Dali::DynamicsShape::GetType
  const Dali::DynamicsShape::ShapeType GetType() const;

  SceneGraph::DynamicsShape* GetSceneObject() const
  {
    return mDynamicsShape;
  }

private:
  // unimplemented copy constructor and assignment operator
  DynamicsShape(const DynamicsShape&);
  DynamicsShape& operator=(const DynamicsShape&);

  // Attributes
protected:
  Dali::DynamicsShape::ShapeType mType;
  SceneGraph::DynamicsShape* mDynamicsShape;
}; // class DynamicsShape

} // namespace Internal

inline Internal::DynamicsShape& GetImplementation(DynamicsShape& object)
{
  DALI_ASSERT_ALWAYS(object && "DynamicsShape object is uninitialized!");

  Dali::RefObject& handle = object.GetBaseObject();

  return static_cast<Internal::DynamicsShape&>(handle);
}

inline const Internal::DynamicsShape& GetImplementation(const DynamicsShape& object)
{
  DALI_ASSERT_ALWAYS(object && "DynamicsShape object is uninitialized!");

  const Dali::RefObject& handle = object.GetBaseObject();

  return static_cast<const Internal::DynamicsShape&>(handle);
}

} // namespace Dali

#endif // __COLLISION_SHAPE_IMPL_H__

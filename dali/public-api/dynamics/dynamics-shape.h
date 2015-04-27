#ifndef __DALI_DYNAMICS_SHAPE_H__
#define __DALI_DYNAMICS_SHAPE_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <string>

// BASE CLASS INCLUDES
#include <dali/public-api/object/base-handle.h>

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class DynamicsShape;
} // namespace Internal

class DynamicsWorld;

/**
 * @brief Defines the shape of an object in the simulation.
 */
class DALI_IMPORT_API DynamicsShape : public BaseHandle
{
public:
  /**
   * @brief The types of shape available.
   */
  enum ShapeType
  {
    CAPSULE = 0,        ///< A capsule (a cylinder capped with half spheres) defined by the radius of the caps and its height (the height is to the center of the caps)
    CONE,               ///< A cone defined by the radius of its base and its height
    CUBE,               ///< A cube defined by width,height and depth
    CYLINDER,           ///< A cylinder defined by the radius of its ends and its height
    MESH,               ///< A shape defined by a triangular mesh
    SPHERE,             ///< A sphere defined by its radius
  }; // enum ShapeType

public:
  /**
   * @brief Creates a capsule. (A cylinder capped with half spheres)
   *
   * @param[in] radius The radius of the capsule.
   * @param[in] length The length of the capsule.
   * @return a handle to the new shape.
   */
  static DynamicsShape NewCapsule(const float radius, const float length);

  /**
   * @brief Creates a cone.
   *
   * @param[in] radius The radius of the cone.
   * @param[in] length The length of the cone.
   * @return a handle to the new shape.
   */
  static DynamicsShape NewCone(const float radius, const float length);

  /**
   * @brief Creates a cube shape.
   *
   * All angles are right angles, and opposite faces are equal.
   * @param[in] dimensions The dimensions of the cuboid ( width, height and depth ).
   * @return A handle to the new shape
   */
  static DynamicsShape NewCube(const Vector3& dimensions);

  /**
   * @brief Creates a cylinder.
   *
   * @param[in] radius The radius of the cylinder.
   * @param[in] length The length of the cylinder.
   * @return A handle to the new shape
   */
  static DynamicsShape NewCylinder(const float radius, const float length);

  /**
   * @brief Creates a sphere.
   *
   * @param[in] radius The radius of the sphere.
   * @return A handle to the new shape
   */
  static DynamicsShape NewSphere(const float radius);

public:
  /**
   * @brief Constructor which creates an uninitialized DynamicsShape handle.
   *
   * Use one of the DynamicsShape::New methods to initialise the handle.
   */
  DynamicsShape();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~DynamicsShape();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  DynamicsShape(const DynamicsShape& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  DynamicsShape& operator=(const DynamicsShape& rhs);

public:
  /**
   * @brief Get the type of shape.
   *
   * @return One of the ShapeType enumeration.
   */
  ShapeType GetType() const;

  // Not intended for application developers
public:
  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param [in] internal A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL DynamicsShape(Internal::DynamicsShape* internal);
}; // class DynamicsShape

} // namespace Dali

#endif /* __DALI_DYNAMICS_SHAPE_H__ */

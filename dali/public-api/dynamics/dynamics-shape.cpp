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

// CLASS HEADER
#include <dali/public-api/dynamics/dynamics-shape.h>

// INTERNAL HEADERS
#include <dali/internal/event/modeling/mesh-impl.h>

#ifdef DYNAMICS_SUPPORT
#include <dali/internal/event/dynamics/dynamics-shape-impl.h>
#include <dali/internal/event/dynamics/dynamics-capsule-shape-impl.h>
#include <dali/internal/event/dynamics/dynamics-cone-shape-impl.h>
#include <dali/internal/event/dynamics/dynamics-cube-shape-impl.h>
#include <dali/internal/event/dynamics/dynamics-cylinder-shape-impl.h>
#include <dali/internal/event/dynamics/dynamics-mesh-shape-impl.h>
#include <dali/internal/event/dynamics/dynamics-sphere-shape-impl.h>
#include <dali/internal/event/dynamics/dynamics-world-impl.h>
#endif

namespace Dali
{

DynamicsShape DynamicsShape::NewCapsule(const float radius, const float length)
{
#ifdef DYNAMICS_SUPPORT
  Internal::DynamicsCapsuleShapePtr internal( new Internal::DynamicsCapsuleShape( radius, length) );

  return DynamicsShape(internal.Get());
#else
  return DynamicsShape();
#endif
}

DynamicsShape DynamicsShape::NewCone(const float radius, const float length)
{
#ifdef DYNAMICS_SUPPORT
  Internal::DynamicsConeShapePtr internal( new Internal::DynamicsConeShape( radius, length) );

  return DynamicsShape(internal.Get());
#else
  return DynamicsShape();
#endif
}

DynamicsShape DynamicsShape::NewCube(const Vector3& dimensions)
{
#ifdef DYNAMICS_SUPPORT
  Internal::DynamicsCubeShapePtr internal( new Internal::DynamicsCubeShape( dimensions ) );

  return DynamicsShape(internal.Get());
#else
  return DynamicsShape();
#endif
}

DynamicsShape DynamicsShape::NewCylinder(const float radius, const float length)
{
#ifdef DYNAMICS_SUPPORT
  Internal::DynamicsCylinderShapePtr internal( new Internal::DynamicsCylinderShape( radius, length) );

  return DynamicsShape(internal.Get());
#else
  return DynamicsShape();
#endif
}

DynamicsShape DynamicsShape::NewMesh(Cloth cloth)
{
#ifdef DYNAMICS_SUPPORT
  Internal::DynamicsMeshShapePtr internal( new Internal::DynamicsMeshShape( GetImplementation(cloth) ) );

  return DynamicsShape(internal.Get());
#else
  return DynamicsShape();
#endif
}

DynamicsShape DynamicsShape::NewSphere(const float radius)
{
#ifdef DYNAMICS_SUPPORT
  Internal::DynamicsSphereShapePtr internal( new Internal::DynamicsSphereShape(  radius ) );

  return DynamicsShape(internal.Get());
#else
  return DynamicsShape();
#endif
}

DynamicsShape::DynamicsShape()
{
}

DynamicsShape::~DynamicsShape()
{
}

DynamicsShape::DynamicsShape(Internal::DynamicsShape* internal)
#ifdef DYNAMICS_SUPPORT
: BaseHandle(internal)
#else
: BaseHandle(NULL)
#endif
{
}

DynamicsShape::ShapeType DynamicsShape::GetType() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetType();
#else
  return DynamicsShape::CAPSULE;
#endif
}

} // namespace Dali

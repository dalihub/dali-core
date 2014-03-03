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

// CLASS HEADER
#include <dali/public-api/dynamics/dynamics-shape.h>

// INTERNAL HEADERS
#include <dali/internal/event/dynamics/dynamics-shape-impl.h>
#include <dali/internal/event/dynamics/dynamics-capsule-shape-impl.h>
#include <dali/internal/event/dynamics/dynamics-cone-shape-impl.h>
#include <dali/internal/event/dynamics/dynamics-cube-shape-impl.h>
#include <dali/internal/event/dynamics/dynamics-cylinder-shape-impl.h>
#include <dali/internal/event/dynamics/dynamics-mesh-shape-impl.h>
#include <dali/internal/event/dynamics/dynamics-sphere-shape-impl.h>
#include <dali/internal/event/dynamics/dynamics-world-impl.h>
#include <dali/internal/event/modeling/mesh-impl.h>

namespace Dali
{

DynamicsShape DynamicsShape::NewCapsule(const float radius, const float length)
{
  Internal::DynamicsCapsuleShapePtr internal( new Internal::DynamicsCapsuleShape( radius, length) );

  return DynamicsShape(internal.Get());
}

DynamicsShape DynamicsShape::NewCone(const float radius, const float length)
{
  Internal::DynamicsConeShapePtr internal( new Internal::DynamicsConeShape( radius, length) );

  return DynamicsShape(internal.Get());
}

DynamicsShape DynamicsShape::NewCube(const Vector3& dimensions)
{
  Internal::DynamicsCubeShapePtr internal( new Internal::DynamicsCubeShape( dimensions ) );

  return DynamicsShape(internal.Get());
}


DynamicsShape DynamicsShape::NewCylinder(const float radius, const float length)
{
  Internal::DynamicsCylinderShapePtr internal( new Internal::DynamicsCylinderShape( radius, length) );

  return DynamicsShape(internal.Get());
}

DynamicsShape DynamicsShape::NewMesh(Mesh mesh)
{
  Internal::DynamicsMeshShapePtr internal( new Internal::DynamicsMeshShape( GetImplementation(mesh) ) );

  return DynamicsShape(internal.Get());
}

DynamicsShape DynamicsShape::NewSphere(const float radius)
{
  Internal::DynamicsSphereShapePtr internal( new Internal::DynamicsSphereShape(  radius ) );

  return DynamicsShape(internal.Get());
}

DynamicsShape::DynamicsShape()
{
}

DynamicsShape::~DynamicsShape()
{
}

DynamicsShape::DynamicsShape(Internal::DynamicsShape* internal)
: BaseHandle(internal)
{
}

DynamicsShape::ShapeType DynamicsShape::GetType() const
{
  return GetImplementation(*this).GetType();
}

} // namespace Dali

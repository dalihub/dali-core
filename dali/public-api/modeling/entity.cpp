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
#include <dali/public-api/modeling/entity.h>
#include <dali/internal/event/modeling/entity-impl.h>

namespace Dali
{

Entity Entity::New(const std::string name)
{
  Internal::EntityPtr internal = Internal::Entity::New(name);

  return Entity(internal.Get());
}

Entity Entity::DownCast( BaseHandle handle )
{
  return Entity( dynamic_cast<Dali::Internal::Entity*>(handle.GetObjectPtr()) );
}

Entity::Entity()
{
}

Entity::Entity(Internal::Entity* internal)
: BaseHandle(internal)
{
}

Entity::~Entity()
{
}

Entity Entity::Find(const std::string& name) const
{
  Internal::Entity* entity = GetImplementation(*this).Find(name);
  return Entity(entity);
}

void Entity::SetName(const char* name)
{
  GetImplementation(*this).SetName(name);
}

void Entity::SetName(const std::string& name)
{
  GetImplementation(*this).SetName(name);
}

const std::string& Entity::GetName() const
{
  return GetImplementation(*this).GetName();
}

void Entity::SetType(const EntityType type)
{
  GetImplementation(*this).SetType(type);
}

Entity::EntityType Entity::GetType() const
{
  return GetImplementation(*this).GetType();
}

void Entity::SetTransformMatrix(Matrix& matrix)
{
  GetImplementation(*this).SetTransformMatrix(matrix);
}

const Matrix& Entity::GetTransformMatrix() const
{
  return GetImplementation(*this).GetTransformMatrix();
}

bool Entity::HasChildren() const
{
  return GetImplementation(*this).HasChildren();
}

int Entity::NumberOfChildren() const
{
  return GetImplementation(*this).NumberOfChildren();
}

const EntityContainer& Entity::GetChildren() const
{
  return GetImplementation(*this).GetChildren();
}

void Entity::Add(Entity child)
{
  DALI_ASSERT_ALWAYS( child && "Child entity handle is empty" );
  GetImplementation(*this).Add(GetImplementation(child));
}

Entity Entity::GetParent() const
{
  Internal::Entity* parent = GetImplementation(*this).GetParent();
  return Entity(parent);
}

void Entity::SetMeshCapacity(unsigned int capacity)
{
  GetImplementation(*this).SetMeshCapacity(capacity);
}

void Entity::AddMeshIndex(unsigned int meshIndex)
{
  GetImplementation(*this).AddMeshIndex(meshIndex);
}

bool Entity::HasMeshes() const
{
  return GetImplementation(*this).HasMeshes();
}

int Entity::NumberOfMeshes() const
{
  return GetImplementation(*this).NumberOfMeshes();
}

const EntityMeshIndices& Entity::GetMeshes() const
{
  return GetImplementation(*this).GetMeshes();
}

unsigned int Entity::GetMeshByIndex(unsigned int meshIndex) const
{
  return GetImplementation(*this).GetMeshByIndex (meshIndex);
}

void Entity::AddToBounds(Entity child)
{
  DALI_ASSERT_ALWAYS( child && "Child entity handle is empty" );
  GetImplementation(*this).AddToBounds(GetImplementation(child));
}

void Entity::AddToBounds( const Vector3& lowerBounds, const Vector3& upperBounds )
{
  GetImplementation(*this).AddToBounds( lowerBounds, upperBounds );
}

const Vector3& Entity::GetLowerBounds() const
{
  return GetImplementation(*this).GetLowerBounds();
}

const Vector3& Entity::GetUpperBounds() const
{
  return GetImplementation(*this).GetUpperBounds();
}

void Entity::SetLowerBounds( const Vector3& lowerBounds )
{
  GetImplementation(*this).SetLowerBounds( lowerBounds );
}

void Entity::SetUpperBounds( const Vector3& upperBounds )
{
  GetImplementation(*this).SetUpperBounds( upperBounds );
}


} // namespace Dali


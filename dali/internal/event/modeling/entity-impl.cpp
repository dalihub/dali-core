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
#include <dali/internal/event/modeling/entity-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/geometry/mesh.h>
#include <dali/internal/event/modeling/model-data-impl.h>
#include <dali/internal/event/modeling/mesh-impl.h>

namespace Dali
{

namespace Internal
{

namespace // unnamed namespace
{
static const float kBoundsDefault = 1e10f;
} // unnamed namespace



Entity::Entity()
: mName(""),
  mModel(NULL),
  mParent(NULL),
  mLowerBounds( kBoundsDefault,  kBoundsDefault,  kBoundsDefault),
  mUpperBounds(-kBoundsDefault, -kBoundsDefault, -kBoundsDefault),
  mType(Dali::Entity::OBJECT)
{
}

EntityPtr Entity::New(const std::string name)
{
  EntityPtr entity(new Entity());
  entity->SetName(name);
  return entity;
}

Entity::~Entity()
{
  mChildren.clear();
  mMeshes.clear();
}


Entity* Entity::Find(const std::string& name) const
{
  Entity* entity = NULL;

  if (mName == name)
  {
    entity = const_cast<Entity*>(this);
  }
  else if (mChildren.size() != 0)
  {
    for (EntityConstIter it = mChildren.begin(); it != mChildren.end() && !entity; ++it)
    {
      const Entity& child = GetImplementation(*it);
      entity = child.Find(name);
      if (entity)
      {
        break;
      }
    }
  }

  return entity;
}


void Entity::SetName(const char* name)
{
  mName = name;
}

void Entity::SetName(const std::string& name)
{
  mName = name;
}

const std::string& Entity::GetName() const
{
  return mName;
}

const Vector3&  Entity::GetLowerBounds() const
{
  return mLowerBounds;
}

void Entity::SetLowerBounds(const Vector3& bounds)
{
  mLowerBounds = bounds;
}

const Vector3&  Entity::GetUpperBounds() const
{
  return mUpperBounds;
}

void Entity::SetUpperBounds(const Vector3& bounds)
{
  mUpperBounds = bounds;
}


void Entity::SetType(const Dali::Entity::EntityType type)
{
  mType = type;
}

Dali::Entity::EntityType Entity::GetType() const
{
  return mType;
}

void Entity::SetTransformMatrix(Matrix& matrix)
{
  mTransformMatrix = matrix;
}

const Matrix& Entity::GetTransformMatrix() const
{
  return mTransformMatrix;
}

bool Entity::HasChildren() const
{
  return 0 != mChildren.size();
}

int Entity::NumberOfChildren() const
{
  return mChildren.size();
}

const EntityContainer& Entity::GetChildren() const
{
  return mChildren;
}

void Entity::Add(Entity& child)
{
  if(this != child.mParent)
  {
    child.SetParent(this);
    mChildren.push_back(Dali::Entity(&child));
  }
}

void Entity::SetChildCapacity(unsigned int capacity)
{
  mChildren.reserve(capacity);
}

void Entity::SetParent(Entity* parent)
{
  if(parent)
  {
    DALI_ASSERT_ALWAYS( this != parent && "Cannot parent an entity to itself" );
    DALI_ASSERT_ALWAYS(mParent == NULL && "Entity already has a parent" );
    mParent = parent;
  }
}

Entity* Entity::GetParent() const
{
  return mParent;
}

bool Entity::HasMeshes() const
{
  return 0 != mMeshes.size();
}

void Entity::SetMeshCapacity(unsigned int capacity)
{
  mMeshes.reserve(capacity);
}

int Entity::NumberOfMeshes() const
{
  return mMeshes.size();
}

void Entity::AddMeshIndex(unsigned int meshIdx)
{
  mMeshes.push_back(meshIdx);
}

unsigned int Entity::GetMeshByIndex(unsigned int meshIndex) const
{
  DALI_ASSERT_DEBUG( meshIndex < mMeshes.size());
  return mMeshes[meshIndex];
}

const EntityMeshIndices& Entity::GetMeshes() const
{
  return mMeshes;
}

void Entity::AddToBounds(Entity& child)
{
  mLowerBounds.x = std::min(mLowerBounds.x, child.GetLowerBounds().x);
  mLowerBounds.y = std::min(mLowerBounds.y, child.GetLowerBounds().y);
  mLowerBounds.z = std::min(mLowerBounds.z, child.GetLowerBounds().z);
  mUpperBounds.x = std::max(mUpperBounds.x, child.GetUpperBounds().x);
  mUpperBounds.y = std::max(mUpperBounds.y, child.GetUpperBounds().y);
  mUpperBounds.z = std::max(mUpperBounds.z, child.GetUpperBounds().z);
}

void Entity::AddToBounds( const Vector3& lowerBounds, const Vector3& upperBounds )
{
  mLowerBounds.x = std::min(mLowerBounds.x, lowerBounds.x);
  mLowerBounds.y = std::min(mLowerBounds.y, lowerBounds.y);
  mLowerBounds.z = std::min(mLowerBounds.z, lowerBounds.z);
  mUpperBounds.x = std::max(mUpperBounds.x, upperBounds.x);
  mUpperBounds.y = std::max(mUpperBounds.y, upperBounds.y);
  mUpperBounds.z = std::max(mUpperBounds.z, upperBounds.z);
}

} // namespace Internal
} // namespace Dali

#ifndef __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_OWNER_MESSAGES_H__
#define __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_OWNER_MESSAGES_H__

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
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/common/event-to-update.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

// Messages for PropertyOwner

inline void InstallCustomPropertyMessage( EventToUpdate& eventToUpdate, const PropertyOwner& owner, PropertyBase* property )
{
  typedef MessageValue1< PropertyOwner, OwnerPointer<PropertyBase> > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &owner, &PropertyOwner::InstallCustomProperty, property );
}

inline void ApplyConstraintMessage( EventToUpdate& eventToUpdate, const PropertyOwner& owner, ConstraintBase& constraint )
{
  typedef MessageValue1< PropertyOwner, OwnerPointer<ConstraintBase> > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &owner, &PropertyOwner::ApplyConstraint, &constraint );
}

inline void RemoveConstraintMessage( EventToUpdate& eventToUpdate, const PropertyOwner& owner, const ConstraintBase& constConstraint )
{
  // The update-thread can modify this object.
  ConstraintBase& constraint = const_cast< ConstraintBase& >( constConstraint );

  typedef MessageValue1< PropertyOwner, ConstraintBase* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &owner, &PropertyOwner::RemoveConstraint, &constraint );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_OWNER_MESSAGES_H__

#ifndef __SCENE_GRAPH_DYNAMICS_CAPSULE_SHAPE_H__
#define __SCENE_GRAPH_DYNAMICS_CAPSULE_SHAPE_H__

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

// BASE CLASS HEADERS
#include <dali/internal/update/dynamics/scene-graph-dynamics-shape.h>

// INTERNAL HEADERS
#include <dali/internal/event/dynamics/dynamics-declarations.h>
#include <dali/internal/common/message.h>
#include <dali/internal/common/event-thread-services.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

class DynamicsWorld;

class DynamicsCapsuleShape : public DynamicsShape
{
public:

  /**
   * Constructor.
   * @param[in] world The dynamics world object
   */
  DynamicsCapsuleShape(DynamicsWorld& world);

  /**
   * Destructor.
   */
  virtual ~DynamicsCapsuleShape();

  /**
   * Initialize the capsule.
   * @param[in] radius  The radius of the capsule.
   * @param[in] length  The length of the capsule.
   */
  void Initialize( float radius, float length );

private:
  // unimplemented copy constructor and assignment operator
  DynamicsCapsuleShape(const DynamicsCapsuleShape&);
  DynamicsCapsuleShape& operator=(const DynamicsCapsuleShape&);

}; // class DynamicsCapsuleShape

// Messages for DynamicsCapsuleShape

inline void InitializeDynamicsCapsuleShapeMessage( EventThreadServices& eventThreadServices, const DynamicsCapsuleShape& shape, const float radius, const float length )
{
  typedef MessageValue2< DynamicsCapsuleShape, float, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &shape, &DynamicsCapsuleShape::Initialize, radius, length );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __SCENE_GRAPH_DYNAMICS_CAPSULE_SHAPE_H__

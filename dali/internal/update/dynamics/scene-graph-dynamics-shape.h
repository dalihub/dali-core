#ifndef __SCENE_GRAPH_DYNAMICS_SHAPE_H__
#define __SCENE_GRAPH_DYNAMICS_SHAPE_H__

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

// INTERNAL HEADERS
#include <dali/internal/event/dynamics/dynamics-declarations.h>
#include <dali/internal/common/message.h>
#include <dali/internal/common/event-to-update.h>

namespace Dali
{

namespace Integration
{

class DynamicsShape;

} // namespace Integration

namespace Internal
{

namespace SceneGraph
{

class DynamicsWorld;

class DynamicsShape
{
public:
  /**
   * Constructor.
   */
  DynamicsShape(DynamicsWorld& world);

  /**
   * Destructor.
   */
  virtual ~DynamicsShape();

  /**
   * Initialize shape, and add it to the world's shape container
   */
  void Initialize();

  /**
   * Remove from world's shape container
   */
  void Delete();

  Integration::DynamicsShape* GetShape() const;

private:
  // unimplemented copy constructor and assignment operator
  DynamicsShape(const DynamicsShape&);
  DynamicsShape& operator=(const DynamicsShape&);

protected:
  Integration::DynamicsShape* mShape;
  DynamicsWorld& mWorld;

}; // class DynamicsCubeShape

inline void DeleteShapeMessage( EventToUpdate& eventToUpdate, const DynamicsShape& shape )
{
  typedef Message< DynamicsShape > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &shape, &DynamicsShape::Delete );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __SCENE_GRAPH_DYNAMICS_SHAPE_H__

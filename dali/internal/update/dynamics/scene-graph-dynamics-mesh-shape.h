#ifndef __SCENE_GRAPH_DYNAMICS_MESH_SHAPE_H__
#define __SCENE_GRAPH_DYNAMICS_MESH_SHAPE_H__

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

// BASE CLASS HEADERS
#include <dali/internal/update/dynamics/scene-graph-dynamics-shape.h>

// INTERNAL HEADERS
#include <dali/integration-api/resource-declarations.h>
#include <dali/internal/common/message.h>
#include <dali/internal/common/event-to-update.h>
#include <dali/internal/event/dynamics/dynamics-declarations.h>
#include <dali/internal/update/modeling/scene-graph-mesh-declarations.h>

namespace Dali
{

namespace Internal
{

class ResourceManager;

namespace SceneGraph
{

class DynamicsWorld;

class DynamicsMeshShape : public DynamicsShape
{
public:
  /**
   * Constructor.
   * @param[in] world           The dynamics world object
   * @param[in] resourceManager The resource manager
   * @param[in] meshId          ID for the mesh resource
   */
  DynamicsMeshShape( DynamicsWorld& world, ResourceManager& resourceManager, const Integration::ResourceId meshId );

  /**
   * Destructor.
   */
  virtual ~DynamicsMeshShape();

  /**
   * Initialize the mesh.
   */
  void Initialize();

  Mesh* GetMesh() const;

private:
  // unimplemented copy constructor and assignment operator
  DynamicsMeshShape(const DynamicsMeshShape&);
  DynamicsMeshShape& operator=(const DynamicsMeshShape&);

private:
  ResourceManager& mResourceManager;
  Integration::ResourceId mMeshId;
}; // class DynamicsMeshShape

inline void InitializeDynamicsMeshShapeMessage( EventToUpdate& eventToUpdate, const DynamicsMeshShape& shape )
{
  typedef Message< DynamicsMeshShape > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &shape, &DynamicsMeshShape::Initialize );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __SCENE_GRAPH_DYNAMICS_MESH_SHAPE_H__

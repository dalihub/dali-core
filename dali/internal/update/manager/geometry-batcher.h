#ifndef DALI_INTERNAL_SCENE_GRAPH_GEOMETRY_BATCHER_H
#define DALI_INTERNAL_SCENE_GRAPH_GEOMETRY_BATCHER_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/internal/render/renderers/render-property-buffer.h>
#include <dali/internal/update/rendering/scene-graph-texture-set.h>
#include <dali/internal/render/common/render-list.h>

// PUBLIC INCLUDES
#include <dali/public-api/common/dali-vector.h>

/// Value used by a node to indicate the batch index as null or invalid
#define BATCH_NULL_HANDLE 0xFFFFFFFF

namespace Dali
{

namespace Internal
{

namespace Render
{
class Geometry;
}

namespace SceneGraph
{
struct BatchInfo;
struct BatchStreamMarker;

class Node;
class RenderInstructionContainer;

class GeometryBatcher
{

public:

  /**
   * The constructor of GeometryBatcher
   */
  GeometryBatcher();

  /**
   * The destructor of GeometryBatcher
   */
  ~GeometryBatcher();

  /**
   * Assigns the update manager object
   * @param[in] updateManager Pointer to instance of update manager
   */
  void SetUpdateManager( UpdateManager* updateManager );

  /**
   * Updates all batches
   * @param[in] index Update buffer index
   */
  void Update( BufferIndex index );

  /**
   * @brief Add node to batch parents list
   * @param[in] node instance of a node to be added
   */
  void AddBatchParent( Node* node );

  /**
   * @brief Remove node from list of batch parents
   * @param[in] node valid instance of node to be removed
   */
  void RemoveBatchParent( Node* node );

  /**
   * @brief Add batchable node
   * @param[in] node valid instance of the node to be added
   */
  void AddNode( Node* node );

  /**
   * @brief Remove batchable node
   * @param[in] node valid instance of the node to be removed
   */
  void RemoveNode( Node* node );

  /**
   * @brief Return the geometry object associated with specified batch index
   * @param[in] batchIndex VALID index of the batch
   * @return instance of the batched geometry
   */
  Render::Geometry* GetGeometry( uint32_t batchIndex );

  /**
   * @brief Query if a batch at given index has been already rendered
   * @param batchIndex VALID index of a batch to query
   * @return true if batch has rendered, false otherwise
   */
  bool HasRendered( uint32_t batchIndex );

  /**
   * @brief Sets a batch at given index as rendered
   * @param batchIndex VALID index of a batch
   */
  void SetRendered( uint32_t batchIndex );

private:

  /**
   * @brief Clones vertex format of source geometry and stores generated data in the batchInfo object
   * @param[in] sourceGeometry Geometry of the very first batched item
   * @param[out] batchInfo Batch info object used to store cloned vertex format
   * @return true on success, false otherwise
   */
  bool CloneVertexFormat( const Render::Geometry* sourceGeometry, struct VertexDescriptor& batchInfo );

private:

  struct Impl* mImpl; ///< Pointer to an instance of geometry batcher implementation

};

} // SceneGraph

} // Internal

} // Dali


#endif //DALI_INTERNAL_SCENE_GRAPH_GEOMETRY_BATCHER_H


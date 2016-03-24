#ifndef __DALI_INTERNAL_SCENE_GRAPH_NODE_ATTACHMENT_H__
#define __DALI_INTERNAL_SCENE_GRAPH_NODE_ATTACHMENT_H__

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

// EXTERNAL INCLUDES
#include <cstddef>    // NULL

// INTERNAL INCLUDES
#include <dali/internal/common/message.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

class Node;
class SceneController;

/**
 * NodeAttachment is a base class for objects attached to Nodes in the scene graph.
 * The position of the object is provided by its parent node.
 * NodeAttachment data is double-buffered. This allows an update thread to modify node data, without interferring
 * with another thread reading the values from the previous update traversal.
 */
class NodeAttachment
{
public:

  /**
   * Virtual destructor
   */
  virtual ~NodeAttachment();

  /**
   * Second-phase construction.
   * This is called by the UpdateManager when the attachment is attached to the scene-graph.
   * @param[in] sceneController Allows attachments to get camera controllers.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  virtual void Initialize( SceneController& sceneController, BufferIndex updateBufferIndex ) = 0;

  /**
   * Called shortly before destruction.
   * After this method has been called, the SceneController cannot be safely accessed.
   */
  virtual void OnDestroy() = 0;

  /**
   * Called when the parent node is connected to the SceneGraph
   */
  virtual void ConnectedToSceneGraph() = 0;

  /**
   * Called when the parent node is disconnected from the SceneGraph
   */
  virtual void DisconnectedFromSceneGraph( ) = 0;

  /**
   * Set the parent of a NodeAttachment.
   * @param[in] parent the new parent.
   */
  void SetParent( Node& parent );

  /**
   * Retrieve the parent node of a NodeAttachment.
   * @return The parent node, or NULL if the NodeAttachment has not been added to the scene-graph.
   */
  Node& GetParent()
  {
    DALI_ASSERT_DEBUG( mParent != NULL );
    return *mParent;
  }

  /**
   * Retrieve the parent node of a NodeAttachment.
   * @return The parent node, or NULL if the NodeAttachment has not been added to the scene-graph.
   */
  Node& GetParent() const
  {
    DALI_ASSERT_DEBUG( mParent != NULL );
    return *mParent;
  }

protected:

  /**
   * Derived versions of NodeAttachment should be constructed.
   */
  NodeAttachment();

private:

  // Undefined
  NodeAttachment( const NodeAttachment& );

  // Undefined
  NodeAttachment& operator=( const NodeAttachment& rhs );

protected:

  Node* mParent; ///< Pointer to parent node which owns the attachment.
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_NODE_ATTACHMENT_H__

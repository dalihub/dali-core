#ifndef DALI_INTERNAL_SCENEGRAPH_NODE_RESETTER_H
#define DALI_INTERNAL_SCENEGRAPH_NODE_RESETTER_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
 */

// EXTERNAL INCLDUES
#include <cstdint> // int8_t

#include <dali/internal/update/animation/property-accessor.h>
#include <dali/internal/update/animation/property-component-accessor.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/nodes/node.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
/**
 * Class to reset the node's properties to their base values. Used by UpdateManager
 * to reset the node properties after the node is connected to the scene graph.
 */
class NodeResetter : public PropertyOwner::Observer
{
public:
  /**
   * New method.
   * @param[in] node  The node
   * @return the new node resetter
   */
  static NodeResetter* New(const Node& node)
  {
    return new NodeResetter(const_cast<Node*>(&node));
  }

  /**
   * Virtual Destructor
   */
  ~NodeResetter() override
  {
    if(mNode != nullptr)
    {
      mNode->RemoveObserver(*this);
    }
  }

  /**
   * Initialize.
   *
   * Watches the node to track if it's disconnected or not.
   */
  void Initialize()
  {
    mNode->AddObserver(*this);
  }

  /**
   * Reset the node properties to their base values if the node is still alive and on stage
   * @param[in] updateBufferIndex the current buffer index
   */
  void ResetToBaseValue(BufferIndex updateBufferIndex)
  {
    if(mNode != nullptr && mActive)
    {
      // Start aging the node properties.
      // We need to reset the node properties for two frames to ensure both
      // property values are set appropriately.
      --mActive;

      mNode->mVisible.ResetToBaseValue(updateBufferIndex);
      mNode->mColor.ResetToBaseValue(updateBufferIndex);
    }
  };

  /**
   * Called when the node is connected to the scene graph.
   *
   * Note, this resetter object may be created after the node has been connected
   * to the scene graph, so we track disconnection and re-connection instead of connection
   *
   * @param[in] owner The property owner
   */
  void PropertyOwnerConnected(PropertyOwner& owner) override
  {
    mDisconnected = false;
    mActive       = ACTIVE;

    mNode->mVisible.MarkAsDirty();
    mNode->mColor.MarkAsDirty();
  }

  /**
   * Called when mPropertyOwner is disconnected from the scene graph.
   * @param[in] bufferIndex the current buffer index
   * @param[in] owner The property owner
   */
  void PropertyOwnerDisconnected(BufferIndex bufferIndex, PropertyOwner& owner) override
  {
    mDisconnected = true;
  }

  /**
   * Called shortly before the propertyOwner is destroyed
   * @param[in] owner The property owner
   */
  void PropertyOwnerDestroyed(PropertyOwner& owner) override
  {
    mDisconnected = true;
    mNode         = nullptr;

    // Don't need to wait another frame as the properties are being destroyed
    mActive = STOPPED;
  }

  /**
   * Determine if the node resetter has finished.
   *
   * @return true if the node resetter has finished.
   */
  virtual bool IsFinished()
  {
    return mActive <= STOPPED;
  }

protected:
  enum
  {
    STOPPED = 0,
    AGING   = 1,
    ACTIVE  = 2,
  };

  /**
   * Constructor
   *
   * @param[in] node The node storing the base properties
   */
  NodeResetter(Node* node)
  : mNode(node),
    mActive(ACTIVE),
    mDisconnected(false)
  {
    mNode->mVisible.MarkAsDirty();
    mNode->mColor.MarkAsDirty();
  }

  Node*  mNode;         ///< The node that owns the properties
  int8_t mActive;       ///< 2 if active, 1 if aging, 0 if stopped
  bool   mDisconnected; ///< True if the node has been disconnected
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif //DALI_INTERNAL_SCENEGRAPH_NODE_RESETTER_H

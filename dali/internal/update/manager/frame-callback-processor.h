#ifndef DALI_INTERNAL_SCENE_GRAPH_FRAME_CALLBACK_PROCESSOR_H
#define DALI_INTERNAL_SCENE_GRAPH_FRAME_CALLBACK_PROCESSOR_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <memory>

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/manager/update-proxy-impl.h>

namespace Dali
{

class FrameCallbackInterface;

namespace Internal
{

namespace SceneGraph
{

class Node;
class TransformManager;

/**
 * This class processes all the registered frame-callbacks.
 */
class FrameCallbackProcessor : public PropertyOwner::Observer
{
public:

  /**
   * Construct a new FrameCallbackProcessor.
   */
  FrameCallbackProcessor( TransformManager& transformManager, Node& rootNode );

  /**
   * Non-virtual Destructor.
   */
  ~FrameCallbackProcessor();

  // Movable but not copyable

  FrameCallbackProcessor( const FrameCallbackProcessor& )            = delete;  ///< Deleted copy constructor.
  FrameCallbackProcessor( FrameCallbackProcessor&& )                 = default; ///< Default move constructor.
  FrameCallbackProcessor& operator=( const FrameCallbackProcessor& ) = delete;  ///< Deleted copy assignment operator.
  FrameCallbackProcessor& operator=( FrameCallbackProcessor&& )      = default; ///< Default move assignment operator.

  /**
   * Adds an implementation of the FrameCallbackInterface.
   * @param[in]  frameCallback  A pointer to the implementation of the FrameCallbackInterface
   * @param[in]  rootNode       A pointer to the root node to apply the FrameCallback to
   */
  void AddFrameCallback( FrameCallbackInterface* frameCallback, const Node* rootNode );

  /**
   * Removes the specified implementation of FrameCallbackInterface.
   * @param[in]  frameCallback  A pointer to the implementation of the FrameCallbackInterface to remove.
   */
  void RemoveFrameCallback( FrameCallbackInterface* frameCallback );

  /**
   * Called on Update by the UpdateManager.
   * @param[in]  bufferIndex     The bufferIndex to use
   * @param[in]  elapsedSeconds  Time elapsed time since the last frame (in seconds)
   */
  void Update( BufferIndex bufferIndex, float elapsedSeconds );

  /**
   * Called by the UpdateManager when the node hierarchy changes.
   */
  void NodeHierarchyChanged()
  {
    mNodeHierarchyChanged = true;
  }

private:

  // From PropertyOwner::Observer

  /**
   * @copydoc PropertyOwner::Observer::PropertyOwnerConnected()
   */
  virtual void PropertyOwnerConnected( PropertyOwner& owner ) { /* Nothing to do */ }

  /**
   * @copydoc PropertyOwner::Observer::PropertyOwnerDisconnected()
   */
  virtual void PropertyOwnerDisconnected( BufferIndex updateBufferIndex, PropertyOwner& owner ) { /* Nothing to do */ }

  /**
   * @copydoc PropertyOwner::Observer::PropertyOwnerDisconnected()
   *
   * Will use this to disconnect the frame-callback if the accompanying node is destroyed
   */
  virtual void PropertyOwnerDestroyed( PropertyOwner& owner );

private:

  struct FrameCallbackInfo
  {
    /**
     * Default Constructor
     * @param[in]  frameCallbackObject  A pointer to the frame-callback object
     * @param[in]  updateProxyPtr       A raw pointer to the newly created updateProxy
     * @note Ownership of @updateProxyPtr is passed to this class.
     */
    FrameCallbackInfo( FrameCallbackInterface* frameCallbackObject, UpdateProxy* updateProxyPtr )
    : frameCallback( frameCallbackObject ),
      updateProxyImpl( updateProxyPtr )
    {
    }

    ~FrameCallbackInfo() = default; ///< Default destructor.

    // Movable but not copyable
    FrameCallbackInfo( const FrameCallbackInfo& )            = delete;  ///< Deleted copy constructor.
    FrameCallbackInfo( FrameCallbackInfo&& )                 = default; ///< Default move constructor.
    FrameCallbackInfo& operator=( const FrameCallbackInfo& ) = delete;  ///< Deleted copy assignment operator.
    FrameCallbackInfo& operator=( FrameCallbackInfo&& )      = default; ///< Default move assignment operator.

    // Data
    FrameCallbackInterface* frameCallback{ nullptr }; ///< Pointer to the implementation of the FrameCallbackInterface.
    std::unique_ptr< UpdateProxy > updateProxyImpl{ nullptr }; ///< A unique pointer to the implementation of the UpdateProxy.
  };

  std::vector< FrameCallbackInfo > mFrameCallbacks; ///< A container of all the frame-callbacks & accompanying update-proxies.

  TransformManager& mTransformManager;
  Node& mRootNode;

  bool mNodeHierarchyChanged; ///< Set to true if the node hierarchy changes
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_FRAME_CALLBACK_PROCESSOR_H

#ifndef DALI_INTERNAL_SCENE_GRAPH_FRAME_CALLBACK_H
#define DALI_INTERNAL_SCENE_GRAPH_FRAME_CALLBACK_H

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
#include <dali/devel-api/threading/mutex.h>
#include <dali/devel-api/update/frame-callback-interface.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/manager/update-proxy-impl.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

class Node;
class TransformManager;

/**
 * This is the update-thread owned entity of the FrameCallbackInterface.
 * @see Dali::FrameCallbackInterface
 */
class FrameCallback final : public PropertyOwner::Observer
{
public:

  /**
   * Creates a new FrameCallback.
   * @param[in]  frameCallbackInterface  A reference to the FrameCallbackInterface implementation
   * @return A new FrameCallback.
   */
  static FrameCallback* New( FrameCallbackInterface& frameCallbackInterface );

  /**
   * Non-virtual Destructor.
   */
  ~FrameCallback();

  /**
   * Called from the update-thread when connecting to the scene-graph.
   * @param[in]  updateManager     The Update Manager
   * @param[in]  transformManager  The Transform Manager
   * @param[in]  rootNode          The rootNode of this frame-callback
   */
  void ConnectToSceneGraph( UpdateManager& updateManager, TransformManager& transformManager, Node& rootNode );

  // Movable but not copyable

  FrameCallback( const FrameCallback& )            = delete;  ///< Deleted copy constructor.
  FrameCallback( FrameCallback&& )                 = default; ///< Default move constructor.
  FrameCallback& operator=( const FrameCallback& ) = delete;  ///< Deleted copy assignment operator.
  FrameCallback& operator=( FrameCallback&& )      = default; ///< Default move assignment operator.

  /**
   * Called from the update-thread after the scene has been updated, and is ready to render.
   * @param[in]  bufferIndex           The bufferIndex to use
   * @param[in]  elapsedSeconds        Time elapsed time since the last frame (in seconds)
   * @param[in]  nodeHierarchyChanged  Whether the node hierarchy has changed
   * @return Whether to continue calling this FrameCallback or not.
   */
  bool Update( BufferIndex bufferIndex, float elapsedSeconds, bool nodeHierarchyChanged );

  /**
   * Invalidates this FrameCallback and will no longer be associated with the FrameCallbackInterface.
   * @note This method is thread-safe.
   */
  void Invalidate();

  /**
   * Comparison operator between a FrameCallback and a FrameCallbackInterface pointer.
   * @param[in]  iFace  The FrameCallbackInterface pointer to compare with
   * @return True if iFace matches our internally stored FrameCallbackInterface.
   */
  inline bool operator==( const FrameCallbackInterface* iFace )
  {
    return mFrameCallbackInterface == iFace;
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

  // Construction

  /**
   * Constructor.
   * @param[in]  frameCallbackInterface  A pointer to the FrameCallbackInterface implementation
   */
  FrameCallback( FrameCallbackInterface* frameCallbackInterface );

private:

  Mutex mMutex;
  std::unique_ptr< UpdateProxy > mUpdateProxy{ nullptr }; ///< A unique pointer to the implementation of the UpdateProxy.
  FrameCallbackInterface* mFrameCallbackInterface;
};

/**
 * Checks if FrameCallback store iFace internally.
 * @param[in]  frameCallback  Reference to the owner-pointer of frame-callback
 * @param[in]  iFace          The FrameCallbackInterface pointer
 * @return True if iFace matches the internally stored FrameCallbackInterface.
 */
inline bool operator==( const OwnerPointer< FrameCallback >& frameCallback, const FrameCallbackInterface* iFace )
{
  return *frameCallback == iFace;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_FRAME_CALLBACK_H

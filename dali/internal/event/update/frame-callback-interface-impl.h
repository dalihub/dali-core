#ifndef DALI_FRAME_CALLBACK_INTERFACE_IMPL_H
#define DALI_FRAME_CALLBACK_INTERFACE_IMPL_H

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

// INTERNAL INCLUDES
#include <dali/devel-api/update/frame-callback-interface.h>

namespace Dali
{

namespace Internal
{
namespace SceneGraph
{
class FrameCallback;
}
}

/**
 * The Implementation of the FrameCallbackInterface
 * @see FrameCallbackInterface
 */
class FrameCallbackInterface::Impl final
{
public:

  Impl() = default; ///< Default constructor.
  ~Impl() = default; ///< Default non-virtual destructor.

  /**
   * Retrieve the Impl of a FrameCallbackInterface.
   * @param[in]  frameCallback  The frame-callb
   */
  static inline Impl& Get( FrameCallbackInterface& frameCallback )
  {
    return *frameCallback.mImpl;
  }

  /**
   * Links this FrameCallback to the given scene-graph-frame-callback.
   * @param[in]  sceneGraphObject  The scene-graph-frame-callback to link this with.
   */
  void ConnectToSceneGraphObject( Internal::SceneGraph::FrameCallback& sceneGraphObject )
  {
    mSceneGraphFrameCallback = &sceneGraphObject;
  }

  /**
   * Disconnects this FrameCallback from the scene-graph-frame-callback.
   */
  void DisconnectFromSceneGraphObject()
  {
    mSceneGraphFrameCallback = nullptr;
  }

  /**
   * Checks whether we are connected to a scene-graph-frame-callback.
   * @return True if connected, false otherwise.
   */
  bool IsConnectedToSceneGraph() const
  {
    return mSceneGraphFrameCallback;
  }

  /**
   * Invalidates this FrameCallback and linked SceneGraph::FrameCallback.
   */
  void Invalidate()
  {
    if( mSceneGraphFrameCallback )
    {
      mSceneGraphFrameCallback->Invalidate();
    }
  }

private:
  Internal::SceneGraph::FrameCallback* mSceneGraphFrameCallback{ nullptr }; ///< Pointer to the scene-graph object, not owned.
};

} // namespace Dali

#endif // DALI_FRAME_CALLBACK_INTERFACE_IMPL_H

#ifndef DALI_FRAME_CALLBACK_INTERFACE_H
#define DALI_FRAME_CALLBACK_INTERFACE_H

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
 *
 */

// EXTERNAL INCLUDES
#include <memory>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
class UpdateProxy;

/**
 * @brief This interface should be implemented if a callback is required on every frame.
 *
 * The Update() method is called from the update-thread after the
 * scene has been updated, and is ready to render.
 *
 * As this method is called from the update-thread, no event thread
 * APIs (e.g. Actor::Get...) can be called.  This will invariably lead
 * to crashes.
 *
 * Instead, setting and retrieving the values should be done by using
 * the UpdateProxy class returned as a parameter to the Update()
 * method.
 *
 * Actors can be identified using Actor IDs which can be retrieved
 * using Actor::GetId() in the event-thread.  However, calling
 * Actor::GetId() will lead to problems if it is called from the
 * update-thread.
 *
 * Instead, the Actor IDs should be stored by the implementation of
 * this class or passed via a thread-safe manner from the
 * event-thread.
 *
 * The implementation can be notified that there is a sync point on
 * the next update. If this frame callback has been notified with a
 * sync point, then the UpdateProxy can be tested at the start of the
 * update in order to check the sync point.
 *   The sync notification is to enable the synchronization of the frame
 * upate callback with the setting of other dali object properties,
 * for instance, actor visibility, on the update thread.
 */
class DALI_CORE_API FrameCallbackInterface
{
public:
  class Extension; ///< Forward declare future extension interface

  /**
   * @brief Called from the update-thread after the scene has been updated, and is ready to render.
   * @param[in]  updateProxy  Use this to get/set required values for the Actor.
   * @param[in]  elapsedSeconds  Time elapsed time since the last frame (in seconds)
   * @return Whether we should keep rendering.
   * @see FrameCallbackInterface
   */
  virtual bool Update(UpdateProxy& updateProxy, float elapsedSeconds) = 0;

  /**
   * @brief Retrieves the extension for the interface.
   *
   * @return The extension if available, nullptr otherwise
   */
  virtual Extension* GetExtension()
  {
    return nullptr;
  }

protected:
  /**
   * @brief Protected constructor.
   */
  FrameCallbackInterface();

  /**
   * @brief Protected virtual destructor.
   */
  virtual ~FrameCallbackInterface();

public:
  /// @cond internal
  class Impl;

private:
  std::unique_ptr<Impl> mImpl;
  /// @endcond
};

} // namespace Dali

#endif // DALI_FRAME_CALLBACK_INTERFACE_H

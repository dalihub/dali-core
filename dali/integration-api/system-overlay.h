#ifndef __DALI_INTEGRATION_SYSTEM_OVERLAY_H__
#define __DALI_INTEGRATION_SYSTEM_OVERLAY_H__

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
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

class Actor;
class RenderTaskList;

namespace Internal
{
class SystemOverlay;
}

namespace Integration
{

/**
 * Use this interface to draw content for system-level indicators, dialogs etc.
 * The SystemOverlay is accessible using Dali::Integration::Core::GetSystemOverlay().
 */
class DALI_CORE_API SystemOverlay
{
public:

  /**
   * Non-virtual destructor. SystemOverlay is not intended as a base class.
   */
  ~SystemOverlay();

  /**
   * Add an Actor to the SystemOverlay.
   * @pre The actor handle is not empty.
   * @param [in] actor A handle to the actor to add.
   * @post The actor will be referenced.
   */
  void Add( Actor actor );

  /**
   * Remove an Actor that was added to the SystemOverlay.
   * @pre The actor handle is not empty.
   * @param [in] actor A handle to the actor to remove.
   * @post The actor will be unreferenced.
   */
  void Remove( Actor actor );

  /**
   * Retrieve the list of render-tasks for system-level overlays.
   * This is a seperate list, processed after the render-tasks provided by Stage::GetRenderTaskList().
   * @return The list of overlay render-tasks.
   */
  RenderTaskList GetOverlayRenderTasks();

  /**
   * Create the SystemOverlay entrance.
   * This is not intended for adaptor implementors; see also Dali::Integration::Core::GetSystemOverlay().
   * @param[in] impl The SystemOverlay implementation.
   */
  SystemOverlay( Internal::SystemOverlay* impl );

  /**
   * Retreive the internal implementation; this is not intended for adaptor implementors.
   * @return The SystemOverlay implementation.
   */
  Internal::SystemOverlay* GetImpl();

private:

  // Undefined copy-constructor.
  SystemOverlay( const SystemOverlay& core );

  // Undefined assignment operator.
  SystemOverlay& operator=( const SystemOverlay& rhs );

private:

  Dali::Internal::SystemOverlay* mImpl;
};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_SYSTEM_OVERLAY_H__

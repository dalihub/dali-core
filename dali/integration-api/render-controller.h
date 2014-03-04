#ifndef __DALI_INTEGRATION_RENDER_CONTROLLER_H__
#define __DALI_INTEGRATION_RENDER_CONTROLLER_H__

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

namespace Dali
{

namespace Integration
{

/**
 * Abstract interface for an object which controls rendering.
 * This will be informed when Dali has new content to render.
 */
class DALI_IMPORT_API RenderController
{
protected:

  /**
   * Virtual protected destructor, no deletion through this interface
   */
  virtual ~RenderController() {}

public:

  /**
   * Requests a future call to Dali::Integration::Core::Update().
   * This is called when Dali has new content, typically in response to Actors/Animations being added.
   * Multi-threading note: this method will be called from the main thread only.
   */
  virtual void RequestUpdate() = 0;

  /**
   * Requests a future call to Dali::Integration::Core::ProcessEvents(), when the application is idle.
   * Multi-threading note: this method will be called from the main thread only.
   */
  virtual void RequestProcessEventsOnIdle() = 0;

};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_RENDER_CONTROLLER_H__

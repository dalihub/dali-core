#ifndef DALI_INTEGRATION_GL_CONTEXT_HELPER_ABSTRACTION_H
#define DALI_INTEGRATION_GL_CONTEXT_HELPER_ABSTRACTION_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/render-surface.h>

namespace Dali
{
namespace Integration
{

/**
 * This abstraction defines the interface for accessing GL context.
 */
class GlContextHelperAbstraction
{
protected:
  /**
   * Virtual protected destructor, no deletion through this interface
   */
  virtual ~GlContextHelperAbstraction() = default;

public:

  /**
   * @brief Switch to the surfaceless GL context
   */
  virtual void MakeSurfacelessContextCurrent() = 0;

  /**
   * @brief Switch to the GL context of the specific render surface
   * @param[in] surface The render surface
   */
  virtual void MakeContextCurrent( Integration::RenderSurface* surface ) = 0;

  /**
   * @brief Clear the GL context
   */
  virtual void MakeContextNull() = 0;

  /**
   * @brief Wait until all GL rendering calls for the current GL context are executed
   */
  virtual void WaitClient() = 0;
};

} // namespace Integration
} // namespace Dali

#endif // DALI_INTEGRATION_GL_CONTEXT_HELPER_ABSTRACTION_H

#ifndef DALI_GRAPHICS_CONTROLLER_H
#define DALI_GRAPHICS_CONTROLLER_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

namespace Dali
{
namespace Integration
{
class GlAbstraction;
class GlSyncAbstraction;
class GlContextHelperAbstraction;
} // namespace Integration

namespace Graphics
{
/**
 * @brief The main controller for creating graphics objects and submitting render commands.
 */
class Controller
{
public:
  virtual ~Controller() = default;

  // Temporary until graphics api is complete
  virtual Integration::GlAbstraction&              GetGlAbstraction()              = 0;
  virtual Integration::GlSyncAbstraction&          GetGlSyncAbstraction()          = 0;
  virtual Integration::GlContextHelperAbstraction& GetGlContextHelperAbstraction() = 0;

public:
  // not copyable
  Controller(const Controller&) = delete;
  Controller& operator=(const Controller&) = delete;

protected:
  // derived types should not be moved direcly to prevent slicing
  Controller(Controller&&) = default;
  Controller& operator=(Controller&&) = default;

  /**
   * Objects of this type should not be directly instantiated.
   */
  Controller() = default;
};

} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_CONTROLLER_H

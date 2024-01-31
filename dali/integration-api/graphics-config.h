#ifndef DALI_INTEGRATION_GRAPHICS_CONFIG_H
#define DALI_INTEGRATION_GRAPHICS_CONFIG_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

#include <dali/devel-api/rendering/renderer-devel.h>

namespace Dali::Integration
{
/**
 * GraphicsConfig is an abstract interface, used to access OpenGRAPHICS services.
 * A concrete implementation must be created for each platform, and provided when creating the
 * Dali::Integration::Core object.
 */
class GraphicsConfig
{
protected:
  // Protected construction & deletion
  GraphicsConfig()          = default; ///< Protected constructor so an instance of this class cannot be created.
  virtual ~GraphicsConfig() = default; ///< Virtual protected destructor, no deletion through this interface.

public:
  // Not copyable but movable
  GraphicsConfig(const GraphicsConfig&) = delete;            ///< Deleted copy constructor.
  GraphicsConfig(GraphicsConfig&&)      = default;           ///< Default move constructor.
  GraphicsConfig& operator=(const GraphicsConfig&) = delete; ///< Deleted copy assignment operator.
  GraphicsConfig& operator=(GraphicsConfig&&) = default;     ///< Default move assignment operator.

  /**
   * Returns current graphicses can support the blend equation
   * @Return true current graphicses support the blend equation
   */
  virtual bool IsBlendEquationSupported(DevelBlendEquation::Type blendEquation) = 0;

  /**
   * Returns shader prefix of shading language version.
   * @Return shader prefix of shading language version.
   */
  virtual std::string GetShaderVersionPrefix() = 0;

  /**
   * Returns vertex shader prefix including shading language version.
   * @Return vertex shader prefix including shading language version.
   */
  virtual std::string GetVertexShaderPrefix() = 0;

  /**
   * Returns fragment shader prefix including shading language version and extension information.
   * @Return fragment shader prefix including shading language version and extension information.
   */
  virtual std::string GetFragmentShaderPrefix() = 0;
};

} // namespace Dali::Integration

#endif

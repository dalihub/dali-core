#ifndef DALI_GRAPHICS_API_FRAMEBUFFER_H
#define DALI_GRAPHICS_API_FRAMEBUFFER_H

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

namespace Dali
{
namespace Graphics
{
namespace API
{

/**
 * @brief Interface class for Framebuffer types in the graphics API.
 */
class Framebuffer
{
public:

  struct ClearColor
  {
    float r, g, b, a;
  };

  struct DepthStencilClearColor
  {
    uint32_t  stencil;
    float     depth;
  };

public:
  // not copyable
  Framebuffer(const Framebuffer&) = delete;
  Framebuffer& operator=(const Framebuffer&) = delete;

  virtual ~Framebuffer() = default;

protected:
  // derived types should not be moved direcly to prevent slicing
  Framebuffer(Framebuffer&&) = default;
  Framebuffer& operator=(Framebuffer&&) = default;

  /**
   * Objects of this type should not be directly instantiated.
   */
  Framebuffer() = default;
};

} // namespace API
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_API_FRAMEBUFFER_H

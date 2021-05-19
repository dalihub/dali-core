#ifndef DALI_GRAPHICS_FRAMEBUFFER_H
#define DALI_GRAPHICS_FRAMEBUFFER_H

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

namespace Dali
{
namespace Graphics
{
/**
 * @brief Class represents a frambuffer object.
 */
class Framebuffer
{
public:
  Framebuffer()          = default;
  virtual ~Framebuffer() = default;

  // not copyable
  Framebuffer(const Framebuffer&) = delete;
  Framebuffer& operator=(const Framebuffer&) = delete;

protected:
  Framebuffer(Framebuffer&&) = default;
  Framebuffer& operator=(Framebuffer&&) = default;
};

} // Namespace Graphics
} // Namespace Dali

#endif

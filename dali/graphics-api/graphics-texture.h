#ifndef DALI_GRAPHICS_TEXTURE_H
#define DALI_GRAPHICS_TEXTURE_H

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
 * @brief The Texture class represents a GPU texture object.
 *
 * It's slightly higher level than the Vulkan VkImage (more like
 * combined image sampler).
 */
class Texture
{
public:
  Texture()          = default;
  virtual ~Texture() = default;

  // not copyable
  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;

protected:
  Texture(Texture&&) = default;
  Texture& operator=(Texture&&) = default;
};

} // namespace Graphics
} // namespace Dali

#endif

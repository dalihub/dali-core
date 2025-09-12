#ifndef DALI_GRAPHICS_SAMPLER_H
#define DALI_GRAPHICS_SAMPLER_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
 * @brief Sampler represents GPU sampler object
 */
class Sampler
{
public:
  Sampler()          = default;
  virtual ~Sampler() = default;

  // not copyable
  Sampler(const Sampler&)            = delete;
  Sampler& operator=(const Sampler&) = delete;

protected:
  Sampler(Sampler&&)            = default;
  Sampler& operator=(Sampler&&) = default;
};

} // Namespace Graphics
} // Namespace Dali

#endif
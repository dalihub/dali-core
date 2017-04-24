#ifndef DALI_GRAPHICS_MANAGER_H
#define DALI_GRAPHICS_MANAGER_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include <dali/graphics-api/graphics-api-manager.h>

namespace Dali
{
namespace Graphics
{

/**
 * @brief Manager implementation of the graphics API
 */
class Manager final : public API::Manager
{
public:
  Manager() = default;

  Manager(const Manager&) = delete;
  Manager& operator=(const Manager&) = delete;

  Manager(Manager&&) = default;
  Manager& operator=(Manager&&) = default;

  virtual ~Manager() = default;

  /**
   * Dummy method
   */
  int GetNumber() const;


};

} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_MANAGER_H

#ifndef DALI_INTERNAL_PROGRAM_CACHE_H
#define DALI_INTERNAL_PROGRAM_CACHE_H

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

// INTERNAL INCLUDES
#include <dali/internal/common/shader-data.h>

namespace Dali
{
namespace Internal
{
class Program;

/**
 * This class is interface for caching Program objects
 */
class ProgramCache
{
public:
  /**
   * Constructor
   */
  ProgramCache() = default;

  /**
   * Destructor, virtual as this is an interface
   */
  virtual ~ProgramCache() = default;

public: // API
  /**
   * Get the program from cache with hash
   * @param shaderHash to use
   * @return program
   */
  virtual Program* GetProgram(size_t shaderHash) = 0;

  /**
   * Add a program to cache
   * @param shaderHash of the program
   * @param program to add
   */
  virtual void AddProgram(size_t shaderHash, Program* program) = 0;

  ProgramCache(const ProgramCache& rhs)            = delete;
  ProgramCache& operator=(const ProgramCache& rhs) = delete;
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_PROGRAM_CACHE_H

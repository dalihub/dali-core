#ifndef __DALI_INTERNAL_CUSTOM_UNIFORM_H__
#define __DALI_INTERNAL_CUSTOM_UNIFORM_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <string>

namespace Dali
{

namespace Internal
{

class Program;

/**
 * Helper class to handle the index of the custom uniforms
 */
class CustomUniform
{
public: // API

  /**
   * Creates a custom uniform
   */
  CustomUniform();

  /**
   * Destructor, nonvirtual as not a base class
   */
  ~CustomUniform();

  /**
   * Initializes the custom uniform. Should be called only once
   * @param program to use
   * @param name of the uniform
   */
  void Initialize( Program& program, const char* name );

  /**
   * Resets the custom uniform in case program was changed
   */
  void Reset();

  /**
   * Gets the uniform location for custom uniform
   * @param program to use
   * @param name of the uniform
   */
  int GetUniformLocation( Program& program, const char* name );

private: // Data

  unsigned int mCacheIndex; ///< internal cache index

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_CUSTOM_UNIFORM_H__


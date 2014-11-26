#ifndef __DALI_INTERNAL_PROGRAM_CACHE_H__
#define __DALI_INTERNAL_PROGRAM_CACHE_H__

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

// INTERNAL INCLUDES
#include <dali/integration-api/gl-abstraction.h> // for GLenum

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
  ProgramCache()
  { }

  /**
   * Destructor, virtual as this is an interface
   */
  virtual ~ProgramCache()
  { }

public: // API

  /**
   * @return GlAbstraction
   */
  virtual Integration::GlAbstraction& GetGlAbstraction() = 0;

  /**
   * Get the program from cache with hash
   * @param shaderHash to use
   * @return program
   */
  virtual Program* GetProgram( size_t shaderHash ) = 0;

  /**
   * Add a program to cache
   * @param shaderHash of the program
   * @param program to add
   */
  virtual void AddProgram( size_t shaderHash, Program* program ) = 0;

  /**
   * Get currently bound program
   * @return program that is currently used
   */
  virtual Program* GetCurrentProgram() = 0;

  /**
   * Set the currently bound program
   * @param program that is used
   */
  virtual void SetCurrentProgram( Program* program ) = 0;

  /**
   * @return true if program binaries are supported
   */
  virtual bool IsBinarySupported() = 0;

  /**
   * @return the binary format to use
   */
  virtual GLenum ProgramBinaryFormat() = 0;

  /**
   * @param programData to store/save
   */
  virtual void StoreBinary( Integration::ShaderDataPtr programData ) = 0;

private: // not implemented as non-copyable

  ProgramCache( const ProgramCache& rhs );
  ProgramCache& operator=( const ProgramCache& rhs );

};


} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_PROGRAM_CACHE_H__


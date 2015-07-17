#ifndef __DALI_INTERNAL_PROGRAM_CONTROLLER_H__
#define __DALI_INTERNAL_PROGRAM_CONTROLLER_H__

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
#include <dali/internal/common/owner-container.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/render/shaders/program-cache.h>

namespace Dali
{

namespace Internal
{

class ShaderSaver;

/**
 * This class is the owner of GL shader programs
 */
class ProgramController : public ProgramCache
{
public:

  /**
   * Wrapper for a program and its hash code
   */
  class ProgramPair
  {
  public: // API

    /**
     * Constructor
     * @param program
     * @param shaderHash
     */
    ProgramPair( Program* program, size_t shaderHash )
    : mProgram( program ), mShaderHash( shaderHash )
    { }

    /**
     * Destructor, non-virtual as not a base
     */
    ~ProgramPair()
    {
      delete mProgram;
    }

    /**
     * Inline getter for the program
     * @return the program
     */
    inline Program* GetProgram()
    {
      return mProgram;
    }

    /**
     * Inline getter for the hash
     * @return the hash
     */
    inline size_t GetHash()
    {
      return mShaderHash;
    }

  private: // Not implemented
    ProgramPair( const ProgramPair& );
    ProgramPair& operator=( const ProgramPair& );

  private: // Data
    Program* mProgram;
    size_t mShaderHash;
  };

  /**
   * Constructor
   * @param postProcessDispatcher to send save binary message back to update
   */
  ProgramController( Integration::GlAbstraction& glAbstraction );

  /**
   * Destructor, non virtual as not a base class
   */
  ~ProgramController();

public: // API

  /**
   * Resets the program matrices. Must be called at the beginning of every frame
   */
  void ResetProgramMatrices();

  /**
   * Notifies the cache that context is (re)created
   */
  void GlContextCreated();

  /**
   * Notifies cache that context is lost
   */
  void GlContextDestroyed();

  /**
   * Set the destination for compiler shader binaries so they can be saved.
   * @note Must be called during initialisation.
   */
  void SetShaderSaver( ShaderSaver& shaderSaver );

private: // From ProgramCache

  /**
   * @copydoc ProgramCache::GetGlAbstraction
   */
  virtual Integration::GlAbstraction& GetGlAbstraction();

  /**
   * @copydoc ProgramCache::GetProgram
   */
  virtual Program* GetProgram( size_t shaderHash );

  /**
   * @copydoc ProgramCache::AddProgram
   */
  virtual void AddProgram( size_t shaderHash, Program* program );

  /**
   * @copydoc ProgramCache::GetCurrentProgram
   */
  virtual Program* GetCurrentProgram();

  /**
   * @copydoc ProgramCache::SetCurrentProgram
   */
  virtual void SetCurrentProgram( Program* program );

  /**
   * @copydoc ProgramCache::IsBinarySupported
   */
  virtual bool IsBinarySupported();

  /**
   * @copydoc ProgramCache::ProgramBinaryFormat
   */
  virtual GLenum ProgramBinaryFormat();

  /**
   * @copydoc ProgramCache::StoreBinary
   */
  virtual void StoreBinary( Internal::ShaderDataPtr programData );

private: // not implemented as non-copyable

  ProgramController( const ProgramController& rhs );
  ProgramController& operator=( const ProgramController& rhs );

private: // Data

  ShaderSaver* mShaderSaver;
  Integration::GlAbstraction& mGlAbstraction;
  Program* mCurrentProgram;

  typedef OwnerContainer< ProgramPair* > ProgramContainer;
  typedef ProgramContainer::Iterator ProgramIterator;
  ProgramContainer mProgramCache;

  GLint mProgramBinaryFormat;
  GLint mNumberOfProgramBinaryFormats;

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_PROGRAM_CONTROLLER_H__


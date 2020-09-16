#ifndef DALI_INTERNAL_PROGRAM_CONTROLLER_H
#define DALI_INTERNAL_PROGRAM_CONTROLLER_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/common/owner-container.h>
#include <dali/internal/common/shader-data.h>
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
  ~ProgramController() override;

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

  /**
   * Clear current cached program
   */
  void ClearCurrentProgram();

private: // From ProgramCache

  /**
   * @copydoc ProgramCache::GetGlAbstraction
   */
  Integration::GlAbstraction& GetGlAbstraction() override;

  /**
   * @copydoc ProgramCache::GetProgram
   */
  Program* GetProgram( size_t shaderHash ) override;

  /**
   * @copydoc ProgramCache::AddProgram
   */
  void AddProgram( size_t shaderHash, Program* program ) override;

  /**
   * @copydoc ProgramCache::GetCurrentProgram
   */
  Program* GetCurrentProgram() override;

  /**
   * @copydoc ProgramCache::SetCurrentProgram
   */
  void SetCurrentProgram( Program* program ) override;

  /**
   * @copydoc ProgramCache::IsBinarySupported
   */
  bool IsBinarySupported() override;

  /**
   * @copydoc ProgramCache::ProgramBinaryFormat
   */
  GLenum ProgramBinaryFormat() override;

  /**
   * @copydoc ProgramCache::StoreBinary
   */
  void StoreBinary( Internal::ShaderDataPtr programData ) override;

private: // not implemented as non-copyable

  ProgramController( const ProgramController& rhs );
  ProgramController& operator=( const ProgramController& rhs );

private: // Data

  ShaderSaver* mShaderSaver;
  Integration::GlAbstraction& mGlAbstraction;
  Program* mCurrentProgram;

  using ProgramContainer = OwnerContainer<ProgramPair*>;
  using ProgramIterator  = ProgramContainer::Iterator;
  ProgramContainer mProgramCache;

  GLint mProgramBinaryFormat;
  GLint mNumberOfProgramBinaryFormats;

};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_PROGRAM_CONTROLLER_H


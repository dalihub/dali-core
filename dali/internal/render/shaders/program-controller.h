#ifndef DALI_INTERNAL_PROGRAM_CONTROLLER_H
#define DALI_INTERNAL_PROGRAM_CONTROLLER_H

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

// INTERNAL INCLUDES
#include <dali/devel-api/common/owner-container.h>
#include <dali/graphics-api/graphics-controller.h>
#include <dali/internal/common/shader-data.h>
#include <dali/internal/render/shaders/program-cache.h>
#include <dali/internal/render/shaders/program.h>

namespace Dali
{
namespace Internal
{
class ShaderSaver;

/**
 * This class is the owner of shader programs
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
    ProgramPair(Program* program, size_t shaderHash)
    : mProgram(program),
      mShaderHash(shaderHash)
    {
    }

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
    [[nodiscard]] inline size_t GetHash() const
    {
      return mShaderHash;
    }

    ProgramPair(const ProgramPair&) = delete;
    ProgramPair& operator=(const ProgramPair&) = delete;

  private: // Data
    Program* mProgram;
    size_t   mShaderHash;
  };

  /**
   * Constructor
   * graphicsController The graphics backend controller
   */
  explicit ProgramController(Graphics::Controller& graphicsController);

  /**
   * Destructor, non virtual as not a base class
   */
  ~ProgramController() override;

  ProgramController(const ProgramController& rhs) = delete;
  ProgramController& operator=(const ProgramController& rhs) = delete;

public: // API
  /**
   * Resets the program matrices. Must be called at the beginning of every frame
   */
  void ResetProgramMatrices();

private: // From ProgramCache
  /**
   * @copydoc ProgramCache::GetProgram
   */
  Program* GetProgram(size_t shaderHash) override;

  /**
   * @copydoc ProgramCache::AddProgram
   */
  void AddProgram(size_t shaderHash, Program* program) override;

private: // Data
  Graphics::Controller& mGraphicsController;

  using ProgramContainer = OwnerContainer<ProgramPair*>;
  using ProgramIterator  = ProgramContainer::Iterator;
  ProgramContainer mProgramCache;
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_PROGRAM_CONTROLLER_H

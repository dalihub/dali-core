#ifndef DALI_INTERNAL_PROGRAM_CONTROLLER_H
#define DALI_INTERNAL_PROGRAM_CONTROLLER_H

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
 * This class ensures that only unique programs are created.
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
      mShaderHash(shaderHash),
      mUsed{true} ///< Initialize as be used at construct time.
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

    [[nodiscard]] inline bool IsUsed() const
    {
      return mUsed;
    }

    void MarkAsUsed()
    {
      mUsed = true;
    }

    void ClearUsedFlag()
    {
      mUsed = false;
    }

    ProgramPair(const ProgramPair&) = delete;
    ProgramPair& operator=(const ProgramPair&) = delete;

  private: // Data
    Program* mProgram;
    size_t   mShaderHash;
    bool     mUsed : 1;
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
   * @brief Reset all program used flags.
   * @note The used flag of program will be marked when we call GetProgram() or AddProgram().
   */
  void ResetUsedFlag();

  /**
   * @brief Clear program incrementally who are not be used.
   *
   * @param[in] fullCollect True if we want to clear whole items.
   * @param[in] forceClearAll True if we want to clear all items forcibly. (Default as false)
   * @return True if we need to iterate more item to check used count. False if we clear cache completely.
   */
  bool ClearUnusedCacheIncrementally(bool fullCollect, bool forceClearAll = false);

  /**
   * @brief Clear all program cache.
   */
  void ClearCache()
  {
    ClearUnusedCacheIncrementally(true, true);
  }

  /**
   * @brief Get the number of cached program
   *
   * @return The number of cached program.
   */
  uint32_t GetCachedProgramCount() const
  {
    return static_cast<uint32_t>(mProgramCache.Count());
  }

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

  ProgramIterator mClearCacheIterator;
  bool            mProgramCacheAdded : 1;
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_PROGRAM_CONTROLLER_H

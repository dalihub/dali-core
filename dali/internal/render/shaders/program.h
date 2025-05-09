#ifndef DALI_INTERNAL_PROGRAM_H
#define DALI_INTERNAL_PROGRAM_H

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

// EXTERNAL INCLUDES
#include <cstdint> // int32_t, uint32_t
#include <memory>
#include <string>
#include <unordered_map>

// INTERNAL INCLUDES
#include <dali/internal/common/const-string.h>
#include <dali/internal/common/shader-data.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/ref-object.h>

#include <dali/internal/render/shaders/render-shader.h>

namespace Dali
{
namespace Graphics
{
class Controller;
class Program;
class Reflection;
} // namespace Graphics

class Matrix;

namespace Internal
{
class ProgramCache;

namespace Render
{
class UniformBlock;
class UniformBufferManager;
class UniformBufferView;
} // namespace Render

/**
 * A program contains a vertex & fragment shader.
 * It interfaces to the implementation program and it's reflection.
 */
class Program
{
public:
  using Hash = std::size_t;

  /**
   * Observer to determine when the program is no longer present
   */
  class LifecycleObserver
  {
  public:
    /**
     * Called shortly before the program is destroyed.
     */
    virtual void ProgramDestroyed(const Program* program) = 0;

  protected:
    /**
     * Virtual destructor, no deletion through this interface
     */
    virtual ~LifecycleObserver() = default;
  };

  /**
   * Indices of default uniforms
   */
  enum class DefaultUniformIndex
  {
    MODEL_MATRIX = 0,
    MVP_MATRIX,
    VIEW_MATRIX,
    MODEL_VIEW_MATRIX,
    NORMAL_MATRIX,
    PROJECTION_MATRIX,
    SCALE,
    SIZE,
    COLOR,
    ACTOR_COLOR,

    COUNT
  };

  /**
   * Creates a new program, or returns a copy of an existing program in the program cache
   * @param[in] cache where the programs are stored
   * @param[in] shaderData  A pointer to a data structure containing the program source
   *                        and optionally precompiled binary. If the binary is empty the program bytecode
   *                        is copied into it after compilation and linking)
   * @param[in] sharedUniformNamesHash Hash value for list of shared uniform buffers.
   * @param[in] gfxController Reference to valid graphics Controller object
   * @return pointer to the program
   */
  static Program* New(ProgramCache& cache, const Internal::ShaderDataPtr& shaderData, std::size_t sharedUniformNamesHash, Graphics::Controller& gfxController);

  Internal::ShaderDataPtr GetShaderData()
  {
    return mProgramData;
  }

  [[nodiscard]] Graphics::Program& GetGraphicsProgram() const
  {
    return *mGfxProgram;
  }

  [[nodiscard]] Graphics::Program* GetGraphicsProgramPtr() const
  {
    return mGfxProgram.get();
  }

  /**
   * Setup the actual program, and ensure that it's reflection is generated.
   */
  void SetGraphicsProgram(Graphics::UniquePtr<Graphics::Program>&& program, Render::UniformBufferManager& uniformBufferManager, const SceneGraph::Shader::UniformBlockContainer& sharedUniformBlockContainer);

  /**
   * Retrieves uniform data.
   * The lookup tries to minimise string comparisons. Ideally, when the hashedName is known
   * and there are no hash collisions in the reflection it's the most optimal case.
   *
   * @param name Name of uniform
   * @param hashedName Hash value from name or 0 if unknown
   * @param hashedNameNoArray Hash value from name without array index & trailing string, or 0 if unknown
   * @param out Reference to output structure
   *
   * @return False when uniform is not found or due to hash collision the result is ambiguous
   */
  bool GetUniform(const std::string_view& name, Hash hashedName, Hash hashedNameNoArray, Graphics::UniformInfo& out) const;

  /**
   * Retrieves default uniform
   * @param[in] defaultUniformIndex index of the uniform
   * @return Valid pointer to the UniformInfo object or nullptr
   */
  [[nodiscard]] const Graphics::UniformInfo* GetDefaultUniform(DefaultUniformIndex defaultUniformIndex) const;

  /**
   * Allows Program to track the life-cycle of this object.
   * Note that we allow to observe lifecycle multiple times.
   * But ProgramDestroyed callback will be called only one time.
   * @param[in] observer The observer to add.
   */
  void AddLifecycleObserver(LifecycleObserver& observer)
  {
    DALI_ASSERT_ALWAYS(!mObserverNotifying && "Cannot add observer while notifying Program::LifecycleObservers");

    auto iter = mLifecycleObservers.find(&observer);
    if(iter != mLifecycleObservers.end())
    {
      // Increase the number of observer count
      ++(iter->second);
    }
    else
    {
      mLifecycleObservers.insert({&observer, 1u});
    }
  }

  /**
   * The Program no longer needs to track the life-cycle of this object.
   * @param[in] observer The observer that to remove.
   */
  void RemoveLifecycleObserver(LifecycleObserver& observer)
  {
    DALI_ASSERT_ALWAYS(!mObserverNotifying && "Cannot remove observer while notifying Program::LifecycleObservers");

    auto iter = mLifecycleObservers.find(&observer);
    DALI_ASSERT_ALWAYS(iter != mLifecycleObservers.end());

    if(--(iter->second) == 0u)
    {
      mLifecycleObservers.erase(iter);
    }
  }

private: // Implementation
  /**
   * Constructor, private so no direct instantiation
   * @param[in] cache where the programs are stored
   * @param[in] shaderData A smart pointer to a data structure containing the program source and binary
   * @param[in] gfxController Reference to Graphics Controller object
   */
  Program(ProgramCache& cache, Internal::ShaderDataPtr shaderData, Graphics::Controller& gfxController);

public:
  Program()               = delete;            ///< default constructor, not defined
  Program(const Program&) = delete;            ///< copy constructor, not defined
  Program& operator=(const Program&) = delete; ///< assignment operator, not defined

  /**
   * Destructor, non virtual as no virtual methods or inheritance
   */
  ~Program();

public:
  /**
   * Struct ReflectionUniformInfo
   * Contains details of a single uniform buffer field and/or sampler.
   */
  struct ReflectionUniformInfo
  {
    size_t                hashValue{0};
    bool                  hasCollision{false};
    Graphics::UniformInfo uniformInfo{};
  };

  /**
   * Build optimized shader reflection of uniforms
   * @param graphicsReflection The graphics reflection
   */
  void BuildRequirements(const Graphics::Reflection& graphicsReflection, Render::UniformBufferManager& uniformBufferManager, const SceneGraph::Shader::UniformBlockContainer& sharedUniformBlockContainer);

  /**
   * Struct UniformBlockMemoryRequirements
   * Contains details of the memory requirements for a RenderItem
   */
  struct UniformBlockMemoryRequirements
  {
    uint32_t blockCount{0u};

    // Ignores explictly allocated blocks
    uint32_t totalSizeRequired{0u};
    uint32_t totalCpuSizeRequired{0u}; ///< requirements for CPU memory
    uint32_t totalGpuSizeRequired{0u}; ///< requirements of hardware buffer for RenderItem

    uint32_t sharedGpuSizeRequired{0u}; ///< requirements of explicitly allocated blocks

    // Per block
    std::vector<uint32_t>              blockSize{};
    std::vector<uint32_t>              blockSizeAligned{};
    std::vector<Render::UniformBlock*> sharedBlock{};
  };
  /**
   * Retrieves uniform blocks requirements
   *
   * @return Reference to the valid UniformBlockMemoryRequirements struct
   */
  [[nodiscard]] const UniformBlockMemoryRequirements& GetUniformBlocksMemoryRequirements() const
  {
    return mUniformBlockMemoryRequirements;
  }

private:                // Data
  ProgramCache& mCache; ///< The program cache

  using LifecycleObserverContainer = std::unordered_map<LifecycleObserver*, uint32_t>; ///< Lifecycle observers container. We allow to add same observer multiple times.
                                                                                       ///< Key is a pointer to observer, value is the number of observer added.
  LifecycleObserverContainer mLifecycleObservers;

  Graphics::UniquePtr<Graphics::Program> mGfxProgram;    ///< Gfx program
  Graphics::Controller&                  mGfxController; ///< Gfx controller
  Internal::ShaderDataPtr                mProgramData;   ///< Shader program source and binary (when compiled & linked or loaded)

  // uniform value caching
  Vector3 mSizeUniformCache; ///< Cache value for size uniform

  using UniformReflectionContainer = std::vector<ReflectionUniformInfo>;

  UniformReflectionContainer mReflection{};                ///< Contains reflection build per program
  UniformReflectionContainer mReflectionDefaultUniforms{}; ///< Contains default uniforms

  UniformBlockMemoryRequirements mUniformBlockMemoryRequirements; ///< Memory requirements per each block, block 0 = standalone/emulated

  bool mObserverNotifying : 1; ///< Safety guard flag to ensure that the LifecycleObserver not be added or deleted while observing.
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_PROGRAM_H

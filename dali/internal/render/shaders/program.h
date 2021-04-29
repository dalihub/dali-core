#ifndef DALI_INTERNAL_PROGRAM_H
#define DALI_INTERNAL_PROGRAM_H

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

// EXTERNAL INCLUDES
#include <cstdint> // int32_t, uint32_t
#include <string>

// INTERNAL INCLUDES
#include <dali/integration-api/gl-abstraction.h>
#include <dali/internal/common/const-string.h>
#include <dali/internal/common/shader-data.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/ref-object.h>

namespace Dali
{
namespace Graphics
{
class Controller;
class Program;
class Reflection;
} // namespace Graphics

class Matrix;

namespace Integration
{
class GlAbstraction;
class ShaderData;
} // namespace Integration

namespace Internal
{
class ProgramCache;

/**
 * A program contains a vertex & fragment shader.
 * It interfaces to the implementation program and it's reflection.
 */
class Program
{
public:
  /**
   * Size of the uniform cache per program
   * GLES specification states that minimum uniform count for fragment shader
   * is 16 and for vertex shader 128. We're caching the 16 common ones for now
   */
  static const int32_t MAX_UNIFORM_CACHE_SIZE = 16;

  /**
   * Constant for uniform not found
   */
  static const int32_t NOT_FOUND = -1;

  /**
   * Common shader uniform names
   */
  enum UniformType
  {
    UNIFORM_NOT_QUERIED = -2,
    UNIFORM_UNKNOWN     = -1,
    UNIFORM_MVP_MATRIX,
    UNIFORM_MODELVIEW_MATRIX,
    UNIFORM_PROJECTION_MATRIX,
    UNIFORM_MODEL_MATRIX,
    UNIFORM_VIEW_MATRIX,
    UNIFORM_NORMAL_MATRIX,
    UNIFORM_COLOR,
    UNIFORM_SAMPLER,
    UNIFORM_SAMPLER_RECT,
    UNIFORM_EFFECT_SAMPLER,

    UNIFORM_SIZE,
    UNIFORM_TYPE_LAST
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
    SIZE,
    COLOR,

    COUNT
  };

  /**
   * Creates a new program, or returns a copy of an existing program in the program cache
   * @param[in] cache where the programs are stored
   * @param[in] shaderData  A pointer to a data structure containing the program source
   *                        and optionally precompiled binary. If the binary is empty the program bytecode
   *                        is copied into it after compilation and linking)
   * param[in]  gfxController Reference to valid graphics Controller object
   * param[in]  gfxProgram Reference to vali graphics Program object
   * @param[in] modifiesGeometry True if the shader modifies geometry
   * @return pointer to the program
   */
  static Program* New(ProgramCache& cache, Internal::ShaderDataPtr shaderData, Graphics::Controller& gfxController, Graphics::UniquePtr<Graphics::Program>&& gfxProgram, bool modifiesGeometry);

  /*
   * Register a uniform name in our local cache
   * @param [in] name uniform name
   * @return the index of the uniform name in local cache
   */
  uint32_t RegisterUniform(ConstString name);

  /**
   * @return true if this program modifies geometry
   */
  bool ModifiesGeometry();

  /**
   * Set the projection matrix that has currently been sent
   * @param matrix to set
   */
  void SetProjectionMatrix(const Matrix* matrix)
  {
    mProjectionMatrix = matrix;
  }

  /**
   * Get the projection matrix that has currently been sent
   * @return the matrix that is set
   */
  const Matrix* GetProjectionMatrix()
  {
    return mProjectionMatrix;
  }

  /**
   * Set the projection matrix that has currently been sent
   * @param matrix to set
   */
  void SetViewMatrix(const Matrix* matrix)
  {
    mViewMatrix = matrix;
  }

  /**
   * Get the projection matrix that has currently been sent
   * @return the matrix that is set
   */
  const Matrix* GetViewMatrix()
  {
    return mViewMatrix;
  }

  GLuint GetProgramId()
  {
    return mProgramId;
  }

  [[nodiscard]] Graphics::Program& GetGraphicsProgram() const
  {
    return *mGfxProgram;
  }

  /**
   * Retrieves uniform data.
   * The lookup tries to minimise string comparisons. Ideally, when the hashedName is known
   * and there are no hash collisions in the reflection it's the most optimal case.
   *
   * @param name Name of uniform
   * @param hashedName Hash value from name or 0 if unknown
   * @param out Reference to output structure
   *
   * @return False when uniform is not found or due to hash collision the result is ambiguous
   */
  bool GetUniform(const std::string& name, size_t hashedName, Graphics::UniformInfo& out) const;

  /**
   * Retrievs default uniform
   * @param[in] defaultUniformIndex index of the uniform
   * @return Valid pointer to the UniformInfo object or nullptr
   */
  const Graphics::UniformInfo* GetDefaultUniform(DefaultUniformIndex defaultUniformIndex) const;

private: // Implementation
  /**
   * Constructor, private so no direct instantiation
   * @param[in] cache where the programs are stored
   * @param[in] shaderData A smart pointer to a data structure containing the program source and binary
   * @param[in] gfxProgram Graphics Program object
   * @param[in] gfxController Reference to Graphics Controller object
   * @param[in] modifiesGeometry True if the vertex shader changes geometry
   */
  Program(ProgramCache& cache, Internal::ShaderDataPtr shaderData, Graphics::Controller& gfxController, Graphics::UniquePtr<Graphics::Program>&& gfxProgram, bool modifiesGeometry);

public:
  /**
   * Destructor, non virtual as no virtual methods or inheritance
   */
  ~Program();

private:
  Program();                          ///< default constructor, not defined
  Program(const Program&);            ///< copy constructor, not defined
  Program& operator=(const Program&); ///< assignment operator, not defined

  /**
   * Resets uniform cache
   */
  void ResetUniformCache();

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
  void BuildReflection(const Graphics::Reflection& graphicsReflection);

private:                                                    // Data
  ProgramCache&                          mCache;            ///< The program cache
  const Matrix*                          mProjectionMatrix; ///< currently set projection matrix
  const Matrix*                          mViewMatrix;       ///< currently set view matrix
  GLuint                                 mProgramId;        ///< GL identifier for program
  Graphics::UniquePtr<Graphics::Program> mGfxProgram;       ///< Gfx program
  Graphics::Controller&                  mGfxController;    /// < Gfx controller
  Internal::ShaderDataPtr                mProgramData;      ///< Shader program source and binary (when compiled & linked or loaded)

  // location caches
  using NameLocationPair = std::pair<ConstString, GLint>;
  using Locations        = std::vector<NameLocationPair>;

  Locations          mUniformLocations;        ///< uniform location cache
  std::vector<GLint> mSamplerUniformLocations; ///< sampler uniform location cache

  // uniform value caching
  GLint   mUniformCacheInt[MAX_UNIFORM_CACHE_SIZE];       ///< Value cache for uniforms of single int
  GLfloat mUniformCacheFloat[MAX_UNIFORM_CACHE_SIZE];     ///< Value cache for uniforms of single float
  GLfloat mUniformCacheFloat2[MAX_UNIFORM_CACHE_SIZE][2]; ///< Value cache for uniforms of two floats
  GLfloat mUniformCacheFloat4[MAX_UNIFORM_CACHE_SIZE][4]; ///< Value cache for uniforms of four floats
  Vector3 mSizeUniformCache;                              ///< Cache value for size uniform
  bool    mModifiesGeometry;                              ///< True if the program changes geometry

  using UniformReflectionContainer = std::vector<ReflectionUniformInfo>;

  UniformReflectionContainer mReflection{};                ///< Contains reflection build per program
  UniformReflectionContainer mReflectionDefaultUniforms{}; ///< Contains default uniforms
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_PROGRAM_H

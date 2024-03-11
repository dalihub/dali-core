#ifndef DALI_INTERNAL_SCENE_GRAPH_SHADER_H
#define DALI_INTERNAL_SCENE_GRAPH_SHADER_H

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
#include <dali/graphics-api/graphics-controller.h>
#include <dali/graphics-api/graphics-shader.h>

#include <dali/internal/common/message.h>
#include <dali/internal/common/shader-data.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/scene-graph-connection-change-propagator.h>
#include <dali/internal/update/rendering/shader-cache.h>

namespace Dali::Internal::SceneGraph
{

class ConnectionObserver;
class SceneController;
class UniformBufferManager;

/**
 * Owner of Graphics Program; also enables sharing of uniform properties.
 * Owned by UpdateManager.
 *
 * Equivalent of Render::Program in modern GLES world, but it's (also) the scene graph object of Dali::Shader,
 */
class Shader : public PropertyOwner
{
public:
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
    COLOR
  };

  struct UniformBlockMemoryRequirements
  {
    uint32_t blockCount{ 0u };
    uint32_t totalSizeRequired {0u};
    uint32_t totalCpuSizeRequired{ 0u };
    uint32_t totalGpuSizeRequired{ 0u };

    std::vector<uint32_t> blockSize{};
    std::vector<uint32_t> blockSizeAligned{};
  };

  /**
   * Devel constructor
   * @param hints
   */
  explicit Shader( Dali::Shader::Hint::Value& hints );

  /**
   * Virtual destructor
   */
  ~Shader() override;

  /**
   * Initialize the shader object with the Graphics API when added to UpdateManager
   *
   * @param[in] graphics The Graphics API
   * @param[in] shaderCache A cache
   */
  void Initialize( Graphics::Controller& graphics, ShaderCache& shaderCache, UniformBufferManager& uboManager );

  /**
   * Query whether a shader hint is set.
   * @pre The shader has been initialized.
   * @param[in] hint The hint to check.
   * @return True if the given hint is set.
   */
  [[nodiscard]] bool HintEnabled( Dali::Shader::Hint::Value hint ) const
  {
    return mHints & hint;
  }

  /**
   * Get the graphics shader object
   * @return the graphics shader object
   */
  [[nodiscard]] const Graphics::Program* GetGraphicsObject() const;

  /**
   * Get the graphics shader object
   * @return the graphics shader object
   */
  Graphics::Program* GetGraphicsObject();

  /**
   * Destroy any graphics objects owned by this scene graph object
   */
  void DestroyGraphicsObjects();

  [[nodiscard]] const UniformBlockMemoryRequirements& GetUniformBlockMemoryRequirements()
  {
    return mUniformBlockMemoryRequirements;
  }

  /**
   * Retrieves uniform data.
   * The lookup tries to minimise string comparisons. Ideally, when the hashedName is known
   * and there are no hash collisions in the reflection it's the most optimal case.
   *
   * @param name Name of uniform
   * @param hashedName Hash value from name or 0 if unknown
   * @param arrayIndex Index within array elements or 0 if the uniform is not an array
   * @param out Reference to output structure
   *
   * @return False when uniform is not found or due to hash collision the result is ambiguous
   */
  bool GetUniform( const std::string& name, size_t hashedName, Graphics::UniformInfo& out ) const;

  bool GetUniform( const std::string& name, size_t hashedName, size_t hashNoArray, Graphics::UniformInfo& out ) const;

  /**
   * Retrievs default uniform
   * @param[in] defaultUniformIndex index of the uniform
   * @return Valid pointer to the UniformInfo object or nullptr
   */
  [[nodiscard]] const Graphics::UniformInfo* GetDefaultUniform( DefaultUniformIndex defaultUniformIndex ) const;

public: // Messages
  /**
   * Set the shader data into the graphics API.
   *
   * @param[in] shaderData The shader source or binary.
   * @param[in] modifiesGeometry Hint to say if the shader modifies geometry. (useful for culling)
   */
  void SetShaderProgram( Internal::ShaderDataPtr shaderData, bool modifiesGeometry );

public: // Implementation of ConnectionChangePropagator

  /**
   * @copydoc ConnectionChangePropagator::AddObserver
   */
  void AddConnectionObserver(ConnectionChangePropagator::Observer& observer);

  /**
   * @copydoc ConnectionChangePropagator::RemoveObserver
   */
  void RemoveConnectionObserver(ConnectionChangePropagator::Observer& observer);

private:

  /**
   * Struct ReflectionUniformInfo
   * Contains details of a single uniform buffer field and/or sampler.
   */
  struct ReflectionUniformInfo
  {
    Graphics::UniformInfo uniformInfo {};
    size_t hashValue { 0 };
    bool   hasCollision { false };
  };

  /**
   * Build optimized shader reflection of uniforms
   */
  void BuildReflection();

private: // Data
  Graphics::Controller*           mController; ///< Graphics interface object
  Graphics::Program*              mGraphicsProgram{nullptr}; ///< The graphics object
  ShaderCache*                    mShaderCache; ///< The Program cache (Owns assoc Graphics::Program)
  UniformBufferManager*           mUboManager;

  Dali::Shader::Hint::Value       mHints; ///< Hints for the shader
  ConnectionChangePropagator      mConnectionObservers; ///< Watch for connection changes

  using UniformReflectionContainer = std::vector< ReflectionUniformInfo >;

  UniformReflectionContainer      mReflection; ///< Contains reflection build per shader
  UniformReflectionContainer      mReflectionDefaultUniforms; ///< Contains default uniforms

  UniformBlockMemoryRequirements mUniformBlockMemoryRequirements;
};


inline void SetShaderProgramMessage( EventThreadServices& eventThreadServices,
                                     Shader& shader,
                                     const Internal::ShaderDataPtr& shaderData,
                                     bool modifiesGeometry )
{
  typedef MessageValue2< Shader, Internal::ShaderDataPtr, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ), false );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &shader, &Shader::SetShaderProgram, shaderData, modifiesGeometry );
}



} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_SHADER_H

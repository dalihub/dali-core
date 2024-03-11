#ifndef DALI_INTERNAL_UNIFORM_BUFFER_MANAGER_H
#define DALI_INTERNAL_UNIFORM_BUFFER_MANAGER_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/common/map-wrapper.h>
#include <dali/graphics-api/graphics-controller.h>

namespace Dali::Internal::SceneGraph
{
class Scene;
class UniformBufferV2;
class UniformBufferView;

/**
 * Class UniformBufferManager
 *
 * Manages the uniform buffers.
 *
 */
class UniformBufferManager
{
public:
  explicit UniformBufferManager(Dali::Graphics::Controller* controller);

  ~UniformBufferManager();

  Graphics::UniquePtr<UniformBufferView> CreateUniformBufferView(uint32_t size, bool emulated = true);

  /**
   * @brief Registers scene with the manager
   * The manager creates a set of UBOs per scene.
   *
   * @param[in] scene Valid pointer to the scene
   */
  void RegisterScene(const SceneGraph::Scene* scene=nullptr);

  /**
   * @brief Removes association with a scene
   *
   * @param[in] scene Valid pointer to a scene object
   */
  void UnregisterScene(const SceneGraph::Scene* scene=nullptr);

  /**
   * @brief Get the uniform buffer for the given scene, if current.
   */
  UniformBufferV2* GetUniformBufferForScene(const SceneGraph::Scene* scene, bool offscreen, bool emulated);

  /**
   * @brief must be called when rendering scene starts, this way the manager knows
   * which UBO set we are going to use.
   */
  void SetCurrentSceneRenderInfo(const SceneGraph::Scene* scene, bool offscreen);

  /**
   * @brief Rolls back UBO set matching conditions
   * @param[in] scene Valid scene pointer
   * @param[in] offscreen Offscreen or onscreen
   */
  void Rollback(const SceneGraph::Scene* scene, bool offscreen);

  /**
   * @brief Flushes current UBO set
   */
  void Flush(const SceneGraph::Scene* scene, bool offscreen);

  /**
   * Gets the uniform block alignment.
   * It will cache it locally, as this doesn't change during a graphics context's lifetime
   * @param[in] emulated - True if this is to query CPU alignment, or false to query GPU
   * alignment.
   * @return the alignment
   */
  uint32_t GetUniformBlockAlignment(bool emulated);

private:
  Dali::Graphics::Controller* mController;

  /**
   * Stores pointers to uniform buffers associated with the scene
   * There may be up to 4 UBOs created per scene:
   * - Emulated on-screen
   * - GPU on-screen
   * - Emulated off-screen
   * - GPU off-screen
   */
  struct UBOSet
  {
    UBOSet()  = default;
    ~UBOSet() = default;

    UBOSet(UBOSet&& rhs) noexcept ;

    enum class BufferType
    {
      CPU_ONSCREEN  = 0,
      GPU_ONSCREEN  = 1,
      CPU_OFFSCREEN = 2,
      GPU_OFFSCREEN = 3,
    };

    static BufferType GetBufferType(bool offscreen, bool emulated)
    {
      if(offscreen)
      {
        return (emulated ? BufferType::CPU_OFFSCREEN : BufferType::GPU_OFFSCREEN);
      }
      return (emulated ? BufferType::CPU_ONSCREEN : BufferType::GPU_ONSCREEN);
    }

    Graphics::UniquePtr<UniformBufferV2> cpuBufferOnScreen;
    Graphics::UniquePtr<UniformBufferV2> gpuBufferOnScreen;
    Graphics::UniquePtr<UniformBufferV2> cpuBufferOffScreen;
    Graphics::UniquePtr<UniformBufferV2> gpuBufferOffScreen;

    Graphics::UniquePtr<UniformBufferV2>& GetBuffer(BufferType bufferType)
    {
      switch(bufferType)
      {
        case BufferType::CPU_ONSCREEN:
        {
          return cpuBufferOnScreen;
        }
        case BufferType::CPU_OFFSCREEN:
        {
          return cpuBufferOffScreen;
        }
        case BufferType::GPU_ONSCREEN:
        {
          return gpuBufferOnScreen;
        }
        case BufferType::GPU_OFFSCREEN:
        {
          return gpuBufferOffScreen;
        }
      }
      // Appease the compiler
      return cpuBufferOnScreen;
    }
  };

  Graphics::UniquePtr<UBOSet>          mUBOSet;
  uint32_t                             mCachedUniformBlockAlignment{0u};
  bool                                 mCurrentSceneOffscreen{false};
};

} // namespace Dali::Internal::Render

#endif // DALI_INTERNAL_UNIFORM_BUFFER_MANAGER_H

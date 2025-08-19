#ifndef DALI_INTERNAL_UNIFORM_BLOCK_H
#define DALI_INTERNAL_UNIFORM_BLOCK_H

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
 */

// EXTERNAL INCLUDES
#include <string_view>
#include <unordered_map>

// INTERNAL INCLUDES
#include <dali/internal/event/common/object-impl.h> // Dali::Internal::Object
#include <dali/public-api/common/intrusive-ptr.h>   // Dali::IntrusivePtr
#include <dali/public-api/rendering/uniform-block.h>

namespace Dali::Internal
{
namespace Render
{
class UniformBlock;
}

class UniformBlock;
using UniformBlockPtr = IntrusivePtr<UniformBlock>;

/**
 * @copydoc Dali::UniformBlock
 */
class UniformBlock : public Object, public Object::Observer
{
public:
  /**
   * @brief Construct a new UniformBlock resource.
   *
   * @param[in] name The name of the uniform block in the shader
   * @return A new UniformBlock resource
   */
  static UniformBlockPtr New(std::string&& name);

  /**
   * @brief Get the scene object associated with this proxy object.
   */
  const Render::UniformBlock& GetUniformBlockSceneObject() const;

public:
  /**
   * @copydoc Dali::UniformBlock::GetUniformBlockName
   */
  std::string_view GetUniformBlockName() const
  {
    return mUniformBlockName;
  }

  /**
   * @copydoc Dali::UniformBlock::ConnectToShader
   */
  bool ConnectToShader(Shader* shader, bool strongConnection)
  {
    return ConnectToShader(shader, strongConnection, true);
  }

  /**
   * @copydoc Dali::UniformBlock::DisconnectFromShader
   */
  void DisconnectFromShader(Shader* shader);

public:
  /**
   * @brief Be connected to the shader.
   *
   * @param[in] shader The shader to be connected.
   * @param[in] strongConnection Whether we connect uniform blocks to shader strong or weak.
   * @param[in] programCacheCleanRequired Whether program cache clean is required or not.
   * Could be false only if the shader never be rendered before. (e.g. shader constructor.)
   */
  bool ConnectToShader(Shader* shader, bool strongConnection, bool programCacheCleanRequired);

protected: ///< From Dali::Internal::Object::Observer
  /**
   * @copydoc Dali::Internal::Object::Observer::SceneObjectAdded()
   */
  void SceneObjectAdded(Object& object) override
  {
    // Do nothing
  }

  /**
   * @copydoc Dali::Internal::Object::Observer::SceneObjectRemoved()
   */
  void SceneObjectRemoved(Object& object) override
  {
    // Do nothing
  }

  /**
   * @copydoc Dali::Internal::Object::Observer::ObjectDestroyed()
   */
  void ObjectDestroyed(Object& object) override;

protected: // implementation
  /**
   * @brief Constructor.
   *
   * @param sceneObject the scene graph uniformBlock
   */
  UniformBlock(const Render::UniformBlock* sceneObject);

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~UniformBlock() override;

private:
  UniformBlock(const UniformBlock&)            = delete; ///< Deleted copy constructor
  UniformBlock& operator=(const UniformBlock&) = delete; ///< Deleted assignment operator

private:
  using ShaderContainer = std::unordered_map<Shader*, bool>;

  ShaderContainer mShaderContainer; // List of shaders that are connected to this UniformBlock (not owned) and strong connected.

  std::string_view mUniformBlockName; ///< The name of uniform blocks. String owned by scene object.
};

}; // namespace Dali::Internal

namespace Dali
{
// Helpers for public-api forwarding methods
inline Internal::UniformBlock& GetImplementation(Dali::UniformBlock& handle)
{
  DALI_ASSERT_ALWAYS(handle && "UniformBlock handle is empty");

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::UniformBlock&>(object);
}

inline const Internal::UniformBlock& GetImplementation(const Dali::UniformBlock& handle)
{
  DALI_ASSERT_ALWAYS(handle && "UniformBlock handle is empty");

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::UniformBlock&>(object);
}

} // namespace Dali

#endif // DALI_INTERNAL_UNIFORM_BLOCK_H

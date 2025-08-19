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

// CLASS HEADER
#include <dali/public-api/rendering/uniform-block.h>

// INTERNAL INCLUDES
#include <dali/internal/event/rendering/shader-impl.h>
#include <dali/internal/event/rendering/uniform-block-impl.h>

namespace Dali
{
UniformBlock UniformBlock::New(std::string blockName)
{
  Internal::UniformBlockPtr object = Internal::UniformBlock::New(std::move(blockName));
  return UniformBlock(object.Get());
}

UniformBlock::UniformBlock() = default;

UniformBlock::~UniformBlock() = default;

UniformBlock::UniformBlock(const UniformBlock& handle) = default;

UniformBlock& UniformBlock::operator=(const UniformBlock& handle) = default;

UniformBlock::UniformBlock(UniformBlock&& rhs) noexcept = default;

UniformBlock& UniformBlock::operator=(UniformBlock&& handle) noexcept = default;

UniformBlock UniformBlock::DownCast(BaseHandle handle)
{
  return UniformBlock(dynamic_cast<Dali::Internal::UniformBlock*>(handle.GetObjectPtr()));
}

std::string_view UniformBlock::GetUniformBlockName() const
{
  return GetImplementation(*this).GetUniformBlockName();
}

bool UniformBlock::ConnectToShader(Shader shader, bool strongConnection)
{
  if(DALI_LIKELY(shader))
  {
    return GetImplementation(*this).ConnectToShader(&GetImplementation(shader), strongConnection);
  }
  return false;
}

void UniformBlock::DisconnectFromShader(Shader shader)
{
  if(DALI_LIKELY(shader))
  {
    GetImplementation(*this).DisconnectFromShader(&GetImplementation(shader));
  }
}

UniformBlock::UniformBlock(Internal::UniformBlock* object)
: Handle(object)
{
}

} // namespace Dali

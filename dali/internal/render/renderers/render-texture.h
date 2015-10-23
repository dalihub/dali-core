#ifndef DALI_INTERNAL_RENDER_TEXTURE_H
#define DALI_INTERNAL_RENDER_TEXTURE_H

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <dali/devel-api/rendering/sampler.h>
#include <dali/integration-api/resource-declarations.h>
#include <string>

namespace Dali
{
namespace Internal
{
namespace Render
{
class Sampler;

class Texture
{
public:

  /**
   * Constructor
   */
  Texture()
  :mUniformName(),
   mSampler(0),
   mTextureId(Integration::InvalidResourceId)
  {}

  /**
   * Constructor
   */
  Texture( const std::string& samplerName, Integration::ResourceId textureId, Render::Sampler* sampler )
  :mUniformName( samplerName ),
   mSampler( sampler ),
   mTextureId( textureId)
  {}

  /**
   * Destructor
   */
  ~Texture()
  {}

  /*
   * Get the Render::Sampler used by the texture
   * @Return The Render::Sampler being used or 0 if using the default
   */
  inline const Render::Sampler* GetSampler() const
  {
    return mSampler;
  }

public: // called from RenderThread

  /**
   * Get the texture unit uniform name
   * @return the name of the texture unit uniform
   */
  inline const std::string& GetUniformName() const
  {
    return mUniformName;
  }

  /**
   * Get the texture ID
   * @param[in] bufferIndex The buffer index to use
   * @return the identity of the associated texture
   */
  inline Integration::ResourceId GetTextureId() const
  {
    return mTextureId;
  }

private:
  std::string             mUniformName;
  Render::Sampler*        mSampler;
  Integration::ResourceId mTextureId;
};

} // namespace SceneGraph

} // namespace Internal
} // namespace Dali


#endif //  DALI_INTERNAL_RENDER_TEXTURE_H

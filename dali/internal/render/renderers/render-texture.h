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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/devel-api/rendering/sampler.h>
#include <dali/integration-api/resource-declarations.h>

namespace Dali
{
namespace Internal
{
namespace Render
{
class Sampler;

/**
 * This class is the mapping between texture id, sampler and sampler uniform name
 */
class Texture
{
public:

  /**
   * Enumeration to tell that this sampler does not have a unique index yet
   */
  enum
  {
    NOT_INITIALIZED = -1
  };

  /**
   * Constructor
   */
  Texture()
  : mSampler( 0 ),
    mUniformName(),
    mTextureId( Integration::InvalidResourceId ),
    mUniformUniqueIndex( NOT_INITIALIZED )
  {}

  /**
   * Constructor
   */
  Texture( const std::string& samplerName, Integration::ResourceId textureId, Render::Sampler* sampler )
  : mSampler( sampler ),
    mUniformName( samplerName ),
    mTextureId( textureId),
    mUniformUniqueIndex( NOT_INITIALIZED )
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
   * @return the id of the associated texture
   */
  inline Integration::ResourceId GetTextureId() const
  {
    return mTextureId;
  }

  /**
   * Get the Uniform unique index
   * @return the identity of the associated texture
   */
  inline void SetUniformUniqueIndex( int32_t index )
  {
    mUniformUniqueIndex = index;
  }

  /**
   * Get the Uniform unique index
   * @return the identity of the associated texture
   */
  inline int32_t GetUniformUniqueIndex() const
  {
    return mUniformUniqueIndex;
  }

private:

  Render::Sampler*        mSampler;
  std::string             mUniformName;
  Integration::ResourceId mTextureId;
  int32_t                 mUniformUniqueIndex;

};

} // namespace Render

} // namespace Internal

} // namespace Dali


#endif //  DALI_INTERNAL_RENDER_TEXTURE_H

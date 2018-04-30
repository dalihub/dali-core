/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#include <dali/internal/update/rendering/data-providers/render-data-provider.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

RenderDataProvider::RenderDataProvider()
: mUniformMapDataProvider( NULL ),
  mShader( NULL )
{
}

RenderDataProvider::~RenderDataProvider()
{
}

void RenderDataProvider::SetUniformMap(const UniformMapDataProvider& uniformMapDataProvider)
{
  mUniformMapDataProvider = &uniformMapDataProvider;
}

const UniformMapDataProvider& RenderDataProvider::GetUniformMap() const
{
  return *mUniformMapDataProvider;
}

void RenderDataProvider::SetShader( Shader& shader )
{
  mShader = &shader;
}

Shader& RenderDataProvider::GetShader() const
{
  return *mShader;
}

RenderDataProvider::Samplers& RenderDataProvider::GetSamplers()
{
  return mSamplers;
}

std::vector<SceneGraph::Texture*>& RenderDataProvider::GetTextures()
{
  return mTextures;
}

} // SceneGraph
} // Internal
} // Dali

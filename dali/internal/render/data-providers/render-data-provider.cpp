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
 *
 */

#include "render-data-provider.h"

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

RenderDataProvider::RenderDataProvider(
  const GeometryDataProvider&   geometryDataProvider,
  const MaterialDataProvider&   materialDataProvider,
  const UniformMapDataProvider& uniformMapDataProvider,
  Shader&                       shader,
  const PropertyBuffer*         indexBuffer,
  const VertexBuffers&          vertexBuffers,
  const Samplers&               samplers )

: mGeometryDataProvider( &geometryDataProvider ),
  mMaterialDataProvider( &materialDataProvider ),
  mUniformMapDataProvider( &uniformMapDataProvider ),
  mShader( &shader ),
  mIndexBuffer( indexBuffer ),
  mVertexBuffers( vertexBuffers ),
  mSamplers( samplers )
{
}

RenderDataProvider::~RenderDataProvider()
{
}

void RenderDataProvider::SetGeometry( const GeometryDataProvider& geometryDataProvider )
{
  mGeometryDataProvider = &geometryDataProvider;
}

const GeometryDataProvider& RenderDataProvider::GetGeometry() const
{
  return *mGeometryDataProvider;
}

void RenderDataProvider::SetMaterial( const MaterialDataProvider& materialDataProvider )
{
  mMaterialDataProvider = &materialDataProvider;
}

const MaterialDataProvider& RenderDataProvider::GetMaterial() const
{
  return *mMaterialDataProvider;
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

void RenderDataProvider::SetIndexBuffer( const PropertyBuffer* indexBuffer )
{
  mIndexBuffer = indexBuffer;
}

const PropertyBuffer* RenderDataProvider::GetIndexBuffer() const
{
  return mIndexBuffer;
}

void RenderDataProvider::SetVertexBuffers( const RenderDataProvider::VertexBuffers& vertexBuffers)
{
  DALI_ASSERT_DEBUG( vertexBuffers.Count() > 0 && "Using empty vertex buffer" );
  mVertexBuffers = vertexBuffers;
}

const RenderDataProvider::VertexBuffers& RenderDataProvider::GetVertexBuffers() const
{
  return mVertexBuffers;
}

void RenderDataProvider::SetSamplers( const RenderDataProvider::Samplers& samplers )
{
  mSamplers = samplers;
}

const RenderDataProvider::Samplers& RenderDataProvider::GetSamplers() const
{
  return mSamplers;
}


} // SceneGraph
} // Internal
} // Dali

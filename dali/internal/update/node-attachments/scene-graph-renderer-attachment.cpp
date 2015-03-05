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

#include "scene-graph-renderer-attachment.h"

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

RendererAttachment::RendererAttachment()
: RenderableAttachment( false ),
  mMaterial(NULL),
  mGeometry(NULL),
  mDepthIndex(0)
{
}

RendererAttachment::~RendererAttachment()
{
  mMaterial=NULL;
  mGeometry=NULL;
}

void RendererAttachment::SetMaterial(const Material* material)
{
  // @todo MESH_REWORK
  mMaterial = material;
}

const Material& RendererAttachment::GetMaterial() const
{
  return *mMaterial;
}

void RendererAttachment::SetGeometry(const Geometry* geometry)
{
  // @todo MESH_REWORK
  mGeometry = geometry;
}

const Geometry& RendererAttachment::GetGeometry() const
{
  return *mGeometry;
}

void RendererAttachment::SetDepthIndex( int index )
{
  // @todo MESH_REWORK
  mDepthIndex = index;
}

int RendererAttachment::GetDepthIndex() const
{
  return mDepthIndex;
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

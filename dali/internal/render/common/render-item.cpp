//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali/internal/render/common/render-item.h>

// INTERNAL INCLUDES
#include <dali/internal/render/renderers/scene-graph-renderer.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

RenderItem::RenderItem()
: mRenderer( NULL ),
  mModelViewMatrix( false )
{
}

RenderItem::~RenderItem()
{
}

void RenderItem::Reset()
{
  mRenderer = NULL;
}

void RenderItem::SetRenderer( Renderer* renderer )
{
  mRenderer = renderer;
}

const Renderer* RenderItem::GetRenderer() const
{
  return mRenderer;
}

Matrix& RenderItem::GetModelViewMatrix()
{
  return mModelViewMatrix;
}

const Matrix& RenderItem::GetModelViewMatrix() const
{
  return mModelViewMatrix;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

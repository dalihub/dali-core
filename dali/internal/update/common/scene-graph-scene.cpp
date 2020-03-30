/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/common/scene-graph-scene.h>

#include <dali/internal/update/render-tasks/scene-graph-render-task-list.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

Scene::Scene()
: mContext( nullptr )
{
}

Scene::~Scene()
{
  if ( mContext )
  {
    mContext->GlContextDestroyed();

    delete mContext;
    mContext = nullptr;
  }
}

void Scene::GlContextDestroyed()
{
  if ( mContext )
  {
    mContext->GlContextDestroyed();
  }
}

void Scene::Initialize( Context& context )
{
  if ( mContext != &context )
  {
    if ( mContext )
    {
      mContext->GlContextDestroyed();
      delete mContext;
    }

    mContext = &context;
    mContext->GlContextCreated();
  }
}

Context* Scene::GetContext()
{
  return mContext;
}

RenderInstructionContainer& Scene::GetRenderInstructions()
{
  return mInstructions;
}

} //SceneGraph

} //Internal

} //Dali

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task-list.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
Scene::Scene()
: mContext(nullptr),
  mFrameRenderedCallbacks(),
  mFramePresentedCallbacks(),
  mSkipRendering(false),
  mSurfaceRect(),
  mSurfaceOrientation(0),
  mSurfaceRectChanged(false)
{
}

Scene::~Scene()
{
  mFrameRenderedCallbacks.clear();
  mFramePresentedCallbacks.clear();
}

void Scene::Initialize(Context& context)
{
  mContext = &context;
}

Context* Scene::GetContext()
{
  return mContext;
}

RenderInstructionContainer& Scene::GetRenderInstructions()
{
  return mInstructions;
}

void Scene::AddFrameRenderedCallback(CallbackBase* callback, int32_t frameId)
{
  mFrameRenderedCallbacks.push_back(std::make_pair(std::unique_ptr<CallbackBase>(callback), frameId));
}

void Scene::AddFramePresentedCallback(CallbackBase* callback, int32_t frameId)
{
  mFramePresentedCallbacks.push_back(std::make_pair(std::unique_ptr<CallbackBase>(callback), frameId));
}

void Scene::GetFrameRenderedCallback(Dali::Integration::Scene::FrameCallbackContainer& callbacks)
{
  // Transfer owership of the callbacks
  for(auto&& iter : mFrameRenderedCallbacks)
  {
    callbacks.push_back(std::make_pair(std::move(iter.first), iter.second));
  }

  mFrameRenderedCallbacks.clear();
}

void Scene::GetFramePresentedCallback(Dali::Integration::Scene::FrameCallbackContainer& callbacks)
{
  // Transfer owership of the callbacks
  for(auto&& iter : mFramePresentedCallbacks)
  {
    callbacks.push_back(std::make_pair(std::move(iter.first), iter.second));
  }

  mFramePresentedCallbacks.clear();
}

void Scene::SetSkipRendering(bool skip)
{
  mSkipRendering = skip;
}

bool Scene::IsRenderingSkipped() const
{
  return mSkipRendering;
}

void Scene::SetSurfaceRect(const Rect<int32_t>& rect)
{
  mSurfaceRect        = rect;
  mSurfaceRectChanged = true;
}

const Rect<int32_t>& Scene::GetSurfaceRect() const
{
  return mSurfaceRect;
}

void Scene::SetSurfaceOrientation(int32_t orientation)
{
  mSurfaceOrientation = orientation;
}

int32_t Scene::GetSurfaceOrientation() const
{
  return mSurfaceOrientation;
}

bool Scene::IsSurfaceRectChanged()
{
  bool surfaceRectChanged = mSurfaceRectChanged;
  mSurfaceRectChanged     = false;

  return surfaceRectChanged;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

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

// CLASS HEADER
#include <dali/internal/event/actor-attachments/renderer-attachment-impl.h>

// INTERNAL INCLUDES
#include <dali/internal/update/node-attachments/scene-graph-renderer-attachment.h>
#include <dali/internal/event/common/stage-impl.h>

namespace Dali
{

namespace Internal
{

RendererAttachmentPtr RendererAttachment::New( Stage& stage, const SceneGraph::Node& parentNode, Renderer& renderer )
{
  RendererAttachmentPtr attachment( new RendererAttachment( stage ) );

  // Transfer object ownership of scene-object to message
  SceneGraph::RendererAttachment* sceneObject = CreateSceneObject();
  AttachToNodeMessage( stage.GetUpdateManager(), parentNode, sceneObject );

  // Keep raw pointer for message passing
  attachment->mSceneObject = sceneObject;

  return attachment;
}

RendererAttachment::RendererAttachment( Stage& stage )
: RenderableAttachment(stage),
  mSceneObject(NULL)
{
}

RendererAttachment::~RendererAttachment()
{
}

SceneGraph::RendererAttachment* RendererAttachment::CreateSceneObject()
{
  return SceneGraph::RendererAttachment::New();
}

const SceneGraph::RendererAttachment& RendererAttachment::GetSceneObject() const
{
  DALI_ASSERT_DEBUG( mSceneObject != NULL );
  return *mSceneObject;
}


void RendererAttachment::OnStageConnection2()
{
  mRendererConnector.OnStageConnect();
}

void RendererAttachment::OnStageDisconnection2()
{
  mRendererConnector.OnStageDisconnect();
}



} // namespace Internal

} // namespace Dali

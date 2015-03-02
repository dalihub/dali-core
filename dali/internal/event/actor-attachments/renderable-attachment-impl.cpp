/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/actor-attachments/renderable-attachment-impl.h>

// INTERNAL INCLUDES
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/update/node-attachments/scene-graph-renderable-attachment.h>

using Dali::Internal::SceneGraph::Shader;

namespace Dali
{

namespace Internal
{

RenderableAttachment::RenderableAttachment( Stage& stage )
: ActorAttachment( stage )
{
}

RenderableAttachment::~RenderableAttachment()
{
}


void RenderableAttachment::OnStageConnection()
{
  // For derived classes
  OnStageConnection2();
}

void RenderableAttachment::OnStageDisconnection()
{
  // For derived classes
  OnStageDisconnection2();
}

} // namespace Internal

} // namespace Dali

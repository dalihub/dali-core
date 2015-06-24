#ifndef DALI_INTERNAL_RENDERER_ATTACHMENT_H
#define DALI_INTERNAL_RENDERER_ATTACHMENT_H

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

// INTERNAL INCLUDES
#include <dali/internal/event/common/object-connector.h>
#include <dali/internal/event/actors/renderer-impl.h>
#include <dali/internal/event/actor-attachments/renderable-attachment-impl.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{
class Node;
class RendererAttachment;
}

class RendererAttachment;
typedef IntrusivePtr<RendererAttachment>  RendererAttachmentPtr;

/**
 * An attachment for rendering renderers.
 *
 * Currently, the Renderer object creates the SceneGraph::RendererAttachment object, not this. It is attached
 * here as needed.
 */
class RendererAttachment : public RenderableAttachment
{
public:

  /**
   * Create a new RendererAttachment.
   * @param[in] eventThreadServices Used for messaging to and reading from the SceneGraph
   * @param[in] parentNode The node to attach a scene-object to.
   * @poaram[in] renderer The renderer for this attachment
   * @return A smart-pointer to the newly allocated RendererAttachment.
   */
  static RendererAttachmentPtr New( EventThreadServices& eventThreadServices, const SceneGraph::Node& parentNode, Renderer& renderer );


  /**
   * Second stage initialization
   *
   * @param[in] eventThreadServices Used for messaging to and reading from the SceneGraph
   * @param[in] parentNode The node to attach a scene-object to.
   * @poaram[in] renderer The renderer for this attachment
   */
  void Initialize( EventThreadServices& eventThreadServices, const SceneGraph::Node& parentNode, Renderer& renderer );

  /**
   * Get the renderer
   *
   * @return The renderer
   */
  Renderer& GetRenderer()
  {
    return *mRendererConnector.Get();
  }

private:

  /**
   * First stage construction of a RendererAttachment.
   * @param[in] eventThreadServices Used for messaging to and reading from the SceneGraph
   */
  RendererAttachment( EventThreadServices& eventThreadServices );

  /**
   * @copydoc Dali::Internal::RenderableAttachment::GetSceneObject()
   */
  virtual const SceneGraph::RendererAttachment& GetSceneObject() const;

  /**
   * @copydoc Dali::Internal::RenderableAttachment::OnStageConnection2()
   */
  virtual void OnStageConnection2();

  /**
   * @copydoc Dali::Internal::RenderableAttachment::OnStageDisconnection2()
   */
  virtual void OnStageDisconnection2();

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~RendererAttachment();

private:
  const SceneGraph::RendererAttachment* mSceneObject; ///< Not owned

  ObjectConnector<Renderer> mRendererConnector;
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_RENDERER_ATTACHMENT_H

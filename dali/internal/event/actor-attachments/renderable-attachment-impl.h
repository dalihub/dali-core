#ifndef __DALI_INTERNAL_RENDERABLE_ATTACHMENT_H__
#define __DALI_INTERNAL_RENDERABLE_ATTACHMENT_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/actors/renderable-actor.h>
#include <dali/internal/event/actor-attachments/actor-attachment-impl.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{
class RenderableAttachment;
}

/**
 * An base class for renderable actor attachments
 */
class RenderableAttachment : public ActorAttachment
{
public:



protected:

  /**
   * Protected constructor; only base classes are instantiatable.
   * @param[in] eventThreadServices Used for messaging to and reading from scene-graph.
   */
  RenderableAttachment( EventThreadServices& eventThreadServices );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~RenderableAttachment();

private:

  // Undefined
  RenderableAttachment(const RenderableAttachment&);

  // Undefined
  RenderableAttachment& operator=(const RenderableAttachment& rhs);

  /**
   * Helper for getting the scene-graph renderable attachment.
   * @return The scene object.
   */
  const SceneGraph::RenderableAttachment& GetSceneAttachment() const;

  /**
   * @copydoc Dali::Internal::ActorAttachment::OnStageConnection()
   */
  virtual void OnStageConnection();

  /**
   * @copydoc Dali::Internal::ActorAttachment::OnStageDisconnection()
   */
  virtual void OnStageDisconnection();

  /**
   * For derived classes, chained from OnStageConnection()
   */
  virtual void OnStageConnection2() = 0;

  /**
   * For derived classes, chained from OnStageDisconnection()
   */
  virtual void OnStageDisconnection2() = 0;

private: // Data, cached for actor-thread getters

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_RENDERABLE_ATTACHMENT_H__

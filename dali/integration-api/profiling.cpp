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
#include <dali/integration-api/profiling.h>

// INTERNAL INCLUDES
#include <dali/integration-api/bitmap.h>

#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/events/gesture-event-processor.h>

#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/camera-actor-impl.h>
#include <dali/internal/event/actors/text-actor-impl.h>
#include <dali/internal/event/actors/image-actor-impl.h>
#include <dali/internal/event/actors/mesh-actor-impl.h>
#include <dali/internal/event/actors/layer-impl.h>

#include <dali/internal/event/actor-attachments/actor-attachment-impl.h>
#include <dali/internal/event/actor-attachments/camera-attachment-impl.h>
#include <dali/internal/event/actor-attachments/text-attachment-impl.h>
#include <dali/internal/event/actor-attachments/image-attachment-impl.h>
#include <dali/internal/event/actor-attachments/mesh-attachment-impl.h>

#include <dali/internal/event/animation/animation-impl.h>
#include <dali/internal/event/animation/animator-connector.h>
#include <dali/internal/event/animation/constraint-impl.h>
#include <dali/internal/event/animation/active-constraint-impl.h>
#include <dali/internal/update/animation/property-accessor.h>
#include <dali/internal/update/animation/scene-graph-animation.h>
#include <dali/internal/update/animation/scene-graph-constraint.h>


#include <dali/internal/event/images/image-impl.h>
#include <dali/internal/event/images/image-factory-cache.h>
#include <dali/internal/common/text-parameters.h>
#include <dali/internal/event/modeling/mesh-impl.h>
#include <dali/internal/event/modeling/material-impl.h>

#include <dali/internal/event/resources/resource-ticket.h>
#include <dali/internal/event/resources/image-ticket.h>

#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/internal/update/modeling/internal-mesh-data.h>
#include <dali/internal/update/modeling/scene-graph-animatable-mesh.h>
#include <dali/internal/update/modeling/scene-graph-material.h>
#include <dali/internal/update/modeling/scene-graph-mesh.h>

#include <dali/internal/update/node-attachments/node-attachment.h>
#include <dali/internal/update/node-attachments/scene-graph-camera-attachment.h>
#include <dali/internal/update/node-attachments/scene-graph-image-attachment.h>
#include <dali/internal/update/node-attachments/scene-graph-mesh-attachment.h>
#include <dali/internal/update/node-attachments/scene-graph-text-attachment.h>

#include <dali/internal/update/resources/bitmap-metadata.h>

#include <dali/internal/render/gl-resources/bitmap-texture.h>

#include <dali/internal/render/renderers/render-material.h>
#include <dali/internal/render/renderers/scene-graph-image-renderer.h>
#include <dali/internal/render/renderers/scene-graph-text-renderer.h>
#include <dali/internal/render/renderers/scene-graph-mesh-renderer.h>

using Dali::Internal::GestureEventProcessor;
using Dali::Internal::ThreadLocalStorage;

namespace Dali
{

namespace Integration
{

void EnableProfiling( ProfilingType type )
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();

  switch( type )
  {
    case PROFILING_TYPE_PAN_GESTURE:
    {
      eventProcessor.EnablePanGestureProfiling();
    }

    default:
    {
      // Do nothing
      break;
    }
  }
}

void SetPanGesturePredictionMode( int mode )
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGesturePredictionMode(mode);
}

void SetPanGesturePredictionAmount(unsigned int amount)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGesturePredictionAmount(amount);
}

void SetPanGestureSmoothingMode(int mode)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureSmoothingMode(mode);
}

void SetPanGestureSmoothingAmount( float amount )
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureSmoothingAmount(amount);
}

namespace Profiling
{

const int ANIMATION_MEMORY_SIZE(
  sizeof( Internal::Animation ) +
  sizeof( Internal::AnimatorConnector<float> ) +
  sizeof( Internal::SceneGraph::Animation ) );
const int CONSTRAINT_MEMORY_SIZE(
  sizeof( Internal::Constraint ) +
  sizeof( Internal::SceneGraph::Constraint<float, Internal::PropertyAccessor<float> > ) +
  sizeof( Internal::ActiveConstraint<float> ) );
const int ACTOR_MEMORY_SIZE(
  sizeof( Internal::Actor ) +
  sizeof( Internal::ActorAttachment ) +
  sizeof( Internal::SceneGraph::Node ) +
  sizeof( Internal::SceneGraph::NodeAttachment ));
const int CAMERA_ACTOR_MEMORY_SIZE(
  sizeof( Internal::CameraActor ) +
  sizeof( Internal::CameraAttachment ) +
  sizeof( Internal::SceneGraph::Node ) +
  sizeof( Internal::SceneGraph::CameraAttachment ) );
const int TEXT_ACTOR_MEMORY_SIZE(
  sizeof( Internal::TextActor ) +
  sizeof( Internal::TextAttachment ) +
  sizeof( Internal::SceneGraph::Node ) +
  sizeof( Internal::SceneGraph::TextAttachment ) +
  sizeof( Internal::TextParameters ) +
  sizeof( Internal::SceneGraph::TextRenderer ) );
const int MESH_ACTOR_MEMORY_SIZE(
  sizeof( Internal::MeshActor ) +
  sizeof( Internal::MeshAttachment ) +
  sizeof( Internal::SceneGraph::Node ) +
  sizeof( Internal::SceneGraph::MeshAttachment ) +
  sizeof( Internal::SceneGraph::MeshRenderer ) );
const int IMAGE_ACTOR_MEMORY_SIZE(
  sizeof( Internal::ImageActor ) +
  sizeof( Internal::ImageAttachment ) +
  sizeof( Internal::SceneGraph::Node ) +
  sizeof( Internal::SceneGraph::ImageAttachment ) +
  sizeof( Internal::SceneGraph::ImageRenderer ));
const int LAYER_MEMORY_SIZE(
  sizeof( Internal::Layer ) +
  sizeof( Internal::ActorAttachment ) +
  sizeof( Internal::SceneGraph::Layer ) +
  sizeof( Internal::SceneGraph::NodeAttachment ) );
const int IMAGE_MEMORY_SIZE(
  sizeof( Internal::Image ) +
  sizeof( Internal::ImageFactoryCache::Request ) +
  sizeof( Integration::Bitmap ) +
  sizeof( Internal::BitmapMetadata ) +
  sizeof( Internal::BitmapTexture ) +
  sizeof( Internal::ImageTicket ) );
const int MESH_MEMORY_SIZE(
  sizeof( Internal::Mesh ) +
  sizeof( Internal::MeshData ) +
  sizeof( Internal::SceneGraph::Mesh ) +
  sizeof( Internal::ResourceTicket ) );
const int MATERIAL_MEMORY_SIZE(
  sizeof( Internal::Material ) +
  sizeof( Internal::SceneGraph::Material ) +
  sizeof( Internal::SceneGraph::RenderMaterial ) );

} // namespace Profiling

} // namespace Integration

} // namespace Dali

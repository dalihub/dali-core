/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/actors/layer-impl.h>

#include <dali/internal/event/animation/animation-impl.h>
#include <dali/internal/event/animation/animator-connector.h>
#include <dali/internal/event/animation/constraint-impl.h>
#include <dali/internal/update/animation/property-accessor.h>
#include <dali/internal/update/animation/scene-graph-animation.h>
#include <dali/internal/update/animation/scene-graph-constraint.h>


#include <dali/internal/event/images/image-impl.h>

#include <dali/internal/event/resources/resource-ticket.h>
#include <dali/internal/event/resources/image-ticket.h>

#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>

#include <dali/internal/update/rendering/scene-graph-renderer.h>
#include <dali/internal/update/resources/texture-metadata.h>

#include <dali/internal/render/gl-resources/bitmap-texture.h>
#include <dali/internal/render/renderers/render-geometry.h>
#include <dali/internal/render/renderers/render-property-buffer.h>
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/render/renderers/render-sampler.h>
#include <dali/internal/update/render-tasks/scene-graph-camera.h>

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
      break;
    }
    case PROFILING_TYPE_END:
    {
      // nothing to do
      break;
    }
  }
}

namespace Profiling
{

const int ANIMATION_MEMORY_SIZE(
  sizeof( Internal::Animation ) +
  sizeof( Internal::AnimatorConnector<float> ) +
  sizeof( Internal::SceneGraph::Animation ) );
const int CONSTRAINT_MEMORY_SIZE(
  sizeof( Internal::Constraint<float> ) +
  sizeof( Internal::SceneGraph::Constraint<float, Internal::PropertyAccessor<float> > ) );
const int ACTOR_MEMORY_SIZE(
  sizeof( Internal::Actor ) +
  sizeof( Internal::SceneGraph::Node ) );
const int CAMERA_ACTOR_MEMORY_SIZE(
  sizeof( Internal::CameraActor ) +
  sizeof( Internal::SceneGraph::Node ) +
  sizeof( Internal::SceneGraph::Camera ) );
const int LAYER_MEMORY_SIZE(
  sizeof( Internal::Layer ) +
  sizeof( Internal::SceneGraph::Layer ) );
const int IMAGE_MEMORY_SIZE(
  sizeof( Internal::Image ) +
  sizeof( Integration::Bitmap ) +
  sizeof( Internal::TextureMetadata ) +
  sizeof( Internal::BitmapTexture ) +
  sizeof( Internal::ImageTicket ) );
const int RENDERER_MEMORY_SIZE(
  sizeof( Internal::Renderer ) +
  sizeof( Internal::SceneGraph::Renderer ) +
  sizeof( Internal::Render::Renderer ) );
const int GEOMETRY_MEMORY_SIZE(
  sizeof( Internal::Geometry ) +
  sizeof( Internal::Render::Geometry) );
const int PROPERTY_BUFFER_MEMORY_SIZE(
  sizeof( Internal::PropertyBuffer ) +
  sizeof( Internal::Render::PropertyBuffer ) );
const int TEXTURE_SET_MEMORY_SIZE(
  sizeof( Internal::TextureSet ) +
  sizeof( Internal::SceneGraph::TextureSet ) );
const int SAMPLER_MEMORY_SIZE(
  sizeof( Internal::Sampler ) +
  sizeof( Internal::Render::Sampler ) );
const int SHADER_MEMORY_SIZE(
  sizeof( Internal::Shader ) +
  sizeof( Internal::SceneGraph::Shader ) );

} // namespace Profiling

} // namespace Integration

} // namespace Dali

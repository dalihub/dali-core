/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/scene.h>

// INTERNAL INCLUDES
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/internal/event/common/scene-impl.h>

namespace Dali
{

namespace Integration
{

Scene Scene::New( const Size& size )
{
  Internal::ScenePtr internal = Internal::Scene::New( size );
  return Scene( internal.Get() );
}

Scene Scene::DownCast( BaseHandle handle )
{
  return Scene( dynamic_cast<Dali::Internal::Scene*>( handle.GetObjectPtr()) );
}

Scene::Scene()
{
}

Scene::~Scene()
{
}

Scene::Scene( const Scene& handle )
:BaseHandle(handle)
{
}

Scene::Scene( Internal::Scene* internal )
: BaseHandle(internal)
{
}

Scene& Scene::operator=( const Scene& rhs )
{
  BaseHandle::operator=(rhs);
  return *this;
}

void Scene::Add( Actor& actor )
{
  GetImplementation(*this).Add( GetImplementation(actor) );
}

void Scene::Remove( Actor& actor )
{
  GetImplementation(*this).Remove( GetImplementation(actor) );
}

Size Scene::GetSize() const
{
  return GetImplementation(*this).GetSize();
}

void Scene::SetDpi( Vector2 dpi )
{
  GetImplementation(*this).SetDpi( dpi );
}

Vector2 Scene::GetDpi() const
{
  return GetImplementation(*this).GetDpi();
}

RenderTaskList Scene::GetRenderTaskList() const
{
  return RenderTaskList( &GetImplementation(*this).GetRenderTaskList() );
}

Layer Scene::GetRootLayer() const
{
  return GetImplementation(*this).GetRootLayer();
}

uint32_t Scene::GetLayerCount() const
{
  return GetImplementation(*this).GetLayerCount();
}

Layer Scene::GetLayer( uint32_t depth ) const
{
  return GetImplementation(*this).GetLayer( depth );
}

void Scene::SetSurface( Integration::RenderSurface& surface )
{
  GetImplementation(*this).SetSurface( surface );
}

Integration::RenderSurface* Scene::GetSurface() const
{
  return GetImplementation(*this).GetSurface();
}

void Scene::QueueEvent( const Integration::Event& event )
{
  GetImplementation(*this).QueueEvent( event );
}

void Scene::ProcessEvents()
{
  GetImplementation(*this).ProcessEvents();
}

Scene::EventProcessingFinishedSignalType& Scene::EventProcessingFinishedSignal()
{
  return GetImplementation(*this).EventProcessingFinishedSignal();
}

Scene::KeyEventSignalType& Scene::KeyEventSignal()
{
  return GetImplementation(*this).KeyEventSignal();
}

Scene::TouchSignalType& Scene::TouchSignal()
{
  return GetImplementation(*this).TouchSignal();
}

Scene::WheelEventSignalType& Scene::WheelEventSignal()
{
  return GetImplementation(*this).WheelEventSignal();
}

} // Integration

} // Dali

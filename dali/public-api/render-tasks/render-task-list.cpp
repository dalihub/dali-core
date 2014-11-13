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
#include <dali/public-api/render-tasks/render-task-list.h>

// INTERNAL INCLUDES
#include <dali/internal/event/render-tasks/render-task-list-impl.h>

namespace Dali
{

RenderTaskList::RenderTaskList()
{
}

RenderTaskList RenderTaskList::DownCast( BaseHandle handle )
{
  return RenderTaskList( dynamic_cast<Dali::Internal::RenderTaskList*>( handle.GetObjectPtr() ) );
}

RenderTaskList::~RenderTaskList()
{
}

RenderTaskList::RenderTaskList(const RenderTaskList& handle)
: BaseHandle(handle)
{
}

RenderTaskList& RenderTaskList::operator=(const RenderTaskList& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

RenderTask RenderTaskList::CreateTask()
{
  return GetImplementation(*this).CreateTask();
}

void RenderTaskList::RemoveTask( RenderTask task )
{
  GetImplementation(*this).RemoveTask( task );
}

unsigned int RenderTaskList::GetTaskCount() const
{
  return GetImplementation(*this).GetTaskCount();
}

RenderTask RenderTaskList::GetTask( unsigned int index ) const
{
  return GetImplementation(*this).GetTask( index );
}

RenderTaskList::RenderTaskList( Internal::RenderTaskList* internal )
: BaseHandle( internal )
{
}

} // namespace Dali


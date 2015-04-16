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
#include <dali/internal/event/dynamics/dynamics-shape-impl.h>

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-shape.h>

namespace Dali
{

namespace Internal
{


DynamicsShape::DynamicsShape(const Dali::DynamicsShape::ShapeType type)
: mType(type),
  mDynamicsShape(NULL)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - (type: %d)\n", __PRETTY_FUNCTION__, static_cast<int>(type));
}

DynamicsShape::~DynamicsShape()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);
  if( mDynamicsShape && Stage::IsInstalled() )
  {
    DeleteShapeMessage( *( Stage::GetCurrent() ), *mDynamicsShape );
  }
}

const Dali::DynamicsShape::ShapeType DynamicsShape::GetType() const
{
  return mType;
}

} // namespace Internal

} // namespace Dali

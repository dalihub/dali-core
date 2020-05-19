/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

#if defined(DEBUG_ENABLED)
#include <sstream>
#endif

// CLASS HEADER
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/events/multi-point-event-util.h>

namespace Dali
{

namespace Internal
{

#if defined(DEBUG_ENABLED)

static bool HIERARCHY_GEOMETRY(true);
static bool HIERARCHY_SENSITIVITY(true);
static bool HIERARCHY_TOUCH_REQUIRED(true);
static bool HIERARCHY_HOVER_REQUIRED(true);
static bool HIERARCHY_HITTABLE(true);

static const Debug::LogLevel HIERARCHY_DEBUG_LOG_LEVEL( Debug::Verbose );

void PrintChildren( Debug::Filter* logFilter, Dali::Actor actor, int level )
{
  std::ostringstream output;

  for ( int t = 0; t < level; ++t )
  {
    output << " | ";
  }

  output << actor.GetProperty< std::string >( Dali::Actor::Property::NAME ) << "(" << actor.GetTypeName() << ", " << actor.GetObjectPtr() << ")";

  if ( HIERARCHY_GEOMETRY )
  {
    output << " Pos: " << actor.GetCurrentProperty< Vector3 >( Dali::Actor::Property::WORLD_POSITION ) << " Size: " << actor.GetCurrentProperty< Vector3 >( Dali::Actor::Property::SIZE ) << " Scale: " << actor.GetCurrentProperty< Vector3 >( Dali::Actor::Property::WORLD_SCALE );
  }

  if ( HIERARCHY_SENSITIVITY )
  {
    output << " Sensitivity: " << ( IsActuallySensitive( &GetImplementation( actor ) ) ? "True " : "False " );
  }

  if ( HIERARCHY_TOUCH_REQUIRED )
  {
    output << " TouchRequired: " << ( GetImplementation(actor).GetTouchRequired() ? "True " : "False " );
  }

  if ( HIERARCHY_HOVER_REQUIRED )
  {
    output << " HoverRequired: " << ( GetImplementation(actor).GetHoverRequired() ? "True " : "False " );
  }

  if ( HIERARCHY_HITTABLE )
  {
    output << " Hittable: " << ( GetImplementation(actor).IsHittable() ? "True " : "False " );
  }

  output << std::endl;

  if( logFilter )
  {
    DALI_LOG_INFO( logFilter, HIERARCHY_DEBUG_LOG_LEVEL, output.str().c_str() );
  }

  ++level;
  unsigned int numChildren=actor.GetChildCount();
  for ( unsigned int i=0; i<numChildren; ++i )
  {
    PrintChildren( logFilter, actor.GetChildAt(i), level );
  }
  --level;
}

void PrintHierarchy( Debug::Filter* logFilter )
{
  if ( logFilter && logFilter->IsEnabledFor( HIERARCHY_DEBUG_LOG_LEVEL ) )
  {
    PrintChildren( logFilter, Dali::Stage().GetCurrent().GetRootLayer(), 0 );
  }
}

#endif // defined(DEBUG_ENABLED)

bool IsActuallySensitive( Actor* actor )
{
  bool sensitive = true;

  while ( actor && sensitive )
  {
    sensitive = actor->IsSensitive();
    actor = actor->GetParent();
  }

  return sensitive;
}

} // namespace Internal

} // namespace Dali


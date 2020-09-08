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

// CLASS HEADER
#include <dali/internal/event/events/actor-gesture-data.h>

namespace Dali
{

namespace Internal
{

ActorGestureData::ActorGestureData()
: gesturesRequired( GestureType::Value( 0 ) ),
  panDetectors( nullptr ),
  pinchDetectors( nullptr ),
  longPressDetectors( nullptr ),
  tapDetectors( nullptr ),
  rotationDetectors( nullptr )
{
}

ActorGestureData::~ActorGestureData()
{
  delete panDetectors;
  delete pinchDetectors;
  delete longPressDetectors;
  delete tapDetectors;
  delete rotationDetectors;
}

void ActorGestureData::AddGestureDetector( GestureDetector& detector )
{
  const GestureType::Value type( detector.GetType() );

  GestureDetectorContainer*& containerPtr( GetContainerPtr( type ) );
  if ( nullptr == containerPtr )
  {
    containerPtr = new GestureDetectorContainer;
  }
  containerPtr->push_back( &detector );

  gesturesRequired = GestureType::Value( gesturesRequired | type );
}

void ActorGestureData::RemoveGestureDetector( GestureDetector& detector )
{
  const GestureType::Value type( detector.GetType() );

  GestureDetectorContainer*& containerPtr( GetContainerPtr( type ) );
  DALI_ASSERT_DEBUG( containerPtr && "Container had not been created" );

  GestureDetectorContainer& container( *containerPtr );
  GestureDetectorContainer::iterator match( std::remove( container.begin(), container.end(), &detector ) );
  DALI_ASSERT_DEBUG( match != container.end() && "Actor does not have the detector" );
  container.erase( match, container.end() );

  if ( container.empty() )
  {
    gesturesRequired = GestureType::Value( gesturesRequired & ~type );
    delete containerPtr;
    containerPtr = nullptr;
  }
}

GestureDetectorContainer& ActorGestureData::GetGestureDetectorContainer( GestureType::Value type )
{
  return *GetContainerPtr( type );
}

GestureDetectorContainer*& ActorGestureData::GetContainerPtr( GestureType::Value type )
{
  switch ( type )
  {
    case GestureType::PAN:
    {
      return panDetectors;
    }

    case GestureType::PINCH:
    {
      return pinchDetectors;
    }

    case GestureType::LONG_PRESS:
    {
      return longPressDetectors;
    }

    case GestureType::TAP:
    {
      return tapDetectors;
    }

    case GestureType::ROTATION:
    {
      return rotationDetectors;
    }
  }

  DALI_ASSERT_DEBUG( ! "Invalid Type" );
  static GestureDetectorContainer* invalidType( nullptr );
  return invalidType;
}

} // namespace Internal

} // namespace Dali


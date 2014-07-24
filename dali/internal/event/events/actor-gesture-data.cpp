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
#include <dali/internal/event/events/actor-gesture-data.h>

namespace Dali
{

namespace Internal
{

ActorGestureData::ActorGestureData()
: gesturesRequired( Gesture::Type( 0 ) ),
  panDetectors( NULL ),
  pinchDetectors( NULL ),
  longPressDetectors( NULL ),
  tapDetectors( NULL )
{
}

ActorGestureData::~ActorGestureData()
{
  delete panDetectors;
  delete pinchDetectors;
  delete longPressDetectors;
  delete tapDetectors;
}

void ActorGestureData::AddGestureDetector( GestureDetector& detector )
{
  const Gesture::Type type( detector.GetType() );

  GestureDetectorContainer*& containerPtr( GetContainerPtr( type ) );
  if ( NULL == containerPtr )
  {
    containerPtr = new GestureDetectorContainer;
  }
  containerPtr->push_back( &detector );

  gesturesRequired = Gesture::Type( gesturesRequired | type );
}

void ActorGestureData::RemoveGestureDetector( GestureDetector& detector )
{
  const Gesture::Type type( detector.GetType() );

  GestureDetectorContainer*& containerPtr( GetContainerPtr( type ) );
  DALI_ASSERT_DEBUG( containerPtr && "Container had not been created" );

  GestureDetectorContainer& container( *containerPtr );
  GestureDetectorContainer::iterator match( std::remove( container.begin(), container.end(), &detector ) );
  DALI_ASSERT_DEBUG( match != container.end() && "Actor does not have the detector" );
  container.erase( match, container.end() );

  if ( container.empty() )
  {
    gesturesRequired = Gesture::Type( gesturesRequired & ~type );
    delete containerPtr;
    containerPtr = NULL;
  }
}

GestureDetectorContainer& ActorGestureData::GetGestureDetectorContainer( Gesture::Type type )
{
  return *GetContainerPtr( type );
}

GestureDetectorContainer*& ActorGestureData::GetContainerPtr( Gesture::Type type )
{
  switch ( type )
  {
    case Gesture::Pan:
    {
      return panDetectors;
    }

    case Gesture::Pinch:
    {
      return pinchDetectors;
    }

    case Gesture::LongPress:
    {
      return longPressDetectors;
    }

    case Gesture::Tap:
    {
      return tapDetectors;
    }
  }

  DALI_ASSERT_DEBUG( ! "Invalid Type" );
  static GestureDetectorContainer* invalidType( NULL );
  return invalidType;
}

} // namespace Internal

} // namespace Dali


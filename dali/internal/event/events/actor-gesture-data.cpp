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

namespace
{

/**
 * Template to add a detector to the appropriate container. Dynamically allocates the container only if it is used.
 */
template< typename DetectorType, typename ContainerType >
inline void AddDetector( ContainerType*& containerPtr, GestureDetector* detector, Gesture::Type& gesturesRequired )
{
  if ( NULL == containerPtr )
  {
    containerPtr = new ContainerType;
  }

  containerPtr->push_back( static_cast< DetectorType* >( detector ) );
  gesturesRequired = Gesture::Type( gesturesRequired | detector->GetType() );
}

/**
 * Template to remove a detector from the appropriate container. Deletes the container if it is no longer required.
 */
template< typename ContainerType >
inline void RemoveDetector( ContainerType*& containerPtr, GestureDetector* detector, Gesture::Type& gesturesRequired )
{
  if ( NULL != containerPtr )
  {
    ContainerType& container( *containerPtr );
    typename ContainerType::iterator match( std::remove( container.begin(), container.end(), detector ) );
    DALI_ASSERT_DEBUG( match != container.end() && "Actor does not have the detector" );
    container.erase( match, container.end() );

    if ( container.empty() )
    {
      gesturesRequired = Gesture::Type( gesturesRequired & ~detector->GetType() );
      delete containerPtr;
      containerPtr = NULL;
    }
  }
}

} // unnamed namespace

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
  switch ( type )
  {
    case Gesture::Pan:
    {
      AddDetector< PanGestureDetector, PanGestureDetectorContainer >( panDetectors, &detector, gesturesRequired );
      break;
    }

    case Gesture::Pinch:
    {
      AddDetector< PinchGestureDetector, PinchGestureDetectorContainer >( pinchDetectors, &detector, gesturesRequired );
      break;
    }

    case Gesture::LongPress:
    {
      AddDetector< LongPressGestureDetector, LongPressGestureDetectorContainer >( longPressDetectors, &detector, gesturesRequired );
      break;
    }

    case Gesture::Tap:
    {
      AddDetector< TapGestureDetector, TapGestureDetectorContainer >( tapDetectors, &detector, gesturesRequired );
      break;
    }
  }
}

void ActorGestureData::RemoveGestureDetector( GestureDetector& detector )
{
  switch ( detector.GetType() )
  {
    case Gesture::Pan:
    {
      RemoveDetector< PanGestureDetectorContainer >( panDetectors, &detector, gesturesRequired );
      break;
    }

    case Gesture::Pinch:
    {
      RemoveDetector< PinchGestureDetectorContainer >( pinchDetectors, &detector, gesturesRequired );
      break;
    }

    case Gesture::LongPress:
    {
      RemoveDetector< LongPressGestureDetectorContainer >( longPressDetectors, &detector, gesturesRequired );
      break;
    }

    case Gesture::Tap:
    {
      RemoveDetector< TapGestureDetectorContainer >( tapDetectors, &detector, gesturesRequired );
      break;
    }
  }
}

} // namespace Internal

} // namespace Dali


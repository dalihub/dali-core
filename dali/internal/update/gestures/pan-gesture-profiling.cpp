//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali/internal/update/gestures/pan-gesture-profiling.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Internal
{

void PanGestureProfiling::PrintData() const
{
  PrintData( mRawData,      "RAW" );
  PrintData( mLatestData,   "LATEST" );
  PrintData( mAveragedData, "AVERAGED" );
}

void PanGestureProfiling::PrintData( const PanPositionContainer& dataContainer, const char * const prefix ) const
{
  const PanPositionContainer::const_iterator endIter = dataContainer.end();
  for ( PanPositionContainer::const_iterator iter = dataContainer.begin(); iter != endIter; ++iter )
  {
    DALI_LOG_UPDATE_STATUS( "%s, %u, %.2f, %.2f\n", prefix, iter->time, iter->position.x, iter->position.y );
  }
}

void PanGestureProfiling::ClearData()
{
  mRawData.clear();
  mLatestData.clear();
  mAveragedData.clear();
}

} // namespace Internal

} // namespace Dali

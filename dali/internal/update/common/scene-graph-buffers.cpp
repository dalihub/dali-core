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
#include <dali/internal/update/common/scene-graph-buffers.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

BufferIndex SceneGraphBuffers::INITIAL_EVENT_BUFFER_INDEX  = 0u;
BufferIndex SceneGraphBuffers::INITIAL_UPDATE_BUFFER_INDEX = 1u;

SceneGraphBuffers::SceneGraphBuffers()
: mEventBufferIndex(INITIAL_EVENT_BUFFER_INDEX),
  mUpdateBufferIndex(INITIAL_UPDATE_BUFFER_INDEX)
{
}

SceneGraphBuffers::~SceneGraphBuffers()
{
}

void SceneGraphBuffers::Swap()
{
  mUpdateBufferIndex = __sync_fetch_and_xor( &mEventBufferIndex, 1 );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

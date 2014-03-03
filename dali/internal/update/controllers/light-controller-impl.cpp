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
#include <dali/internal/update/controllers/light-controller-impl.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/update/nodes/node.h>

#ifdef DEBUG_ENABLED
#include <dali/internal/update/node-attachments/scene-graph-light-attachment.h>
#endif


namespace Dali
{

namespace Internal
{

namespace SceneGraph
{
namespace
{
const unsigned int MAX_ACTIVE_LIGHTS = 3;
}

LightControllerImpl::LightControllerImpl()
{

}
LightControllerImpl::~LightControllerImpl()
{

}

void LightControllerImpl::AddLight( Node& lightNode )
{
  if (mLights.Count() < MAX_ACTIVE_LIGHTS)
  {
    NodeIter iter;
    for (iter = mLights.Begin(); iter !=  mLights.End(); ++iter)
    {
      Node& iterNode = **iter;
      if ( &iterNode == &lightNode )
      {
        break;
      }
    }
    if (iter == mLights.End())
    {
      mLights.PushBack( &lightNode );
#ifdef DEBUG_ENABLED
      LightAttachment& light = dynamic_cast<LightAttachment&>(lightNode.GetAttachment());
      DALI_LOG_INFO(Debug::Filter::gRender, Debug::Concise, "LightControllerImpl::AddActiveLight('%s')\n", light.GetName().c_str());
#endif
    }
  }
}

void LightControllerImpl::RemoveLight( Node& lightNode )
{
  for (NodeIter iter = mLights.Begin(); iter !=  mLights.End(); ++iter)
  {
    Node &iterNode = **iter;
    if ( &iterNode == &lightNode )
    {
#ifdef DEBUG_ENABLED
      LightAttachment& light = dynamic_cast<LightAttachment&>(lightNode.GetAttachment());
      DALI_LOG_INFO(Debug::Filter::gRender, Debug::Concise, "LightControllerImpl::RemoveActiveLight('%s')\n", light.GetName().c_str());
#endif

      mLights.Remove(iter); // order of lights does not matter
      return;
    }
  }
}

Node& LightControllerImpl::GetLight( const unsigned int lightIndex ) const
{
  return *(mLights[ lightIndex ]);
}

unsigned int LightControllerImpl::GetNumberOfLights() const
{
  return mLights.Count();
}




} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

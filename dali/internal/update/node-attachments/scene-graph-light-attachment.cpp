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
#include <dali/internal/update/node-attachments/scene-graph-light-attachment.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/controllers/scene-controller.h>
#include <dali/internal/update/resources/resource-manager.h>
#include <dali/integration-api/debug.h>

using namespace std;

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

LightAttachment::LightAttachment()
: NodeAttachment(),
  mLight(),
  mUpdateRequired(false),
  mLightController(NULL)
{
}

void LightAttachment::ConnectToSceneGraph( SceneController& sceneController, BufferIndex updateBufferIndex )
{
  mLightController = &sceneController.GetLightController();
}

void LightAttachment::OnDestroy()
{
  // do nothing
}

LightAttachment::~LightAttachment()
{
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

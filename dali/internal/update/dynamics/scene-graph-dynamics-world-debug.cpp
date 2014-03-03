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
#include "scene-graph-dynamics-world-debug.h"

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/update/controllers/scene-controller.h>
#include <dali/internal/render/dynamics/scene-graph-dynamics-debug-renderer.h>
#include <dali/internal/render/shaders/shader.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

DynamicsWorldDebug::DynamicsWorldDebug(SceneController& sceneController, const Shader& debugShader)
: mDebugMode(0),//(btIDebugDraw::DBG_DrawWireframe),//(0),
  mSceneController(sceneController)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);
  mRenderer = new DynamicsDebugRenderer( debugShader );
}

DynamicsWorldDebug::~DynamicsWorldDebug()
{
  // TODO Auto-generated destructor stub
}

//void DynamicsWorldDebug::drawLine(const btVector3& from,const btVector3& to,const btVector3& color)
//{
////  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);
//
//  DynamicsDebugVertex vertex;
//  vertex.position = Vector3( from.getX(), from.getY(), from.getZ() );
//  vertex.color = Vector4( color.getX(), color.getY(), color.getZ(), 1.0f );
//  mVertices.push_back( vertex );
//  vertex.position = Vector3( to.getX(), to.getY(), to.getZ());
//  mVertices.push_back( vertex );
//}
//
//void DynamicsWorldDebug::drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color)
//{
//  btVector3 to = PointOnB + (normalOnB * distance);// * 10.0f;
//
//  drawLine(PointOnB, to, color);
//}
//
//void DynamicsWorldDebug::reportErrorWarning(const char* warningString)
//{
//  DALI_LOG_ERROR( "Dynamics error: %s\n", warningString );
//}
//
//void DynamicsWorldDebug::draw3dText(const btVector3& location,const char* textString)
//{
//}

void DynamicsWorldDebug::setDebugMode(int debugMode)
{
  mDebugMode = debugMode;
}

int DynamicsWorldDebug::getDebugMode() const
{
  return mDebugMode;
}

void DynamicsWorldDebug::UpdateMatrices(BufferIndex bufferIndex, const Matrix& projectionMatrix, const Matrix& viewMatrix)
{
  mSceneController.GetRenderQueue().QueueMessage(bufferIndex, UpdateMatricesMessage(GetRenderer(), bufferIndex, projectionMatrix, viewMatrix) );
}

void DynamicsWorldDebug::StartFrame()
{
  mVertices.clear();
}

void DynamicsWorldDebug::EndFrame(BufferIndex bufferIndex)
{
  mSceneController.GetRenderQueue().QueueMessage(bufferIndex, UpdateVerticesMessage(GetRenderer(), mVertices) );
}


const DynamicsDebugVertexContainer& DynamicsWorldDebug::GetVertices() const
{
  return mVertices;
}

DynamicsDebugRenderer& DynamicsWorldDebug::GetRenderer() const
{
  return *mRenderer;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

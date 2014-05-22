#ifndef __SCENE_GRAPH_DYNAMICS_WORLD_DEBUG_H__
#define __SCENE_GRAPH_DYNAMICS_WORLD_DEBUG_H__

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


// INTERNAL HEADERS
#include <dali/internal/update/dynamics/scene-graph-dynamics-debug-vertex.h>

namespace Dali
{

class Matrix;

namespace Internal
{

namespace SceneGraph
{

class DynamicsDebugRenderer;
class SceneController;
class Shader;

/**
 * Debug draw class, Physics engine will invoke methods on this object to render debug information
 */
class DynamicsWorldDebug// : public btIDebugDraw
{
public:
  /**
   * @param[in] sceneController Allows access to render manager message queue
   */
  DynamicsWorldDebug(SceneController& sceneController, const Shader& debugShader );
  virtual ~DynamicsWorldDebug();

//  virtual void drawLine(const btVector3& from,const btVector3& to,const btVector3& color);
//  virtual void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);
//  virtual void reportErrorWarning(const char* warningString);
//  virtual void draw3dText(const btVector3& location,const char* textString);
  virtual void setDebugMode(int debugMode);
  virtual int  getDebugMode() const;

  void UpdateMatrices(BufferIndex bufferIndex, const Matrix& projectionMatrix, const Matrix& viewMatrix);
  void StartFrame();
  void EndFrame(BufferIndex bufferIndex);
  const DynamicsDebugVertexContainer& GetVertices() const;
  DynamicsDebugRenderer& GetRenderer() const;
private:
  int                          mDebugMode;
  SceneController&             mSceneController;
  DynamicsDebugRenderer*       mRenderer;
  DynamicsDebugVertexContainer mVertices;
}; // class DynamicsWorldDebug

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif /* __SCENE_GRAPH_DYNAMICS_WORLD_DEBUG_H__ */

#ifndef __SCENE_GRAPH_DYNAMICS_DEBUG_RENDERER_H__
#define __SCENE_GRAPH_DYNAMICS_DEBUG_RENDERER_H__

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
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/message.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/render/gl-resources/gpu-buffer.h>
#include <dali/integration-api/dynamics/dynamics-debug-vertex.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/math/matrix.h>

namespace Dali
{

namespace Internal
{

class Context;

namespace SceneGraph
{

class Shader;

/**
 * Dynamics world debug renderer
 * Renderers an array of colored line sections
 */
class DynamicsDebugRenderer
{
public:

  DynamicsDebugRenderer(const Shader& debugShader);
  virtual ~DynamicsDebugRenderer();

  void Initialize(Context& context);
  void Render();
  void UpdateMatrices( BufferIndex bufferIndex, const Matrix& projectionMatrix, const Matrix& viewMatrix );
  void UpdateBuffer( const Integration::DynamicsDebugVertexContainer& vertices );

private:
  Shader& mShader;
  Context* mContext;
  BufferIndex mBufferIndex;
  Matrix mViewMatrix;
  Matrix mProjectionMatrix;
  OwnerPointer<GpuBuffer> mBuffer;
  int mNumberOfPoints;
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif /* __SCENE_GRAPH_DYNAMICS_DEBUG_RENDERER_H__ */

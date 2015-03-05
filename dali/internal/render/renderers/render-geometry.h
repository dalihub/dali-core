#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDER_GEOMETRY_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDER_GEOMETRY_H

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
 */

#include <dali/internal/common/buffer-index.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

/**
 * This class encapsulates the GPU buffers. It is used to upload vertex data
 * to it's GPU buffers, to bind all the buffers and to setup/teardown vertex attribute
 * bindings
 */
class RenderGeometry
{
public:
  typedef OwnerContainer< GPUBuffer* > GPUBuffers;

  /**
   * Constructor
   */
  RenderGeometry();

  /**
   * Destructor
   */

  void UploadVertexData();

  void BindBuffers();

  void EnableVertexAttributes( Program& progam );

  void DisableVertexAttributes( Program& program );

private:
  GPUBuffers mGpuBuffers;
  AttributeDataProvider& mAttributeDataProvider;
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_SAMPLER_DATA_PROVIDER_H

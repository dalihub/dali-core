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
#include <dali/internal/common/owner-container.h>
#include <dali/internal/common/owner-pointer.h>

namespace Dali
{
namespace Internal
{
class Context;
class Program;
class GpuBuffer;

namespace SceneGraph
{
class GeometryDataProvider;

/**
 * This class encapsulates the GPU buffers. It is used to upload vertex data
 * to it's GPU buffers, to bind all the buffers and to setup/teardown vertex attribute
 * bindings
 */
class RenderGeometry
{
public:
  typedef OwnerContainer< GpuBuffer* > GpuBuffers;

  /**
   * Constructor. Creates a render geometry object with no GPU buffers.
   */
  RenderGeometry( );

  /**
   * Destructor
   */
  ~RenderGeometry();

  /**
   * Called on Gl Context created
   */
  void GlContextCreated( Context& context );

  /**
   * Called on Gl Context destroyed.
   */
  void GlContextDestroyed();

  /**
   * Upload the geometry if it has changed, set up the attributes and perform
   * the Draw call corresponding to the geometry type
   * @param[in] context The GL context
   * @param[in] program The shader program to query for attribute locations
   * @param[in] bufferIndex The current buffer index
   * @param[in] geometryDataProvider The geometry data provider (to fetch geometry from)
   */
  void UploadAndDraw(Context* context,
                     Program& program,
                     BufferIndex bufferIndex,
                     const GeometryDataProvider& geometryDataProvider );

  /**
   * Tell the object that the geometry has been updated.
   * It will be uploaded on the next UploadAndDraw call.
   */
  void GeometryUpdated();

  /**
   * Upload the vertex data if it needs uploading.
   * @param[in] context The GL context
   * @param[in] bufferIndex The current buffer index
   * @param[in] geometryDataProvider The geometry data provider (to fetch geometry from)
   */
  void UploadVertexData( Context* context,
                         BufferIndex bufferIndex,
                         const GeometryDataProvider& geometryDataProvider );


private:
  /**
   * Perform the upload of the geometry
   * @param[in] context The GL context
   * @param[in] bufferIndex The current buffer index
   * @param[in] geometryDataProvider The geometry data provider (to fetch geometry from)
   */
  void DoUpload( Context* context,
                 BufferIndex bufferIndex,
                 const GeometryDataProvider& geometryDataProvider );

  /**
   * Bind the geometry buffers
   */
  void BindBuffers();

  /**
   * Enable the vertex attributes for each vertex buffer from the corresponding
   * shader program.
   * @param[in] context The GL context
   * @param[in] program The shader program to query for attribute locations
   */
  void EnableVertexAttributes( Context* context, Program& progam );

  /**
   * Disable the vertex attributes for each vertex buffer from the corresponding
   * shader program.
   * @param[in] context The GL context
   * @param[in] program The shader program to query for attribute locations
   */
  void DisableVertexAttributes( Context* context, Program& program );

  /**
   * Perform the correct draw call corresponding to the geometry type
   * @param[in] context The GL context
   * @param[in] bufferIndex The current buffer index
   * @param[in] geometryDataProvider The geometry data provider (to fetch geometry from)
   */
  void Draw( Context* context,
             BufferIndex bufferIndex,
             const GeometryDataProvider& geometry );

private:
  GpuBuffers mVertexBuffers;
  OwnerPointer< GpuBuffer > mIndexBuffer;

  bool mDataNeedsUploading;
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_SAMPLER_DATA_PROVIDER_H

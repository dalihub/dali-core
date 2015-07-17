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

#include <dali/public-api/common/dali-vector.h>
#include <dali/integration-api/gl-defines.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/owner-container.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/render/data-providers/render-data-provider.h>
#include <dali/internal/render/renderers/render-renderer-property-buffer.h>

namespace Dali
{
namespace Internal
{
class Context;
class Program;
class GpuBuffer;

namespace SceneGraph
{
class RenderDataProvider;
class GeometryDataProvider;

typedef OwnerPointer< RenderPropertyBuffer > RenderPropertyBufferPtr;
typedef OwnerContainer< RenderPropertyBuffer* > RenderPropertyBufferContainer;
typedef RenderPropertyBufferContainer::Iterator RenderPropertyBufferIter;


/**
 * This class encapsulates the GPU buffers. It is used to upload vertex data
 * to it's GPU buffers, to bind all the buffers and to setup/teardown vertex attribute
 * bindings
 */
class RenderGeometry
{
public:

  /**
   * Constructor. Creates a render geometry object with no GPU buffers.
   */
  RenderGeometry( const GeometryDataProvider& geometryDataProvider );

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
   * Adds a property buffer to the geometry
   * @param[in] dataProvider The PropertyBuffer data provider
   * @param[in] gpuBufferTarget target Specifies the type of the buffer
   * @param[in] gpuBufferUsage usage Specifies how will the buffer be used
   */
  void AddPropertyBuffer( const PropertyBufferDataProvider* dataProvider, GpuBuffer::Target gpuBufferTarget, GpuBuffer::Usage gpuBufferUsage );

  /**
   * Removes a PropertyBuffer from the geometry
   * @param[in] dataProvider The property buffer to be removed
   */
  void RemovePropertyBuffer(  const PropertyBufferDataProvider* dataProvider );

  /**
   * Gets the attribute locations on the shader for the attributes defined in the geometry RenderBuffers
   * @param[out] attributeLocation The vector where the attributes locations will be stored
   * @param[in] program The program
   * @param[in] bufferIndex The current buffer index
   */
  void GetAttributeLocationFromProgram( Vector<GLint>& attributeLocation, Program& program, BufferIndex bufferIndex ) const;

  /**
   * Called from RenderManager to notify the geometry that current rendering pass has finished.
   */
  void OnRenderFinished();

  /**
   * Chack if the attributes for the geometry have changed
   * @return True if vertex buffers have been added or removed since last frame, false otherwise
   */
  bool AttributesChanged() const
  {
    return mAttributesChanged;
  }

  /**
   * Upload the geometry if it has changed, set up the attributes and perform
   * the Draw call corresponding to the geometry type
   * @param[in] context The GL context
   * @param[in] bufferIndex The current buffer index
   * @param[in] attributeLocation The location for the attributes in the shader
   */
  void UploadAndDraw(Context& context,
                     BufferIndex bufferIndex,
                     Vector<GLint>& attributeLocation );

private:

  const GeometryDataProvider& mGeometryDataProvider;  //Reference to update thread object

  // PropertyBuffers
  RenderPropertyBufferPtr       mIndexBuffer;
  RenderPropertyBufferContainer mVertexBuffers;

  // Booleans
  bool mHasBeenUpdated : 1;
  bool mAttributesChanged : 1;
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_SAMPLER_DATA_PROVIDER_H

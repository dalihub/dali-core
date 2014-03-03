#ifndef __DALI_INTERNAL_SCENE_GRAPH_TEXT_RENDERER_H__
#define __DALI_INTERNAL_SCENE_GRAPH_TEXT_RENDERER_H__

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

// INTERNAL INCLUDES
#include <dali/internal/common/message.h>
#include <dali/internal/render/shaders/custom-uniform.h>
#include <dali/internal/render/gl-resources/gpu-buffer.h>
#include <dali/internal/event/effects/shader-declarations.h>
#include <dali/internal/render/renderers/scene-graph-renderer.h>
#include <dali/internal/render/gl-resources/texture-observer.h>
#include <dali/internal/update/resources/resource-manager-declarations.h>
#include <dali/internal/common/text-vertex-buffer.h>

namespace Dali
{

namespace Internal
{

class TextParameters;

namespace SceneGraph
{

class RenderDataProvider;
class TextureCache;

/**
 * Renders text from a font atlas.
 */
class TextRenderer : public Renderer, public TextureObserver
{
public:

  /**
   * Construct a new TextRenderer.
   * @param dataprovider to render
   * @return The newly allocated TextRenderer
   */
  static TextRenderer* New( RenderDataProvider& dataprovider );

  /**
   * Virtual destructor
   */
  virtual ~TextRenderer();

  /**
   * Set the vertex data used for rendering the text.
   * Deletes the data once it has been uploaded to GL.
   * @param[in] vertexData The vertex data.
   */
  void SetVertexData( TextVertexBuffer* vertexData );

  /**
   * @copydoc Dali::TextActor::SetGradientColor()
   */
  void SetGradientColor( const Vector4& color );

  /**
   * @copydoc Dali::TextActor::SetGradientStartPoint()
   */
  void SetGradientStartPoint( const Vector2& position );

  /**
   * @copydoc Dali::TextActor::SetGradientEndPoint()
   */
  void SetGradientEndPoint( const Vector2& position );

  /**
   * @copydoc Dali::TextActor::SetTextColor()
   */
  void SetTextColor( const Vector4& color );

  /**
   * @copydoc Dali::TextActor::SetOutline(const bool,const Vector4&,const Vector2&)
   */
  void SetOutline( bool enable, const Vector4& color, const Vector2& params );

  /**
   * @copydoc Dali::TextActor::SetGlow(const bool,const Vector4&,const float)
   */
  void SetGlow( bool enable, const Vector4& color, float params );

  /**
   * @copydoc Dali::TextActor::SetShadow(const bool,const Vector4&,const Vector2&,const float)
   */
  void SetDropShadow( bool enable, const Vector4& color, const Vector2& offset, const float size );

  /**
   * @copydoc Dali::TextActor::SetSmoothEdge(const float)
   */
  void SetSmoothEdge( float params );

  /**
   * @copydoc Dali::Internal::GlResourceOwner::GlCleanup()
   */
  virtual void GlCleanup();

  /**
   * @copydoc Dali::Internal::SceneGraph::Renderer::RequiresDepthTest()
   */
  virtual bool RequiresDepthTest() const;

  /**
   * @copydoc Dali::Internal::SceneGraph::Renderer::CheckResources()
   */
  virtual bool CheckResources();

  /**
   * @copydoc Dali::Internal::SceneGraph::Renderer::DoRender()
   */
  virtual void DoRender( BufferIndex bufferIndex, const Matrix& modelViewMatrix, const Matrix& modelMatrix, const Matrix& viewMatrix, const Matrix& projectionMatrix, const Vector4& color );

protected: // TextureObserver implementation
  /**
   * @copydoc Dali::Internal::TextureObserver::TextureDiscarded()
   */
  virtual void TextureDiscarded( ResourceId textureId );

private:
  /**
   * Set the texture used to render.
   * @param[in] textureId The id of the texture used to render.
   */
  void SetTextureId( ResourceId textureId );

  /**
   * Update the index buffer
   * @param size number of verts
   */
  void UpdateIndexBuffer( std::size_t size );

  /**
   * Allocate a TextParameters object on this TextRenderer if one does not exist
   */
  void AllocateTextParameters();

  /**
   * Private constructor. @see New()
   */
  TextRenderer( RenderDataProvider& dataprovider );

  // Undefined
  TextRenderer( const TextRenderer& copy );

  // Undefined
  TextRenderer& operator=(const TextRenderer& rhs);

private:

  ResourceId  mTextureId;
  Texture*    mTexture;

  OwnerPointer< GpuBuffer > mVertexBuffer;        ///< GPU Buffer containing Vertex coordinate (and texture coordinate) information
  OwnerPointer< GpuBuffer > mIndexBuffer;         ///< GPU Buffer containing Index information
  Vector2 mInvTextSize;                           ///< 1.0 / (2D size of Vertices in vertex buffer (max extent - min extent))

  OwnerPointer< TextParameters > mTextParameters; ///< Optional text parameters
  Vector4* mTextColor;
  float   mSmoothing;

};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_TEXT_RENDERER_H__

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

// CLASS HEADER
#include <dali/internal/render/renderers/render-image-renderer.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/common/internal-constants.h>
#include <dali/internal/render/common/performance-monitor.h>
#include <dali/internal/render/common/vertex.h>
#include <dali/internal/render/gl-resources/gpu-buffer.h>
#include <dali/internal/render/gl-resources/texture.h>
#include <dali/internal/render/gl-resources/texture-cache.h>
#include <dali/internal/render/gl-resources/texture-units.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>
#include <dali/internal/update/controllers/scene-controller.h>


namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gImageRenderFilter=Debug::Filter::New(Debug::NoLogging, false, "LOG_IMAGE_RENDERER");
#endif

/**
 * VertexToTextureCoord
 * Represents a mapping between a 1 dimensional vertex coordinate
 * and a 1 dimensional texture coordinate.
 */
struct VertexToTextureCoord
{
  /**
   * @param[in] xVertex Vertex coordinate
   * @param[in] uTexture Texture coordinate
   */
  VertexToTextureCoord(float xVertex, float uTexture)
  : x(xVertex),
    u(uTexture)
  {
  }

  float x;    ///< 1D Vertex position
  float u;    ///< 1D Texture position
};

/**
 * Generates a list of equally spaced intervals along a line, including
 * intervals at the points specified in insertionList.
 * The line starts from insertionList.begin() and ends at insertionList.end()-1
 * The number of intervals and spacing of these intervals is specified by
 * the caller.
 * @param[out] intervalList An empty vector to be populated with the list of intervals.
 * @param[in] intervals The number of intervals to be generated.
 * @param[in] insertionList A vector containing the points on the line to be inserted.
 */
void GenerateIntervals(std::vector<VertexToTextureCoord>& intervalList, int intervals, const std::vector<VertexToTextureCoord>& insertionList)
{
  DALI_ASSERT_DEBUG(insertionList.size() >= 2);
  DALI_ASSERT_DEBUG(intervals > 0);

  std::vector<VertexToTextureCoord>::const_iterator iter = insertionList.begin();
  if( iter != insertionList.end() )
  {
    std::vector<VertexToTextureCoord>::const_iterator end = insertionList.end()-1;

    const float length = end->x - iter->x;
    const float intervalSize = length / static_cast<float>(intervals);
    float x = iter->x;

    for(;iter!=end;++iter)
    {
      float x0 = iter[0].x;
      float u0 = iter[0].u;
      float x1 = iter[1].x;
      float u1 = iter[1].u;

      for(;x<x1;x+=intervalSize)
      {
        float progress = (x - x0) / (x1 - x0);  // progress value between current interval and next.
        float u = u0 + (u1 - u0) * progress;    // u 1D texture coordinate value for this x position.
        intervalList.push_back( VertexToTextureCoord( x, u ) );
      }
      intervalList.push_back( VertexToTextureCoord( x1, u1 ) );
    }
  }
}

}

namespace Dali
{

namespace Internal
{

namespace Render
{

ImageRenderer* ImageRenderer::New()
{
  return new ImageRenderer();
}

ImageRenderer::~ImageRenderer()
{
  if ( mTextureId > 0 )
  {
    mTextureCache->RemoveObserver(mTextureId, this);
  }

  GlCleanup();
}

void ImageRenderer::SetTextureId( ResourceId textureId )
{
  if ( mTextureId > 0 )
  {
    mTextureCache->RemoveObserver(mTextureId, this);
  }

  mTextureId = textureId;
  mTexture = NULL;

  if ( textureId > 0 )
  {
    mTextureCache->AddObserver(textureId, this);
  }
}

void ImageRenderer::SetPixelArea( const ImageRenderer::PixelArea& pixelArea )
{
  mUsePixelArea    = true;
  mPixelArea       = pixelArea;
  mIsMeshGenerated = false;
}

void ImageRenderer::SetNinePatchBorder( const Vector4& border, bool inPixels )
{
  mBorder          = border;
  mBorderInPixels  = inPixels;
  mIsMeshGenerated = false;
}

void ImageRenderer::SetUseBlend( bool useBlend )
{
  mUseBlend = useBlend;
}

void ImageRenderer::SetBlendingOptions( unsigned int options )
{
  mBlendingOptions.SetBitmask( options );
}

void ImageRenderer::SetBlendColor( const Vector4& color )
{
  mBlendingOptions.SetBlendColor( color );
}

void ImageRenderer::CalculateMeshData( MeshType type, const Vector2& targetSize, bool usePixelArea )
{
  mMeshType        = type;
  mGeometrySize    = targetSize;
  mUsePixelArea    = usePixelArea;
  mIsMeshGenerated = false;
}

void ImageRenderer::TextureDiscarded( ResourceId textureId )
{
  DALI_ASSERT_DEBUG( mTextureId == textureId || mTextureId == 0 );

  mTextureId = 0;
  mTexture = NULL;
}

void ImageRenderer::GlContextDestroyed()
{
  if( mVertexBuffer )
  {
    mVertexBuffer->GlContextDestroyed();
  }
  if( mIndexBuffer )
  {
    mIndexBuffer->GlContextDestroyed();
  }
  // force recreation of the geometry in next render
  mIsMeshGenerated = false;
}

void ImageRenderer::GlCleanup()
{
  DALI_LOG_INFO( gImageRenderFilter, Debug::Verbose, "GlCleanup() textureId=%d  texture:%p\n", mTextureId, mTexture);

  mVertexBuffer.Reset();
  mIndexBuffer.Reset();
}

bool ImageRenderer::RequiresDepthTest() const
{
  return false;
}

bool ImageRenderer::CheckResources()
{
  if( mTexture == NULL )
  {
    if ( mTextureCache )
    {
      mTexture = mTextureCache->GetTexture( mTextureId );
    }
  }

  if( mTexture == NULL )
  {
    return false;
  }

  if( ( mTexture->GetWidth() <= 0u ) ||
      ( mTexture->GetHeight() <= 0u ) )
  {
    return false;
  }

  Integration::ResourceId shaderTextureId =  mShader->GetTextureIdToRender();

  if( shaderTextureId &&  mTextureCache->GetTexture( shaderTextureId ) == NULL )
  {
    return false;
  }

  return true;
}

void ImageRenderer::DoRender( Context& context, SceneGraph::TextureCache& textureCache, const SceneGraph::NodeDataProvider& node, BufferIndex bufferIndex, Program& program, const Matrix& modelViewMatrix, const Matrix& viewMatrix )
{
  DALI_LOG_INFO( gImageRenderFilter, Debug::Verbose, "DoRender() textureId=%d  texture:%p\n", mTextureId, mTexture);

  DALI_ASSERT_DEBUG( 0 != mTextureId && "ImageRenderer::DoRender. mTextureId == 0." );
  DALI_ASSERT_DEBUG( NULL != mTexture && "ImageRenderer::DoRender. mTexture == NULL." );

  if(! mIsMeshGenerated )
  {
    GenerateMeshData( mTexture );
  }

  DALI_ASSERT_DEBUG( mVertexBuffer );

  mTextureCache->BindTexture( mTexture, mTextureId,  GL_TEXTURE_2D, TEXTURE_UNIT_IMAGE );

  if( mTexture->GetTextureId() == 0 )
  {
    return; // early out if we haven't got a GL texture yet (e.g. due to context loss)
  }

  mTexture->ApplySampler( TEXTURE_UNIT_IMAGE, mSamplerBitfield );

  // Set sampler uniform
  GLint samplerLoc = program.GetUniformLocation( Program::UNIFORM_SAMPLER );
  if( -1 != samplerLoc )
  {
    // set the uniform
    program.SetUniform1i( samplerLoc, TEXTURE_UNIT_IMAGE );
  }

  // make sure the vertex is bound, this has to be done before
  // we call VertexAttribPointer otherwise you get weird output on the display
  mVertexBuffer->Bind(GpuBuffer::ARRAY_BUFFER);

  samplerLoc = program.GetUniformLocation( Program::UNIFORM_SAMPLER_RECT );
  if( -1 != samplerLoc )
  {
    UvRect uv;

    if ( mUsePixelArea )
    {
      mTexture->GetTextureCoordinates( uv, &mPixelArea );
    }
    else
    {
      mTexture->GetTextureCoordinates( uv, NULL );
    }

    // set the uniform
    program.SetUniform4f( samplerLoc, uv.u0, uv.v0, uv.u2, uv.v2 );
  }

  // Check whether the program supports the expected attributes/uniforms
  const GLint positionLoc = program.GetAttribLocation( Program::ATTRIB_POSITION );
  const GLint texCoordLoc = program.GetAttribLocation( Program::ATTRIB_TEXCOORD );

  if ( positionLoc != -1 )
  {
    context.EnableVertexAttributeArray( positionLoc );

    const int stride = 4 * sizeof(float);
    context.VertexAttribPointer( positionLoc, 2, GL_FLOAT, GL_FALSE, stride, 0 );
  }

  if ( texCoordLoc != -1 )
  {
    context.EnableVertexAttributeArray( texCoordLoc );

    const int stride = 4 * sizeof(float);
    context.VertexAttribPointer( texCoordLoc, 2, GL_FLOAT, GL_FALSE, stride, (const void*) (sizeof(float)*2) );
  }

  switch(mMeshType)
  {
    case QUAD:
    case NINE_PATCH:
    case NINE_PATCH_NO_CENTER:
    {
      const GLsizei vertexCount = mVertexBuffer->GetBufferSize() / sizeof(Vertex2D); // compiler will optimize this to >> if possible
      context.DrawArrays( GL_TRIANGLE_STRIP, 0, vertexCount );
      break;
    }
    case GRID_QUAD:
    case GRID_NINE_PATCH:
    case GRID_NINE_PATCH_NO_CENTER:
    {
      const GLsizei indexCount = mIndexBuffer->GetBufferSize() / sizeof(GLushort); // compiler will optimize this to >> if possible
      mIndexBuffer->Bind(GpuBuffer::ELEMENT_ARRAY_BUFFER);
      context.DrawElements( GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, 0 );
      break;
    }
  }

  if ( positionLoc != -1 )
  {
    context.DisableVertexAttributeArray( positionLoc );
  }

  if ( texCoordLoc != -1 )
  {
    context.DisableVertexAttributeArray( texCoordLoc );
  }
}

void ImageRenderer::DoSetBlending( Context& context )
{
  // Set the blend color
  const Vector4* const customColor = mBlendingOptions.GetBlendColor();
  if( customColor )
  {
    context.SetCustomBlendColor( *customColor );
  }
  else
  {
    context.SetDefaultBlendColor();
  }

  // Set blend source & destination factors
  context.BlendFuncSeparate( mBlendingOptions.GetBlendSrcFactorRgb(),
                             mBlendingOptions.GetBlendDestFactorRgb(),
                             mBlendingOptions.GetBlendSrcFactorAlpha(),
                             mBlendingOptions.GetBlendDestFactorAlpha() );

  // Set blend equations
  context.BlendEquationSeparate( mBlendingOptions.GetBlendEquationRgb(),
                                 mBlendingOptions.GetBlendEquationAlpha() );

}

void ImageRenderer::UpdateVertexBuffer( Context& context, GLsizeiptr size, const GLvoid *data )
{
  // create/destroy if needed/not needed.
  if ( size && !mVertexBuffer )
  {
    mVertexBuffer = new GpuBuffer( context );
  }
  else if ( !size && mVertexBuffer )
  {
    mVertexBuffer.Reset();
  }

  // update
  if ( mVertexBuffer )
  {
    mVertexBuffer->UpdateDataBuffer( size, data, GpuBuffer::DYNAMIC_DRAW, GpuBuffer::ARRAY_BUFFER);
  }
}

void ImageRenderer::UpdateIndexBuffer( Context& context, GLsizeiptr size, const GLvoid *data )
{
  // create/destroy if needed/not needed.
  if ( size && !mIndexBuffer )
  {
    mIndexBuffer = new GpuBuffer( context );
  }
  else if ( !size && mIndexBuffer )
  {
    mIndexBuffer.Reset();
  }

  // update
  if ( mIndexBuffer )
  {
    mIndexBuffer->UpdateDataBuffer(size,data,GpuBuffer::STATIC_DRAW, GpuBuffer::ELEMENT_ARRAY_BUFFER);
  }
}

void ImageRenderer::GenerateMeshData( Texture* texture )
{
  const PixelArea* pixelArea = NULL;
  if( mUsePixelArea )
  {
    pixelArea = &mPixelArea;
  }

  switch( mMeshType )
  {
    case ImageRenderer::QUAD:
    {
      SetQuadMeshData( texture, mGeometrySize, pixelArea );
      break;
    }
    case ImageRenderer::NINE_PATCH:
    {
      SetNinePatchMeshData( texture, mGeometrySize, mBorder, mBorderInPixels, pixelArea, false );
      break;
    }
    case ImageRenderer::NINE_PATCH_NO_CENTER:
    {
      SetNinePatchMeshData( texture, mGeometrySize, mBorder, mBorderInPixels, pixelArea, true );
      break;
    }
    case ImageRenderer::GRID_QUAD:
    {
      SetGridMeshData( texture, mGeometrySize, NULL, false, pixelArea );
      break;
    }
    case ImageRenderer::GRID_NINE_PATCH:
    case ImageRenderer::GRID_NINE_PATCH_NO_CENTER:
    {
      SetGridMeshData( texture, mGeometrySize, &mBorder, mBorderInPixels, pixelArea );
      break;
    }
  }
  mIsMeshGenerated = true;
}

void ImageRenderer::SetQuadMeshData( Texture* texture, const Vector2& size, const PixelArea* pixelArea )
{
  const float x0 = -0.5f * size.x;
  const float y0 = -0.5f * size.y;
  const float x1 =  0.5f * size.x;
  const float y1 =  0.5f * size.y;

  /*
   * Here we render the square as a single square, as texture
   * coordinates linearly interpolate between the 4 vertices.
   *
   * Note: a square (or a quad) is rendered as 2 triangles.
   * Vertices 0,1,2 represent triangle A.
   * Vertices 1,2,3 represent triangle B.
   *
   * No indices are needed as we tell GL to render in strip mode
   * (GL_TRIANGLE_STRIP), which is faster, and consumes less
   * memory.
   *
   *  0---------2
   *  |        /|
   *  |  A    / |
   *  |      /  |
   *  |     /   |
   *  |    /    |
   *  |   /     |
   *  |  /      |
   *  | /    B  |
   *  |/        |
   *  1---------3
   */

  Vertex2D verts[]={
                     {x0, y0, 0.0, 0.0},
                     {x0, y1, 0.0, 1.0},
                     {x1, y0, 1.0, 0.0},
                     {x1, y1, 1.0, 1.0}
                   };

  // We may only be displaying an area of the texture.
  // Calling MapUV converts the u,v values to correct values for the pixel area

  texture->MapUV( sizeof(verts)/sizeof(Vertex2D), verts, pixelArea );

  UpdateVertexBuffer( *mContext, sizeof(verts), verts );
  UpdateIndexBuffer( *mContext, 0, NULL );
}

void ImageRenderer::SetNinePatchMeshData( Texture* texture, const Vector2& size, const Vector4& border, bool borderInPixels, const PixelArea* pixelArea, bool noCenter )
{
  DALI_ASSERT_ALWAYS( mTexture->GetWidth()  > 0.0f && "Invalid Texture width" );
  DALI_ASSERT_ALWAYS( mTexture->GetHeight() > 0.0f && "Invalid Texture height" );

  float textureWidth  = mTexture->GetWidth();
  float textureHeight = mTexture->GetHeight();

  float borderLeft, borderTop, borderRight, borderBottom; // pixels from edge
  float borderX0, borderY0, borderX1, borderY1; // In the range 0 -> 1

  if ( borderInPixels )
  {
    borderLeft   = border.x;
    borderTop    = border.y;
    borderRight  = border.z;
    borderBottom = border.w;

    borderX0 = border.x / textureWidth;
    borderY0 = border.y / textureHeight;
    borderX1 = 1.0f - ( border.z / textureWidth );
    borderY1 = 1.0f - ( border.w / textureHeight );
  }
  else
  {
    borderLeft   = textureWidth  * border.x;
    borderTop    = textureHeight * border.y;
    borderRight  = textureWidth  * (1.0f - border.z);
    borderBottom = textureHeight * (1.0f - border.w);

    borderX0 = border.x;
    borderY0 = border.y;
    borderX1 = border.z;
    borderY1 = border.w;
  }

  const float u0 = 0.0f;
  const float u3 = 1.0f;
  const float u1 = borderX0;
  const float u2 = borderX1;

  const float v0 = 0.0f;
  const float v3 = 1.0f;
  const float v1 = borderY0;
  const float v2 = borderY1;

  const float x0 = size.x * -0.5;
  const float x1 = x0 + borderLeft;
  const float x2 = x0 + size.x - borderRight;
  const float x3 = x0 + size.x;

  const float y0 = size.y * -0.5;
  const float y1 = y0 + borderTop;
  const float y2 = y0 + size.y - borderBottom;
  const float y3 = y0 + size.y;


  if ( !noCenter )
  {
    /*
     * We're breaking a quad in to 9 smaller quads, so that when it's
     * stretched the corners maintain their size.
     * For speed the 9-patch is drawn with a single triangle span, the draw
     * order of the span is 1->9.
     * Previously it would draw three separate spans (1->3, 4->6, 7->9), but now it
     * it does it one go by turning the corner when gets to the end of each row.
     *
     * No indices are needed as we tell GL to render in strip mode
     * (GL_TRIANGLE_STRIP), which is faster, and consumes less
     * memory.
     *
     *  |---|---------------|---|
     *  |  7|    --> 8      | 9 |
     *  |---|---------------|---|
     *  |   |               |   |
     *  | 6 |     <-- 5     | 4 |
     *  |   |               |   |
     *  |   |               |   |
     *  |---|---------------|---|
     *  | 1 |      2 -->    | 3 |
     *  |---|---------------|---|
     */

    Vertex2D vertsWithCenter[]={
                                 // bottom left
                                 {  x0, y0, u0, v0 },
                                 {  x0, y1, u0, v1 },
                                 {  x1, y0, u1, v0 },
                                 {  x1, y1, u1, v1 },

                                 // bottom middle
                                 {  x2, y0, u2, v0 },
                                 {  x2, y1, u2, v1 },

                                 // bottom right
                                 {  x3, y0, u3, v0 },
                                 {  x3, y1, u3, v1 },

                                 // turn the corner
                                 {  x3, y1, u3, v1 },
                                 {  x3, y1, u3, v1 },

                                 // 10 verts here

                                 // middle right
                                 {  x3, y2, u3, v2 },
                                 {  x2, y1, u2, v1 },
                                 {  x2, y2, u2, v2 },

                                 // middle left

                                 {  x1, y1, u1, v1 },
                                 {  x1, y2, u1, v2 },
                                 {  x0, y1, u0, v1 },
                                 {  x0, y2, u0, v2 },

                                 // turn the corner
                                 {  x0, y2, u0, v2 },
                                 {  x0, y2, u0, v2 },

                                 // top left
                                 {  x0, y3, u0, v3 },
                                 {  x1, y2, u1, v2 },
                                 {  x1, y3, u1, v3 },

                                 // top middle
                                 {  x2, y2, u2, v2 },
                                 {  x2, y3, u2, v3 },

                                 // top right
                                 {  x3, y2, u3, v2 },
                                 {  x3, y3, u3, v3 },
                               };

    const size_t vertsSize = sizeof( vertsWithCenter );
    const unsigned int vertexCount = vertsSize / sizeof( vertsWithCenter[0] );
    texture->MapUV( vertexCount, vertsWithCenter, pixelArea );
    UpdateVertexBuffer( *mContext, vertsSize, vertsWithCenter );
  }
  else
  {
    /*
     * The center part is not going to be rendered. The 9-patch border is drawn with
     * a single triangle span, and the draw order of the span is 1->8.
     *
     *  |---|---------------|---|
     *  | 7 |    <-- 6      | 5 |
     *  |---|---------------|---|
     *  |   |               |   |
     *  |   |               |   |
     *  | 8 |               | 4 |
     *  |   | (not rendered)|   |
     *  |   |               |   |
     *  |---|---------------|---|
     *  | 1 |      2 -->    | 3 |
     *  |---|---------------|---|
     */

    Vertex2D vertsWithNoCenter[]={
                                   // bottom left (1)
                                   {  x0, y0, u0, v0 },
                                   {  x0, y1, u0, v1 },
                                   {  x1, y0, u1, v0 },
                                   {  x1, y1, u1, v1 },

                                   // bottom middle (2)
                                   {  x2, y0, u2, v0 },
                                   {  x2, y1, u2, v1 },

                                   // bottom right (3)
                                   {  x3, y0, u3, v0 },
                                   {  x3, y1, u3, v1 },

                                   // reset the starting point to x3, y1
                                   {  x3, y1, u3, v1 },
                                   {  x3, y1, u3, v1 },

                                   // middle right (4)
                                   {  x3, y2, u3, v2 },
                                   {  x2, y1, u2, v1 },
                                   {  x2, y2, u2, v2 },

                                   // corner turning
                                   {  x2, y2, u2, v2 },
                                   {  x3, y2, u3, v2 },
                                   {  x3, y2, u3, v2 },

                                   // top right (5)
                                   {  x3, y3, u3, v3 },
                                   {  x2, y2, u2, v2 },
                                   {  x2, y3, u2, v3 },

                                   // top middle (6)
                                   {  x1, y2, u1, v2 },
                                   {  x1, y3, u1, v3 },

                                   // reset point to x0,y3
                                   {  x1, y3, u1, v3 },
                                   {  x0, y3, u0, v3 },
                                   {  x0, y3, u0, v3 },

                                   // top left box (starting from (x0,y3)) (7)
                                   {  x0, y2, u0, v2 },
                                   {  x1, y3, u1, v3 },
                                   {  x1, y2, u1, v2 },

                                   // reset point
                                   {  x1, y2, u1, v2 },

                                   // middle left (8)
                                   {  x0, y2, u0, v2 },
                                   {  x1, y1, u1, v2 },
                                   {  x0, y1, u0, v2 },
                                 };

    const size_t vertsSize = sizeof( vertsWithNoCenter );
    const unsigned int vertexCount = vertsSize / sizeof( vertsWithNoCenter[0] );
    texture->MapUV( vertexCount, vertsWithNoCenter, pixelArea );
    UpdateVertexBuffer( *mContext, vertsSize, vertsWithNoCenter );
  }
  // not using an index buffer
  UpdateIndexBuffer( *mContext, 0, NULL );

}

void ImageRenderer::SetGridMeshData( Texture* texture, const Vector2& size, const Vector4* border, bool borderInPixels, const PixelArea* pixelArea )
{
  /*
   * Quad Grid:
   * In Grid Mode, we tessellate the single quad into smaller quads
   * at approximately (guideGridSize x guideGridSize) in size.
   *
   * Conversion of Quad to Gridded Quad.
   *
   * |-----------|    |---|---|---|
   * |           |    |   |   |   |
   * |           | -> |---|---|---|
   * |           |    |   |   |   |
   * |-----------|    |---|---|---|
   *
   * 9-Patch Grid:
   * In Grid Mode, we tessellate each quad of a 9-patch
   * (see SetNinePatchMeshData) into smaller quads at approximately
   * (guideGridSize x guideGridSize) in size.
   *
   * This satisfies the two requirements of a 9-patch with grid:
   *
   * 1. Texture coordinates within each section of the 9-patch
   * should change linearly to that 9-patch's rules.
   * 2. The image as as whole should provide Vertex points at
   * approximate guideGridSize intervals.
   *
   * The result should be the horizontal and vertical lines of
   * a 9-patch overlayed by the horizontal and vertical lines of
   * a grid.
   *
   * Non-overlayed:
   *  |     |     |     |     |     |     | <- Grid Markers
   * -|-------|-------------------|-------|
   *  |       |                   |       |
   *  |   1   |         2         |   3   |
   * -|       |                   |       |
   *  |-------+-------------------+-------|
   *  |       |                   |       |
   * -|       |                   |       |
   *  |       |                   |       |
   *  |       |                   |       |
   * -|   4   |         5         |   6   | <- 9 Patch.
   *  |       |                   |       |
   *  |       |                   |       |
   * -|       |                   |       |
   *  |       |                   |       |
   *  |-------+-------------------+-------|
   * -|       |                   |       |
   *  |   7   |         8         |   9   |
   *  |       |                   |       |
   * -|-------|-------------------|-------|
   *
   *
   * Overlayed:
   *  |     |     |     |     |     |     | <- Grid Markers
   * -|-------|-------------------|-------|
   *  |     | |   |     |     |   | |     |
   *  |     | |   |     |     |   | |     |
   * -|-----|-|---|-----|-----|---|-|-----|
   *  |-------+-------------------+-------|
   *  |     | |   |     |     |   | |     |
   * -|-----|-|---|-----|-----|---|-|-----|
   *  |     | |   |     |     |   | |     |
   *  |     | |   |     |     |   | |     |
   * -|-----|-|---|-----|-----|---|-|-----| <- 9 Patch.
   *  |     | |   |     |     |   | |     |
   *  |     | |   |     |     |   | |     |
   * -|-----|-|---|-----|-----|---|-|-----|
   *  |     | |   |     |     |   | |     |
   *  |-------+-------------------+-------|
   * -|-----|-|---|-----|-----|---|-|-----|
   *  |     | |   |     |     |   | |     |
   *  |     | |   |     |     |   | |     |
   * -|-------|-------------------|-------|
   */

  std::vector<VertexToTextureCoord> horizontalDivisions;
  std::vector<VertexToTextureCoord> verticalDivisions;

  const float guideGridSize = mShader->GetGridDensity();

  const int textureWidth = texture->GetWidth();
  const int textureHeight = texture->GetHeight();

  const float halfWidth = size.width * 0.5f;
  const float halfHeight = size.height * 0.5f;

  // Determine how many rectangles across and down to tesselate image into.
  const int guideRectX = (size.width / guideGridSize);
  const int guideRectY = (size.height / guideGridSize);

  // Build up list of points in X axis where vertices need to go.
  std::vector<VertexToTextureCoord> insertionList;
  insertionList.reserve(4);
  insertionList.push_back( VertexToTextureCoord( -halfWidth, 0.0f ) );

  // If 9-Patch Border exists, add additional border points in list.
  if(border)
  {
    float borderX0, borderX1, borderU0, borderU1;

    if ( borderInPixels )
    {
      borderX0 = border->x - halfWidth;
      borderX1 = halfWidth - border->z;

      borderU0 = border->x / textureWidth;
      borderU1 = 1.0f - (border->z / textureWidth);
    }
    else
    {
      borderX0 = border->x * textureWidth - halfWidth;
      borderX1 = halfWidth - (1.0f - border->z) * textureWidth;
      borderU0 = border->x;
      borderU1 = border->z;
    }

    insertionList.push_back( VertexToTextureCoord( borderX0, borderU0 ) );
    insertionList.push_back( VertexToTextureCoord( borderX1, borderU1 ) );
  }

  insertionList.push_back( VertexToTextureCoord( halfWidth, 1.0f ) );
  GenerateIntervals(horizontalDivisions, guideRectX + 2, insertionList);

  // Build up list of points in Y axis where vertices need to go.
  insertionList.clear();
  insertionList.push_back( VertexToTextureCoord( -halfHeight, 0.0f ) );

  // If 9-Patch Border exists, add additional border points in list.
  if(border)
  {
    float borderY0, borderY1, borderU0, borderU1;

    if ( borderInPixels )
    {
      borderY0 = border->y - halfHeight;
      borderY1 = halfHeight - border->w;

      borderU0 = border->y / textureHeight;
      borderU1 = 1.0f - (border->w / textureHeight);
    }
    else
    {
      borderY0 = border->y * textureHeight - halfHeight;
      borderY1 = halfHeight - (1.0f - border->w) * textureHeight;

      borderU0 = border->y;
      borderU1 = border->w;
    }

    insertionList.push_back( VertexToTextureCoord( borderY0, borderU0 ) );
    insertionList.push_back( VertexToTextureCoord( borderY1, borderU1 ) );
  }

  insertionList.push_back( VertexToTextureCoord( halfHeight, 1.0f ) );
  GenerateIntervals(verticalDivisions, guideRectY + 2, insertionList);

  // Now build up Vertex pattern based on the above X and Y lists.
  const int totalVertices = horizontalDivisions.size() * verticalDivisions.size();
  Vertex2D* vertices = new Vertex2D[totalVertices];
  Vertex2D* vertex = vertices;

  for(std::vector<VertexToTextureCoord>::const_iterator yIter = verticalDivisions.begin(); yIter != verticalDivisions.end(); ++yIter )
  {
    for(std::vector<VertexToTextureCoord>::const_iterator xIter = horizontalDivisions.begin(); xIter != horizontalDivisions.end(); ++xIter )
    {
      vertex->mX = xIter->x;
      vertex->mU = xIter->u;
      vertex->mY = yIter->x;
      vertex->mV = yIter->u;
      vertex++;
    }
  }

  // Build up Triangle indicies, very predictable pattern.
  const size_t rectX = horizontalDivisions.size() - 1;
  const size_t rectY = verticalDivisions.size() - 1;
  const size_t totalIndices = rectX * rectY * 6;             // 2 triangles per quad (rect) and 3 points to define each triangle.
  GLushort* indices = new GLushort[totalIndices];

  GenerateMeshIndices(indices, rectX, rectY);

  texture->MapUV( totalVertices, vertices, pixelArea );

  UpdateVertexBuffer( *mContext, totalVertices * sizeof(Vertex2D) , vertices );
  UpdateIndexBuffer( *mContext, totalIndices * sizeof(GLushort), indices );

  delete[] vertices;
  delete[] indices;
}

void ImageRenderer::GenerateMeshIndices(GLushort* indices, int rectanglesX, int rectanglesY)
{
  GLushort *i = indices;
  const int meshEndIndex = rectanglesY * (rectanglesX + 1);

  int index = 0;
  while(index < meshEndIndex)
  {
    const int rowEndIndex = index + rectanglesX;
    for (; index < rowEndIndex; index++ )
    {
      *i++ = index;
      *i++ = index + 1 + rectanglesX;
      *i++ = index + 1;

      *i++ = index + 1;
      *i++ = index + 1 + rectanglesX;
      *i++ = index + 2 + rectanglesX;
    }
    index++;    // one extra vertex per row than rects.
  }
}

ImageRenderer::ImageRenderer()
: Renderer(),
  mTexture( NULL ),
  mBorder( 0.45, 0.45, 0.1, 0.1 ),
  mPixelArea(),
  mGeometrySize(),
  mTextureId( 0 ),
  mMeshType( ImageRenderer::QUAD ),
  mIsMeshGenerated( false ),
  mBorderInPixels( false ),
  mUseBlend( false ),
  mUsePixelArea( false )
{
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

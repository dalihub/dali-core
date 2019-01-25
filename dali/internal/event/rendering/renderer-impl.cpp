/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/rendering/renderer-impl.h> // Dali::Internal::Renderer

// INTERNAL INCLUDES
#include <dali/devel-api/scripting/scripting.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/event/common/property-helper.h>    // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END
#include <dali/internal/event/common/property-input-impl.h>
#include <dali/internal/render/renderers/render-geometry.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/rendering/scene-graph-renderer.h>

namespace Dali
{
namespace Internal
{

namespace
{

/**
 * Properties: |name                              |type     |writable|animatable|constraint-input|enum for index-checking|
 */
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "depthIndex",                      INTEGER,   true, false,  false, Dali::Renderer::Property::DEPTH_INDEX )
DALI_PROPERTY( "faceCullingMode",                 INTEGER,   true, false,  false, Dali::Renderer::Property::FACE_CULLING_MODE )
DALI_PROPERTY( "blendMode",                       INTEGER,   true, false,  false, Dali::Renderer::Property::BLEND_MODE )
DALI_PROPERTY( "blendEquationRgb",                INTEGER,   true, false,  false, Dali::Renderer::Property::BLEND_EQUATION_RGB )
DALI_PROPERTY( "blendEquationAlpha",              INTEGER,   true, false,  false, Dali::Renderer::Property::BLEND_EQUATION_ALPHA )
DALI_PROPERTY( "blendFactorSrcRgb",               INTEGER,   true, false,  false, Dali::Renderer::Property::BLEND_FACTOR_SRC_RGB )
DALI_PROPERTY( "blendFactorDestRgb",              INTEGER,   true, false,  false, Dali::Renderer::Property::BLEND_FACTOR_DEST_RGB )
DALI_PROPERTY( "blendFactorSrcAlpha",             INTEGER,   true, false,  false, Dali::Renderer::Property::BLEND_FACTOR_SRC_ALPHA )
DALI_PROPERTY( "blendFactorDestAlpha",            INTEGER,   true, false,  false, Dali::Renderer::Property::BLEND_FACTOR_DEST_ALPHA )
DALI_PROPERTY( "blendColor",                      VECTOR4,   true, false,  false, Dali::Renderer::Property::BLEND_COLOR )
DALI_PROPERTY( "blendPreMultipliedAlpha",         BOOLEAN,   true, false,  false, Dali::Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA )
DALI_PROPERTY( "indexRangeFirst",                 INTEGER,   true, false,  false, Dali::Renderer::Property::INDEX_RANGE_FIRST )
DALI_PROPERTY( "indexRangeCount",                 INTEGER,   true, false,  false, Dali::Renderer::Property::INDEX_RANGE_COUNT )
DALI_PROPERTY( "depthWriteMode",                  INTEGER,   true, false,  false, Dali::Renderer::Property::DEPTH_WRITE_MODE )
DALI_PROPERTY( "depthFunction",                   INTEGER,   true, false,  false, Dali::Renderer::Property::DEPTH_FUNCTION )
DALI_PROPERTY( "depthTestMode",                   INTEGER,   true, false,  false, Dali::Renderer::Property::DEPTH_TEST_MODE )
DALI_PROPERTY( "renderMode",                      INTEGER,   true, false,  false, Dali::Renderer::Property::RENDER_MODE )
DALI_PROPERTY( "stencilFunction",                 INTEGER,   true, false,  false, Dali::Renderer::Property::STENCIL_FUNCTION )
DALI_PROPERTY( "stencilFunctionMask",             INTEGER,   true, false,  false, Dali::Renderer::Property::STENCIL_FUNCTION_MASK )
DALI_PROPERTY( "stencilFunctionReference",        INTEGER,   true, false,  false, Dali::Renderer::Property::STENCIL_FUNCTION_REFERENCE )
DALI_PROPERTY( "stencilMask",                     INTEGER,   true, false,  false, Dali::Renderer::Property::STENCIL_MASK )
DALI_PROPERTY( "stencilOperationOnFail",          INTEGER,   true, false,  false, Dali::Renderer::Property::STENCIL_OPERATION_ON_FAIL )
DALI_PROPERTY( "stencilOperationOnZFail",         INTEGER,   true, false,  false, Dali::Renderer::Property::STENCIL_OPERATION_ON_Z_FAIL )
DALI_PROPERTY( "stencilOperationOnZPass",         INTEGER,   true, false,  false, Dali::Renderer::Property::STENCIL_OPERATION_ON_Z_PASS )
DALI_PROPERTY( "opacity",                         FLOAT,     true, true,   true,  Dali::DevelRenderer::Property::OPACITY )
DALI_PROPERTY( "renderingBehavior",               INTEGER,   true, false,  false, Dali::DevelRenderer::Property::RENDERING_BEHAVIOR )
DALI_PROPERTY_TABLE_END( DEFAULT_RENDERER_PROPERTY_START_INDEX, RendererDefaultProperties )

// Property string to enumeration tables:

DALI_ENUM_TO_STRING_TABLE_BEGIN( FACE_CULLING_MODE )
DALI_ENUM_TO_STRING_WITH_SCOPE( FaceCullingMode, NONE )
DALI_ENUM_TO_STRING_WITH_SCOPE( FaceCullingMode, FRONT )
DALI_ENUM_TO_STRING_WITH_SCOPE( FaceCullingMode, BACK )
DALI_ENUM_TO_STRING_WITH_SCOPE( FaceCullingMode, FRONT_AND_BACK )
DALI_ENUM_TO_STRING_TABLE_END( FACE_CULLING_MODE )

DALI_ENUM_TO_STRING_TABLE_BEGIN( BLEND_MODE )
DALI_ENUM_TO_STRING_WITH_SCOPE( BlendMode, OFF )
DALI_ENUM_TO_STRING_WITH_SCOPE( BlendMode, AUTO )
DALI_ENUM_TO_STRING_WITH_SCOPE( BlendMode, ON )
DALI_ENUM_TO_STRING_TABLE_END( BLEND_MODE )

DALI_ENUM_TO_STRING_TABLE_BEGIN( BLEND_EQUATION )
DALI_ENUM_TO_STRING_WITH_SCOPE( BlendEquation, ADD )
DALI_ENUM_TO_STRING_WITH_SCOPE( BlendEquation, SUBTRACT )
DALI_ENUM_TO_STRING_WITH_SCOPE( BlendEquation, REVERSE_SUBTRACT )
DALI_ENUM_TO_STRING_TABLE_END( BLEND_EQUATION )

DALI_ENUM_TO_STRING_TABLE_BEGIN( BLEND_FACTOR )
DALI_ENUM_TO_STRING_WITH_SCOPE( BlendFactor, ZERO )
DALI_ENUM_TO_STRING_WITH_SCOPE( BlendFactor, ONE )
DALI_ENUM_TO_STRING_WITH_SCOPE( BlendFactor, SRC_COLOR )
DALI_ENUM_TO_STRING_WITH_SCOPE( BlendFactor, ONE_MINUS_SRC_COLOR )
DALI_ENUM_TO_STRING_WITH_SCOPE( BlendFactor, SRC_ALPHA )
DALI_ENUM_TO_STRING_WITH_SCOPE( BlendFactor, ONE_MINUS_SRC_ALPHA )
DALI_ENUM_TO_STRING_WITH_SCOPE( BlendFactor, DST_ALPHA )
DALI_ENUM_TO_STRING_WITH_SCOPE( BlendFactor, ONE_MINUS_DST_ALPHA )
DALI_ENUM_TO_STRING_WITH_SCOPE( BlendFactor, DST_COLOR )
DALI_ENUM_TO_STRING_WITH_SCOPE( BlendFactor, ONE_MINUS_DST_COLOR )
DALI_ENUM_TO_STRING_WITH_SCOPE( BlendFactor, SRC_ALPHA_SATURATE )
DALI_ENUM_TO_STRING_WITH_SCOPE( BlendFactor, CONSTANT_COLOR )
DALI_ENUM_TO_STRING_WITH_SCOPE( BlendFactor, ONE_MINUS_CONSTANT_COLOR )
DALI_ENUM_TO_STRING_WITH_SCOPE( BlendFactor, CONSTANT_ALPHA )
DALI_ENUM_TO_STRING_WITH_SCOPE( BlendFactor, ONE_MINUS_CONSTANT_ALPHA )
DALI_ENUM_TO_STRING_TABLE_END( BLEND_FACTOR )

DALI_ENUM_TO_STRING_TABLE_BEGIN( DEPTH_WRITE_MODE )
DALI_ENUM_TO_STRING_WITH_SCOPE( DepthWriteMode, OFF )
DALI_ENUM_TO_STRING_WITH_SCOPE( DepthWriteMode, AUTO )
DALI_ENUM_TO_STRING_WITH_SCOPE( DepthWriteMode, ON )
DALI_ENUM_TO_STRING_TABLE_END( DEPTH_WRITE_MODE )

DALI_ENUM_TO_STRING_TABLE_BEGIN( DEPTH_TEST_MODE )
DALI_ENUM_TO_STRING_WITH_SCOPE( DepthTestMode, OFF )
DALI_ENUM_TO_STRING_WITH_SCOPE( DepthTestMode, AUTO )
DALI_ENUM_TO_STRING_WITH_SCOPE( DepthTestMode, ON )
DALI_ENUM_TO_STRING_TABLE_END( DEPTH_TEST_MODE )

DALI_ENUM_TO_STRING_TABLE_BEGIN( DEPTH_FUNCTION )
DALI_ENUM_TO_STRING_WITH_SCOPE( DepthFunction, NEVER )
DALI_ENUM_TO_STRING_WITH_SCOPE( DepthFunction, ALWAYS )
DALI_ENUM_TO_STRING_WITH_SCOPE( DepthFunction, LESS )
DALI_ENUM_TO_STRING_WITH_SCOPE( DepthFunction, GREATER )
DALI_ENUM_TO_STRING_WITH_SCOPE( DepthFunction, EQUAL )
DALI_ENUM_TO_STRING_WITH_SCOPE( DepthFunction, NOT_EQUAL )
DALI_ENUM_TO_STRING_WITH_SCOPE( DepthFunction, LESS_EQUAL )
DALI_ENUM_TO_STRING_WITH_SCOPE( DepthFunction, GREATER_EQUAL )
DALI_ENUM_TO_STRING_TABLE_END( DEPTH_FUNCTION )

DALI_ENUM_TO_STRING_TABLE_BEGIN( STENCIL_FUNCTION )
DALI_ENUM_TO_STRING_WITH_SCOPE( StencilFunction, NEVER )
DALI_ENUM_TO_STRING_WITH_SCOPE( StencilFunction, LESS )
DALI_ENUM_TO_STRING_WITH_SCOPE( StencilFunction, EQUAL )
DALI_ENUM_TO_STRING_WITH_SCOPE( StencilFunction, LESS_EQUAL )
DALI_ENUM_TO_STRING_WITH_SCOPE( StencilFunction, GREATER )
DALI_ENUM_TO_STRING_WITH_SCOPE( StencilFunction, NOT_EQUAL )
DALI_ENUM_TO_STRING_WITH_SCOPE( StencilFunction, GREATER_EQUAL )
DALI_ENUM_TO_STRING_WITH_SCOPE( StencilFunction, ALWAYS )
DALI_ENUM_TO_STRING_TABLE_END( STENCIL_FUNCTION )

DALI_ENUM_TO_STRING_TABLE_BEGIN( RENDER_MODE )
DALI_ENUM_TO_STRING_WITH_SCOPE( RenderMode, NONE )
DALI_ENUM_TO_STRING_WITH_SCOPE( RenderMode, AUTO )
DALI_ENUM_TO_STRING_WITH_SCOPE( RenderMode, COLOR )
DALI_ENUM_TO_STRING_WITH_SCOPE( RenderMode, STENCIL )
DALI_ENUM_TO_STRING_WITH_SCOPE( RenderMode, COLOR_STENCIL )
DALI_ENUM_TO_STRING_TABLE_END( RENDER_MODE )

DALI_ENUM_TO_STRING_TABLE_BEGIN( STENCIL_OPERATION )
DALI_ENUM_TO_STRING_WITH_SCOPE( StencilOperation, ZERO )
DALI_ENUM_TO_STRING_WITH_SCOPE( StencilOperation, KEEP )
DALI_ENUM_TO_STRING_WITH_SCOPE( StencilOperation, REPLACE )
DALI_ENUM_TO_STRING_WITH_SCOPE( StencilOperation, INCREMENT )
DALI_ENUM_TO_STRING_WITH_SCOPE( StencilOperation, DECREMENT )
DALI_ENUM_TO_STRING_WITH_SCOPE( StencilOperation, INVERT )
DALI_ENUM_TO_STRING_WITH_SCOPE( StencilOperation, INCREMENT_WRAP )
DALI_ENUM_TO_STRING_WITH_SCOPE( StencilOperation, DECREMENT_WRAP )
DALI_ENUM_TO_STRING_TABLE_END( STENCIL_OPERATION )

DALI_ENUM_TO_STRING_TABLE_BEGIN( RENDERING_BEHAVIOR )
DALI_ENUM_TO_STRING_WITH_SCOPE( DevelRenderer::Rendering, IF_REQUIRED )
DALI_ENUM_TO_STRING_WITH_SCOPE( DevelRenderer::Rendering, CONTINUOUSLY )
DALI_ENUM_TO_STRING_TABLE_END( RENDERING_BEHAVIOR )

BaseHandle Create()
{
  return Dali::BaseHandle();
}

TypeRegistration mType( typeid( Dali::Renderer ), typeid( Dali::Handle ), Create, RendererDefaultProperties );

} // unnamed namespace

RendererPtr Renderer::New()
{
  // create scene object first so it's guaranteed to exist for the event side
  auto sceneObject = SceneGraph::Renderer::New();
  OwnerPointer< SceneGraph::Renderer > transferOwnership( sceneObject );
  // pass the pointer to base for message passing
  RendererPtr rendererPtr( new Renderer( sceneObject ) );
  // transfer scene object ownership to update manager
  EventThreadServices& eventThreadServices = rendererPtr->GetEventThreadServices();
  SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();
  AddRendererMessage( updateManager, transferOwnership );

  eventThreadServices.RegisterObject( rendererPtr.Get() );
  return rendererPtr;
}

void Renderer::SetGeometry( Geometry& geometry )
{
  mGeometry = &geometry;

  const Render::Geometry* geometrySceneObject = geometry.GetRenderObject();
  SetGeometryMessage( GetEventThreadServices(), GetRendererSceneObject(), *geometrySceneObject );
}

GeometryPtr Renderer::GetGeometry() const
{
  return mGeometry;
}

void Renderer::SetTextures( TextureSet& textureSet )
{
  mTextureSet = &textureSet;
  const SceneGraph::TextureSet* textureSetSceneObject = textureSet.GetTextureSetSceneObject();
  SetTexturesMessage( GetEventThreadServices(), GetRendererSceneObject(), *textureSetSceneObject );
}

TextureSetPtr Renderer::GetTextures() const
{
  return mTextureSet;
}

void Renderer::SetShader( Shader& shader )
{
  mShader = &shader;
  const SceneGraph::Shader& sceneGraphShader = shader.GetShaderSceneObject();
  SceneGraph::SetShaderMessage( GetEventThreadServices(), GetRendererSceneObject(), sceneGraphShader );
}

ShaderPtr Renderer::GetShader() const
{
  return mShader;
}

void Renderer::SetDepthIndex( int32_t depthIndex )
{
  if ( mDepthIndex != depthIndex )
  {
    mDepthIndex = depthIndex;
    SetDepthIndexMessage( GetEventThreadServices(), GetRendererSceneObject(), depthIndex );
  }
}

int32_t Renderer::GetDepthIndex() const
{
  return mDepthIndex;
}

void Renderer::SetBlendMode( BlendMode::Type mode )
{
  if( mBlendMode != mode )
  {
    mBlendMode = mode;

    SetBlendModeMessage( GetEventThreadServices(), GetRendererSceneObject(), mBlendMode );
  }
}

BlendMode::Type Renderer::GetBlendMode() const
{
  return mBlendMode;
}

void Renderer::SetBlendFunc( BlendFactor::Type srcFactorRgba, BlendFactor::Type destFactorRgba )
{
  mBlendingOptions.SetBlendFunc( srcFactorRgba, destFactorRgba, srcFactorRgba, destFactorRgba );
  SetBlendingOptionsMessage( GetEventThreadServices(), GetRendererSceneObject(), mBlendingOptions.GetBitmask() );
}

void Renderer::SetBlendFunc( BlendFactor::Type srcFactorRgb,
                             BlendFactor::Type destFactorRgb,
                             BlendFactor::Type srcFactorAlpha,
                             BlendFactor::Type destFactorAlpha )
{
  mBlendingOptions.SetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
  SetBlendingOptionsMessage( GetEventThreadServices(), GetRendererSceneObject(), mBlendingOptions.GetBitmask() );
}

void Renderer::GetBlendFunc( BlendFactor::Type& srcFactorRgb,
                             BlendFactor::Type& destFactorRgb,
                             BlendFactor::Type& srcFactorAlpha,
                             BlendFactor::Type& destFactorAlpha ) const
{
  srcFactorRgb    = mBlendingOptions.GetBlendSrcFactorRgb();
  destFactorRgb   = mBlendingOptions.GetBlendDestFactorRgb();
  srcFactorAlpha  = mBlendingOptions.GetBlendSrcFactorAlpha();
  destFactorAlpha = mBlendingOptions.GetBlendDestFactorAlpha();
}

void Renderer::SetBlendEquation( BlendEquation::Type equationRgba )
{
  mBlendingOptions.SetBlendEquation( equationRgba, equationRgba );
  SetBlendingOptionsMessage( GetEventThreadServices(), GetRendererSceneObject(), mBlendingOptions.GetBitmask() );
}

void Renderer::SetBlendEquation( BlendEquation::Type equationRgb,
                                 BlendEquation::Type equationAlpha )
{
  mBlendingOptions.SetBlendEquation( equationRgb, equationAlpha );
  SetBlendingOptionsMessage( GetEventThreadServices(), GetRendererSceneObject(), mBlendingOptions.GetBitmask() );
}

void Renderer::GetBlendEquation( BlendEquation::Type& equationRgb,
                                 BlendEquation::Type& equationAlpha ) const
{
  // These are not animatable, the cached values are up-to-date.
  equationRgb   = mBlendingOptions.GetBlendEquationRgb();
  equationAlpha = mBlendingOptions.GetBlendEquationAlpha();
}

void Renderer::SetIndexedDrawFirstElement( uint32_t firstElement )
{
  if( firstElement != mIndexedDrawFirstElement )
  {
    mIndexedDrawFirstElement = firstElement;
    SetIndexedDrawFirstElementMessage( GetEventThreadServices(), GetRendererSceneObject(), mIndexedDrawFirstElement );
  }
}

void Renderer::SetIndexedDrawElementsCount( uint32_t elementsCount )
{
  if( elementsCount != mIndexedDrawElementCount )
  {
    mIndexedDrawElementCount = elementsCount;
    SetIndexedDrawElementsCountMessage( GetEventThreadServices(), GetRendererSceneObject(), mIndexedDrawElementCount );
  }
}


void Renderer::EnablePreMultipliedAlpha( bool preMultipled )
{
  if(  mPremultipledAlphaEnabled != preMultipled )
  {
    if( preMultipled )
    {
      SetBlendFunc( BlendFactor::ONE, BlendFactor::ONE_MINUS_SRC_ALPHA, BlendFactor::ONE, BlendFactor::ONE_MINUS_SRC_ALPHA );
    }
    else
    {
      SetBlendFunc( BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA, BlendFactor::ONE, BlendFactor::ONE_MINUS_SRC_ALPHA );
    }
    mPremultipledAlphaEnabled = preMultipled;
    SetEnablePreMultipliedAlphaMessage( GetEventThreadServices(), GetRendererSceneObject(), mPremultipledAlphaEnabled );
  }
}

bool Renderer::IsPreMultipliedAlphaEnabled() const
{
  return mPremultipledAlphaEnabled;
}

const SceneGraph::Renderer& Renderer::GetRendererSceneObject() const
{
  return static_cast<const SceneGraph::Renderer&>( GetSceneObject() );
}

void Renderer::SetDefaultProperty( Property::Index index,
                                   const Property::Value& propertyValue )
{
  switch( index )
  {
    case Dali::Renderer::Property::DEPTH_INDEX:
    {
      SetDepthIndex( propertyValue.Get<int32_t>() );
      break;
    }
    case Dali::Renderer::Property::FACE_CULLING_MODE:
    {
      FaceCullingMode::Type convertedValue = mFaceCullingMode;
      if( Scripting::GetEnumerationProperty< FaceCullingMode::Type >( propertyValue, FACE_CULLING_MODE_TABLE, FACE_CULLING_MODE_TABLE_COUNT, convertedValue ) )
      {
        mFaceCullingMode = convertedValue;
        SetFaceCullingModeMessage( GetEventThreadServices(), GetRendererSceneObject(), convertedValue );
      }
      break;
    }
    case Dali::Renderer::Property::BLEND_MODE:
    {
      BlendMode::Type convertedValue = mBlendMode;
      if( Scripting::GetEnumerationProperty< BlendMode::Type >( propertyValue, BLEND_MODE_TABLE, BLEND_MODE_TABLE_COUNT, convertedValue ) )
      {
        SetBlendMode( convertedValue );
      }
      break;
    }
    case Dali::Renderer::Property::BLEND_EQUATION_RGB:
    {
      BlendEquation::Type convertedValue = mBlendingOptions.GetBlendEquationRgb();

      if( Scripting::GetEnumerationProperty< BlendEquation::Type >( propertyValue, BLEND_EQUATION_TABLE, BLEND_EQUATION_TABLE_COUNT, convertedValue ) )
      {
        BlendEquation::Type alphaEquation = mBlendingOptions.GetBlendEquationAlpha();
        mBlendingOptions.SetBlendEquation( convertedValue, alphaEquation );
        SetBlendingOptionsMessage( GetEventThreadServices(), GetRendererSceneObject(), mBlendingOptions.GetBitmask() );
      }
      break;
    }
    case Dali::Renderer::Property::BLEND_EQUATION_ALPHA:
    {
      BlendEquation::Type convertedValue = mBlendingOptions.GetBlendEquationAlpha();

      if( Scripting::GetEnumerationProperty< BlendEquation::Type >( propertyValue, BLEND_EQUATION_TABLE, BLEND_EQUATION_TABLE_COUNT, convertedValue ) )
      {
        BlendEquation::Type rgbEquation = mBlendingOptions.GetBlendEquationRgb();
        mBlendingOptions.SetBlendEquation( rgbEquation, convertedValue );
        SetBlendingOptionsMessage( GetEventThreadServices(), GetRendererSceneObject(), mBlendingOptions.GetBitmask() );
      }
      break;
    }
    case Dali::Renderer::Property::BLEND_FACTOR_SRC_RGB:
    {
      BlendFactor::Type sourceFactorRgb, destinationFactorRgb, sourceFactorAlpha, destinationFactorAlpha;
      GetBlendFunc( sourceFactorRgb, destinationFactorRgb, sourceFactorAlpha, destinationFactorAlpha );

      if( Scripting::GetEnumerationProperty< BlendFactor::Type >( propertyValue, BLEND_FACTOR_TABLE, BLEND_FACTOR_TABLE_COUNT, sourceFactorRgb ) )
      {
        SetBlendFunc( sourceFactorRgb, destinationFactorRgb, sourceFactorAlpha, destinationFactorAlpha );
      }
      break;
    }
    case Dali::Renderer::Property::BLEND_FACTOR_DEST_RGB:
    {
      BlendFactor::Type sourceFactorRgb, destinationFactorRgb, sourceFactorAlpha, destinationFactorAlpha;
      GetBlendFunc( sourceFactorRgb, destinationFactorRgb, sourceFactorAlpha, destinationFactorAlpha );

      if( Scripting::GetEnumerationProperty< BlendFactor::Type >( propertyValue, BLEND_FACTOR_TABLE, BLEND_FACTOR_TABLE_COUNT, destinationFactorRgb ) )
      {
        SetBlendFunc( sourceFactorRgb, destinationFactorRgb, sourceFactorAlpha, destinationFactorAlpha );
      }
      break;
    }
    case Dali::Renderer::Property::BLEND_FACTOR_SRC_ALPHA:
    {
      BlendFactor::Type sourceFactorRgb, destinationFactorRgb, sourceFactorAlpha, destinationFactorAlpha;
      GetBlendFunc( sourceFactorRgb, destinationFactorRgb, sourceFactorAlpha, destinationFactorAlpha );

      if( Scripting::GetEnumerationProperty< BlendFactor::Type >( propertyValue, BLEND_FACTOR_TABLE, BLEND_FACTOR_TABLE_COUNT, sourceFactorAlpha ) )
      {
        SetBlendFunc( sourceFactorRgb, destinationFactorRgb, sourceFactorAlpha, destinationFactorAlpha );
      }
      break;
    }
    case Dali::Renderer::Property::BLEND_FACTOR_DEST_ALPHA:
    {
      BlendFactor::Type sourceFactorRgb, destinationFactorRgb, sourceFactorAlpha, destinationFactorAlpha;
      GetBlendFunc( sourceFactorRgb, destinationFactorRgb, sourceFactorAlpha, destinationFactorAlpha );

      if( Scripting::GetEnumerationProperty< BlendFactor::Type >( propertyValue, BLEND_FACTOR_TABLE, BLEND_FACTOR_TABLE_COUNT, destinationFactorAlpha ) )
      {
        SetBlendFunc( sourceFactorRgb, destinationFactorRgb, sourceFactorAlpha, destinationFactorAlpha );
      }
      break;
    }
    case Dali::Renderer::Property::BLEND_COLOR:
    {
      Vector4 blendColor;
      if( propertyValue.Get( blendColor ) )
      {
        SetBlendColor( blendColor );
      }
      break;
    }
    case Dali::Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA:
    {
      bool preMultipled;
      if( propertyValue.Get( preMultipled ) )
      {
        EnablePreMultipliedAlpha( preMultipled );
      }
      break;
    }
    case Dali::Renderer::Property::INDEX_RANGE_FIRST:
    {
      int32_t firstElement;
      if( propertyValue.Get( firstElement ) )
      {
        SetIndexedDrawFirstElement( firstElement );
      }
      break;
    }
    case Dali::Renderer::Property::INDEX_RANGE_COUNT:
    {
      int32_t elementsCount;
      if( propertyValue.Get( elementsCount ) )
      {
        SetIndexedDrawElementsCount( elementsCount );
      }
      break;
    }
    case Dali::Renderer::Property::DEPTH_WRITE_MODE:
    {
      DepthWriteMode::Type convertedValue = mDepthWriteMode;
      if( Scripting::GetEnumerationProperty< DepthWriteMode::Type >( propertyValue, DEPTH_WRITE_MODE_TABLE, DEPTH_WRITE_MODE_TABLE_COUNT, convertedValue ) )
      {
        mDepthWriteMode = convertedValue;
        SetDepthWriteModeMessage( GetEventThreadServices(), GetRendererSceneObject(), convertedValue );
      }
      break;
    }
    case Dali::Renderer::Property::DEPTH_FUNCTION:
    {
      DepthFunction::Type convertedValue = mDepthFunction;
      if( Scripting::GetEnumerationProperty< DepthFunction::Type >( propertyValue, DEPTH_FUNCTION_TABLE, DEPTH_FUNCTION_TABLE_COUNT, convertedValue ) )
      {
        mDepthFunction = convertedValue;
        SetDepthFunctionMessage( GetEventThreadServices(), GetRendererSceneObject(), convertedValue );
      }
      break;
    }
    case Dali::Renderer::Property::DEPTH_TEST_MODE:
    {
      DepthTestMode::Type convertedValue = mDepthTestMode;
      if( Scripting::GetEnumerationProperty< DepthTestMode::Type >( propertyValue, DEPTH_TEST_MODE_TABLE, DEPTH_TEST_MODE_TABLE_COUNT, convertedValue ) )
      {
        mDepthTestMode = convertedValue;
        SetDepthTestModeMessage( GetEventThreadServices(), GetRendererSceneObject(), convertedValue );
      }
      break;
    }
    case Dali::Renderer::Property::RENDER_MODE:
    {
      RenderMode::Type convertedValue = mStencilParameters.renderMode;
      if( Scripting::GetEnumerationProperty< RenderMode::Type >( propertyValue, RENDER_MODE_TABLE, RENDER_MODE_TABLE_COUNT, convertedValue ) )
      {
        mStencilParameters.renderMode = convertedValue;
        SetRenderModeMessage( GetEventThreadServices(), GetRendererSceneObject(), convertedValue );
      }
      break;
    }
    case Dali::Renderer::Property::STENCIL_FUNCTION:
    {
      StencilFunction::Type convertedValue = mStencilParameters.stencilFunction;
      if( Scripting::GetEnumerationProperty< StencilFunction::Type >( propertyValue, STENCIL_FUNCTION_TABLE, STENCIL_FUNCTION_TABLE_COUNT, convertedValue ) )
      {
        mStencilParameters.stencilFunction = convertedValue;
        SetStencilFunctionMessage( GetEventThreadServices(), GetRendererSceneObject(), convertedValue );
      }
      break;
    }
    case Dali::Renderer::Property::STENCIL_FUNCTION_MASK:
    {
      int32_t stencilFunctionMask;
      if( propertyValue.Get( stencilFunctionMask ) )
      {
        if( stencilFunctionMask != mStencilParameters.stencilFunctionMask )
        {
          mStencilParameters.stencilFunctionMask = stencilFunctionMask;
          SetStencilFunctionMaskMessage( GetEventThreadServices(), GetRendererSceneObject(), stencilFunctionMask );
        }
      }
      break;
    }
    case Dali::Renderer::Property::STENCIL_FUNCTION_REFERENCE:
    {
      int32_t stencilFunctionReference;
      if( propertyValue.Get( stencilFunctionReference ) )
      {
        if( stencilFunctionReference != mStencilParameters.stencilFunctionReference )
        {
          mStencilParameters.stencilFunctionReference = stencilFunctionReference;
          SetStencilFunctionReferenceMessage( GetEventThreadServices(), GetRendererSceneObject(), stencilFunctionReference );
        }
      }
      break;
    }
    case Dali::Renderer::Property::STENCIL_MASK:
    {
      int32_t stencilMask;
      if( propertyValue.Get( stencilMask ) )
      {
        if( stencilMask != mStencilParameters.stencilMask )
        {
          mStencilParameters.stencilMask = stencilMask;
          SetStencilMaskMessage( GetEventThreadServices(), GetRendererSceneObject(), stencilMask );
        }
      }
      break;
    }
    case Dali::Renderer::Property::STENCIL_OPERATION_ON_FAIL:
    {
      StencilOperation::Type convertedValue = mStencilParameters.stencilOperationOnFail;
      if( Scripting::GetEnumerationProperty< StencilOperation::Type >( propertyValue, STENCIL_OPERATION_TABLE, STENCIL_OPERATION_TABLE_COUNT, convertedValue ) )
      {
        mStencilParameters.stencilOperationOnFail = convertedValue;
        SetStencilOperationOnFailMessage( GetEventThreadServices(), GetRendererSceneObject(), convertedValue );
      }
      break;
    }
    case Dali::Renderer::Property::STENCIL_OPERATION_ON_Z_FAIL:
    {
      StencilOperation::Type convertedValue = mStencilParameters.stencilOperationOnZFail;
      if( Scripting::GetEnumerationProperty< StencilOperation::Type >( propertyValue, STENCIL_OPERATION_TABLE, STENCIL_OPERATION_TABLE_COUNT, convertedValue ) )
      {
        mStencilParameters.stencilOperationOnZFail = convertedValue;
        SetStencilOperationOnZFailMessage( GetEventThreadServices(), GetRendererSceneObject(), convertedValue );
      }
      break;
    }
    case Dali::Renderer::Property::STENCIL_OPERATION_ON_Z_PASS:
    {
      StencilOperation::Type convertedValue = mStencilParameters.stencilOperationOnZPass;
      if( Scripting::GetEnumerationProperty< StencilOperation::Type >( propertyValue, STENCIL_OPERATION_TABLE, STENCIL_OPERATION_TABLE_COUNT, convertedValue ) )
      {
        mStencilParameters.stencilOperationOnZPass = convertedValue;
        SetStencilOperationOnZPassMessage( GetEventThreadServices(), GetRendererSceneObject(), convertedValue );
      }
      break;
    }
    case Dali::DevelRenderer::Property::OPACITY:
    {
      float opacity;
      if( propertyValue.Get( opacity ) )
      {
        if( !Equals( mOpacity, opacity ) )
        {
          mOpacity = opacity;
          BakeOpacityMessage( GetEventThreadServices(), GetRendererSceneObject(), mOpacity );
        }
      }
      break;
    }
    case DevelRenderer::Property::RENDERING_BEHAVIOR:
    {
      DevelRenderer::Rendering::Type convertedValue = mRenderingBehavior;
      if( Scripting::GetEnumerationProperty< DevelRenderer::Rendering::Type >( propertyValue, RENDERING_BEHAVIOR_TABLE, RENDERING_BEHAVIOR_TABLE_COUNT, convertedValue ) )
      {
        mRenderingBehavior = convertedValue;
        SetRenderingBehaviorMessage( GetEventThreadServices(), GetRendererSceneObject(), convertedValue );
      }
      break;
    }
  }
}

Property::Value Renderer::GetDefaultProperty( Property::Index index ) const
{
  Property::Value value;

  if( ! GetCachedPropertyValue( index, value ) )
  {
    // If property value is not stored in the event-side, then it must be a scene-graph only property
    GetCurrentPropertyValue( index, value );
  }

  return value;
}

Property::Value Renderer::GetDefaultPropertyCurrentValue( Property::Index index ) const
{
  Property::Value value;

  if( ! GetCurrentPropertyValue( index, value ) )
  {
    // If unable to retrieve scene-graph property value, then it must be an event-side only property
    GetCachedPropertyValue( index, value );
  }

  return value;
}

void Renderer::OnNotifyDefaultPropertyAnimation( Animation& animation, Property::Index index, const Property::Value& value, Animation::Type animationType )
{
  switch( animationType )
  {
    case Animation::TO:
    case Animation::BETWEEN:
    {
      switch( index )
      {
        case Dali::DevelRenderer::Property::OPACITY:
        {
          value.Get( mOpacity );
          break;
        }
      }
      break;
    }

    case Animation::BY:
    {
      switch( index )
      {
        case Dali::DevelRenderer::Property::OPACITY:
        {
          AdjustValue< float >( mOpacity, value );
          break;
        }
      }
      break;
    }
  }
}

const SceneGraph::PropertyBase* Renderer::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  const SceneGraph::PropertyBase* property = NULL;

  if( index == DevelRenderer::Property::OPACITY )
  {
    property = &GetRendererSceneObject().mOpacity;
  }
  if( !property )
  {
    // not our property, ask base
    property = Object::GetSceneObjectAnimatableProperty( index );
  }

  return property;
}

const PropertyInputImpl* Renderer::GetSceneObjectInputProperty( Property::Index index ) const
{
  // reuse animatable property getter, Object::GetSceneObjectInputProperty does the same so no need to call that0
  return GetSceneObjectAnimatableProperty( index );
}

Renderer::Renderer( const SceneGraph::Renderer* sceneObject )
: Object( sceneObject ),
  mDepthIndex( 0 ),
  mIndexedDrawFirstElement( 0 ),
  mIndexedDrawElementCount( 0 ),
  mStencilParameters( RenderMode::AUTO, StencilFunction::ALWAYS, 0xFF, 0x00, 0xFF, StencilOperation::KEEP, StencilOperation::KEEP, StencilOperation::KEEP ),
  mBlendingOptions(),
  mOpacity( 1.0f ),
  mDepthFunction( DepthFunction::LESS ),
  mFaceCullingMode( FaceCullingMode::NONE ),
  mBlendMode( BlendMode::AUTO ),
  mDepthWriteMode( DepthWriteMode::AUTO ),
  mDepthTestMode( DepthTestMode::AUTO ),
  mRenderingBehavior( DevelRenderer::Rendering::IF_REQUIRED ),
  mPremultipledAlphaEnabled( false )
{
}

void Renderer::SetBlendColor( const Vector4& blendColor )
{
  mBlendingOptions.SetBlendColor( blendColor );
  SetBlendColorMessage( GetEventThreadServices(), GetRendererSceneObject(), GetBlendColor() );
}

const Vector4& Renderer::GetBlendColor() const
{
  const Vector4* blendColor = mBlendingOptions.GetBlendColor();
  if( blendColor )
  {
    return *blendColor;
  }
  return Color::TRANSPARENT; // GL default
}

Renderer::~Renderer()
{
  if( EventThreadServices::IsCoreRunning() )
  {
    EventThreadServices& eventThreadServices = GetEventThreadServices();
    SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();
    RemoveRendererMessage( updateManager, GetRendererSceneObject() );

    eventThreadServices.UnregisterObject( this );
  }
}

bool Renderer::GetCachedPropertyValue( Property::Index index, Property::Value& value ) const
{
  bool valueSet = true;

  switch( index )
  {
    case Dali::Renderer::Property::DEPTH_INDEX:
    {
      value = GetDepthIndex();
      break;
    }
    case Dali::Renderer::Property::FACE_CULLING_MODE:
    {
      value = mFaceCullingMode;
      break;
    }
    case Dali::Renderer::Property::BLEND_MODE:
    {
      value = mBlendMode;
      break;
    }
    case Dali::Renderer::Property::BLEND_EQUATION_RGB:
    {
      value = static_cast<int32_t>( mBlendingOptions.GetBlendEquationRgb() );
      break;
    }
    case Dali::Renderer::Property::BLEND_EQUATION_ALPHA:
    {
      value = static_cast<int32_t>( mBlendingOptions.GetBlendEquationAlpha() );
      break;
    }
    case Dali::Renderer::Property::BLEND_FACTOR_SRC_RGB:
    {
      BlendFactor::Type srcFactorRgb;
      BlendFactor::Type destFactorRgb;
      BlendFactor::Type srcFactorAlpha;
      BlendFactor::Type destFactorAlpha;
      GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
      value = static_cast<int32_t>( srcFactorRgb );
      break;
    }
    case Dali::Renderer::Property::BLEND_FACTOR_DEST_RGB:
    {
      BlendFactor::Type srcFactorRgb;
      BlendFactor::Type destFactorRgb;
      BlendFactor::Type srcFactorAlpha;
      BlendFactor::Type destFactorAlpha;
      GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
      value = static_cast<int32_t>( destFactorRgb );
      break;
    }
    case Dali::Renderer::Property::BLEND_FACTOR_SRC_ALPHA:
    {
      BlendFactor::Type srcFactorRgb;
      BlendFactor::Type destFactorRgb;
      BlendFactor::Type srcFactorAlpha;
      BlendFactor::Type destFactorAlpha;
      GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
      value = static_cast<int32_t>( srcFactorAlpha );
      break;
    }
    case Dali::Renderer::Property::BLEND_FACTOR_DEST_ALPHA:
    {
      BlendFactor::Type srcFactorRgb;
      BlendFactor::Type destFactorRgb;
      BlendFactor::Type srcFactorAlpha;
      BlendFactor::Type destFactorAlpha;
      GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
      value = static_cast<int32_t>( destFactorAlpha );
      break;
    }
    case Dali::Renderer::Property::BLEND_COLOR:
    {
      value = GetBlendColor();
      break;
    }
    case Dali::Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA:
    {
      value = IsPreMultipliedAlphaEnabled();
      break;
    }
    case Dali::Renderer::Property::INDEX_RANGE_FIRST:
    {
      value = static_cast<int32_t>( mIndexedDrawFirstElement );
      break;
    }
    case Dali::Renderer::Property::INDEX_RANGE_COUNT:
    {
      value = static_cast<int32_t>( mIndexedDrawElementCount );
      break;
    }
    case Dali::Renderer::Property::DEPTH_WRITE_MODE:
    {
      value = mDepthWriteMode;
      break;
    }
    case Dali::Renderer::Property::DEPTH_FUNCTION:
    {
      value = mDepthFunction;
      break;
    }
    case Dali::Renderer::Property::DEPTH_TEST_MODE:
    {
      value = mDepthTestMode;
      break;
    }
    case Dali::Renderer::Property::STENCIL_FUNCTION:
    {
      value = mStencilParameters.stencilFunction;
      break;
    }
    case Dali::Renderer::Property::STENCIL_FUNCTION_MASK:
    {
      value = mStencilParameters.stencilFunctionMask;
      break;
    }
    case Dali::Renderer::Property::STENCIL_FUNCTION_REFERENCE:
    {
      value = mStencilParameters.stencilFunctionReference;
      break;
    }
    case Dali::Renderer::Property::STENCIL_MASK:
    {
      value = mStencilParameters.stencilMask;
      break;
    }
    case Dali::Renderer::Property::RENDER_MODE:
    {
      value = mStencilParameters.renderMode;
      break;
    }
    case Dali::Renderer::Property::STENCIL_OPERATION_ON_FAIL:
    {
      value = mStencilParameters.stencilOperationOnFail;
      break;
    }
    case Dali::Renderer::Property::STENCIL_OPERATION_ON_Z_FAIL:
    {
      value = mStencilParameters.stencilOperationOnZFail;
      break;
    }
    case Dali::Renderer::Property::STENCIL_OPERATION_ON_Z_PASS:
    {
      value = mStencilParameters.stencilOperationOnZPass;
      break;
    }
    case Dali::DevelRenderer::Property::OPACITY:
    {
      value = mOpacity;
      break;
    }
    case Dali::DevelRenderer::Property::RENDERING_BEHAVIOR:
    {
      value = mRenderingBehavior;
      break;
    }
    default:
    {
      // Must be a scene-graph only property
      valueSet = false;
      break;
    }
  }

  return valueSet;
}

bool Renderer::GetCurrentPropertyValue( Property::Index index, Property::Value& value  ) const
{
  bool valueSet = true;
  const SceneGraph::Renderer& sceneObject = GetRendererSceneObject();

  switch( index )
  {
    case Dali::Renderer::Property::DEPTH_INDEX:
    {
      value = sceneObject.GetDepthIndex();
      break;
    }
    case Dali::Renderer::Property::FACE_CULLING_MODE:
    {
      value = sceneObject.GetFaceCullingMode();
      break;
    }
    case Dali::Renderer::Property::BLEND_MODE:
    {
      value = sceneObject.GetBlendMode();
      break;
    }
    case Dali::Renderer::Property::BLEND_EQUATION_RGB:
    {
      uint32_t bitMask = sceneObject.GetBlendingOptions();
      BlendingOptions blendingOptions;
      blendingOptions.SetBitmask( bitMask );
      value = static_cast<int32_t>( blendingOptions.GetBlendEquationRgb() );
      break;
    }
    case Dali::Renderer::Property::BLEND_EQUATION_ALPHA:
    {
      uint32_t bitMask = sceneObject.GetBlendingOptions();
      BlendingOptions blendingOptions;
      blendingOptions.SetBitmask( bitMask );
      value = static_cast<int32_t>( blendingOptions.GetBlendEquationAlpha() );
      break;
    }
    case Dali::Renderer::Property::BLEND_FACTOR_SRC_RGB:
    {
      uint32_t bitMask = sceneObject.GetBlendingOptions();
      BlendingOptions blendingOptions;
      blendingOptions.SetBitmask( bitMask );
      value = static_cast<int32_t>( blendingOptions.GetBlendSrcFactorRgb() );
      break;
    }
    case Dali::Renderer::Property::BLEND_FACTOR_DEST_RGB:
    {
      uint32_t bitMask = sceneObject.GetBlendingOptions();
      BlendingOptions blendingOptions;
      blendingOptions.SetBitmask( bitMask );
      value = static_cast<int32_t>( blendingOptions.GetBlendDestFactorRgb() );
      break;
    }
    case Dali::Renderer::Property::BLEND_FACTOR_SRC_ALPHA:
    {
      uint32_t bitMask = sceneObject.GetBlendingOptions();
      BlendingOptions blendingOptions;
      blendingOptions.SetBitmask( bitMask );
      value = static_cast<int32_t>( blendingOptions.GetBlendSrcFactorAlpha() );
      break;
    }
    case Dali::Renderer::Property::BLEND_FACTOR_DEST_ALPHA:
    {
      uint32_t bitMask = sceneObject.GetBlendingOptions();
      BlendingOptions blendingOptions;
      blendingOptions.SetBitmask( bitMask );
      value = static_cast<int32_t>( blendingOptions.GetBlendDestFactorAlpha() );
      break;
    }
    case Dali::Renderer::Property::BLEND_COLOR:
    {
      value = sceneObject.GetBlendColor();
      break;
    }
    case Dali::Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA:
    {
      value = sceneObject.IsPreMultipliedAlphaEnabled();
      break;
    }
    case Dali::Renderer::Property::INDEX_RANGE_FIRST:
    {
      value = static_cast<int32_t>( sceneObject.GetIndexedDrawFirstElement() );
      break;
    }
    case Dali::Renderer::Property::INDEX_RANGE_COUNT:
    {
      value = static_cast<int32_t>( sceneObject.GetIndexedDrawElementsCount() );
      break;
    }
    case Dali::Renderer::Property::DEPTH_WRITE_MODE:
    {
      value = sceneObject.GetDepthWriteMode();
      break;
    }
    case Dali::Renderer::Property::DEPTH_FUNCTION:
    {
      value = sceneObject.GetDepthFunction();
      break;
    }
    case Dali::Renderer::Property::DEPTH_TEST_MODE:
    {
      value = sceneObject.GetDepthTestMode();
      break;
    }
    case Dali::Renderer::Property::STENCIL_FUNCTION:
    {
      Render::Renderer::StencilParameters stencilParameters = sceneObject.GetStencilParameters();
      value = stencilParameters.stencilFunction;
      break;
    }
    case Dali::Renderer::Property::STENCIL_FUNCTION_MASK:
    {
      Render::Renderer::StencilParameters stencilParameters = sceneObject.GetStencilParameters();
      value = stencilParameters.stencilFunctionMask;
      break;
    }
    case Dali::Renderer::Property::STENCIL_FUNCTION_REFERENCE:
    {
      Render::Renderer::StencilParameters stencilParameters = sceneObject.GetStencilParameters();
      value = stencilParameters.stencilFunctionReference;
      break;
    }
    case Dali::Renderer::Property::STENCIL_MASK:
    {
      Render::Renderer::StencilParameters stencilParameters = sceneObject.GetStencilParameters();
      value = stencilParameters.stencilMask;
      break;
    }
    case Dali::Renderer::Property::RENDER_MODE:
    {
      Render::Renderer::StencilParameters stencilParameters = sceneObject.GetStencilParameters();
      value = stencilParameters.renderMode;
      break;
    }
    case Dali::Renderer::Property::STENCIL_OPERATION_ON_FAIL:
    {
      Render::Renderer::StencilParameters stencilParameters = sceneObject.GetStencilParameters();
      value = stencilParameters.stencilOperationOnFail;
      break;
    }
    case Dali::Renderer::Property::STENCIL_OPERATION_ON_Z_FAIL:
    {
      Render::Renderer::StencilParameters stencilParameters = sceneObject.GetStencilParameters();
      value = stencilParameters.stencilOperationOnZFail;
      break;
    }
    case Dali::Renderer::Property::STENCIL_OPERATION_ON_Z_PASS:
    {
      Render::Renderer::StencilParameters stencilParameters = sceneObject.GetStencilParameters();
      value = stencilParameters.stencilOperationOnZPass;
      break;
    }
    case Dali::DevelRenderer::Property::OPACITY:
    {
      value = sceneObject.GetOpacity( GetEventThreadServices().GetEventBufferIndex() );
      break;
    }
    case Dali::DevelRenderer::Property::RENDERING_BEHAVIOR:
    {
      value = sceneObject.GetRenderingBehavior();
      break;
    }
    default:
    {
      // Must be an event-side only property
      valueSet = false;
      break;
    }
  }

  return valueSet;
}

} // namespace Internal

} // namespace Dali

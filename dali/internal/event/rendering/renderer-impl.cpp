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
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/event/common/object-impl-helper.h> // Dali::Internal::ObjectHelper
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
DALI_PROPERTY_TABLE_END( DEFAULT_RENDERER_PROPERTY_START_INDEX )

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

const ObjectImplHelper<DEFAULT_PROPERTY_COUNT> RENDERER_IMPL = { DEFAULT_PROPERTY_DETAILS };

BaseHandle Create()
{
  return Dali::BaseHandle();
}

TypeRegistration mType( typeid( Dali::Renderer ), typeid( Dali::Handle ), Create );

} // unnamed namespace

RendererPtr Renderer::New()
{
  RendererPtr rendererPtr( new Renderer() );
  rendererPtr->Initialize();
  return rendererPtr;
}

void Renderer::SetGeometry( Geometry& geometry )
{
  mGeometry = &geometry;

  const Render::Geometry* geometrySceneObject = geometry.GetRenderObject();
  SetGeometryMessage( GetEventThreadServices(), *mSceneObject, *geometrySceneObject );
}

Geometry* Renderer::GetGeometry() const
{
  return mGeometry.Get();
}

void Renderer::SetTextures( TextureSet& textureSet )
{
  mTextureSet = &textureSet;
  const SceneGraph::TextureSet* textureSetSceneObject = textureSet.GetTextureSetSceneObject();
  SetTexturesMessage( GetEventThreadServices(), *mSceneObject, *textureSetSceneObject );
}

TextureSet* Renderer::GetTextures() const
{
  return mTextureSet.Get();
}

void Renderer::SetShader( Shader& shader )
{
  mShader = &shader;
  SceneGraph::Shader& sceneGraphShader = *shader.GetShaderSceneObject();
  SceneGraph::SetShaderMessage( GetEventThreadServices(), *mSceneObject, sceneGraphShader );
}

Shader* Renderer::GetShader() const
{
  return mShader.Get();
}

void Renderer::SetDepthIndex( int depthIndex )
{
  if ( mDepthIndex != depthIndex )
  {
    mDepthIndex = depthIndex;
    SetDepthIndexMessage( GetEventThreadServices(), *mSceneObject, depthIndex );
  }
}

int Renderer::GetDepthIndex() const
{
  return mDepthIndex;
}

void Renderer::SetBlendMode( BlendMode::Type mode )
{
  if( mBlendMode != mode )
  {
    mBlendMode = mode;

    SetBlendModeMessage( GetEventThreadServices(), *mSceneObject, mBlendMode );
  }
}

BlendMode::Type Renderer::GetBlendMode() const
{
  return mBlendMode;
}

void Renderer::SetBlendFunc( BlendFactor::Type srcFactorRgba, BlendFactor::Type destFactorRgba )
{
  mBlendingOptions.SetBlendFunc( srcFactorRgba, destFactorRgba, srcFactorRgba, destFactorRgba );
  SetBlendingOptionsMessage( GetEventThreadServices(), *mSceneObject, mBlendingOptions.GetBitmask() );
}

void Renderer::SetBlendFunc( BlendFactor::Type srcFactorRgb,
                             BlendFactor::Type destFactorRgb,
                             BlendFactor::Type srcFactorAlpha,
                             BlendFactor::Type destFactorAlpha )
{
  mBlendingOptions.SetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
  SetBlendingOptionsMessage( GetEventThreadServices(), *mSceneObject, mBlendingOptions.GetBitmask() );
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
  SetBlendingOptionsMessage( GetEventThreadServices(), *mSceneObject, mBlendingOptions.GetBitmask() );
}

void Renderer::SetBlendEquation( BlendEquation::Type equationRgb,
                                 BlendEquation::Type equationAlpha )
{
  mBlendingOptions.SetBlendEquation( equationRgb, equationAlpha );
  SetBlendingOptionsMessage( GetEventThreadServices(), *mSceneObject, mBlendingOptions.GetBitmask() );
}

void Renderer::GetBlendEquation( BlendEquation::Type& equationRgb,
                                 BlendEquation::Type& equationAlpha ) const
{
  // These are not animatable, the cached values are up-to-date.
  equationRgb   = mBlendingOptions.GetBlendEquationRgb();
  equationAlpha = mBlendingOptions.GetBlendEquationAlpha();
}

void Renderer::SetIndexedDrawFirstElement( size_t firstElement )
{
  if( firstElement != mIndexedDrawFirstElement )
  {
    mIndexedDrawFirstElement = firstElement;
    SetIndexedDrawFirstElementMessage( GetEventThreadServices(), *mSceneObject, mIndexedDrawFirstElement );
  }
}

void Renderer::SetIndexedDrawElementsCount( size_t elementsCount )
{
  if( elementsCount != mIndexedDrawElementCount )
  {
    mIndexedDrawElementCount = elementsCount;
    SetIndexedDrawElementsCountMessage( GetEventThreadServices(), *mSceneObject, mIndexedDrawElementCount );
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
    mPremultipledAlphaEnabled = preMultipled;
    SetEnablePreMultipliedAlphaMessage( GetEventThreadServices(), *mSceneObject, mPremultipledAlphaEnabled );
  }
}

bool Renderer::IsPreMultipliedAlphaEnabled() const
{
  return mPremultipledAlphaEnabled;
}

SceneGraph::Renderer* Renderer::GetRendererSceneObject()
{
  return mSceneObject;
}

unsigned int Renderer::GetDefaultPropertyCount() const
{
  return RENDERER_IMPL.GetDefaultPropertyCount();
}

void Renderer::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  RENDERER_IMPL.GetDefaultPropertyIndices( indices );
}

const char* Renderer::GetDefaultPropertyName(Property::Index index) const
{
  return RENDERER_IMPL.GetDefaultPropertyName( index );
}

Property::Index Renderer::GetDefaultPropertyIndex( const std::string& name ) const
{
  return RENDERER_IMPL.GetDefaultPropertyIndex( name );
}

bool Renderer::IsDefaultPropertyWritable( Property::Index index ) const
{
  return RENDERER_IMPL.IsDefaultPropertyWritable( index );
}

bool Renderer::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  return RENDERER_IMPL.IsDefaultPropertyAnimatable( index );
}

bool Renderer::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  return RENDERER_IMPL.IsDefaultPropertyAConstraintInput( index );
}

Property::Type Renderer::GetDefaultPropertyType( Property::Index index ) const
{
  return RENDERER_IMPL.GetDefaultPropertyType( index );
}

void Renderer::SetDefaultProperty( Property::Index index,
                                   const Property::Value& propertyValue )
{
  switch( index )
  {
    case Dali::Renderer::Property::DEPTH_INDEX:
    {
      SetDepthIndex( propertyValue.Get<int>() );
      break;
    }
    case Dali::Renderer::Property::FACE_CULLING_MODE:
    {
      FaceCullingMode::Type convertedValue = mFaceCullingMode;
      if( Scripting::GetEnumerationProperty< FaceCullingMode::Type >( propertyValue, FACE_CULLING_MODE_TABLE, FACE_CULLING_MODE_TABLE_COUNT, convertedValue ) )
      {
        mFaceCullingMode = convertedValue;
        SetFaceCullingModeMessage( GetEventThreadServices(), *mSceneObject, convertedValue );
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
        SetBlendingOptionsMessage( GetEventThreadServices(), *mSceneObject, mBlendingOptions.GetBitmask() );
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
        SetBlendingOptionsMessage( GetEventThreadServices(), *mSceneObject, mBlendingOptions.GetBitmask() );
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
      int firstElement;
      if( propertyValue.Get( firstElement ) )
      {
        SetIndexedDrawFirstElement( firstElement );
      }
      break;
    }
    case Dali::Renderer::Property::INDEX_RANGE_COUNT:
    {
      int elementsCount;
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
        SetDepthWriteModeMessage( GetEventThreadServices(), *mSceneObject, convertedValue );
      }
      break;
    }
    case Dali::Renderer::Property::DEPTH_FUNCTION:
    {
      DepthFunction::Type convertedValue = mDepthFunction;
      if( Scripting::GetEnumerationProperty< DepthFunction::Type >( propertyValue, DEPTH_FUNCTION_TABLE, DEPTH_FUNCTION_TABLE_COUNT, convertedValue ) )
      {
        mDepthFunction = convertedValue;
        SetDepthFunctionMessage( GetEventThreadServices(), *mSceneObject, convertedValue );
      }
      break;
    }
    case Dali::Renderer::Property::DEPTH_TEST_MODE:
    {
      DepthTestMode::Type convertedValue = mDepthTestMode;
      if( Scripting::GetEnumerationProperty< DepthTestMode::Type >( propertyValue, DEPTH_TEST_MODE_TABLE, DEPTH_TEST_MODE_TABLE_COUNT, convertedValue ) )
      {
        mDepthTestMode = convertedValue;
        SetDepthTestModeMessage( GetEventThreadServices(), *mSceneObject, convertedValue );
      }
      break;
    }
    case Dali::Renderer::Property::RENDER_MODE:
    {
      RenderMode::Type convertedValue = mStencilParameters.renderMode;
      if( Scripting::GetEnumerationProperty< RenderMode::Type >( propertyValue, RENDER_MODE_TABLE, RENDER_MODE_TABLE_COUNT, convertedValue ) )
      {
        mStencilParameters.renderMode = convertedValue;
        SetRenderModeMessage( GetEventThreadServices(), *mSceneObject, convertedValue );
      }
      break;
    }
    case Dali::Renderer::Property::STENCIL_FUNCTION:
    {
      StencilFunction::Type convertedValue = mStencilParameters.stencilFunction;
      if( Scripting::GetEnumerationProperty< StencilFunction::Type >( propertyValue, STENCIL_FUNCTION_TABLE, STENCIL_FUNCTION_TABLE_COUNT, convertedValue ) )
      {
        mStencilParameters.stencilFunction = convertedValue;
        SetStencilFunctionMessage( GetEventThreadServices(), *mSceneObject, convertedValue );
      }
      break;
    }
    case Dali::Renderer::Property::STENCIL_FUNCTION_MASK:
    {
      int stencilFunctionMask;
      if( propertyValue.Get( stencilFunctionMask ) )
      {
        if( stencilFunctionMask != mStencilParameters.stencilFunctionMask )
        {
          mStencilParameters.stencilFunctionMask = stencilFunctionMask;
          SetStencilFunctionMaskMessage( GetEventThreadServices(), *mSceneObject, stencilFunctionMask );
        }
      }
      break;
    }
    case Dali::Renderer::Property::STENCIL_FUNCTION_REFERENCE:
    {
      int stencilFunctionReference;
      if( propertyValue.Get( stencilFunctionReference ) )
      {
        if( stencilFunctionReference != mStencilParameters.stencilFunctionReference )
        {
          mStencilParameters.stencilFunctionReference = stencilFunctionReference;
          SetStencilFunctionReferenceMessage( GetEventThreadServices(), *mSceneObject, stencilFunctionReference );
        }
      }
      break;
    }
    case Dali::Renderer::Property::STENCIL_MASK:
    {
      int stencilMask;
      if( propertyValue.Get( stencilMask ) )
      {
        if( stencilMask != mStencilParameters.stencilMask )
        {
          mStencilParameters.stencilMask = stencilMask;
          SetStencilMaskMessage( GetEventThreadServices(), *mSceneObject, stencilMask );
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
        SetStencilOperationOnFailMessage( GetEventThreadServices(), *mSceneObject, convertedValue );
      }
      break;
    }
    case Dali::Renderer::Property::STENCIL_OPERATION_ON_Z_FAIL:
    {
      StencilOperation::Type convertedValue = mStencilParameters.stencilOperationOnZFail;
      if( Scripting::GetEnumerationProperty< StencilOperation::Type >( propertyValue, STENCIL_OPERATION_TABLE, STENCIL_OPERATION_TABLE_COUNT, convertedValue ) )
      {
        mStencilParameters.stencilOperationOnZFail = convertedValue;
        SetStencilOperationOnZFailMessage( GetEventThreadServices(), *mSceneObject, convertedValue );
      }
      break;
    }
    case Dali::Renderer::Property::STENCIL_OPERATION_ON_Z_PASS:
    {
      StencilOperation::Type convertedValue = mStencilParameters.stencilOperationOnZPass;
      if( Scripting::GetEnumerationProperty< StencilOperation::Type >( propertyValue, STENCIL_OPERATION_TABLE, STENCIL_OPERATION_TABLE_COUNT, convertedValue ) )
      {
        mStencilParameters.stencilOperationOnZPass = convertedValue;
        SetStencilOperationOnZPassMessage( GetEventThreadServices(), *mSceneObject, convertedValue );
      }
      break;
    }
  }
}

void Renderer::SetSceneGraphProperty( Property::Index index,
                                      const PropertyMetadata& entry,
                                      const Property::Value& value )
{
  RENDERER_IMPL.SetSceneGraphProperty( GetEventThreadServices(), this, index, entry, value );
  OnPropertySet(index, value);
}

Property::Value Renderer::GetDefaultProperty( Property::Index index ) const
{
  Property::Value value;
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
      value = static_cast<int>( mBlendingOptions.GetBlendEquationRgb() );
      break;
    }
    case Dali::Renderer::Property::BLEND_EQUATION_ALPHA:
    {
      value = static_cast<int>( mBlendingOptions.GetBlendEquationAlpha() );
      break;
    }
    case Dali::Renderer::Property::BLEND_FACTOR_SRC_RGB:
    {
      BlendFactor::Type srcFactorRgb;
      BlendFactor::Type destFactorRgb;
      BlendFactor::Type srcFactorAlpha;
      BlendFactor::Type destFactorAlpha;
      GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
      value = static_cast<int>( srcFactorRgb );
      break;
    }
    case Dali::Renderer::Property::BLEND_FACTOR_DEST_RGB:
    {
      BlendFactor::Type srcFactorRgb;
      BlendFactor::Type destFactorRgb;
      BlendFactor::Type srcFactorAlpha;
      BlendFactor::Type destFactorAlpha;
      GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
      value = static_cast<int>( destFactorRgb );
      break;
    }
    case Dali::Renderer::Property::BLEND_FACTOR_SRC_ALPHA:
    {
      BlendFactor::Type srcFactorRgb;
      BlendFactor::Type destFactorRgb;
      BlendFactor::Type srcFactorAlpha;
      BlendFactor::Type destFactorAlpha;
      GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
      value = static_cast<int>( srcFactorAlpha );
      break;
    }
    case Dali::Renderer::Property::BLEND_FACTOR_DEST_ALPHA:
    {
      BlendFactor::Type srcFactorRgb;
      BlendFactor::Type destFactorRgb;
      BlendFactor::Type srcFactorAlpha;
      BlendFactor::Type destFactorAlpha;
      GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
      value = static_cast<int>( destFactorAlpha );
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
      value = static_cast<int>( mIndexedDrawFirstElement );
      break;
    }
    case Dali::Renderer::Property::INDEX_RANGE_COUNT:
    {
      value = static_cast<int>( mIndexedDrawElementCount );
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
  }
  return value;
}

Property::Value Renderer::GetDefaultPropertyCurrentValue( Property::Index index ) const
{
  return GetDefaultProperty( index ); // Event-side only properties
}

const SceneGraph::PropertyOwner* Renderer::GetPropertyOwner() const
{
  return mSceneObject;
}

const SceneGraph::PropertyOwner* Renderer::GetSceneObject() const
{
  return mSceneObject;
}

const SceneGraph::PropertyBase* Renderer::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  DALI_ASSERT_ALWAYS( IsPropertyAnimatable(index) && "Property is not animatable" );
  const SceneGraph::PropertyBase* property = NULL;

  property = RENDERER_IMPL.GetRegisteredSceneGraphProperty(
    this,
    &Renderer::FindAnimatableProperty,
    &Renderer::FindCustomProperty,
    index );

  return property;
}

const PropertyInputImpl* Renderer::GetSceneObjectInputProperty( Property::Index index ) const
{
  const PropertyInputImpl* property = NULL;

  const SceneGraph::PropertyBase* baseProperty =
    RENDERER_IMPL.GetRegisteredSceneGraphProperty( this,
                                                   &Renderer::FindAnimatableProperty,
                                                   &Renderer::FindCustomProperty,
                                                   index );
  property = static_cast<const PropertyInputImpl*>( baseProperty );

  return property;
}

int Renderer::GetPropertyComponentIndex( Property::Index index ) const
{
  return Property::INVALID_COMPONENT_INDEX;
}

Renderer::Renderer()
: mSceneObject(NULL ),
  mDepthIndex( 0 ),
  mIndexedDrawFirstElement( 0 ),
  mIndexedDrawElementCount( 0 ),
  mStencilParameters( RenderMode::AUTO, StencilFunction::ALWAYS, 0xFF, 0x00, 0xFF, StencilOperation::KEEP, StencilOperation::KEEP, StencilOperation::KEEP ),
  mBlendingOptions(),
  mDepthFunction( DepthFunction::LESS ),
  mFaceCullingMode( FaceCullingMode::NONE ),
  mBlendMode( BlendMode::AUTO ),
  mDepthWriteMode( DepthWriteMode::AUTO ),
  mDepthTestMode( DepthTestMode::AUTO ),
  mPremultipledAlphaEnabled( false )
{
}

void Renderer::Initialize()
{
  EventThreadServices& eventThreadServices = GetEventThreadServices();
  SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();

  mSceneObject = SceneGraph::Renderer::New();
  OwnerPointer< SceneGraph::Renderer > transferOwnership( mSceneObject );
  AddRendererMessage( updateManager, transferOwnership );

  eventThreadServices.RegisterObject( this );
}

void Renderer::SetBlendColor( const Vector4& blendColor )
{
  mBlendingOptions.SetBlendColor( blendColor );
  SetBlendColorMessage( GetEventThreadServices(), *mSceneObject, GetBlendColor() );
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
    RemoveRendererMessage( updateManager, *mSceneObject );

    eventThreadServices.UnregisterObject( this );
  }
}

} // namespace Internal
} // namespace Dali

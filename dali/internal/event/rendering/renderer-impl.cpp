/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/event/common/object-impl-helper.h> // Dali::Internal::ObjectHelper
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END
#include <dali/internal/event/common/property-input-impl.h>
#include <dali/internal/update/rendering/scene-graph-renderer.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/render/renderers/render-geometry.h>

namespace Dali
{
namespace Internal
{

namespace
{

/**
 *            |name                              |type     |writable|animatable|constraint-input|enum for index-checking|
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
DALI_PROPERTY( "stencilFunction",                 INTEGER,   true, false,  false, Dali::Renderer::Property::STENCIL_FUNCTION )
DALI_PROPERTY( "stencilFunctionMask",             INTEGER,   true, false,  false, Dali::Renderer::Property::STENCIL_FUNCTION_MASK )
DALI_PROPERTY( "stencilFunctionReference",        INTEGER,   true, false,  false, Dali::Renderer::Property::STENCIL_FUNCTION_REFERENCE )
DALI_PROPERTY( "stencilMask",                     INTEGER,   true, false,  false, Dali::Renderer::Property::STENCIL_MASK )
DALI_PROPERTY( "stencilMode",                     INTEGER,   true, false,  false, Dali::Renderer::Property::STENCIL_MODE )
DALI_PROPERTY( "stencilOperationOnFail",          INTEGER,   true, false,  false, Dali::Renderer::Property::STENCIL_OPERATION_ON_FAIL )
DALI_PROPERTY( "stencilOperationOnZFail",         INTEGER,   true, false,  false, Dali::Renderer::Property::STENCIL_OPERATION_ON_Z_FAIL )
DALI_PROPERTY( "stencilOperationOnZPass",         INTEGER,   true, false,  false, Dali::Renderer::Property::STENCIL_OPERATION_ON_Z_PASS )
DALI_PROPERTY( "writeToColorBuffer",              BOOLEAN,   true, false,  false, Dali::Renderer::Property::WRITE_TO_COLOR_BUFFER )
DALI_PROPERTY_TABLE_END( DEFAULT_RENDERER_PROPERTY_START_INDEX )

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
  mTextureSetConnector.Set( textureSet, OnStage() );
  const SceneGraph::TextureSet* textureSetSceneObject = textureSet.GetTextureSetSceneObject();
  SetTexturesMessage( GetEventThreadServices(), *mSceneObject, *textureSetSceneObject );
}

TextureSet* Renderer::GetTextures() const
{
  return mTextureSetConnector.Get().Get();
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

void Renderer::SetFaceCullingMode( FaceCullingMode::Type cullingMode )
{
  if( mFaceCullingMode != cullingMode )
  {
    mFaceCullingMode = cullingMode;

    SetFaceCullingModeMessage( GetEventThreadServices(), *mSceneObject, mFaceCullingMode );
  }
}

FaceCullingMode::Type Renderer::GetFaceCullingMode()
{
  return mFaceCullingMode;
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

void Renderer::SetBlendColor( const Vector4& color )
{
  if( !mBlendColor )
  {
    mBlendColor = new Vector4();
  }
  if( *mBlendColor != color )
  {
    *mBlendColor = color;
    SetBlendColorMessage( GetEventThreadServices(), *mSceneObject, *mBlendColor );
  }
}

Vector4 Renderer::GetBlendColor() const
{
  if( mBlendColor )
  {
    return *mBlendColor;
  }
  return Color::TRANSPARENT; // GL default
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
      SetBlendFunc( BlendFactor::ONE, BlendFactor::ONE_MINUS_SRC_ALPHA, BlendFactor::ONE, BlendFactor::ONE );
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
      int faceCullingMode;
      if( propertyValue.Get( faceCullingMode ) )
      {
        SetFaceCullingMode( FaceCullingMode::Type( faceCullingMode ) );
      }
      break;
    }
    case Dali::Renderer::Property::BLEND_MODE:
    {
      int blendingMode;
      if( propertyValue.Get( blendingMode ) )
      {
        SetBlendMode( BlendMode::Type( blendingMode ) );
      }
      break;
    }
    case Dali::Renderer::Property::BLEND_EQUATION_RGB:
    {
      int blendingEquation;
      if( propertyValue.Get( blendingEquation ) )
      {
        BlendEquation::Type alphaEquation = mBlendingOptions.GetBlendEquationAlpha();
        mBlendingOptions.SetBlendEquation( static_cast<BlendEquation::Type>( blendingEquation ), alphaEquation );
        SetBlendingOptionsMessage( GetEventThreadServices(), *mSceneObject, mBlendingOptions.GetBitmask() );
      }
      break;
    }
    case Dali::Renderer::Property::BLEND_EQUATION_ALPHA:
    {
      int blendingEquation;
      if( propertyValue.Get( blendingEquation ) )
      {
        BlendEquation::Type rgbEquation = mBlendingOptions.GetBlendEquationRgb();
        mBlendingOptions.SetBlendEquation( rgbEquation, static_cast<BlendEquation::Type>( blendingEquation ) );
        SetBlendingOptionsMessage( GetEventThreadServices(), *mSceneObject, mBlendingOptions.GetBitmask() );
      }
      break;
    }
    case Dali::Renderer::Property::BLEND_FACTOR_SRC_RGB:
    {
      int blendingFactor;
      if( propertyValue.Get( blendingFactor ) )
      {
        BlendFactor::Type srcFactorRgb;
        BlendFactor::Type destFactorRgb;
        BlendFactor::Type srcFactorAlpha;
        BlendFactor::Type destFactorAlpha;
        GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
        SetBlendFunc( static_cast<BlendFactor::Type>( blendingFactor ),
            destFactorRgb,
            srcFactorAlpha,
            destFactorAlpha );
      }
      break;
    }
    case Dali::Renderer::Property::BLEND_FACTOR_DEST_RGB:
    {
      int blendingFactor;
      if( propertyValue.Get( blendingFactor ) )
      {
        BlendFactor::Type srcFactorRgb;
        BlendFactor::Type destFactorRgb;
        BlendFactor::Type srcFactorAlpha;
        BlendFactor::Type destFactorAlpha;
        GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
        SetBlendFunc( srcFactorRgb,
            static_cast<BlendFactor::Type>( blendingFactor ),
            srcFactorAlpha,
            destFactorAlpha );
      }
      break;
    }
    case Dali::Renderer::Property::BLEND_FACTOR_SRC_ALPHA:
    {
      int blendingFactor;
      if( propertyValue.Get( blendingFactor ) )
      {
        BlendFactor::Type srcFactorRgb;
        BlendFactor::Type destFactorRgb;
        BlendFactor::Type srcFactorAlpha;
        BlendFactor::Type destFactorAlpha;
        GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
        SetBlendFunc( srcFactorRgb,
            destFactorRgb,
            static_cast<BlendFactor::Type>( blendingFactor ),
            destFactorAlpha );
      }
      break;
    }
    case Dali::Renderer::Property::BLEND_FACTOR_DEST_ALPHA:
    {
      int blendingFactor;
      if( propertyValue.Get( blendingFactor ) )
      {
        BlendFactor::Type srcFactorRgb;
        BlendFactor::Type destFactorRgb;
        BlendFactor::Type srcFactorAlpha;
        BlendFactor::Type destFactorAlpha;
        GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
        SetBlendFunc( srcFactorRgb,
            destFactorRgb,
            srcFactorAlpha,
            static_cast<BlendFactor::Type>( blendingFactor ) );
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
      int value;
      propertyValue.Get( value );
      DepthWriteMode::Type mode = static_cast<DepthWriteMode::Type>(value);
      if( mode != mDepthWriteMode )
      {
        mDepthWriteMode = mode;
        SetDepthWriteModeMessage( GetEventThreadServices(), *mSceneObject, mode );
      }
      break;
    }
    case Dali::Renderer::Property::DEPTH_FUNCTION:
    {
      int value;
      propertyValue.Get( value );
      DepthFunction::Type depthFunction = static_cast<DepthFunction::Type>(value);
      if( depthFunction != mDepthFunction )
      {
        mDepthFunction = depthFunction;
        SetDepthFunctionMessage( GetEventThreadServices(), *mSceneObject, depthFunction );
      }
      break;
    }
    case Dali::Renderer::Property::DEPTH_TEST_MODE:
    {
      int value;
      propertyValue.Get( value );
      DepthTestMode::Type mode = static_cast<DepthTestMode::Type>(value);
      if( mode != mDepthTestMode )
      {
        mDepthTestMode = mode;
        SetDepthTestModeMessage( GetEventThreadServices(), *mSceneObject, mode );
      }
      break;
    }
    default:
    {
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
      if( mBlendColor )
      {
        value = *mBlendColor;
      }
      else
      {
        value = Color::TRANSPARENT;
      }
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
  }
  return value;
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

  if( OnStage() )
  {
    property = RENDERER_IMPL.GetRegisteredSceneGraphProperty(
      this,
      &Renderer::FindAnimatableProperty,
      &Renderer::FindCustomProperty,
      index );
  }

  return property;
}

const PropertyInputImpl* Renderer::GetSceneObjectInputProperty( Property::Index index ) const
{
  const PropertyInputImpl* property = NULL;

  if( OnStage() )
  {
    const SceneGraph::PropertyBase* baseProperty =
      RENDERER_IMPL.GetRegisteredSceneGraphProperty( this,
                                                     &Renderer::FindAnimatableProperty,
                                                     &Renderer::FindCustomProperty,
                                                     index );
    property = static_cast<const PropertyInputImpl*>( baseProperty );
  }

  return property;
}

int Renderer::GetPropertyComponentIndex( Property::Index index ) const
{
  return Property::INVALID_COMPONENT_INDEX;
}

bool Renderer::OnStage() const
{
  return mOnStageCount > 0;
}

void Renderer::Connect()
{
  if( mOnStageCount == 0 )
  {
    OnStageConnectMessage( GetEventThreadServices(), *mSceneObject );
    mTextureSetConnector.OnStageConnect();
  }
  ++mOnStageCount;
}

void Renderer::Disconnect()
{
  --mOnStageCount;
  if( mOnStageCount == 0 )
  {
    OnStageDisconnectMessage( GetEventThreadServices(), *mSceneObject);
    mTextureSetConnector.OnStageDisconnect();
  }
}

Renderer::Renderer()
: mSceneObject (NULL ),
  mBlendColor( NULL ),
  mDepthIndex( 0 ),
  mOnStageCount( 0 ),
  mIndexedDrawFirstElement( 0 ),
  mIndexedDrawElementCount( 0 ),
  mFaceCullingMode( FaceCullingMode::NONE ),
  mBlendMode( BlendMode::AUTO ),
  mBlendingOptions(),
  mDepthWriteMode( DepthWriteMode::AUTO ),
  mDepthFunction( DepthFunction::LESS ),
  mDepthTestMode( DepthTestMode::AUTO ),
  mPremultipledAlphaEnabled( false )
{
}

void Renderer::Initialize()
{
  EventThreadServices& eventThreadServices = GetEventThreadServices();
  SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();

  mSceneObject = SceneGraph::Renderer::New();
  AddMessage( updateManager, updateManager.GetRendererOwner(), *mSceneObject );

  eventThreadServices.RegisterObject( this );
}

Renderer::~Renderer()
{
  if( EventThreadServices::IsCoreRunning() )
  {
    EventThreadServices& eventThreadServices = GetEventThreadServices();
    SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();
    RemoveMessage( updateManager, updateManager.GetRendererOwner(), *mSceneObject );

    eventThreadServices.UnregisterObject( this );
  }
}

} // namespace Internal
} // namespace Dali

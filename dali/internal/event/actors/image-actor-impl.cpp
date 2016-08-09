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
#include <dali/internal/event/actors/image-actor-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali/public-api/animation/constraints.h> // for EqualToConstraint
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/internal/event/animation/constraint-impl.h>
#include <dali/internal/event/common/property-helper.h>
#include <dali/internal/event/effects/shader-effect-impl.h>
#include <dali/internal/event/images/image-connector.h>
#include <dali/internal/event/images/nine-patch-image-impl.h>

namespace Dali
{

namespace Internal
{

namespace
{

// Properties

//              Name           Type   writable animatable constraint-input  enum for index-checking
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "pixelArea",    RECTANGLE, true,    false,   true,    Dali::ImageActor::Property::PIXEL_AREA )
DALI_PROPERTY( "style",        STRING,    true,    false,   true,    Dali::ImageActor::Property::STYLE      )
DALI_PROPERTY( "border",       VECTOR4,   true,    false,   true,    Dali::ImageActor::Property::BORDER     )
DALI_PROPERTY( "image",        MAP,       true,    false,   false,   Dali::ImageActor::Property::IMAGE      )
DALI_PROPERTY_TABLE_END( DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX )

BaseHandle Create()
{
  return Dali::ImageActor::New();
}

TypeRegistration mType( typeid( Dali::ImageActor ), typeid( Dali::Actor ), Create );

struct GridVertex
{
  GridVertex( float positionX, float positionY, const Vector2& size )
  : mPosition( positionX*size.x, positionY*size.y, 0.f ),
    mTextureCoord( positionX+0.5f, positionY+0.5f )
  {
  }

  Vector3 mPosition;
  Vector2 mTextureCoord;
};

GeometryPtr CreateGeometry( unsigned int gridWidth, unsigned int gridHeight, const Vector2& size )
{
  // Create vertices
  std::vector< GridVertex > vertices;
  vertices.reserve( ( gridWidth + 1 ) * ( gridHeight + 1 ) );

  for( unsigned int y = 0u; y < gridHeight + 1; ++y )
  {
    float yPos = (float)y / gridHeight;
    for( unsigned int x = 0u; x < gridWidth + 1; ++x )
    {
      float xPos = (float)x / gridWidth;
      vertices.push_back( GridVertex( xPos - 0.5f, yPos - 0.5f, size ) );
    }
  }

  // Create indices
  Vector< unsigned short > indices;
  indices.Reserve( ( gridWidth + 2 ) * gridHeight * 2 - 2);

  for( unsigned int row = 0u; row < gridHeight; ++row )
  {
    unsigned int rowStartIndex = row*(gridWidth+1u);
    unsigned int nextRowStartIndex = rowStartIndex + gridWidth +1u;

    if( row != 0u ) // degenerate index on non-first row
    {
      indices.PushBack( rowStartIndex );
    }

    for( unsigned int column = 0u; column < gridWidth+1u; column++) // main strip
    {
      indices.PushBack( rowStartIndex + column);
      indices.PushBack( nextRowStartIndex + column);
    }

    if( row != gridHeight-1u ) // degenerate index on non-last row
    {
      indices.PushBack( nextRowStartIndex + gridWidth );
    }
  }

  Property::Map vertexFormat;
  vertexFormat[ "aPosition" ] = Property::VECTOR3;
  vertexFormat[ "aTexCoord" ] = Property::VECTOR2;
  PropertyBufferPtr vertexPropertyBuffer = PropertyBuffer::New( vertexFormat );
  if( vertices.size() > 0 )
  {
    vertexPropertyBuffer->SetData( &vertices[ 0 ], vertices.size() );
  }

  // Create the geometry object
  GeometryPtr geometry = Geometry::New();
  geometry->AddVertexBuffer( *vertexPropertyBuffer );
  if( indices.Size() > 0 )
  {
    geometry->SetIndexBuffer( &indices[0], indices.Size() );
  }
  geometry->SetType( Dali::Geometry::TRIANGLE_STRIP );

  return geometry;
}

const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec3 aPosition;\n
  attribute mediump vec2 aTexCoord;\n
  varying mediump vec2 vTexCoord;\n
  uniform mediump mat4 uMvpMatrix;\n
  uniform mediump vec3 uSize;\n
  uniform mediump vec4 sTextureRect;\n
  \n
  void main()\n
  {\n
    gl_Position = uMvpMatrix * vec4(aPosition, 1.0);\n
    vTexCoord = aTexCoord;\n
  }\n
);

const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  varying mediump vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform lowp vec4 uColor;\n
  \n
  void main()\n
  {\n
    gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n
  }\n
);

const char * const TEXTURE_RECT_UNIFORM_NAME( "sTextureRect" );

const int INVALID_RENDERER_ID = -1;
const uint16_t MAXIMUM_GRID_SIZE = 2048;
}

ImageActorPtr ImageActor::New()
{
  ImageActorPtr actor( new ImageActor );

  // Second-phase construction of base class
  actor->Initialize();

  //Create the renderer
  actor->mRenderer = Renderer::New();

  GeometryPtr quad  = CreateGeometry( 1u, 1u, Vector2::ONE );
  actor->mRenderer->SetGeometry( *quad );

  ShaderPtr shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER, Dali::Shader::Hint::NONE );
  actor->mRenderer->SetShader( *shader );
  TextureSetPtr textureSet = TextureSet::New();
  actor->mRenderer->SetTextures( *textureSet );

  return actor;
}

void ImageActor::OnInitialize()
{
  // TODO: Remove this, at the moment its needed for size negotiation to work
  SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
}

void ImageActor::SetImage( ImagePtr& image )
{
  if( !image )
  {
    if( mRendererIndex != INVALID_RENDERER_ID )
    {
      RemoveRenderer( mRendererIndex );
      mRendererIndex = INVALID_RENDERER_ID;
    }
  }
  else
  {
    SamplerPtr sampler = Sampler::New();
    sampler->SetFilterMode( mMinFilter, mMagFilter );

    TextureSet* textureSet( mRenderer->GetTextures() );
    textureSet->SetImage( 0u, image.Get() );
    textureSet->SetSampler( 0u, sampler );

    if( mRendererIndex == INVALID_RENDERER_ID )
    {
      mRendererIndex = AddRenderer( *mRenderer );
    }

    if( !mIsPixelAreaSet )
    {
      mPixelArea = PixelArea( 0, 0, image->GetWidth(), image->GetHeight() );
    }

    RelayoutRequest();
    UpdateTexureRect();
  }
}

ImagePtr ImageActor::GetImage() const
{
  return mRenderer->GetTextures()->GetImage( 0u );
}

void ImageActor::SetPixelArea( const PixelArea& pixelArea )
{
  mPixelArea = pixelArea;
  mIsPixelAreaSet = true;

  RelayoutRequest();
  UpdateTexureRect();
}

const ImageActor::PixelArea& ImageActor::GetPixelArea() const
{
  return mPixelArea;
}

bool ImageActor::IsPixelAreaSet() const
{
  return mIsPixelAreaSet;
}

void ImageActor::ClearPixelArea()
{
  mIsPixelAreaSet = false;

  int imageWidth = 0;
  int imageHeight = 0;
  ImagePtr image = GetImage();
  if( image )
  {
    imageWidth = image->GetWidth();
    imageHeight = image->GetHeight();
  }

  mPixelArea = PixelArea( 0, 0, imageWidth, imageHeight );

  RelayoutRequest();
  UpdateTexureRect();
}

void ImageActor::SetStyle( Dali::ImageActor::Style style )
{
  DALI_LOG_WARNING( "SetStyle Deprecated. Only STYLE_QUAD supported.\n" );
  mStyle = style;
}

Dali::ImageActor::Style ImageActor::GetStyle() const
{
  DALI_LOG_WARNING( "GetStyle Deprecated. Only STYLE_QUAD supported.\n" );
  return mStyle;
}

void ImageActor::SetNinePatchBorder( const Vector4& border )
{
  DALI_LOG_WARNING( "SetNinePatchBorder Deprecated. Only STYLE_QUAD supported.\n" );
  mNinePatchBorder = border;
}

Vector4 ImageActor::GetNinePatchBorder() const
{
  DALI_LOG_WARNING( "GetNinePatchBorder Deprecated. Only STYLE_QUAD supported.\n" );
  return mNinePatchBorder;
}

ImageActor::ImageActor()
: Actor( Actor::BASIC ),
  mActorSize( Vector2::ZERO ),
  mGridSize( 1u, 1u ),
  mRendererIndex( INVALID_RENDERER_ID ),
  mMinFilter( FilterMode::DEFAULT ),
  mMagFilter( FilterMode::DEFAULT ),
  mStyle( Dali::ImageActor::STYLE_QUAD ),
  mIsPixelAreaSet( false )
{
}

ImageActor::~ImageActor()
{
}

Vector3 ImageActor::GetNaturalSize() const
{
  Vector2 naturalSize( CalculateNaturalSize() );
  return Vector3( naturalSize.width, naturalSize.height, 0.f );
}

Vector2 ImageActor::CalculateNaturalSize() const
{
  // if no image then natural size is 0
  Vector2 size( 0.0f, 0.0f );

  ImagePtr image = GetImage();
  if( image )
  {
    if( IsPixelAreaSet() )
    {
      PixelArea area(GetPixelArea());
      size.width = area.width;
      size.height = area.height;
    }
    else
    {
      size = image->GetNaturalSize();
    }
  }

  return size;
}

void ImageActor::UpdateGeometry()
{
  uint16_t gridWidth = 1u;
  uint16_t gridHeight = 1u;

  if( mShaderEffect )
  {
    Vector2 gridSize = mShaderEffect->GetGridSize( Vector2(mPixelArea.width, mPixelArea.height) );

    //limit the grid size
    gridWidth = std::min( MAXIMUM_GRID_SIZE, static_cast<uint16_t>(gridSize.width) );
    gridHeight = std::min( MAXIMUM_GRID_SIZE, static_cast<uint16_t>(gridSize.height) );
  }

  mGridSize.SetWidth( gridWidth );
  mGridSize.SetHeight( gridHeight );

  GeometryPtr geometry = CreateGeometry( gridWidth, gridHeight, mActorSize );
  mRenderer->SetGeometry( *geometry );
}
void ImageActor::UpdateTexureRect()
{
  Vector4 textureRect( 0.f, 0.f, 1.f, 1.f );

  ImagePtr image = GetImage();
  if( mIsPixelAreaSet && image )
  {
    const float uScale = 1.0f / float(image->GetWidth());
    const float vScale = 1.0f / float(image->GetHeight());
    // bottom left
    textureRect.x = uScale * float(mPixelArea.x);
    textureRect.y = vScale * float(mPixelArea.y);
    // top right
    textureRect.z  = uScale * float(mPixelArea.x + mPixelArea.width);
    textureRect.w = vScale * float(mPixelArea.y + mPixelArea.height);
  }

  mRenderer->RegisterProperty( TEXTURE_RECT_UNIFORM_NAME, textureRect );
}

unsigned int ImageActor::GetDefaultPropertyCount() const
{
  return Actor::GetDefaultPropertyCount() + DEFAULT_PROPERTY_COUNT;
}

void ImageActor::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  Actor::GetDefaultPropertyIndices( indices ); // Actor class properties

  indices.Reserve( indices.Size() + DEFAULT_PROPERTY_COUNT );

  int index = DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX;
  for ( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i, ++index )
  {
    indices.PushBack( index );
  }
}

bool ImageActor::IsDefaultPropertyWritable( Property::Index index ) const
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    return Actor::IsDefaultPropertyWritable(index);
  }

  index -= DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX;
  if ( ( index >= 0 ) && ( index < DEFAULT_PROPERTY_COUNT ) )
  {
    return DEFAULT_PROPERTY_DETAILS[ index ].writable;
  }

  return false;
}

bool ImageActor::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    return Actor::IsDefaultPropertyAnimatable( index );
  }

  index -= DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX;
  if ( ( index >= 0 ) && ( index < DEFAULT_PROPERTY_COUNT ) )
  {
    return DEFAULT_PROPERTY_DETAILS[ index ].animatable;
  }

  return false;
}

bool ImageActor::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    return Actor::IsDefaultPropertyAConstraintInput( index );
  }

  index -= DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX;
  if ( ( index >= 0 ) && ( index < DEFAULT_PROPERTY_COUNT ) )
  {
    return DEFAULT_PROPERTY_DETAILS[ index ].constraintInput;
  }

  return false;
}

Property::Type ImageActor::GetDefaultPropertyType( Property::Index index ) const
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    return Actor::GetDefaultPropertyType( index );
  }

  index -= DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX;
  if ( ( index >= 0 ) && ( index < DEFAULT_PROPERTY_COUNT ) )
  {
    return DEFAULT_PROPERTY_DETAILS[index].type;
  }

  // index out-of-bounds
  return Property::NONE;
}

const char* ImageActor::GetDefaultPropertyName( Property::Index index ) const
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    return Actor::GetDefaultPropertyName(index);
  }

  index -= DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX;
  if ( ( index >= 0 ) && ( index < DEFAULT_PROPERTY_COUNT ) )
  {
    return DEFAULT_PROPERTY_DETAILS[index].name;
  }

  // index out-of-bounds
  return NULL;
}

Property::Index ImageActor::GetDefaultPropertyIndex(const std::string& name) const
{
  Property::Index index = Property::INVALID_INDEX;

  // Look for name in default properties
  for( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
  {
    const Internal::PropertyDetails* property = &DEFAULT_PROPERTY_DETAILS[ i ];
    if( 0 == strcmp( name.c_str(), property->name ) ) // Don't want to convert rhs to string
    {
      index = i + DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX;
      break;
    }
  }

  // If not found, check in base class
  if( Property::INVALID_INDEX == index )
  {
    index = Actor::GetDefaultPropertyIndex( name );
  }
  return index;
}

void ImageActor::SetDefaultProperty( Property::Index index, const Property::Value& propertyValue )
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    Actor::SetDefaultProperty( index, propertyValue );
  }
  else
  {
    switch(index)
    {
      case Dali::ImageActor::Property::PIXEL_AREA:
      {
        SetPixelArea(propertyValue.Get<Rect<int> >());
        break;
      }
      case Dali::ImageActor::Property::STYLE:
      {
        //not supported
        break;
      }
      case Dali::ImageActor::Property::BORDER:
      {
        //not supported
        break;
      }
      case Dali::ImageActor::Property::IMAGE:
      {
        Dali::Image img = Scripting::NewImage( propertyValue );
        if(img)
        {
          ImagePtr image( &GetImplementation(img) );
          SetImage( image );
        }
        else
        {
          DALI_LOG_WARNING("Cannot create image from property value\n");
        }
        break;
      }
      default:
      {
        DALI_LOG_WARNING("Unknown property (%d)\n", index);
        break;
      }
    } // switch(index)

  } // else
}

Property::Value ImageActor::GetDefaultProperty( Property::Index index ) const
{
  Property::Value ret;
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    ret = Actor::GetDefaultProperty( index );
  }
  else
  {
    switch( index )
    {
      case Dali::ImageActor::Property::PIXEL_AREA:
      {
        Rect<int> r = GetPixelArea();
        ret = r;
        break;
      }
      case Dali::ImageActor::Property::STYLE:
      {
        //not supported
        break;
      }
      case Dali::ImageActor::Property::BORDER:
      {
        //not supported
        break;
      }
      case Dali::ImageActor::Property::IMAGE:
      {
        Property::Map map;
        Scripting::CreatePropertyMap( Dali::Image( GetImage().Get() ), map );
        ret = Property::Value( map );
        break;
      }
      default:
      {
        DALI_LOG_WARNING( "Unknown property (%d)\n", index );
        break;
      }
    } // switch(index)
  }

  return ret;
}

void ImageActor::SetSortModifier(float modifier)
{
  mRenderer->SetDepthIndex( modifier );
}

float ImageActor::GetSortModifier() const
{
  return mRenderer->GetDepthIndex();
}

void ImageActor::SetBlendMode( BlendingMode::Type mode )
{
  mRenderer->SetBlendMode( static_cast<BlendMode::Type>( mode ) );
}

BlendingMode::Type ImageActor::GetBlendMode() const
{
  return static_cast<BlendingMode::Type>( mRenderer->GetBlendMode() );
}

void ImageActor::SetBlendFunc( BlendingFactor::Type srcFactorRgba,   BlendingFactor::Type destFactorRgba )
{
  mRenderer->SetBlendFunc( static_cast<BlendFactor::Type>(srcFactorRgba), static_cast<BlendFactor::Type>(destFactorRgba), static_cast<BlendFactor::Type>(srcFactorRgba), static_cast<BlendFactor::Type>(destFactorRgba) );
}

void ImageActor::SetBlendFunc( BlendingFactor::Type srcFactorRgb,   BlendingFactor::Type destFactorRgb,
                               BlendingFactor::Type srcFactorAlpha, BlendingFactor::Type destFactorAlpha )
{
  mRenderer->SetBlendFunc( static_cast<BlendFactor::Type>(srcFactorRgb), static_cast<BlendFactor::Type>(destFactorRgb), static_cast<BlendFactor::Type>(srcFactorAlpha), static_cast<BlendFactor::Type>(destFactorAlpha) );
}

void ImageActor::GetBlendFunc( BlendingFactor::Type& srcFactorRgb,   BlendingFactor::Type& destFactorRgb,
                               BlendingFactor::Type& srcFactorAlpha, BlendingFactor::Type& destFactorAlpha ) const
{
  mRenderer->GetBlendFunc( reinterpret_cast<BlendFactor::Type&>(srcFactorRgb), reinterpret_cast<BlendFactor::Type&>(destFactorRgb), reinterpret_cast<BlendFactor::Type&>(srcFactorAlpha), reinterpret_cast<BlendFactor::Type&>(destFactorAlpha) );
}

void ImageActor::SetBlendEquation( BlendingEquation::Type equationRgba )
{
  mRenderer->SetBlendEquation( static_cast<BlendEquation::Type>(equationRgba), static_cast<BlendEquation::Type>(equationRgba) );
}

void ImageActor::SetBlendEquation( BlendingEquation::Type equationRgb, BlendingEquation::Type equationAlpha )
{
  mRenderer->SetBlendEquation( static_cast<BlendEquation::Type>(equationRgb), static_cast<BlendEquation::Type>(equationAlpha) );
}

void ImageActor::GetBlendEquation( BlendingEquation::Type& equationRgb, BlendingEquation::Type& equationAlpha ) const
{
  mRenderer->GetBlendEquation( reinterpret_cast<BlendEquation::Type&>(equationRgb), reinterpret_cast<BlendEquation::Type&>(equationAlpha) );
}

void ImageActor::SetBlendColor( const Vector4& color )
{
  mBlendColor = color;
  mRenderer->SetBlendColor( mBlendColor );
}

const Vector4& ImageActor::GetBlendColor() const
{
  return mBlendColor;
}

void ImageActor::SetFilterMode( FilterMode::Type minFilter, FilterMode::Type magFilter )
{
  mMinFilter = minFilter;
  mMagFilter = magFilter;

  SamplerPtr sampler = Sampler::New();
  sampler->SetFilterMode( minFilter, magFilter );
  mRenderer->GetTextures()->SetSampler( 0u, sampler.Get() );
}

void ImageActor::GetFilterMode( FilterMode::Type& minFilter, FilterMode::Type& magFilter ) const
{
  minFilter = mMinFilter;
  magFilter = mMagFilter;
}

void ImageActor::SetShaderEffect( ShaderEffect& effect )
{
  if( mShaderEffect )
  {
    mShaderEffect->Disconnect( this );
  }

  mShaderEffect = ShaderEffectPtr( &effect );
  effect.Connect( this );

  ShaderPtr shader = mShaderEffect->GetShader();
  mRenderer->SetShader( *shader );

  EffectImageUpdated();

  UpdateGeometry();
}

ShaderEffectPtr ImageActor::GetShaderEffect() const
{
  return mShaderEffect;
}

void ImageActor::RemoveShaderEffect()
{
  if( mShaderEffect )
  {
    mShaderEffect->Disconnect( this );
    // change to the standard shader and quad geometry
    ShaderPtr shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER, Dali::Shader::Hint::NONE );
    mRenderer->SetShader( *shader );
    mShaderEffect.Reset();

    UpdateGeometry();
  }
}

void ImageActor::EffectImageUpdated()
{
  if( mShaderEffect )
  {
    Dali::Image effectImage = mShaderEffect->GetEffectImage();
    if( effectImage )
    {
      Image& effectImageImpl = GetImplementation( effectImage );
      mRenderer->GetTextures()->SetImage( 1u, &effectImageImpl );
    }
    else
    {
       mRenderer->GetTextures()->SetImage( 1u, 0 );
    }
  }
}

void ImageActor::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  if( mActorSize != size )
  {
    mActorSize = size;
    UpdateGeometry();
  }
}

void ImageActor::OnSizeSet( const Vector3& targetSize )
{
  Vector2 size( targetSize.x, targetSize.y );
  if( mActorSize != size )
  {
    mActorSize = size;
    UpdateGeometry();
  }
}

} // namespace Internal

} // namespace Dali

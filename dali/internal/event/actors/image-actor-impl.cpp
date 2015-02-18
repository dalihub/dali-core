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
#include <dali/internal/event/actors/image-actor-impl.h>

// INTERNAL INCLUDES
#include <dali/internal/event/images/nine-patch-image-impl.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/event/common/property-index-ranges.h>
#include <dali/internal/event/images/image-connector.h>
#include <dali/public-api/scripting/scripting.h>

namespace Dali
{

const Property::Index ImageActor::PIXEL_AREA           = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT;
const Property::Index ImageActor::STYLE                = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 1;
const Property::Index ImageActor::BORDER               = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 2;
const Property::Index ImageActor::IMAGE                = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 3;

namespace Internal
{

namespace
{

BaseHandle Create()
{
  return Dali::ImageActor::New();
}

TypeRegistration mType( typeid(Dali::ImageActor), typeid(Dali::RenderableActor), Create );

const Internal::PropertyDetails DEFAULT_IMAGE_ACTOR_PROPERTY_DETAILS[] =
{
  // Name            Type              writable animatable constraint-input
  { "pixel-area",   Property::RECTANGLE, true,    false,   true },  // PIXEL_AREA
  { "style",        Property::STRING,    true,    false,   true },  // STYLE
  { "border",       Property::VECTOR4,   true,    false,   true },  // BORDER
  { "image",        Property::MAP,       true,    false,   false }, // IMAGE
};
const int DEFAULT_IMAGE_ACTOR_PROPERTY_COUNT = sizeof( DEFAULT_IMAGE_ACTOR_PROPERTY_DETAILS ) / sizeof( DEFAULT_IMAGE_ACTOR_PROPERTY_DETAILS[0] );

ImageActor::Style StyleEnum(const std::string &s)
{
  if(s == "STYLE_NINE_PATCH")
  {
    return Dali::ImageActor::STYLE_NINE_PATCH;
  }
  else if(s == "STYLE_NINE_PATCH_NO_CENTER")
  {
    return Dali::ImageActor::STYLE_NINE_PATCH_NO_CENTER;
  }
  else // if(s == "QUAD")
  {
    return Dali::ImageActor::STYLE_QUAD;
  }
}

std::string StyleString(const ImageActor::Style style)
{
  if(style == Dali::ImageActor::STYLE_NINE_PATCH)
  {
    return "STYLE_NINE_PATCH";
  }
  else if(style == Dali::ImageActor::STYLE_NINE_PATCH_NO_CENTER)
  {
    return "STYLE_NINE_PATCH_NO_CENTER";
  }
  else // if(s == "QUAD")
  {
    return "STYLE_QUAD";
  }
}
}

ImageActorPtr ImageActor::New()
{
  ImageActorPtr actor( new ImageActor );

  // Second-phase construction of base class
  actor->Initialize();

  // Create the attachment
  actor->mImageAttachment = ImageAttachment::New( *actor->mStage, *actor->mNode );
  actor->Attach( *actor->mImageAttachment );

  return actor;
}

void ImageActor::OnInitialize()
{
}

void ImageActor::SetImage( ImagePtr& image )
{
  ImagePtr currentImage = mImageAttachment->GetImage();
  // early exit if it's the same image as we already have
  if ( currentImage == image )
  {
    return;
  }

  // NOTE! image might be pointing to NULL, which is fine as in that case app wants to just remove the image
  ImagePtr newImage( image );
  // if image is not NULL, check for 9 patch
  if( newImage )
  {
    // Automatically convert nine-patch images to cropped bitmap
    NinePatchImage* ninePatchImage = NinePatchImage::DownCast( image.Get() );
    if( ninePatchImage )
    {
      newImage = ninePatchImage->CreateCroppedBitmapImage();
      SetStyle( Dali::ImageActor::STYLE_NINE_PATCH );
      SetNinePatchBorder( ninePatchImage->GetStretchBorders(), true );
    }
  }
  // set the actual image (normal or 9 patch) and natural size based on that
  mImageAttachment->SetImage( newImage );
  SetNaturalSize();
}

ImagePtr ImageActor::GetImage()
{
  return mImageAttachment->GetImage();
}

void ImageActor::SetToNaturalSize()
{
  mUsingNaturalSize = true;

  SetNaturalSize();
}

void ImageActor::SetPixelArea( const PixelArea& pixelArea )
{
  mImageAttachment->SetPixelArea( pixelArea );

  SetNaturalSize();
}

const ImageActor::PixelArea& ImageActor::GetPixelArea() const
{
  return mImageAttachment->GetPixelArea();
}

bool ImageActor::IsPixelAreaSet() const
{
  return mImageAttachment->IsPixelAreaSet();
}

void ImageActor::ClearPixelArea()
{
  mImageAttachment->ClearPixelArea();

  if( mUsingNaturalSize )
  {
    ImagePtr image = mImageAttachment->GetImage();
    if( image )
    {
      mInternalSetSize = true;
      SetSize( image->GetNaturalSize() );
      mInternalSetSize = false;
    }
  }
}

void ImageActor::SetStyle( Style style )
{
  mImageAttachment->SetStyle( style );
}

ImageActor::Style ImageActor::GetStyle() const
{
  return mImageAttachment->GetStyle();
}

void ImageActor::SetNinePatchBorder( const Vector4& border, bool inPixels )
{
  mImageAttachment->SetNinePatchBorder( border, inPixels );
}

Vector4 ImageActor::GetNinePatchBorder() const
{
  return mImageAttachment->GetNinePatchBorder();
}

ImageAttachment& ImageActor::GetImageAttachment()
{
  return *mImageAttachment;
}

RenderableAttachment& ImageActor::GetRenderableAttachment() const
{
  DALI_ASSERT_DEBUG( mImageAttachment && "ImageAttachment missing from ImageActor" );
  return *mImageAttachment;
}

ImageActor::ImageActor()
: RenderableActor(),
  mUsingNaturalSize(true),
  mInternalSetSize(false)
{
}

ImageActor::~ImageActor()
{
}

void ImageActor::SetNaturalSize()
{
  if( mUsingNaturalSize )
  {
    mInternalSetSize = true;
    SetSize( CalculateNaturalSize() );
    mInternalSetSize = false;
  }
}

Vector3 ImageActor::GetNaturalSize() const
{
  Vector2 naturalSize( CalculateNaturalSize() );
  return Vector3( naturalSize.width, naturalSize.height, CalculateSizeZ( naturalSize ) );
}

Vector2 ImageActor::CalculateNaturalSize() const
{
  // if no image then natural size is 0
  Vector2 size( 0.0f, 0.0f );

  ImagePtr image = mImageAttachment->GetImage();
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

void ImageActor::OnSizeSet( const Vector3& targetSize )
{
  if( !mInternalSetSize )
  {
    mUsingNaturalSize = false;
  }
}

void ImageActor::OnSizeAnimation(Animation& animation, const Vector3& targetSize)
{
  mUsingNaturalSize = false;
}

void ImageActor::OnStageConnectionInternal()
{
}

void ImageActor::OnStageDisconnectionInternal()
{
}

unsigned int ImageActor::GetDefaultPropertyCount() const
{
  return RenderableActor::GetDefaultPropertyCount() + DEFAULT_IMAGE_ACTOR_PROPERTY_COUNT;
}

void ImageActor::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  RenderableActor::GetDefaultPropertyIndices( indices ); // RenderableActor class properties

  indices.reserve( indices.size() + DEFAULT_IMAGE_ACTOR_PROPERTY_COUNT );

  int index = DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT;
  for ( int i = 0; i < DEFAULT_IMAGE_ACTOR_PROPERTY_COUNT; ++i, ++index )
  {
    indices.push_back( index );
  }
}

bool ImageActor::IsDefaultPropertyWritable( Property::Index index ) const
{
  if( index < DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT )
  {
    return RenderableActor::IsDefaultPropertyWritable(index);
  }
  else
  {
    index -= DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT;
    if ( ( index >= 0 ) && ( index < DEFAULT_IMAGE_ACTOR_PROPERTY_COUNT ) )
    {
      return DEFAULT_IMAGE_ACTOR_PROPERTY_DETAILS[ index ].writable;
    }
  }
  return false;
}

bool ImageActor::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  if(index < DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT)
  {
    return RenderableActor::IsDefaultPropertyAnimatable(index);
  }
  else
  {
    index -= DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT;
    if ( ( index >= 0 ) && ( index < DEFAULT_IMAGE_ACTOR_PROPERTY_COUNT ) )
    {
      return DEFAULT_IMAGE_ACTOR_PROPERTY_DETAILS[ index ].animatable;
    }
  }
  return false;
}

bool ImageActor::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  if( index < DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT )
  {
    return RenderableActor::IsDefaultPropertyAConstraintInput(index);
  }
  else
  {
    index -= DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT;
    if ( ( index >= 0 ) && ( index < DEFAULT_IMAGE_ACTOR_PROPERTY_COUNT ) )
    {
      return DEFAULT_IMAGE_ACTOR_PROPERTY_DETAILS[ index ].constraintInput;
    }
  }
  return false;
}

Property::Type ImageActor::GetDefaultPropertyType( Property::Index index ) const
{
  if(index < DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT)
  {
    return RenderableActor::GetDefaultPropertyType(index);
  }
  else
  {
    index -= DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT;

    if ( ( index >= 0 ) && ( index < DEFAULT_IMAGE_ACTOR_PROPERTY_COUNT ) )
    {
      return DEFAULT_IMAGE_ACTOR_PROPERTY_DETAILS[index].type;
    }
    else
    {
      // index out-of-bounds
      return Property::NONE;
    }
  }
}

const char* ImageActor::GetDefaultPropertyName( Property::Index index ) const
{
  if(index < DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT)
  {
    return RenderableActor::GetDefaultPropertyName(index);
  }
  else
  {
    index -= DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT;

    if ( ( index >= 0 ) && ( index < DEFAULT_IMAGE_ACTOR_PROPERTY_COUNT ) )
    {
      return DEFAULT_IMAGE_ACTOR_PROPERTY_DETAILS[index].name;
    }
    else
    {
      // index out-of-bounds
      return NULL;
    }
  }
}

Property::Index ImageActor::GetDefaultPropertyIndex(const std::string& name) const
{
  Property::Index index = Property::INVALID_INDEX;

  // Look for name in default properties
  for( int i = 0; i < DEFAULT_IMAGE_ACTOR_PROPERTY_COUNT; ++i )
  {
    const Internal::PropertyDetails* property = &DEFAULT_IMAGE_ACTOR_PROPERTY_DETAILS[ i ];
    if( 0 == strcmp( name.c_str(), property->name ) ) // dont want to convert rhs to string
    {
      index = i + DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT;
      break;
    }
  }

  // If not found, check in base class
  if( Property::INVALID_INDEX == index )
  {
    index = RenderableActor::GetDefaultPropertyIndex( name );
  }
  return index;
}

void ImageActor::SetDefaultProperty( Property::Index index, const Property::Value& propertyValue )
{
  if(index < DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT)
  {
    RenderableActor::SetDefaultProperty(index, propertyValue);
  }
  else
  {
    switch(index)
    {
      case Dali::ImageActor::PIXEL_AREA:
      {
        SetPixelArea(propertyValue.Get<Rect<int> >());
        break;
      }
      case Dali::ImageActor::STYLE:
      {
        SetStyle(StyleEnum(propertyValue.Get<std::string>()));
        break;
      }
      case Dali::ImageActor::BORDER:
      {
        SetNinePatchBorder( propertyValue.Get<Vector4>(), true /*in pixels*/ );
        break;
      }
      case Dali::ImageActor::IMAGE:
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
  if(index < DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT)
  {
    ret = RenderableActor::GetDefaultProperty(index);
  }
  else
  {
    switch(index)
    {
      case Dali::ImageActor::PIXEL_AREA:
      {
        Rect<int> r = GetPixelArea();
        ret = r;
        break;
      }
      case Dali::ImageActor::STYLE:
      {
        ret = StyleString(GetStyle());
        break;
      }
      case Dali::ImageActor::BORDER:
      {
        ret = GetNinePatchBorder();
        break;
      }
      case Dali::ImageActor::IMAGE:
      {
        Property::Map map;
        Scripting::CreatePropertyMap( Dali::Image( mImageAttachment->GetImage().Get() ), map );
        ret = Property::Value( map );
        break;
      }
      default:
      {
        DALI_LOG_WARNING("Unknown property (%d)\n", index);
        break;
      }
    } // switch(index)
  }

  return ret;
}

} // namespace Internal

} // namespace Dali

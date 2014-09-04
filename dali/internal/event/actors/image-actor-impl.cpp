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
const Property::Index ImageActor::FADE_IN              = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 1;
const Property::Index ImageActor::FADE_IN_DURATION     = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 2;
const Property::Index ImageActor::STYLE                = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 3;
const Property::Index ImageActor::BORDER               = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 4;
const Property::Index ImageActor::IMAGE                = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 5;

namespace Internal
{
bool ImageActor::mFirstInstance = true;
Actor::DefaultPropertyLookup* ImageActor::mDefaultImageActorPropertyLookup = NULL;

namespace
{

BaseHandle Create()
{
  return Dali::ImageActor::New();
}

TypeRegistration mType( typeid(Dali::ImageActor), typeid(Dali::RenderableActor), Create );

const std::string DEFAULT_IMAGE_ACTOR_PROPERTY_NAMES[] =
{
  "pixel-area",
  "fade-in",
  "fade-in-duration",
  "style",
  "border",
  "image"
};
const int DEFAULT_IMAGE_ACTOR_PROPERTY_COUNT = sizeof( DEFAULT_IMAGE_ACTOR_PROPERTY_NAMES ) / sizeof( std::string );

const Property::Type DEFAULT_IMAGE_ACTOR_PROPERTY_TYPES[DEFAULT_IMAGE_ACTOR_PROPERTY_COUNT] =
{
  Property::RECTANGLE,  // "pixel-area",
  Property::BOOLEAN,    // "fade-in",
  Property::FLOAT,      // "fade-in-duration",
  Property::STRING,     // "style",
  Property::VECTOR4,    // "border",
  Property::MAP,        // "image",
};

ImageActor::Style StyleEnum(const std::string &s)
{
  if(s == "STYLE_NINE_PATCH")
  {
    return Dali::ImageActor::STYLE_NINE_PATCH;
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
  else // if(s == "QUAD")
  {
    return "STYLE_QUAD";
  }
}
}

ImageActorPtr ImageActor::New( Image* anImage )
{
  ImageActorPtr actor( new ImageActor() );
  ImagePtr theImage( anImage );

  // Second-phase construction of base class
  actor->Initialize();

  // Create the attachment
  actor->mImageAttachment = ImageAttachment::New( *actor->mNode, theImage.Get() );
  actor->Attach( *actor->mImageAttachment );

  // don't call the external version as attachment already has the image
  actor->SetImageInternal( NULL, anImage );

  return actor;
}


ImageActorPtr ImageActor::New( Image* image, const PixelArea& pixelArea )
{
  // re-use basic New
  ImageActorPtr actor = New( image );
  // then set the pixel area
  actor->mImageAttachment->SetPixelArea( pixelArea );
  return actor;
}

void ImageActor::OnInitialize()
{
  if(ImageActor::mFirstInstance)
  {
    mDefaultImageActorPropertyLookup = new DefaultPropertyLookup();
    const int start = DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT;
    for ( int i = 0; i < DEFAULT_IMAGE_ACTOR_PROPERTY_COUNT; ++i )
    {
      (*mDefaultImageActorPropertyLookup)[DEFAULT_IMAGE_ACTOR_PROPERTY_NAMES[i]] = i + start;
    }
    ImageActor::mFirstInstance = false;
  }
}

void ImageActor::SetImage( Image* image )
{
  Image* currentImage = static_cast<Image*>(mImageAttachment->GetImage().GetObjectPtr());
  // early exit if it's the same image
  if ( currentImage == image || mImageNext.Get() == image )
  {
    return;
  }

  SetImageInternal( currentImage, image );
}

Dali::Image ImageActor::GetImage()
{
  return mImageAttachment->GetImage();
}

void ImageActor::SetToNaturalSize()
{
  mUsingNaturalSize = true;
  Dali::Image image = mImageAttachment->GetImage();

  if( image )
  {
    SetNaturalSize( GetImplementation(image) );
  }
}

void ImageActor::SetPixelArea( const PixelArea& pixelArea )
{
  mImageAttachment->SetPixelArea( pixelArea );

  if( mUsingNaturalSize )
  {
    mInternalSetSize = true;
    SetSize(pixelArea.width, pixelArea.height);
    mInternalSetSize = false;
  }
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
    Dali::Image image = mImageAttachment->GetImage();
    if( image )
    {
      mInternalSetSize = true;
      SetSize(GetImplementation(image).GetNaturalSize());
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

void ImageActor::SetFadeIn( bool enableFade )
{
  mFadeIn = enableFade;
}

bool ImageActor::GetFadeIn() const
{
  return mFadeIn;
}

void ImageActor::SetFadeInDuration( float durationSeconds )
{
  mFadeInDuration = durationSeconds;
}

float ImageActor::GetFadeInDuration() const
{
  return mFadeInDuration;
}

ImageAttachment& ImageActor::GetImageAttachment()
{
  return *mImageAttachment;
}

Vector2 ImageActor::GetCurrentImageSize() const
{
  Vector3 size;
  Vector3 currentSize;

  // get the texture size / pixel area if only a subset of it is displayed
  if( IsPixelAreaSet() )
  {
    PixelArea area(GetPixelArea());
    return Vector2(area.width, area.height );
  }
  else
  {
    return Vector2( GetCurrentSize() );
  }
}

RenderableAttachment& ImageActor::GetRenderableAttachment() const
{
  DALI_ASSERT_DEBUG( mImageAttachment && "ImageAttachment missing from ImageActor" );
  return *mImageAttachment;
}

void ImageActor::SignalConnected( SlotObserver*, CallbackBase* )
{
  // nothing to do as we only ever connect to one signal, which we disconnect from in the destructor
}

void ImageActor::SignalDisconnected( SlotObserver*, CallbackBase* )
{
  // nothing to do as we only ever connect to one signal, which we disconnect from in the destructor
  // also worth noting that we own the image whose signal we connect to so in practice this method is never called.
}

ImageActor::ImageActor()
: RenderableActor(),
  mFadeInDuration( 1.0f ),
  mUsingNaturalSize(true),
  mInternalSetSize(false),
  mFadeIn( false ),
  mFadeInitial( true )
{
}

ImageActor::~ImageActor()
{
  if( mImageAttachment )
  {
    Dali::Image image = mImageAttachment->GetImage();
    if( image )
    {
      // just call Disconnect as if not connected it is a no-op
      image.LoadingFinishedSignal().Disconnect( this, &ImageActor::ImageLoaded );
    }
  }
}

void ImageActor::SetNaturalSize( Image& image )
{
  if( mUsingNaturalSize )
  {
    Vector2 size;
    if( IsPixelAreaSet() )
    {
      PixelArea area(GetPixelArea());
      size.width = area.width;
      size.height = area.height;
    }
    else
    {
      size = image.GetNaturalSize();
    }

    mInternalSetSize = true;
    SetSize( size );
    mInternalSetSize = false;
  }
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
  FadeIn();

  mImageNext.OnStageConnect();
}

void ImageActor::OnStageDisconnectionInternal()
{
  mImageNext.OnStageDisconnect();
}

void ImageActor::ImageLoaded( Dali::Image image )
{
  DALI_ASSERT_DEBUG (image && "Image handle empty!");

  // TODO: Handle case where image loading failed

  // Need to keep mUploadedConnection connected as image may change later through Reload
  // Note: Reloaded images may have changed size.

  // Set the attachment's image once we know the image has loaded to prevent
  // blank frames during load / reload.
  mImageAttachment->SetImage( &GetImplementation( image ) );

  // If size has never been set by application
  if( mUsingNaturalSize )
  {
    // If a pixel area has been set, use this size
    if( IsPixelAreaSet() )
    {
      const PixelArea& area = GetPixelArea();
      mInternalSetSize = true;
      SetSize(area.width, area.height);
      mInternalSetSize = false;
    }
    else
    {
      mInternalSetSize = true;
      SetSize( GetImplementation(image).GetNaturalSize() );
      mInternalSetSize = false;
    }
  }

  // fade in if required
  FadeIn();
}

void ImageActor::FadeIn()
{
  // only fade in if enabled and newly displayed on screen
  if( mFadeIn && mFadeInitial && ( mFadeInDuration > 0.0f ) )
  {
    // need to set opacity immediately to 0 otherwise child actors might get rendered
    SetOpacity( 0.0f );

    Dali::Image image = mImageAttachment->GetImage();

    // Fade-in when on-stage & the image is loaded
    if (OnStage() &&
        image &&
        image.GetLoadingState() == Dali::ResourceLoadingSucceeded)
    {
      // fire and forget animation; will clean up after it's finished
      Dali::Animation animation = Dali::Animation::New( mFadeInDuration );
      animation.OpacityTo( Dali::Actor( this ), 1.0f, AlphaFunctions::EaseOut );
      animation.Play();
      mFadeInitial = false;
    }
  }
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
  if(index < DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT)
  {
    return RenderableActor::IsDefaultPropertyWritable(index);
  }
  else
  {
    return true;
  }
}

bool ImageActor::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  if(index < DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT)
  {
    return RenderableActor::IsDefaultPropertyAnimatable(index);
  }
  else
  {
    return false;
  }
}

bool ImageActor::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  if( index < DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT )
  {
    return RenderableActor::IsDefaultPropertyAConstraintInput(index);
  }
  return true; // Our properties can be used as input to constraints.
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
      return DEFAULT_IMAGE_ACTOR_PROPERTY_TYPES[index];
    }
    else
    {
      // index out-of-bounds
      return Property::NONE;
    }
  }
}

const std::string& ImageActor::GetDefaultPropertyName( Property::Index index ) const
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
      return DEFAULT_IMAGE_ACTOR_PROPERTY_NAMES[index];
    }
    else
    {
      // index out-of-bounds
      static const std::string INVALID_PROPERTY_NAME;
      return INVALID_PROPERTY_NAME;
    }
  }
}

Property::Index ImageActor::GetDefaultPropertyIndex(const std::string& name) const
{
  Property::Index index = Property::INVALID_INDEX;

  DALI_ASSERT_DEBUG( NULL != mDefaultImageActorPropertyLookup );

  // Look for name in current class' default properties
  DefaultPropertyLookup::const_iterator result = mDefaultImageActorPropertyLookup->find( name );
  if ( mDefaultImageActorPropertyLookup->end() != result )
  {
    index = result->second;
  }
  else
  {
    // If not found, check in base class
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
      case Dali::ImageActor::FADE_IN:
      {
        SetFadeIn(propertyValue.Get<bool>());
        break;
      }
      case Dali::ImageActor::FADE_IN_DURATION:
      {
        SetFadeInDuration(propertyValue.Get<float>());
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
          SetImage( &GetImplementation(img) );
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
      case Dali::ImageActor::FADE_IN:
      {
        ret = GetFadeIn();
        break;
      }
      case Dali::ImageActor::FADE_IN_DURATION:
      {
        ret = GetFadeInDuration();
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
        Scripting::CreatePropertyMap( mImageAttachment->GetImage(), map );
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

void ImageActor::SetImageInternal( Image* currentImage, Image* image )
{
  if( currentImage )
  {
    // just call Disconnect as if not connected it is a no-op
    currentImage->LoadingFinishedSignal().Disconnect( this, &ImageActor::ImageLoaded );
  }

  ImagePtr imagePtr( image );
  // Automatically convert nine-patch images to cropped bitmap
  NinePatchImage* ninePatchImage = NinePatchImage::GetNinePatchImage( image );
  if( ninePatchImage )
  {
    imagePtr = ninePatchImage->CreateCroppedBitmapImage();
  }
  mImageNext.Set( imagePtr.Get(), OnStage() );
  if( ninePatchImage )
  {
    SetStyle( Dali::ImageActor::STYLE_NINE_PATCH );
    SetNinePatchBorder( ninePatchImage->GetStretchBorders(), true );
  }
  if( !imagePtr )
  {
    mImageAttachment->SetImage( NULL );
  }
  else
  {
    // don't disconnect currently shown image until we made sure that the new one is loaded
    if( Dali::ResourceLoading == image->GetLoadingState() && !image->GetFilename().empty() )
    {
      // observe image loading, @todo stop using signals internally!
      image->LoadingFinishedSignal().Connect( this, &ImageActor::ImageLoaded );
    }
    else
    {
      // image already loaded, generated or 9 patch
      ImageLoaded( Dali::Image( image ) );
    }
  }
}

} // namespace Internal

} // namespace Dali

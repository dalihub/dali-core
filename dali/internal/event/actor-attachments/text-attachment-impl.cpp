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

// CLASS HEADER

#include <dali/internal/event/actor-attachments/text-attachment-impl.h>

// INTERNAL INCLUDES

#include <dali/public-api/common/dali-common.h>
#include <dali/internal/update/node-attachments/scene-graph-text-attachment.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/common/text-parameters.h>


namespace Dali
{

namespace Internal
{

TextAttachmentPtr TextAttachment::New( const SceneGraph::Node& parentNode, const TextArray& text, FontPointer font, bool isLeftToRight )
{
  StagePtr stage = Stage::GetCurrent();

  TextAttachmentPtr attachment( new TextAttachment( *stage ) );

  // Second-phase construction

  attachment->mFont = font;
  attachment->mText = text;
  attachment->mIsLeftToRight = isLeftToRight;

  // Transfer object ownership of scene-object to message
  SceneGraph::TextAttachment* sceneObject = SceneGraph::TextAttachment::New();

  AttachToNodeMessage( stage->GetUpdateManager(), parentNode, sceneObject );

  // Keep raw pointer for message passing
  attachment->mSceneObject = sceneObject;

  attachment->SetSmoothEdge(attachment->mSmoothing);      // adjust smoothedge for font weight

  return attachment;
}

TextAttachment::TextAttachment( Stage& stage )
: RenderableAttachment( stage ),
  mTextRequestHelper( *this ),
  mUnderlineEnabled( false ),
  mIsLeftToRight(true),
  mTextChanged( true ),
  mFontChanged( true ),
  mUnderlineChanged( true ),
  mItalicsChanged( true ),
  mItalicsEnabled( false ),
  mTextureIdSet( false ),
  mTextureId(0),
  mSmoothing(Dali::TextStyle::DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD),
  mItalicsAngle( Radian(0.0f) ),
  mUnderlineThickness( 0.f ),
  mUnderlinePosition( 0.f ),
  mTextSize( Vector3::ZERO ),
  mWeight( TextStyle::REGULAR ),
  mTextColor( NULL )
{
}

TextAttachment::~TextAttachment()
{
  delete mTextColor;
}

void TextAttachment::SetText(const TextArray& text)
{
  // return if the text hasn't changed
  if( (text.size() == mText.size() ) &&
      std::equal(mText.begin(), mText.end(), text.begin ()) )
  {
    return;
  }

  // Cache for public getters
  mText = text;
  mTextChanged = true;
}

void TextAttachment::SetFont(Font& font)
{
  if( font == *mFont )
  {
    return;
  }
  // References the font
  mFont = &font;
  mFontChanged = true;
}

void TextAttachment::SetGradientColor( const Vector4& color )
{
  AllocateTextParameters();

  if( mTextParameters->mGradientColor != color )
  {
    mTextParameters->mGradientColor = color;

    SetGradientColorMessage( mStage->GetUpdateInterface(), *mSceneObject, color );
  }
}

const Vector4& TextAttachment::GetGradientColor() const
{
  if( mTextParameters )
  {
    return mTextParameters->mGradientColor;
  }
  else
  {
    return TextStyle::DEFAULT_GRADIENT_COLOR;
  }
}

void TextAttachment::SetGradientStartPoint( const Vector2& position )
{
  AllocateTextParameters();

  if( mTextParameters->mGradientStartPoint != position )
  {
    mTextParameters->mGradientStartPoint = position;

    SetGradientStartPointMessage( mStage->GetUpdateInterface(), *mSceneObject, position );
  }
}

const Vector2& TextAttachment::GetGradientStartPoint() const
{
  if( mTextParameters )
  {
    return mTextParameters->mGradientStartPoint;
  }
  else
  {
    return TextStyle::DEFAULT_GRADIENT_START_POINT;
  }
}

void TextAttachment::SetGradientEndPoint( const Vector2& position )
{
  AllocateTextParameters();

  if( mTextParameters->mGradientEndPoint != position )
  {
    mTextParameters->mGradientEndPoint = position;

    SetGradientEndPointMessage( mStage->GetUpdateInterface(), *mSceneObject, position );
  }
}

const Vector2& TextAttachment::GetGradientEndPoint() const
{
  if( mTextParameters )
  {
    return mTextParameters->mGradientEndPoint;
  }
  else
  {
    return TextStyle::DEFAULT_GRADIENT_END_POINT;
  }
}

void TextAttachment::SetSmoothEdge( float smoothEdge )
{
  mSmoothing = smoothEdge;
  float weightedSmoothing = smoothEdge;

  // Adjust edge smoothing for font weight
  const float BOLDER = 0.20f;
  const float LIGHTER = 1.65f;
  const float offset = 1.0f - ( ( 1.0f / Dali::TextStyle::EXTRABLACK ) * mWeight );
  weightedSmoothing *= BOLDER + ( ( LIGHTER - BOLDER ) * offset );
  weightedSmoothing = std::max( 0.0f, weightedSmoothing );
  weightedSmoothing = std::min( 1.0f, weightedSmoothing );

  SetSmoothEdgeMessage( mStage->GetUpdateInterface(), *mSceneObject, weightedSmoothing );
}

float TextAttachment::GetSmoothEdge() const
{
  return mSmoothing;
}

void TextAttachment::SetOutline( bool enable, const Vector4& color, const Vector2& thickness )
{
  AllocateTextParameters();

  if (enable != mTextParameters->mOutlineEnabled || color != mTextParameters->mOutlineColor || thickness != mTextParameters->mOutline)
  {
    mTextParameters->SetOutline( enable, color, thickness );

    SetOutlineMessage( mStage->GetUpdateInterface(), *mSceneObject, enable, color, thickness );
  }
}

bool TextAttachment::GetOutline() const
{
  bool result( false );

  if( mTextParameters )
  {
    result = mTextParameters->mOutlineEnabled;
  }

  return result;
}

void TextAttachment::GetOutlineParams( Vector4& color, Vector2& thickness ) const
{
  if( mTextParameters )
  {
    color = mTextParameters->mOutlineColor;
    thickness = mTextParameters->mOutline;
  }
  else
  {
    color = TextStyle::DEFAULT_OUTLINE_COLOR;
    thickness = TextStyle::DEFAULT_OUTLINE_THICKNESS;
  }
}

void TextAttachment::SetGlow( bool enable, const Vector4& color, float intensity )
{
  AllocateTextParameters();

  if (enable != mTextParameters->mGlowEnabled || color != mTextParameters->mGlowColor || fabsf(intensity - mTextParameters->mGlow) > Math::MACHINE_EPSILON_1000)
  {
    mTextParameters->SetGlow( enable, color, intensity );

    SetGlowMessage( mStage->GetUpdateInterface(), *mSceneObject, enable, color, intensity );
  }
}

bool TextAttachment::GetGlow() const
{
  bool result( false );

  if( mTextParameters )
  {
    result = mTextParameters->mGlowEnabled;
  }

  return result;
}

void TextAttachment::GetGlowParams( Vector4& color, float&  intensity) const
{
  if( mTextParameters )
  {
    color = mTextParameters->mGlowColor;
    intensity = mTextParameters->mGlow;
  }
  else
  {
    color = TextStyle::DEFAULT_GLOW_COLOR;
    intensity = TextStyle::DEFAULT_GLOW_INTENSITY;
  }
}

void TextAttachment::SetShadow(bool enable, const Vector4& color, const Vector2& offset, const float size)
{
  AllocateTextParameters();

  if (enable != mTextParameters->mDropShadowEnabled ||
      color != mTextParameters->mDropShadowColor ||
      offset != mTextParameters->mDropShadow ||
      fabsf(size - mTextParameters->mDropShadowSize) > Math::MACHINE_EPSILON_1 )
  {
    mTextParameters->SetShadow( enable, color, offset, size );

    const float unitPointSize( 64.0f );
    const float unitsToPixels( mFont->GetUnitsToPixels());

    float shadowSize( (size * 0.25f) / unitsToPixels );

    Vector2 shadowOffset( offset * ( unitPointSize / mFont->GetPointSize() ) );

    SetDropShadowMessage( mStage->GetUpdateInterface(), *mSceneObject, enable, color, shadowOffset, shadowSize );
  }
}

bool TextAttachment::GetShadow() const
{
  bool result( false );

  if( mTextParameters )
  {
    result = mTextParameters->mDropShadowEnabled;
  }

  return result;
}

void TextAttachment::GetShadowParams( Vector4& color, Vector2& offset, float& size ) const
{
  if( mTextParameters )
  {
    color = mTextParameters->mDropShadowColor;
    offset = mTextParameters->mDropShadow;
    size = mTextParameters->mDropShadowSize;
  }
  else
  {
    color = TextStyle::DEFAULT_SHADOW_COLOR;
    offset = TextStyle::DEFAULT_SHADOW_OFFSET;
    size = TextStyle::DEFAULT_SHADOW_SIZE;
  }
}

void TextAttachment::SetTextColor(const Vector4& color)
{
  bool sendMessage( false );

  if( NULL == mTextColor )
  {
    if( color != TextStyle::DEFAULT_TEXT_COLOR )
    {
      // A color (different from default) has been set, so allocate storage for the text color
      mTextColor = new Vector4( color );
      sendMessage = true;
    }
  }
  else
  {
    if( *mTextColor != color )
    {
      // text color has changed
      *mTextColor = color;
      sendMessage = true;
    }
  }

  if( sendMessage )
  {
    SetTextColorMessage( mStage->GetUpdateInterface(), *mSceneObject, color );
  }
}

Vector4 TextAttachment::GetTextColor() const
{
  Vector4 color;

  if( NULL != mTextColor )
  {
    color = *mTextColor;
  }
  else
  {
    color = TextStyle::DEFAULT_TEXT_COLOR;
  }

  return color;
}

void TextAttachment::SetItalics( const Radian& angle )
{
  if ( angle != mItalicsAngle )
  {
    mItalicsChanged = true;
    mItalicsAngle = angle;
  }
  Radian radian0( 0.0f );

  if( radian0 != mItalicsAngle )
  {
    mItalicsEnabled = true;
  }
  else
  {
    mItalicsEnabled = false;
  }
}

const Radian& TextAttachment::GetItalics() const
{
  return mItalicsAngle;
}

void TextAttachment::SetUnderline( bool enable, float thickness, float position )
{
  if ( enable != mUnderlineEnabled )
  {
    mUnderlineEnabled = enable;
    mUnderlineChanged = true;
  }

  if( mUnderlineEnabled )
  {
    if( fabsf( thickness - mUnderlineThickness ) > Math::MACHINE_EPSILON_0 )
    {
      mUnderlineThickness = thickness;
      mUnderlineChanged = true;
    }

    if( fabsf( position - mUnderlinePosition ) > Math::MACHINE_EPSILON_0 )
    {
      mUnderlinePosition = position;
      mUnderlineChanged = true;
    }
  }
}

bool TextAttachment::GetUnderline() const
{
  return mUnderlineEnabled;
}

float TextAttachment::GetUnderlineThickness() const
{
  return mUnderlineThickness;
}

float TextAttachment::GetUnderlinePosition() const
{
  return mUnderlinePosition;
}

void TextAttachment::SetWeight( TextStyle::Weight weight )
{
  mWeight = weight;

  SetSmoothEdge(mSmoothing);
}

TextStyle::Weight TextAttachment::GetWeight() const
{
  return mWeight;
}

Vector3 TextAttachment::MeasureText() const
{
  return mFont->MeasureText( mText );
}

void TextAttachment::TextChanged()
{
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::Verbose, "TextAttachment::TextChanged()  TextModified:%s  TextEmpty:%s\n", IsTextModified()?"Y":"N", mText.empty()?"Y":"N");

  if( !IsTextModified() )
  {
    return;
  }

  // if the underline or italics have changed we trigger a text request
  if( mUnderlineChanged || mItalicsChanged )
  {
    mTextChanged = true;
  }

  TextFormat format( mUnderlineEnabled, mIsLeftToRight, mItalicsEnabled, mItalicsAngle, mFont->GetPointSize(), mUnderlineThickness, mUnderlinePosition );

  if ( mTextChanged && mFontChanged )
  {
    mVertexBuffer = mTextRequestHelper.SetTextAndFont( mText, mFont, format );
  }
  else if( mTextChanged )
  {
    mVertexBuffer = mTextRequestHelper.SetText( mText, format );
  }
  else if ( mFontChanged )
  {
    mVertexBuffer = mTextRequestHelper.SetFont( mFont, format );
  }

  // sceneObject is being used in a separate thread; queue a message to set
  SetTextChanges();

  mTextChanged = false;
  mFontChanged = false;
  mUnderlineChanged = false;
  mItalicsChanged = false;
}

bool TextAttachment::IsTextLoaded()
{
  bool loaded = mTextRequestHelper.IsTextLoaded();
  if( loaded && !mTextureIdSet )
  {
    mTextureIdSet = true;
  }

  return loaded;
}

void TextAttachment::TextureResized( const TextureIdList& oldTextureIds, unsigned int newTextureId )
{
  bool matched( false );

  // check if resized texture is the one we are using
  for( std::size_t i = 0, count = oldTextureIds.size(); i < count; ++i )
  {
    if( oldTextureIds[i] == mTextureId )
    {
      matched = true;
      break;
    }
  }

  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "TextAttachment::TextureResized() Current texture: %d  New texture: %d\n", mTextureId, newTextureId);

  if( newTextureId == mTextureId )
  {
    // nothing has changed, we are using the new texture already
    return;
  }

  // the texture we're using has been replaced
  // re-request the text vertex information and update the texture id on the scene graph text attachment
  if( matched )
  {
    mTextRequestHelper.TextureChanged( mTextureId, newTextureId );
    mTextureId = newTextureId;
    mTextChanged = true;
    TextChanged();
    return;
  }
}

void TextAttachment::TextureSplit( FontId fontId, const TextureIdList& oldTextureIds, unsigned int newTextureId )
{
  // currently not supported.
  // the implementation will be  if( fontId == mFont->GetId() ) TextureResized(..);
}

bool TextAttachment::IsTextModified()
{
  return ( mTextChanged || mFontChanged || mUnderlineChanged || mItalicsChanged );
}

void TextAttachment::AllocateTextParameters()
{
  if( !mTextParameters )
  {
    mTextParameters = new TextParameters;
  }
}

void TextAttachment::OnStageConnection2()
{
  // do nothing
}

void TextAttachment::OnStageDisconnection2()
{
  // do nothing
}

const SceneGraph::RenderableAttachment& TextAttachment::GetSceneObject() const
{
  DALI_ASSERT_DEBUG( mSceneObject != NULL );
  return *mSceneObject;
}

void TextAttachment::SetTextChanges()
{
  // record the natural size of the text
  mTextSize = mVertexBuffer->mVertexMax;

  // remember the texture id, so we can detect atlas resizes / splits
  mTextureId = mVertexBuffer->mTextureId;

  EventToUpdate& eventToUpdate(  mStage->GetUpdateInterface() );
  const SceneGraph::TextAttachment& attachment( *mSceneObject );

  if( mTextChanged  || mFontChanged )
  {
    DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "TextAttachment::SetTextChanges() Sending VertexBuffer to attachment:%p  textureId:%d\n", &attachment, mVertexBuffer->mTextureId);

    // release the vertex buffer to pass  ownership to the scene-graph-text-attachment
    SetTextVertexBufferMessage( eventToUpdate, attachment, *mVertexBuffer.Release() );
  }
}


} // namespace Internal

} // namespace Dali

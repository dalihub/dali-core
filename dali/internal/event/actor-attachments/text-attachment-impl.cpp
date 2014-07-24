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

#include <dali/internal/event/actor-attachments/text-attachment-impl.h>

// INTERNAL INCLUDES

#include <dali/public-api/common/dali-common.h>
#include <dali/internal/update/node-attachments/scene-graph-text-attachment.h>
#include <dali/internal/event/common/stage-impl.h>

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

  attachment->CalculateWeightedSmoothing( TextStyle::DEFAULT_FONT_WEIGHT, TextStyle::DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD );      // adjust smoothedge for font weight

  return attachment;
}

TextAttachment::TextAttachment( Stage& stage )
: RenderableAttachment( stage ),
  mTextRequestHelper( *this ),
  mTextColor( NULL ),
  mIsLeftToRight(true),
  mTextChanged( true ),
  mFontChanged( true ),
  mUnderlineChanged( true ),
  mItalicsChanged( true ),
  mTextureIdSet( false ),
  mTextureId(0),
  mTextSize( Vector3::ZERO )
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

void TextAttachment::SetTextColor( const Vector4& color )
{
  bool sendMessage( false );

  Vector4 clampedColor = Clamp( color, 0.f, 1.f );

  if( NULL == mTextColor )
  {
    // A color (different from default) has been set, so allocate storage for the text color
    mTextColor = new Vector4( clampedColor );
    sendMessage = true;
  }
  else
  {
    if( *mTextColor != clampedColor )
    {
      // text color has changed
      *mTextColor = clampedColor;
      sendMessage = true;
    }
  }

  if( sendMessage )
  {
    SetTextColorMessage( mStage->GetUpdateInterface(), *mSceneObject, clampedColor );
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

void TextAttachment::ResetTextColor()
{
  if( NULL != mTextColor )
  {
    delete mTextColor;
    mTextColor = NULL;

    SetTextColorMessage( mStage->GetUpdateInterface(), *mSceneObject, TextStyle::DEFAULT_TEXT_COLOR );
  }
}

void TextAttachment::SetWeight( TextStyle::Weight weight )
{
  if( mStyle.IsFontWeightDefault() ||
      ( mStyle.GetWeight() != weight ) )
  {
    mStyle.SetWeight( weight );

    CalculateWeightedSmoothing( weight, mStyle.GetSmoothEdge() );
  }
}

TextStyle::Weight TextAttachment::GetWeight() const
{
  return mStyle.GetWeight();
}

void TextAttachment::ResetWeight()
{
  if( !mStyle.IsFontWeightDefault() )
  {
    mStyle.Reset( TextStyle::WEIGHT );

    CalculateWeightedSmoothing( TextStyle::DEFAULT_FONT_WEIGHT, mStyle.GetSmoothEdge() );
  }
}

void TextAttachment::SetSmoothEdge( float smoothEdge )
{
  if( mStyle.IsSmoothEdgeDefault() ||
      ( fabsf( smoothEdge - mStyle.GetSmoothEdge() ) > Math::MACHINE_EPSILON_1000 ) )
  {
    mStyle.SetSmoothEdge( smoothEdge );
    CalculateWeightedSmoothing( mStyle.GetWeight(), smoothEdge );
  }
}

float TextAttachment::GetSmoothEdge() const
{
  return mStyle.GetSmoothEdge();
}

void TextAttachment::ResetSmoothEdge()
{
  if( !mStyle.IsSmoothEdgeDefault() )
  {
    mStyle.Reset( TextStyle::SMOOTH );

    CalculateWeightedSmoothing( mStyle.GetWeight(), TextStyle::DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD );
  }
}

void TextAttachment::SetItalics( Radian angle )
{
  if( mStyle.IsItalicsDefault() ||
      ( Radian( mStyle.GetItalicsAngle() ) != angle ) )
  {
    mItalicsChanged = true;

    const Radian radian0( 0.0f );
    mStyle.SetItalics( ( radian0 != angle ), Degree( angle ) );
  }
}

bool TextAttachment::GetItalics() const
{
  return mStyle.IsItalicsEnabled();
}

Radian TextAttachment::GetItalicsAngle() const
{
  return Radian( mStyle.GetItalicsAngle() );
}

void TextAttachment::ResetItalics()
{
  if( !mStyle.IsItalicsDefault() )
  {
    mStyle.Reset( TextStyle::ITALICS );

    mItalicsChanged = true;
  }
}

void TextAttachment::SetUnderline( bool enable, float thickness, float position )
{
  if( mStyle.IsUnderlineDefault() ||
      ( mStyle.IsUnderlineEnabled() != enable ) ||
      ( fabsf( mStyle.GetUnderlineThickness() - thickness ) > Math::MACHINE_EPSILON_1000 ) ||
      ( fabsf( mStyle.GetUnderlinePosition() - position ) > Math::MACHINE_EPSILON_1000 ) )
  {
    mUnderlineChanged = true;

    mStyle.SetUnderline( enable, thickness, position );
  }
}

bool TextAttachment::GetUnderline() const
{
  return mStyle.IsUnderlineEnabled();
}

float TextAttachment::GetUnderlineThickness() const
{
  return mStyle.GetUnderlineThickness();
}

float TextAttachment::GetUnderlinePosition() const
{
  return mStyle.GetUnderlinePosition();
}

void TextAttachment::ResetUnderline()
{
  if( !mStyle.IsUnderlineDefault()  )
  {
    mStyle.Reset( TextStyle::UNDERLINE );

    mUnderlineChanged = true;
  }
}

void TextAttachment::SetOutline( bool enable, const Vector4& color, const Vector2& thickness )
{
  if( mStyle.IsOutlineDefault() ||
      ( mStyle.IsOutlineEnabled() != enable ) ||
      ( mStyle.GetOutlineColor() != color ) ||
      ( mStyle.GetOutlineThickness() != thickness ) )
  {
    mStyle.SetOutline( enable, color, thickness );

    SetOutlineMessage( mStage->GetUpdateInterface(), *mSceneObject, enable, color, thickness );
  }
}

bool TextAttachment::GetOutline() const
{
  return mStyle.IsOutlineEnabled();
}

void TextAttachment::GetOutlineParams( Vector4& color, Vector2& thickness ) const
{
  color = mStyle.GetOutlineColor();
  thickness = mStyle.GetOutlineThickness();
}

void TextAttachment::ResetOutline()
{
  if( !mStyle.IsOutlineDefault() )
  {
    mStyle.Reset( TextStyle::OUTLINE );

    SetOutlineMessage( mStage->GetUpdateInterface(), *mSceneObject, false, TextStyle::DEFAULT_OUTLINE_COLOR, TextStyle::DEFAULT_OUTLINE_THICKNESS );
  }
}

void TextAttachment::SetGlow( bool enable, const Vector4& color, float intensity )
{
  if( mStyle.IsGlowDefault() ||
      ( mStyle.IsGlowEnabled() != enable ) ||
      ( mStyle.GetGlowColor() != color ) ||
      ( fabsf( mStyle.GetGlowIntensity() - intensity ) > Math::MACHINE_EPSILON_1000 ) )
  {
    mStyle.SetGlow( enable, color, intensity );

    SetGlowMessage( mStage->GetUpdateInterface(), *mSceneObject, enable, color, intensity );
  }
}

bool TextAttachment::GetGlow() const
{
  return mStyle.IsGlowEnabled();
}

void TextAttachment::GetGlowParams( Vector4& color, float&  intensity) const
{
    color = mStyle.GetGlowColor();
    intensity = mStyle.GetGlowIntensity();
}

void TextAttachment::ResetGlow()
{
  if( !mStyle.IsGlowDefault() )
  {
    mStyle.Reset( TextStyle::GLOW );

    SetGlowMessage( mStage->GetUpdateInterface(), *mSceneObject, false, TextStyle::DEFAULT_GLOW_COLOR, TextStyle::DEFAULT_GLOW_INTENSITY );
  }
}

void TextAttachment::SetShadow( bool enable, const Vector4& color, const Vector2& offset, float size )
{
  if( mStyle.IsShadowDefault() ||
      ( mStyle.IsShadowEnabled() != enable ) ||
      ( mStyle.GetShadowColor() != color ) ||
      ( mStyle.GetShadowOffset() != offset ) ||
      ( fabsf( mStyle.GetShadowSize() - size ) > Math::MACHINE_EPSILON_1000 ) )
  {
    mStyle.SetShadow( enable, color, offset, size );

    const float unitPointSize( 64.0f );
    const float unitsToPixels( mFont->GetUnitsToPixels());
    const float fontPointSize( mFont->GetPointSize() );

    float shadowSize( (size * 0.25f) / unitsToPixels );

    Vector2 shadowOffset( offset );
    Vector2 maxOffset( fontPointSize / 4.5f, fontPointSize / 4.5f );
    shadowOffset = Min( shadowOffset, maxOffset );
    shadowOffset = Max( shadowOffset, -maxOffset );
    shadowOffset *= unitPointSize / fontPointSize;
    SetDropShadowMessage( mStage->GetUpdateInterface(), *mSceneObject, enable, color, shadowOffset, shadowSize );
  }
}

bool TextAttachment::GetShadow() const
{
  return mStyle.IsShadowEnabled();
}

void TextAttachment::GetShadowParams( Vector4& color, Vector2& offset, float& size ) const
{
  color = mStyle.GetShadowColor();
  offset = mStyle.GetShadowOffset();
  size = mStyle.GetShadowSize();
}

void TextAttachment::ResetShadow()
{
  if( !mStyle.IsShadowDefault() )
  {
    mStyle.Reset( TextStyle::SHADOW );

    const float unitPointSize( 64.0f );
    const float unitsToPixels( mFont->GetUnitsToPixels());
    const float fontPointSize( mFont->GetPointSize() );

    float shadowSize( ( TextStyle::DEFAULT_SHADOW_SIZE * 0.25f ) / unitsToPixels );

    Vector2 shadowOffset( TextStyle::DEFAULT_SHADOW_OFFSET );
    Vector2 maxOffset( fontPointSize / 4.5f, fontPointSize / 4.5f );
    shadowOffset = Min( shadowOffset, maxOffset );
    shadowOffset = Max( shadowOffset, -maxOffset );
    shadowOffset *= unitPointSize / fontPointSize;
    SetDropShadowMessage( mStage->GetUpdateInterface(), *mSceneObject, false, TextStyle::DEFAULT_SHADOW_COLOR, shadowOffset, shadowSize );
  }
}

void TextAttachment::SetGradient( const Vector4& color, const Vector2& startPoint, const Vector2& endPoint )
{
  if( mStyle.IsGradientDefault() ||
      ( mStyle.GetGradientColor() != color ) ||
      ( mStyle.GetGradientStartPoint() != startPoint ) ||
      ( mStyle.GetGradientEndPoint() != endPoint ) )
  {
    mStyle.SetGradient( true, color, startPoint, endPoint );
    SetGradientMessage( mStage->GetUpdateInterface(), *mSceneObject, color, startPoint, endPoint );
  }
}

const Vector4& TextAttachment::GetGradientColor() const
{
  return mStyle.GetGradientColor();
}

const Vector2& TextAttachment::GetGradientStartPoint() const
{
  return mStyle.GetGradientStartPoint();
}

const Vector2& TextAttachment::GetGradientEndPoint() const
{
  return mStyle.GetGradientEndPoint();
}

void TextAttachment::ResetGradient()
{
  if( !mStyle.IsGradientDefault() )
  {
    mStyle.Reset( TextStyle::GRADIENT );

    SetGradientMessage( mStage->GetUpdateInterface(), *mSceneObject, TextStyle::DEFAULT_GRADIENT_COLOR, TextStyle::DEFAULT_GRADIENT_START_POINT, TextStyle::DEFAULT_GRADIENT_END_POINT );
  }
}

void TextAttachment::GetTextStyle( TextStyle& style ) const
{
  style.Copy( mStyle );

  // Font name, font style, font point size and color are not store inside the mStyle, so they need to be copied after.

  if( NULL != mTextColor )
  {
    style.SetTextColor( *mTextColor );
  }

  if( !mFont->IsDefaultSystemFont() )
  {
    style.SetFontName( mFont->GetName() );
    style.SetFontStyle( mFont->GetStyle() );
  }

  if( !mFont->IsDefaultSystemSize() )
  {
    style.SetFontPointSize( PointSize( mFont->GetPointSize() ) );
  }
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

  TextFormat format( mStyle.IsUnderlineEnabled(),
                     mIsLeftToRight,
                     mStyle.IsItalicsEnabled(),
                     Radian( mStyle.GetItalicsAngle() ),
                     mFont->GetPointSize(),
                     mStyle.GetUnderlineThickness(),
                     mStyle.GetUnderlinePosition() );

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

void TextAttachment::CalculateWeightedSmoothing( TextStyle::Weight weight, float smoothEdge )
{
  float weightedSmoothing = smoothEdge;

  // Adjust edge smoothing for font weight
  const float BOLDER = 0.20f;
  const float LIGHTER = 1.65f;
  const float offset = 1.0f - ( ( 1.0f / Dali::TextStyle::EXTRABLACK ) * weight );
  weightedSmoothing *= BOLDER + ( ( LIGHTER - BOLDER ) * offset );
  weightedSmoothing = std::max( 0.0f, weightedSmoothing );
  weightedSmoothing = std::min( 1.0f, weightedSmoothing );

  SetSmoothEdgeMessage( mStage->GetUpdateInterface(), *mSceneObject, weightedSmoothing );
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

    if( mFontChanged )
    {
      SetTextFontSizeMessage( eventToUpdate, attachment, mFont->GetPixelSize() );
    }
  }
}

} // namespace Internal

} // namespace Dali

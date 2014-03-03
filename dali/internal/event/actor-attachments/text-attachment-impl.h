#ifndef __DALI_INTERNAL_TEXT_ATTACHMENT_H__
#define __DALI_INTERNAL_TEXT_ATTACHMENT_H__

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
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/text/text-style.h>
#include <dali/internal/event/actor-attachments/renderable-attachment-impl.h>
#include <dali/internal/event/actor-attachments/actor-attachment-declarations.h>
#include <dali/internal/event/text/font-declarations.h>
#include <dali/internal/common/text-array.h>
#include <dali/internal/event/text/text-request-helper.h>
#include <dali/internal/event/text/resource/glyph-texture-observer.h>
#include <dali/internal/common/owner-pointer.h>

namespace Dali
{

namespace Internal
{

class TextParameters;

namespace SceneGraph
{
class TextAttachment;
class Node;
}

/**
 * An attachment for rendering text from a font atlas.
 */
class TextAttachment : public RenderableAttachment, public GlyphTextureObserver
{
public:

  /**
   * Create a new TextAttachment.
   * @param[in] parentNode The node to attach a scene-object to.
   * @param [in] text The text which will be displayed.
   * @param [in] font The font which will be used for the text.
   * @param [in] isLeftToRight Text is displayed from left to right if true, otherwise from right to left.
   * @return A smart-pointer to the newly allocated TextAttachment.
   */
  static TextAttachmentPtr New( const SceneGraph::Node& parentNode, const TextArray& text, FontPointer font, bool isLeftToRight );

  /**
   * Set the text label displayed by the attachment
   * @param [in] text The new text label
   */
  void SetText(const TextArray& text);

  /**
   * Get the text label displayed by the attachment
   * @return The text label
   */
  const TextArray& GetText() const
  {
    // This is not animatable; the cached value is up-to-date.
    return mText;
  }

  /**
   * Get the natural text size
   * @return the natural size of the text.
   */
  const Vector2& GetNaturalTextSize() const
  {
    // This is not animatable; the cached value is up-to-date.
    return mTextSize;
  }

  /**
   * Set the font used to display the text label displayed by the attachment
   * @param [in] font The new font
   */
  void SetFont(Font& font);

  /**
   * Get the font used to display the text label displayed by the attachment
   * @return The font currently in use
   */
  Font& GetFont()
  {
    return *mFont;
  }

  /**
   * @copydoc Dali::TextActor::SetGradientColor()
   */
  void SetGradientColor( const Vector4& color );

  /**
   * @copydoc Dali::TextActor::GetGradientColor()
   */
  const Vector4& GetGradientColor() const;

  /**
   * @copydoc Dali::TextActor::SetGradientStartPoint()
   */
  void SetGradientStartPoint( const Vector2& position );

  /**
   * @copydoc Dali::TextActor::GetGradientStartPoint()
   */
  const Vector2& GetGradientStartPoint() const;

  /**
   * @copydoc Dali::TextActor::SetGradientEndPoint()
   */
  void SetGradientEndPoint( const Vector2& position );

  /**
   * @copydoc Dali::TextActor::GetGradientEndPoint()
   */
  const Vector2& GetGradientEndPoint() const;

  /**
   * @copydoc Dali::TextActor::SetSmoothEdge(const float)
   */
  void SetSmoothEdge(const float smoothEdge);

  /**
   * Retrieve the smooth edge value.
   * @return the smooth edge value.
   */
  float GetSmoothEdge() const;

  /**
   * @copydoc Dali::TextActor::SetOutline(const bool,const Vector4&,const Vector2&)
   */
  void SetOutline( bool enable, const Vector4& color, const Vector2& thickness );

  /**
   * Get outline state.
   * @return \e true if outline is enabled, otherwise \e false.
   */
  bool GetOutline() const;

  /**
   * Retrieve outline parameters.
   * @param[out] color The outline color.
   * @param[out] thickness The outline parameters.
   */
  void GetOutlineParams( Vector4& color, Vector2& thickness ) const;

  /**
   * @copydoc Dali::TextActor::SetGlow(const bool,const Vector4&,const float)
   */
  void SetGlow( bool enable, const Vector4& color, const float intensity);

  /**
   * Get glow state.
   * @return \e true if glow is enabled, otherwise \e false.
   */
  bool GetGlow() const;

  /**
   * Retrieve glow parameters.
   * @param[out] color The glow color.
   * @param[out] intensity The glow intensity.
   */
  void GetGlowParams( Vector4& color, float& intensity ) const;

  /**
   * @copydoc Dali::TextActor::SetShadow(const bool,const Vector4&,const Vector2&,const float)
   */
  void SetShadow( bool enable, const Vector4& color, const Vector2& offset, const float size);

  /**
   * Retrieve the shadow state.
   * @return \e true if shadow is enabled, otherwise \e false.
   */
  bool GetShadow() const;

  /**
   * Retrieve shadow parameters.
   * @param[out] color The shadow color.
   * @param[out] offset The shadow offset.
   * @param[out] size The shadow size.
   */
  void GetShadowParams( Vector4& color, Vector2& offset, float& size ) const;

  /**
   * Set the text color
   * @param[in] color The text color
   */
  void SetTextColor(const Vector4& color);

  /**
   * Get the text color
   * @return The text color
   */
  Vector4 GetTextColor() const;

  /**
   * Enable italics on the text actor, the text will be sheared by the given angle.
   * @param[in] angle Italics angle in radians.
   */
  void SetItalics( const Radian& angle );

  /**
   * @copydoc Dali::TextActor::GetItalics()
   */
  const Radian& GetItalics() const;

  /**
   * @copydoc Dali::TextActor::SetUnderline()
   */
  void SetUnderline( bool enable, float thickness, float position );

  /**
   * @copydoc Dali::TextActor::GetUnderline()
   */
  bool GetUnderline() const;

  /**
   * @copydoc Internal::TextActor::GetUnderlineThickness()
   */
  float GetUnderlineThickness() const;

  /**
   * @copydoc Internal::TextActor::GetUnderlinePosition()
   */
  float GetUnderlinePosition() const;

  /**
   * @copydoc Dali::TextActor::SetWeight()
   */
  void SetWeight( TextStyle::Weight weight );

  /**
   * @copydoc Dali::TextActor::GetWeight()
   */
  TextStyle::Weight GetWeight() const;

  /**
   * Measure the natural size of a text string, as displayed in this font.
   * @return The natural size of the text.
   */
  Vector3 MeasureText() const;

  /**
   * Inform the text attachment the text / font it is using has changed.
   * The TextChanged() and TextNotRequired() functions are used to avoid duplicate
   * requests for text. For example if the text is changed, and the font is changed
   * then we want to avoid requesting the new text with the old font, then the new
   * text with the new font.
   */
  void TextChanged();

  /**
   * @return true if all text has been loaded and is ready to display
   */
  bool IsTextLoaded();

public: // glyph texture observer

  /**
   *  @copydoc Dali::Internal::GlyphTextureObserver::TextureResized()
   */
  virtual void TextureResized( const TextureIdList& oldTextureIds, unsigned int newTextureId );

  /**
   * @copydoc Dali::Internal::GlyphTextureObserver::TextureSplit()
   */
  virtual void TextureSplit( FontId fontId, const TextureIdList& oldTextureIds, unsigned int newTextureId );

private:

  /**
   * Check if the text has been modified
   * @return true if the text or it's formating has been modified
   */
   bool IsTextModified();

  /**
   * Allocate a TextParameters object on this TextAttachment if one does not exist
   */
  void AllocateTextParameters();

  /**
   * First stage construction of a TextAttachment.
   * @param[in] stage Used to send messages to scene-graph.
   */
  TextAttachment( Stage& stage );

  /**
   * @copydoc Dali::Internal::RenderableAttachment::OnStageConnection2()
   */
  virtual void OnStageConnection2();

  /**
   * @copydoc Dali::Internal::RenderableAttachment::OnStageDisconnection2()
   */
  virtual void OnStageDisconnection2();

  /**
   * @copydoc Dali::Internal::RenderableAttachment::GetSceneObject()
   */
  virtual const SceneGraph::RenderableAttachment& GetSceneObject() const;

  /**
   * Update any text & font changes to the on stage scene graph text attachment
   */
  void SetTextChanges();

  /**
   * Send new texture-id message
   */
  void SendTextureIdMessage();

  /**
   * Make a request to load the current text
   * @param uniqueText the text to request to be loaded
   */
  void RequestText( const TextArray& uniqueText );

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~TextAttachment();

private:

  const SceneGraph::TextAttachment* mSceneObject; ///< Not owned

  TextRequestHelper mTextRequestHelper;  ///< Text request helper
  FontPointer mFont; ///< The font resource

  // Cached values for public getters
  TextArray mText;

  bool    mUnderlineEnabled:1;  ///< whether the text is underlined
  bool    mIsLeftToRight:1;     ///< whether the text is left to right
  bool    mTextChanged:1;       ///< whether the text has changed
  bool    mFontChanged:1;       ///< whether the font has changed
  bool    mUnderlineChanged:1;  ///< whether the underline has changed
  bool    mItalicsChanged:1;    ///< whether the italics has changed
  bool    mItalicsEnabled:1;    ///< whether italics are enabled
  bool    mTextureIdSet:1;      ///< flag to signify if the texture id has been set on the scene-graph text-attachment
  unsigned int mTextureId;      ///< the texture id of the glyph atlas being used
  float   mSmoothing;           ///< edge smoothing for the font
  Radian  mItalicsAngle;        ///< font italics angle
  float mUnderlineThickness;    ///< The underline's thickness.
  float mUnderlinePosition;     ///< The underline's position.
  Vector2 mTextSize;            ///< text natural size
  TextStyle::Weight mWeight;    ///< font weight

  OwnerPointer< TextVertexBuffer > mVertexBuffer; ///< vertex data to display the text
  OwnerPointer< TextParameters > mTextParameters; ///< on demand storage for text effect parameters
  Vector4* mTextColor;                            ///< on demand storage for text color
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_TEXT_ATTACHMENT_H__

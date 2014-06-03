#ifndef __DALI_INTERNAL_TEXT_ACTOR_H__
#define __DALI_INTERNAL_TEXT_ACTOR_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/actors/text-actor.h>
#include <dali/internal/event/actors/actor-declarations.h>
#include <dali/internal/event/actors/renderable-actor-impl.h>
#include <dali/internal/event/text/text-observer.h>
#include <dali/internal/event/actor-attachments/actor-attachment-declarations.h>
#include <dali/internal/common/text-array.h>


namespace Dali
{

namespace Internal
{

class Font;

/**
 * TextActor implementation.
 * A text actor is an actor with a TextAttachment.
 * If no size is given to the actor, then after the font has loaded
 * the actor will be set to the size of the text displayed.
 *
 */
class TextActor : public RenderableActor, public TextObserver
{
public:

  enum TextRequestMode
  {
    REQUEST_NEW_TEXT,       ///< When changing font, request new text
    DONT_REQUEST_NEW_TEXT   ///< When changing font, don't request new text
  };

  /**
   * @copydoc Dali::TextActor::New(const Dali::Text&,bool,bool)
   * @return A pointer to a new TextActor.
   */
  static TextActorPtr New(const Dali::Text& text, bool fontDetection, bool isLeftToRight);

  /**
   * @copydoc Dali::TextActor::New(const Dali::Text&,Dali::Font,bool,bool)
   * @return A pointer to a new TextActor.
   */
  static TextActorPtr New(const Dali::Text& text, bool fontDetection, bool isLeftToRight, Font& font);

  /**
   * @copydoc Dali::TextActor::New(const Text&,const TextStyle&,bool,bool)
   * @return A pointer to a new TextActor.
   */
  static TextActorPtr New(const Dali::Text& text, bool fontDetection, bool isLeftToRight, const Dali::TextStyle& style );

  /**
   * @copydoc Dali::Internal::Actor::OnInitialize
   */
  void OnInitialize();

  /**
   * @copydoc Dali::TextActor::GetText()
   */
  const std::string GetText() const;

  /**
   * @copydoc Dali::TextActor::SetText()
   */
  void SetText(const std::string& text);

  /**
   * @copydoc Dali::TextActor::SetText()
   */
  void SetText(const Dali::Text& text);

  /**
   * @copybrief Dali::TextActor::SetText(const std::string&)
   * @param[in] utfCodes  An std::vector containing the UTF-32 codes
   */
  void SetText(const TextArray& utfCodes);

  /**
   * @copydoc Dali::TextActor::SetToNaturalSize()
   */
  void SetToNaturalSize();

  /**
   * @copydoc Dali::TextActor::GetFont()
   */
  Font* GetFont() const;

  /**
   * @copydoc Dali::TextActor::SetFont()
   * @param TextRequestMode whether new text should be requested
   */
  void SetFont(Font& font, TextRequestMode mode = REQUEST_NEW_TEXT );

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
   * @copydoc Dali::TextActor::SetGradientStartPoint()
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
   * @copydoc Dali::TextActor::SetTextStyle( const TextStyle& style )
   * @param mode whether new text should be requested
   */
  void SetTextStyle( const TextStyle& style, TextRequestMode mode = REQUEST_NEW_TEXT );

  /**
   * @copydoc Dali::TextActor::GetTextStyle()
   */
  TextStyle GetTextStyle() const;

  /**
   * @copydoc Dali::TextActor::SetTextColor(const Vector4&)
   */
  void SetTextColor(const Vector4& color);

  /**
   * @copydoc Dali::TextActor::GetTextColor()
   */
  Vector4 GetTextColor() const;

  /**
   * @copydoc Dali::TextActor::SetSmoothEdge(const float)
   */
  void SetSmoothEdge(const float smoothEdge);

  /**
   * @copydoc Dali::TextActor::SetOutline(const bool,const Vector4&,const Vector2&)
   */
  void SetOutline(const bool enable, const Vector4& color, const Vector2& thickness);

  /**
   * @copydoc Dali::TextActor::SetGlow(const bool,const Vector4&,const float)
   */
  void SetGlow(const bool enable, const Vector4& color, const float intensity);

  /**
   * @copydoc Dali::TextActor::SetShadow(const bool,const Vector4&,const Vector2&,const float)
   */
  void SetShadow(const bool enable, const Vector4& color, const Vector2& offset, const float size);

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
   * @return The underline's thickness.
   */
  float GetUnderlineThickness() const;

  /**
   * @return The underline's position.
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
   * @copydoc Dali::TextActor::SetFontDetectionAutomatic()
   */
  void SetFontDetectionAutomatic(bool value);

  /**
   * @copydoc Dali::TextActor::IsFontDetectionAutomatic()
   */
  bool IsFontDetectionAutomatic() const;

private: // from Actor

  /**
   * @copydoc Actor::OnSizeSet
   */
  virtual void OnSizeSet(const Vector3& targetSize);

  /**
   * @copydoc Actor::OnSizeAnimation( Animation& animation, const Vector3& targetSize )
   */
  virtual void OnSizeAnimation(Animation& animation, const Vector3& targetSize);

private: // From RenderableActor

  /**
   * @copydoc RenderableActor::GetRenderableAttachment
   */
  virtual RenderableAttachment& GetRenderableAttachment() const;

protected:

  /**
   * Protected constructor; see also TextActor::New()
   */
  TextActor(bool fontDetection, bool isLeftToRight );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~TextActor();

public:

  /**
   * @copydoc Dali::Image::GetLoadingState()
   */
  Dali::LoadingState GetLoadingState() const { return mLoadingState; }

  /**
   * @copydoc Dali::TextActor::TextAvailableSignal()
   */
  Dali::TextActor::TextSignalV2& TextAvailableSignal() { return mLoadingFinishedV2; }

  /**
   * Connects a callback function with the text actors signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

public: // From TextObserver

  /**
   * @copydoc Dali::Internal::TextObserver::TextLoaded()
   */
  virtual void TextLoaded();

private:

  /**
   * Text has been changed observe ticket or
   * emit text available signal
   */
  void TextChanged();

  /**
   * Checks whether the text is loaded or not
   * @return true if text is loaded
   */
  bool CheckTextLoadState();

  /**
   * Stop observing text loads on the current font.
   */
  void StopObservingTextLoads();

  /**
   * Start observing text loads on the current font.
   */
  void StartObservingTextLoads();

  // Undefined
  TextActor(const TextActor&);

  // Undefined
  TextActor& operator=(const TextActor& rhs);

private: // ProxyObject default non-animatable properties
  /**
   * copydoc Dali::Internal::ProxyObject
   */
  virtual unsigned int GetDefaultPropertyCount() const ;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyIndices()
   */
  virtual void GetDefaultPropertyIndices( Property::IndexContainer& indices ) const;

  /**
   * copydoc Dali::Internal::ProxyObject
   */
  virtual const std::string& GetDefaultPropertyName( Property::Index index ) const ;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyIndex()
   */
  virtual Property::Index GetDefaultPropertyIndex(const std::string& name) const;

  /**
   * copydoc Dali::Internal::ProxyObject
   */
  virtual bool IsDefaultPropertyWritable( Property::Index index ) const ;

  /**
   * copydoc Dali::Internal::ProxyObject
   */
  virtual bool IsDefaultPropertyAnimatable( Property::Index index ) const ;

  /**
   * @copydoc Dali::Internal::ProxyObject::IsDefaultPropertyAConstraintInput()
   */
  virtual bool IsDefaultPropertyAConstraintInput( Property::Index index ) const;

  /**
   * copydoc Dali::Internal::ProxyObject
   */
  virtual Property::Type GetDefaultPropertyType( Property::Index index ) const ;

  /**
   * copydoc Dali::Internal::ProxyObject
   */
  virtual void SetDefaultProperty( Property::Index index, const Property::Value& propertyValue ) ;

  /**
   * copydoc Dali::Internal::ProxyObject
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const ;

protected:

  TextAttachmentPtr mTextAttachment; ///< Used to display the text

  Dali::LoadingState mLoadingState;

private:

  Dali::TextActor::TextSignalV2 mLoadingFinishedV2;
  bool mUsingNaturalSize:1;  ///< whether the actor is using natural size
  bool mInternalSetSize:1;  ///< to determine when we are internally setting size
  bool mFontDetection:1;  ///< tells whether TextActor should query platform abstraction after SetText
  bool mIsLeftToRight:1;  ///< tells whether text is displayed from left to right or from right to left
  bool mObserving:1;      ///< Whether the text actor is waiting for text to load
  static bool mFirstInstance;
  static DefaultPropertyLookup* mDefaultTextActorPropertyLookup; ///< Default properties
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::TextActor& GetImplementation(Dali::TextActor& actor)
{
  DALI_ASSERT_ALWAYS( actor && "Actor handle is empty" );

  BaseObject& handle = actor.GetBaseObject();

  return static_cast<Internal::TextActor&>(handle);
}

inline const Internal::TextActor& GetImplementation(const Dali::TextActor& actor)
{
  DALI_ASSERT_ALWAYS( actor && "Actor handle is empty" );

  const BaseObject& handle = actor.GetBaseObject();

  return static_cast<const Internal::TextActor&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_TEXT_ACTOR_H__

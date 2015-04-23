#ifndef __DALI_INTERNAL_IMAGE_ACTOR_H__
#define __DALI_INTERNAL_IMAGE_ACTOR_H__

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
#include <dali/public-api/actors/image-actor.h>
#include <dali/internal/event/actors/actor-declarations.h>
#include <dali/internal/event/actors/renderable-actor-impl.h>
#include <dali/internal/event/actor-attachments/image-attachment-impl.h>
#include <dali/internal/event/animation/animation-impl.h>
#include <dali/internal/event/images/nine-patch-image-impl.h>

namespace Dali
{

namespace Internal
{

class Image;

/**
 * An actor which displays an Image object.
 *
 * This handles image fade-in if required, waiting for the image to load.
 *
 * If a new image is set on the actor, then this ensures that the old image
 * is displayed until the new image is ready to render to prevent flashing
 * to the actor color. This will also happen if the image is reloaded.
 *
 * This is achieved by using two connector objects to Image: mImageNext and
 * mImageAttachment's member object. The first one points to the Image object that is going to
 * be displayed next, the second one to the Image that is currently being displayed.
 */
class ImageActor : public RenderableActor
{
public:

  typedef Dali::ImageActor::Style Style;
  typedef Dali::ImageActor::PixelArea PixelArea;

  /**
   * @brief Create an image actor instance.
   * @return A smart-pointer to a newly allocated image actor.
   */
  static ImageActorPtr New();

  /**
   * @copydoc Dali::Internal::Actor::OnInitialize
   */
  void OnInitialize() ;

  /**
   * @see Dali::ImageActor::SetImage()
   * @param[in] ImagePtr reference to the image object to display. Reference to avoid unnecessary increment/decrement reference.
   */
  void SetImage( ImagePtr& image );

  /**
   * Retrieve the image rendered by the actor's attachment.
   * @return smart pointer to the image or an empty one if no image is assigned
   */
  ImagePtr GetImage();

  /**
   * @copydoc Dali::ImageActor::SetPixelArea()
   */
  void SetPixelArea( const PixelArea& pixelArea );

  /**
   * @copydoc Dali::ImageActor::GetPixelArea()
   */
  const PixelArea& GetPixelArea() const;

  /**
   * @copydoc Dali::ImageActor::IsPixelAreaSet()
   */
  bool IsPixelAreaSet() const;

  /**
   * @copydoc Dali::ImageActor::ClearPixelArea()
   */
  void ClearPixelArea();

  /**
   * @copydoc Dali::ImageActor::SetStyle()
   */
  void SetStyle( Style style );

  /**
   * @copydoc Dali::ImageActor::GetStyle()
   */
  Style GetStyle() const;

  /**
   * @copydoc Dali::ImageActor::SetNinePatchBorder
   */
  void SetNinePatchBorder( const Vector4& border, bool inPixels = false );

  /**
   * @copydoc Dali::ImageActor::GetNinePatchBorder
   */
  Vector4 GetNinePatchBorder() const;

  /**
   * Retrieve the attachment which renders the image.
   * @return The attachment.
   */
  ImageAttachment& GetImageAttachment();

public: // Default property extensions from Object

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyCount()
   */
  virtual unsigned int GetDefaultPropertyCount() const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyIndices()
   */
  virtual void GetDefaultPropertyIndices( Property::IndexContainer& indices ) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyName()
   */
  virtual const char* GetDefaultPropertyName(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyIndex()
   */
  virtual Property::Index GetDefaultPropertyIndex(const std::string& name) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyWritable()
   */
  virtual bool IsDefaultPropertyWritable(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyAnimatable()
   */
  virtual bool IsDefaultPropertyAnimatable(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyAConstraintInput()
   */
  virtual bool IsDefaultPropertyAConstraintInput( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyType()
   */
  virtual Property::Type GetDefaultPropertyType(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::SetDefaultProperty()
   */
  virtual void SetDefaultProperty(Property::Index index, const Property::Value& propertyValue);

  /**
   * @copydoc Dali::Internal::Object::GetDefaultProperty()
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const;

public: // From Actor

  /**
   * @copydoc Dali::Actor::GetNaturalSize()
   */
  virtual Vector3 GetNaturalSize() const;

private: // From RenderableActor

  /**
   * @copydoc RenderableActor::GetRenderableAttachment
   */
  virtual RenderableAttachment& GetRenderableAttachment() const;

protected:

  /**
   * Protected constructor; see also ImageActor::New()
   */
  ImageActor();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ImageActor();

private:

  /**
   * Calculate the natural size of this image actor
   *
   * @return Return the natural size as a Vector2
   */
  Vector2 CalculateNaturalSize() const;

  /**
   * From Actor; used to trigger fade-in animations.
   */
  virtual void OnStageConnectionInternal();

  /**
   * From Actor; used to notify Image.
   */
  virtual void OnStageDisconnectionInternal();

private:

  ImageAttachmentPtr mImageAttachment; ///< Used to display the image (holds a pointer to currently showed Image)

};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::ImageActor& GetImplementation(Dali::ImageActor& image)
{
  DALI_ASSERT_ALWAYS(image && "Image handle is empty");

  BaseObject& handle = image.GetBaseObject();

  return static_cast<Internal::ImageActor&>(handle);
}

inline const Internal::ImageActor& GetImplementation(const Dali::ImageActor& image)
{
  DALI_ASSERT_ALWAYS(image && "Image handle is empty");

  const BaseObject& handle = image.GetBaseObject();

  return static_cast<const Internal::ImageActor&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_IMAGE_ACTOR_H__

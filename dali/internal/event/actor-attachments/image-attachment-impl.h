#ifndef __DALI_INTERNAL_IMAGE_ATTACHMENT_H__
#define __DALI_INTERNAL_IMAGE_ATTACHMENT_H__

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
#include <dali/public-api/actors/image-actor.h>
#include <dali/internal/event/actor-attachments/actor-attachment-declarations.h>
#include <dali/internal/event/actor-attachments/renderable-attachment-impl.h>
#include <dali/internal/event/images/image-impl.h>
#include <dali/internal/event/images/image-connector.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{
class ImageAttachment;
class Node;
}

/**
 * An attachment for rendering images in various ways.
 * The default geometry scaling mode is ScaleToFill.
 */
class ImageAttachment : public RenderableAttachment
{
public:

  typedef Dali::ImageActor::Style Style;
  typedef Dali::ImageActor::PixelArea PixelArea;

  /**
   * Create a new ImageAttachment.
   * @param[in] parentNode The node to attach a scene-object to.
   * @param[in] image A pointer to the image to display or NULL to not render anything.
   * @return A smart-pointer to the newly allocated ImageAttachment.
   */
  static ImageAttachmentPtr New( const SceneGraph::Node& parentNode, Image* image );

  /**
   * Sets image rendered by the attachment.
   * @param [in] image A pointer to the image to display or NULL to clear.
   */
  void SetImage(Image* image);

  /**
   * Retrieve the image rendered by the attachment.
   * @return The image or an uninitialized image in case the ImageActor was cleared.
   */
  Dali::Image GetImage();

  /**
   * Set a region of the image to display, in pixels.
   * @param [in] pixelArea The area of the image to display.
   * This in pixels, relative to the top-left (0,0) of the image.
   */
  void SetPixelArea(const PixelArea& pixelArea);

  /**
   * Retrieve the region of the image to display, in pixels.
   * @return The pixel area, or a default-constructed area if none was set.
   */
  const PixelArea& GetPixelArea() const
  {
    // This is not animatable; the cached value is up-to-date.
    return mPixelArea;
  }

  /**
   * Query whether a pixel area has been set.
   * @return True if a pixel area has been set.
   */
  bool IsPixelAreaSet() const
  {
    // This is not animatable; the cached value is up-to-date.
    return mIsPixelAreaSet;
  }

  /**
   * Remove any pixel areas specified with SetPixelArea; the entire image will be displayed.
   * @pre image must be initialized.
   */
  void ClearPixelArea();

  /**
   * Set how the ImageAttachment is rendered; the default is STYLE_QUAD.
   * @param [in] style The new style.
   */
  void SetStyle(Style style);

  /**
   * Query how the image is rendered.
   * @return The rendering style.
   */
  Style GetStyle()
  {
    // This is not animatable; the cached value is up-to-date.
    return mStyle;
  }

  /**
   * @copydoc Dali::ImageActor::SetNinePatchBorder
   */
  void SetNinePatchBorder(const Vector4& border, bool inPixels);

  /**
   * @copydoc Dali::ImageActor::GetNinePatchBorder
   */
  Vector4 GetNinePatchBorder()
  {
    // This is not animatable; the cached value is up-to-date.
    return mBorder;
  }

private:

  /**
   * First stage construction of a ImageAttachment.
   * @param[in] stage Used to send messages to scene-graph.
   * @param[in] image A pointer to the image to display or NULL to not render anything.
   */
  ImageAttachment(Stage& stage, Image* image);

  /**
   * Creates the corresponding scene-graph ImageAttachment.
   * @return A newly allocated scene object.
   */
  static SceneGraph::ImageAttachment* CreateSceneObject( const Image* current );

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

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ImageAttachment();

private:

  const SceneGraph::ImageAttachment* mSceneObject; ///< Not owned

  ImageConnector mImageConnectable; ///< Manages the image displayed by the attachment

  // Cached for public getters

  PixelArea mPixelArea;

  Style mStyle;
  Vector4 mBorder;

  bool mIsPixelAreaSet : 1;
  bool mBorderInPixels : 1;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_IMAGE_ATTACHMENT_H__

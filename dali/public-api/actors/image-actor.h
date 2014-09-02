#ifndef __DALI_IMAGE_ACTOR_H__
#define __DALI_IMAGE_ACTOR_H__

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/actors/renderable-actor.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/images/image.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class ImageActor;
}

/**
 * @brief An actor for displaying images.
 *
 * Allows the developer to add an actor to stage which displays the content of an Image object.
 *
 * By default CullFaceMode is set to CullNone to enable the ImageActor to be viewed from all angles.
 *
 * If an ImageActor is created without setting size, then the actor takes the size of the image -
 * this is the natural size.
 * Setting a size on the ImageActor, e.g through the SetSize api or through an animation will
 * stop the natural size being used.
 *
 * Such a set size can be changed back to the image's size by calling SetToNaturalSize().
 *
 * If a pixel area is set on an ImageActor with natural size, the actor size will change
 * to match the pixel area. If a pixel area is set on an ImageActor that has had it's size set,
 * then the size doesn't change, and the partial image will be stretched to fill the set size.
 *
 * Clearing the pixel area on an Image actor with natural size will cause the actor to show the
 * whole image again, and will change size back to that of the image.
 *
 * Clearing the pixel area on an Image actor with a set size will cause the actor to show the
 * whole image again, but will not change the image size.
 */
class DALI_IMPORT_API ImageActor : public RenderableActor
{
public:

  // Default Properties additional to RenderableActor
  static const Property::Index PIXEL_AREA;           ///< name "pixel-area",          type RECTANGLE
  static const Property::Index STYLE;                ///< name "style",               type STRING
  static const Property::Index BORDER;               ///< name "border",              type VECTOR4
  static const Property::Index IMAGE;                ///< name "image",               type MAP {"filename":"", "load-policy":...}

  /**
   * @brief Style determines how the Image is rendered.
   *
   * @code
   * STYLE_QUAD:
   *
   *  0---------2           0-----------------2
   *  |        /|           |                /|
   *  |  A    / |           |      A       /  |
   *  |      /  |           |            /    |
   *  |     /   | SCALE (X) |          /      |
   *  |    /    | --------> |        /        |
   *  |   /     |           |      /          |
   *  |  /      |           |    /            |
   *  | /    B  |           |  /        B     |
   *  |/        |           |/                |
   *  1---------3           1-----------------3
   *
   * Image is rendered as a textured rectangle. The texture
   * is scaled uniformly as the quad is resized.
   *
   * STYLE_NINE_PATCH:
   *
   *  |---|---------------|---|       |---|-----------------------------|---|
   *  | 1 |       2       | 3 |       | 1 |              2              | 3 |
   *  |---|---------------|---|       |---|-----------------------------|---|
   *  |   |               |   |       |   |                             |   |
   *  |   |               |   |       |   |                             |   |
   *  | 4 |       5       | 6 | SCALE |   |                             |   |
   *  |   |               |   | ----> |   |                             |   |
   *  |   |               |   |       | 4 |              5              | 6 |
   *  |-------------------|---|       |   |                             |   |
   *  | 7 |       8       | 9 |       |   |                             |   |
   *  |---|---------------|---|       |   |                             |   |
   *                                  |---------------------------------|---|
   *                                  | 7 |              8              | 9 |
   *                                  |---|-----------------------------|---|
   *
   * @endcode
   * Image is rendered as a textured rectangle. The texture
   * is scaled differently over each of the 9 sections.
   *
   * Visualise a Picture Frame:
   *
   * - Corner sections (1,3,7,9) are not scaled, regardless
   * of how big the Image is.
   * - Horizontal edge sections (2,8) are scaled only in the
   * X axis as the image increases in width.
   * - Vertical edge sections (4,6) are scaled only in the
   * Y axis as the image increases in height.
   * - Center section (5) is scaled in both X and Y axes as
   * the image increases in width and/or height.
   *
   * Note: If GRID hints are enabled (via a Shader that requires it),
   * the above geometry will be further subdivided into rectangles of
   * approx. 40x40 in size.
   *
   */
  enum Style
  {
    STYLE_QUAD,       ///< As a simple quad.
    STYLE_NINE_PATCH  ///< As a nine-patch.
  };

  /**
   * @brief Pixel area is relative to the top-left (0,0) of the image.
   */
  typedef Rect<int> PixelArea;

  /**
   * @brief Create an uninitialized ImageActor handle.
   *
   * This can be initialized with ImageActor::New(...)
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  ImageActor();

  /**
   * @brief Create an empty image actor object.
   *
   * @return A handle to a newly allocated actor.
   */
  static ImageActor New();

  /**
   * @brief Create a image actor object.
   *
   * The actor will take the image's natural size unless a custom size
   * is chosen, e.g. via Actor:SetSize().
   * If the handle is empty, ImageActor will display nothing
   * @pre ImageActor must be initialized.
   * @param[in] image The image to display.
   * @return A handle to a newly allocated actor.
   */
  static ImageActor New(Image image);

  /**
   * @brief Create a image actor object.
   *
   * The actor will take the image's natural size unless a custom size
   * is chosen, e.g. via Actor:SetSize()
   * If the handle is empty, ImageActor will display nothing
   * @pre ImageActor must be initialized.
   * @param [in] image The image to display.
   * @param [in] pixelArea The area of the image to display.
   * This in pixels, relative to the top-left (0,0) of the image.
   * @return A handle to a newly allocated actor.
   */
  static ImageActor New(Image image, PixelArea pixelArea);

  /**
   * @brief Downcast an Object handle to ImageActor.
   *
   *
   * If handle points to a ImageActor the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle to An object
   * @return handle to a ImageActor or an uninitialized handle
   */
  static ImageActor DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ImageActor();

  /**
   * @brief Copy constructor
   *
   * @param [in] copy The actor to copy.
   */
  ImageActor(const ImageActor& copy);

  /**
   * @brief Assignment operator
   *
   * @param [in] rhs The actor to copy.
   */
  ImageActor& operator=(const ImageActor& rhs);

  /**
   * @brief This method is defined to allow assignment of the NULL value,
   * and will throw an exception if passed any other value.
   *
   * Assigning to NULL is an alias for Reset().
   * @param [in] rhs  A NULL pointer
   * @return A reference to this handle
   */
  ImageActor& operator=(BaseHandle::NullType* rhs);

  /**
   * @brief Set the image rendered by the actor.
   * Set the image rendered by the actor.
   * If actor was already displaying a different image, the old image is dropped and actor may
   * temporarily display nothing. Setting an empty image (handle) causes the current image to be
   * dropped and actor displays nothing.
   * The actor will take the image's natural size unless a custom size
   * is chosen, e.g. via Actor:SetSize()
   *
   * @pre ImageActor must be initialized.
   * @param [in] image The image to display.
   */
  void SetImage(Image image);

  /**
   * @brief Retrieve the image rendered by the actor.
   *
   * If no image is assigned, an empty handle is returned
   * @return The image.
   */
  Image GetImage();

  /**
   * @brief Tell the image actor to use the natural size of the current image
   * or future images.
   *
   * Calling SetSize on this actor or animating the size of the actor
   * overrides this behaviour.
   *
   * @post The image actor uses the natural image size after an image
   * has been loaded.
   * @note Actor::SetSizeSignal() will be triggered if there is a current image.
   */
  void SetToNaturalSize();

  /**
   * @brief Set a region of the image to display, in pixels.
   *
   * When the image is loaded the actor's size will be reset to the pixelArea,
   * unless a custom size was chosen, e.g. via Actor:SetSize().
   * Note! PixelArea should be inside the image data size. It gets clamped by GL
   * @pre image must be initialized.
   * @param [in] pixelArea The area of the image to display.
   * This in pixels, relative to the top-left (0,0) of the image.
   */
  void SetPixelArea(const PixelArea& pixelArea);

  /**
   * @brief Retrieve the region of the image to display, in pixels.
   *
   * @pre image must be initialized.
   * @return The pixel area, or a default-constructed area if none was set.
   */
  PixelArea GetPixelArea() const;

  /**
   * @brief Query whether a pixel area has been set.
   *
   * @pre image must be initialized.
   * @return True if a pixel area has been set.
   */
  bool IsPixelAreaSet() const;

  /**
   * @brief Remove any pixel areas specified with SetPixelArea; the entire image will be displayed.
   *
   * The actor size will change to that of the Image unless a custom size was set, e.g. via
   * Actor::SetSize().
   * @pre image must be initialized.
   */
  void ClearPixelArea();

  /**
   * @brief Set how the image is rendered; the default is STYLE_QUAD.
   *
   * @pre image must be initialized.
   * @param [in] style The new style.
   */
  void SetStyle(Style style);

  /**
   * @brief Query how the image is rendered.
   *
   * @pre image must be initialized.
   * @return The rendering style.
   */
  Style GetStyle() const;

  /**
   * @brief Set the border used with STYLE_NINE_PATCH.
   *
   * The values are in pixels from the left, top, right, and bottom of the image respectively.
   * i.e. SetNinePatchBorder( Vector4(1,2,3,4) ) sets the left-border to 1, top-border to 2, right-border to 3, and bottom-border to 4 pixels.
   * @param [in] border The new nine-patch border.
   */
  void SetNinePatchBorder(const Vector4& border);

  /**
   * @brief Retrieve the border used with STYLE_NINE_PATCH.
   *
   * @return The nine-patch border.
   */
  Vector4 GetNinePatchBorder() const;

public: // Not intended for application developers

  explicit DALI_INTERNAL ImageActor(Internal::ImageActor*);
};

} // namespace Dali

#endif // __DALI_IMAGE_ACTOR_H__

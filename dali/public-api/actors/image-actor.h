#ifndef __DALI_IMAGE_ACTOR_H__
#define __DALI_IMAGE_ACTOR_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/public-api/actors/blending.h>
#include <dali/public-api/actors/sampling.h>

namespace Dali
{
/**
 * @addtogroup dali_core_actors
 * @{
 */

namespace Internal DALI_INTERNAL
{
class ImageActor;
}

/**
 * @brief An actor for displaying images.
 *
 * Allows the developer to add an actor to stage which displays the content of an Image object.
 *
 * By default ImageActor can be viewed from all angles.
 *
 * If an ImageActor is created without setting size, then the actor takes the size of the image -
 * this is the natural size.
 * Setting a size on the ImageActor, e.g through the SetSize api or through an animation will
 * stop the natural size being used.
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
class DALI_IMPORT_API ImageActor : public Actor
{
public:

  /**
   * @brief An enumeration of properties belonging to the ImageActor class.
   * Properties additional to RenderableActor.
   */
  struct Property
  {
    enum
    {
      PIXEL_AREA = DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX, ///< name "pixel-area",  type Rect<int>
      STYLE,                                                   ///< name "style",       type std::string
      BORDER,                                                  ///< name "border",      type Vector4
      IMAGE,                                                   ///< name "image",       type Map {"filename":"", "load-policy":...}
    };
  };

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
   * Image is rendered as a textured rectangle. The texture
   * is scaled differently over each of the 9 sections.
   *
   * STYLE_NINE_PATCH_NO_CENTER:
   *
   * Image is rendered in the same way as STYLE_NINE_PATCH,
   * but the Center Section (5) is not rendered.
   * @endcode
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
   * approx. 40x40 in size. STYLE_NINE_PATCH_NO_CENTER is not supported
   * yet when GRID hints are enabled.
   */
  enum Style
  {
    STYLE_QUAD,                 ///< As a simple quad.
    STYLE_NINE_PATCH,           ///< As a nine-patch.
    STYLE_NINE_PATCH_NO_CENTER  ///< As a nine-patch without center section being rendered.
  };

  /**
   * @brief Pixel area is relative to the top-left (0,0) of the image.
   */
  typedef Rect<int> PixelArea;

  static const BlendingMode::Type DEFAULT_BLENDING_MODE; ///< default value is BlendingMode::AUTO

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


  /**
   * @brief Allows modification of an actors position in the depth sort algorithm.
   *
   * The offset can be altered for each coplanar actor hence allowing an order of painting.
   * @pre The Actor has been initialized.
   * @param [in] depthOffset the offset to be given to the actor. Positive values pushing it further back.
   */
  void SetSortModifier(float depthOffset);

  /**
   * @brief Retrieves the offset used to modify an actors position in the depth sort algorithm.
   *
   * The offset can be altered for each coplanar actor hence allowing an order of painting.
   * @pre The Actor has been initialized.
   * @return  the offset that has been given to the actor. Positive values pushing it further back.
   */
  float GetSortModifier() const;

  /**
   * @brief Sets the blending mode.
   *
   * Possible values are: BlendingMode::OFF, BlendingMode::AUTO and BlendingMode::ON. Default is BlendingMode::AUTO.
   *
   * If blending is disabled (BlendingMode::OFF) fade in and fade out animations do not work.
   *
   * <ul>
   *   <li> \e OFF Blending is disabled.
   *   <li> \e AUTO Blending is enabled only if the renderable actor has alpha channel.
   *   <li> \e ON Blending is enabled.
   * </ul>
   *
   * @param[in] mode The blending mode.
   */
  void SetBlendMode( BlendingMode::Type mode );

  /**
   * @brief Retrieves the blending mode.
   *
   * @return The blending mode, one of BlendingMode::OFF, BlendingMode::AUTO or BlendingMode::ON.
   */
  BlendingMode::Type GetBlendMode() const;

  /**
   * @brief Specify the pixel arithmetic used when the actor is blended.
   *
   * @param[in] srcFactorRgba Specifies how the red, green, blue, and alpha source blending factors are computed.
   * The options are BlendingFactor::ZERO, ONE, SRC_COLOR, ONE_MINUS_SRC_COLOR, DST_COLOR, ONE_MINUS_DST_COLOR,
   * SRC_ALPHA, ONE_MINUS_SRC_ALPHA, DST_ALPHA, ONE_MINUS_DST_ALPHA, CONSTANT_COLOR, ONE_MINUS_CONSTANT_COLOR,
   * GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA, and GL_SRC_ALPHA_SATURATE.
   *
   * @param[in] destFactorRgba Specifies how the red, green, blue, and alpha destination blending factors are computed.
   * The options are BlendingFactor::ZERO, ONE, SRC_COLOR, ONE_MINUS_SRC_COLOR, DST_COLOR, ONE_MINUS_DST_COLOR,
   * SRC_ALPHA, ONE_MINUS_SRC_ALPHA, DST_ALPHA, ONE_MINUS_DST_ALPHA, CONSTANT_COLOR, ONE_MINUS_CONSTANT_COLOR,
   * GL_CONSTANT_ALPHA, and GL_ONE_MINUS_CONSTANT_ALPHA.
   */
  void SetBlendFunc( BlendingFactor::Type srcFactorRgba, BlendingFactor::Type destFactorRgba );

  /**
   * @brief Specify the pixel arithmetic used when the actor is blended.
   *
   * @param[in] srcFactorRgb Specifies how the red, green, and blue source blending factors are computed.
   * The options are BlendingFactor::ZERO, ONE, SRC_COLOR, ONE_MINUS_SRC_COLOR, DST_COLOR, ONE_MINUS_DST_COLOR,
   * SRC_ALPHA, ONE_MINUS_SRC_ALPHA, DST_ALPHA, ONE_MINUS_DST_ALPHA, CONSTANT_COLOR, ONE_MINUS_CONSTANT_COLOR,
   * GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA, and GL_SRC_ALPHA_SATURATE.
   *
   * @param[in] destFactorRgb Specifies how the red, green, blue, and alpha destination blending factors are computed.
   * The options are BlendingFactor::ZERO, ONE, SRC_COLOR, ONE_MINUS_SRC_COLOR, DST_COLOR, ONE_MINUS_DST_COLOR,
   * SRC_ALPHA, ONE_MINUS_SRC_ALPHA, DST_ALPHA, ONE_MINUS_DST_ALPHA, CONSTANT_COLOR, ONE_MINUS_CONSTANT_COLOR,
   * GL_CONSTANT_ALPHA, and GL_ONE_MINUS_CONSTANT_ALPHA.
   *
   * @param[in] srcFactorAlpha Specifies how the alpha source blending factor is computed.
   * The options are the same as for srcFactorRgb.
   *
   * @param[in] destFactorAlpha Specifies how the alpha source blending factor is computed.
   * The options are the same as for destFactorRgb.
   */
  void SetBlendFunc( BlendingFactor::Type srcFactorRgb,   BlendingFactor::Type destFactorRgb,
                     BlendingFactor::Type srcFactorAlpha, BlendingFactor::Type destFactorAlpha );

  /**
   * @brief Query the pixel arithmetic used when the actor is blended.
   *
   * @param[out] srcFactorRgb Specifies how the red, green, blue, and alpha source blending factors are computed.
   * @param[out] destFactorRgb Specifies how the red, green, blue, and alpha destination blending factors are computed.
   * @param[out] srcFactorAlpha Specifies how the red, green, blue, and alpha source blending factors are computed.
   * @param[out] destFactorAlpha Specifies how the red, green, blue, and alpha destination blending factors are computed.
   */
  void GetBlendFunc( BlendingFactor::Type& srcFactorRgb,   BlendingFactor::Type& destFactorRgb,
                     BlendingFactor::Type& srcFactorAlpha, BlendingFactor::Type& destFactorAlpha ) const;

  /**
   * @brief Specify the equation used when the actor is blended.
   *
   * The options are BlendingEquation::ADD, SUBTRACT, or REVERSE_SUBTRACT.
   * @param[in] equationRgba The equation used for combining red, green, blue, and alpha components.
   */
  void SetBlendEquation( BlendingEquation::Type equationRgba );

  /**
   * @brief Specify the equation used when the actor is blended.
   *
   * @param[in] equationRgb The equation used for combining red, green, and blue components.
   * @param[in] equationAlpha The equation used for combining the alpha component.
   * The options are BlendingEquation::ADD, SUBTRACT, or REVERSE_SUBTRACT.
   */
  void SetBlendEquation( BlendingEquation::Type equationRgb, BlendingEquation::Type equationAlpha );

  /**
   * @brief Query the equation used when the actor is blended.
   *
   * @param[out] equationRgb The equation used for combining red, green, and blue components.
   * @param[out] equationAlpha The equation used for combining the alpha component.
   */
  void GetBlendEquation( BlendingEquation::Type& equationRgb, BlendingEquation::Type& equationAlpha ) const;

  /**
   * @brief Specify the color used when the actor is blended; the default is Vector4::ZERO.
   *
   * @param[in] color The blend color.
   */
  void SetBlendColor( const Vector4& color );

  /**
   * @brief Query the color used when the actor is blended.
   *
   * @return The blend color.
   */
  const Vector4& GetBlendColor() const;

  /**
   * @brief Sets the filtering mode.
   *
   * Possible values are: FilterMode::NEAREST and FilterMode::LINEAR. Default is FilterMode::LINEAR.
   *
   * <ul>
   *   <li> \e NEAREST Use nearest filtering
   *   <li> \e LINEAR Use linear filtering
   * </ul>
   *
   * @param[in] minFilter The minification filtering mode.
   * @param[in] magFilter The magnification filtering mode.
   */
  void SetFilterMode( FilterMode::Type minFilter, FilterMode::Type magFilter );

  /**
   * @brief Retrieves the filtering mode.
   *
   * @param[out] minFilter The return minification value
   * @param[out] magFilter The return magnification value
   */
  void GetFilterMode( FilterMode::Type& minFilter, FilterMode::Type& magFilter) const;

  /**
   * @brief Sets the shader effect for the RenderableActor.
   *
   * Shader effects provide special effects like ripple and bend.
   * Setting a shader effect removes any shader effect previously set by SetShaderEffect.
   * @pre The actor has been initialized.
   * @pre effect has been initialized.
   * @param [in] effect The shader effect.
   */
  void SetShaderEffect( ShaderEffect effect );

  /**
   * @brief Retrieve the custom shader effect for the RenderableActor.
   * If default shader is used an empty handle is returned.
   *
   * @pre The Actor has been initialized.
   * @return The shader effect
   */
  ShaderEffect GetShaderEffect() const;

  /**
   * @brief Removes the current shader effect.
   *
   * @pre The Actor has been initialized.
   */
  void RemoveShaderEffect();


public: // Not intended for application developers

  explicit DALI_INTERNAL ImageActor(Internal::ImageActor*);
};


/**
 * @brief Sets the shader effect for all ImageActors in a tree of Actors.
 *
 * @see ImageActor::SetShaderEffect
 *
 * @param [in] actor root of a tree of actors.
 * @param [in] effect The shader effect.
 */
DALI_IMPORT_API void SetShaderEffectRecursively( Actor actor, ShaderEffect effect );

/**
 * @brief Removes the shader effect from all ImageActors in a tree of Actors.
 *
 * @see ImageActor::RemoveShaderEffect
 *
 * @param [in] actor root of a tree of actors.
 */
DALI_IMPORT_API void RemoveShaderEffectRecursively( Actor actor );

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_IMAGE_ACTOR_H__

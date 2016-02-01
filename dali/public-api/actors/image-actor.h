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
 * @DEPRECATED_1_1.11
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
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API ImageActor : public Actor
{
public:

  /**
   * @DEPRECATED_1_1.11
   * @brief An enumeration of properties belonging to the ImageActor class.
   * Properties additional to RenderableActor.
   * @SINCE_1_0.0
   */
  struct Property
  {
    enum
    {
      /**
       * @DEPRECATED_1_1.11
       * @brief name "pixelArea",   type Rect<int>
       * @SINCE_1_0.0
       */
      PIXEL_AREA = DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX,
      /**
       * @DEPRECATED_1_1.11
       * @brief name "style",       type std::string
       * @SINCE_1_0.0
       */
      STYLE,
      /**
       * @DEPRECATED_1_1.11
       * @brief name "border",      type Vector4
       * @SINCE_1_0.0
       */
      BORDER,
      /**
       * @DEPRECATED_1_1.11
       * @brief name "image",       type Map {"filename":"", "loadPolicy":...}
       * @SINCE_1_0.0
       */
      IMAGE,
    };
  };

  /**
   * @DEPRECATED_1_1.11. Only quad style supported, use ImageView instead for nine patch.
   *
   * @brief Style determines how the Image is rendered.
   *
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
   * @SINCE_1_0.0
   */
  enum Style
  {
    /**
     * @DEPRECATED_1_1.11
     * @brief As a simple quad.
     * @SINCE_1_0.0
     */
    STYLE_QUAD,
    /**
     * @DEPRECATED_1_1.11
     * @brief As a nine-patch.
     * @SINCE_1_0.0
     */
    STYLE_NINE_PATCH,
    /**
     * @DEPRECATED_1_1.11
     * @brief As a nine-patch without center section being rendered.
     * @SINCE_1_0.0
     */
    STYLE_NINE_PATCH_NO_CENTER
  };

  /**
   * @DEPRECATED_1_1.11
   * @brief Pixel area is relative to the top-left (0,0) of the image.
   * @SINCE_1_0.0
   */
  typedef Rect<int> PixelArea;

  static const BlendingMode::Type DEFAULT_BLENDING_MODE; ///< default value is BlendingMode::AUTO

  /**
   * @DEPRECATED_1_1.11
   * @brief Create an uninitialized ImageActor handle.
   *
   * This can be initialized with ImageActor::New(...)
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   * @SINCE_1_0.0
   */
  ImageActor();

  /**
   * @DEPRECATED_1_1.11
   * @brief Create an empty image actor object.
   *
   * @SINCE_1_0.0
   * @return A handle to a newly allocated actor.
   */
  static ImageActor New();

  /**
   * @DEPRECATED_1_1.11
   * @brief Create a image actor object.
   *
   * The actor will take the image's natural size unless a custom size
   * is chosen, e.g. via Actor:SetSize().
   * If the handle is empty, ImageActor will display nothing
   * @SINCE_1_0.0
   * @param[in] image The image to display.
   * @return A handle to a newly allocated actor.
   * @pre ImageActor must be initialized.
   */
  static ImageActor New(Image image);

  /**
   * @DEPRECATED_1_1.11
   * @brief Create a image actor object.
   *
   * The actor will take the image's natural size unless a custom size
   * is chosen, e.g. via Actor:SetSize()
   * If the handle is empty, ImageActor will display nothing
   * @SINCE_1_0.0
   * @param [in] image The image to display.
   * @param [in] pixelArea The area of the image to display.
   * This in pixels, relative to the top-left (0,0) of the image.
   * @return A handle to a newly allocated actor.
   * @pre ImageActor must be initialized.
   */
  static ImageActor New(Image image, PixelArea pixelArea);

  /**
   * @DEPRECATED_1_1.11
   * @brief Downcast an Object handle to ImageActor.
   *
   *
   * If handle points to a ImageActor the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @SINCE_1_0.0
   * @param[in] handle to An object
   * @return handle to a ImageActor or an uninitialized handle
   */
  static ImageActor DownCast( BaseHandle handle );

  /**
   * @DEPRECATED_1_1.11
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~ImageActor();

  /**
   * @DEPRECATED_1_1.11
   * @brief Copy constructor
   *
   * @SINCE_1_0.0
   * @param [in] copy The actor to copy.
   */
  ImageActor(const ImageActor& copy);

  /**
   * @DEPRECATED_1_1.11
   * @brief Assignment operator
   *
   * @SINCE_1_0.0
   * @param [in] rhs The actor to copy.
   */
  ImageActor& operator=(const ImageActor& rhs);

  /**
   * @DEPRECATED_1_1.11
   * @brief Set the image rendered by the actor.
   *
   * Set the image rendered by the actor.
   * If actor was already displaying a different image, the old image is dropped and actor may
   * temporarily display nothing. Setting an empty image (handle) causes the current image to be
   * dropped and actor displays nothing.
   * The actor will take the image's natural size unless a custom size
   * is chosen, e.g. via Actor:SetSize()
   *
   * @SINCE_1_0.0
   * @param [in] image The image to display.
   * @pre ImageActor must be initialized.
   */
  void SetImage(Image image);

  /**
   * @DEPRECATED_1_1.11
   * @brief Retrieve the image rendered by the actor.
   *
   * If no image is assigned, an empty handle is returned
   * @SINCE_1_0.0
   * @return The image.
   */
  Image GetImage();

  /**
   * @DEPRECATED_1_1.11
   * @brief Set a region of the image to display, in pixels.
   *
   * When the image is loaded the actor's size will be reset to the pixelArea,
   * unless a custom size was chosen, e.g. via Actor:SetSize().
   * Note! PixelArea should be inside the image data size. It gets clamped by GL
   * @SINCE_1_0.0
   * @param [in] pixelArea The area of the image to display.
   * This in pixels, relative to the top-left (0,0) of the image.
   * @pre image must be initialized.
   */
  void SetPixelArea(const PixelArea& pixelArea);

  /**
   * @DEPRECATED_1_1.11
   * @brief Retrieve the region of the image to display, in pixels.
   *
   * @SINCE_1_0.0
   * @return The pixel area, or a default-constructed area if none was set.
   * @pre image must be initialized.
   */
  PixelArea GetPixelArea() const;

  /**
   * @DEPRECATED_1_1.11. Use ImageView instead.
   *
   * @brief Set how the image is rendered; the default is STYLE_QUAD.
   *
   * @SINCE_1_0.0
   * @param [in] style The new style.
   *
   * @pre image must be initialized.
   * @note The style specified is set (so GetStyle will return what's set) but ignored internally.
   */
  void SetStyle(Style style);

  /**
   * @DEPRECATED_1_1.11. Use ImageView instead.
   *
   * @brief Query how the image is rendered.
   *
   * @SINCE_1_0.0
   * @return The rendering style.
   *
   * @pre image must be initialized.
   * @note This just returns the style set by SetStyle. In reality, only STYLE_QUAD is supported.
   */
  Style GetStyle() const;

  /**
   * @DEPRECATED_1_1.11. Use ImageView instead.
   *
   * @brief Set the border used with STYLE_NINE_PATCH.
   *
   * The values are in pixels from the left, top, right, and bottom of the image respectively.
   * i.e. SetNinePatchBorder( Vector4(1,2,3,4) ) sets the left-border to 1, top-border to 2, right-border to 3, and bottom-border to 4 pixels.
   * @SINCE_1_0.0
   * @param [in] border The new nine-patch border.
   */
  void SetNinePatchBorder(const Vector4& border);

  /**
   * @DEPRECATED_1_1.11. Use ImageView instead.
   *
   * @brief Retrieve the border used with STYLE_NINE_PATCH.
   *
   * @SINCE_1_0.0
   * @return The nine-patch border.
   */
  Vector4 GetNinePatchBorder() const;


  /**
   * @DEPRECATED_1_1.11
   * @brief Allows modification of an actors position in the depth sort algorithm.
   *
   * The offset can be altered for each coplanar actor hence allowing an order of painting.
   * @SINCE_1_0.0
   * @param [in] depthOffset the offset to be given to the actor. Positive values pushing it further back.
   * @pre The Actor has been initialized.
   */
  void SetSortModifier(float depthOffset);

  /**
   * @DEPRECATED_1_1.11
   * @brief Retrieves the offset used to modify an actors position in the depth sort algorithm.
   *
   * The offset can be altered for each coplanar actor hence allowing an order of painting.
   * @SINCE_1_0.0
   * @return  the offset that has been given to the actor. Positive values pushing it further back.
   * @pre The Actor has been initialized.
   */
  float GetSortModifier() const;

  /**
   * @DEPRECATED_1_1.11
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
   * @SINCE_1_0.0
   * @param[in] mode The blending mode.
   */
  void SetBlendMode( BlendingMode::Type mode );

  /**
   * @DEPRECATED_1_1.11
   * @brief Retrieves the blending mode.
   *
   * @SINCE_1_0.0
   * @return The blending mode, one of BlendingMode::OFF, BlendingMode::AUTO or BlendingMode::ON.
   */
  BlendingMode::Type GetBlendMode() const;

  /**
   * @DEPRECATED_1_1.11
   * @brief Specify the pixel arithmetic used when the actor is blended.
   *
   * @SINCE_1_0.0
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
   * @DEPRECATED_1_1.11
   * @brief Specify the pixel arithmetic used when the actor is blended.
   *
   * @SINCE_1_0.0
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
   * @DEPRECATED_1_1.11
   * @brief Query the pixel arithmetic used when the actor is blended.
   *
   * @SINCE_1_0.0
   * @param[out] srcFactorRgb Specifies how the red, green, blue, and alpha source blending factors are computed.
   * @param[out] destFactorRgb Specifies how the red, green, blue, and alpha destination blending factors are computed.
   * @param[out] srcFactorAlpha Specifies how the red, green, blue, and alpha source blending factors are computed.
   * @param[out] destFactorAlpha Specifies how the red, green, blue, and alpha destination blending factors are computed.
   */
  void GetBlendFunc( BlendingFactor::Type& srcFactorRgb,   BlendingFactor::Type& destFactorRgb,
                     BlendingFactor::Type& srcFactorAlpha, BlendingFactor::Type& destFactorAlpha ) const;

  /**
   * @DEPRECATED_1_1.11
   * @brief Specify the equation used when the actor is blended.
   *
   * The options are BlendingEquation::ADD, SUBTRACT, or REVERSE_SUBTRACT.
   * @SINCE_1_0.0
   * @param[in] equationRgba The equation used for combining red, green, blue, and alpha components.
   */
  void SetBlendEquation( BlendingEquation::Type equationRgba );

  /**
   * @DEPRECATED_1_1.11
   * @brief Specify the equation used when the actor is blended.
   *
   * @SINCE_1_0.0
   * @param[in] equationRgb The equation used for combining red, green, and blue components.
   * @param[in] equationAlpha The equation used for combining the alpha component.
   * The options are BlendingEquation::ADD, SUBTRACT, or REVERSE_SUBTRACT.
   */
  void SetBlendEquation( BlendingEquation::Type equationRgb, BlendingEquation::Type equationAlpha );

  /**
   * @DEPRECATED_1_1.11
   * @brief Query the equation used when the actor is blended.
   *
   * @SINCE_1_0.0
   * @param[out] equationRgb The equation used for combining red, green, and blue components.
   * @param[out] equationAlpha The equation used for combining the alpha component.
   */
  void GetBlendEquation( BlendingEquation::Type& equationRgb, BlendingEquation::Type& equationAlpha ) const;

  /**
   * @DEPRECATED_1_1.11
   * @brief Specify the color used when the actor is blended; the default is Vector4::ZERO.
   *
   * @SINCE_1_0.0
   * @param[in] color The blend color.
   */
  void SetBlendColor( const Vector4& color );

  /**
   * @DEPRECATED_1_1.11
   * @brief Query the color used when the actor is blended.
   *
   * @SINCE_1_0.0
   * @return The blend color.
   */
  const Vector4& GetBlendColor() const;

  /**
   * @DEPRECATED_1_1.11
   * @brief Sets the filtering mode.
   *
   * Possible values are: FilterMode::NEAREST and FilterMode::LINEAR. Default is FilterMode::LINEAR.
   *
   * <ul>
   *   <li> \e NEAREST Use nearest filtering
   *   <li> \e LINEAR Use linear filtering
   * </ul>
   *
   * @SINCE_1_0.0
   * @param[in] minFilter The minification filtering mode.
   * @param[in] magFilter The magnification filtering mode.
   */
  void SetFilterMode( FilterMode::Type minFilter, FilterMode::Type magFilter );

  /**
   * @DEPRECATED_1_1.11
   * @brief Retrieves the filtering mode.
   *
   * @SINCE_1_0.0
   * @param[out] minFilter The return minification value
   * @param[out] magFilter The return magnification value
   */
  void GetFilterMode( FilterMode::Type& minFilter, FilterMode::Type& magFilter) const;

  /**
   * @DEPRECATED_1_1.11
   * @brief Sets the shader effect for the RenderableActor.
   *
   * Shader effects provide special effects like ripple and bend.
   * Setting a shader effect removes any shader effect previously set by SetShaderEffect.
   * @SINCE_1_0.0
   * @param [in] effect The shader effect.
   * @pre The actor has been initialized.
   * @pre effect has been initialized.
   */
  void SetShaderEffect( ShaderEffect effect );

  /**
   * @DEPRECATED_1_1.11
   * @brief Retrieve the custom shader effect for the RenderableActor.
   * If default shader is used an empty handle is returned.
   *
   * @SINCE_1_0.0
   * @return The shader effect
   * @pre The Actor has been initialized.
   */
  ShaderEffect GetShaderEffect() const;

  /**
   * @DEPRECATED_1_1.11
   * @brief Removes the current shader effect.
   *
   * @SINCE_1_0.0
   * @pre The Actor has been initialized.
   */
  void RemoveShaderEffect();


public: // Not intended for application developers

  explicit DALI_INTERNAL ImageActor(Internal::ImageActor*);
};


/**
 * @DEPRECATED_1_1.11
 * @brief Sets the shader effect for all ImageActors in a tree of Actors.
 *
 * @SINCE_1_0.0
 * @param [in] actor root of a tree of actors.
 * @param [in] effect The shader effect.
 * @see ImageActor::SetShaderEffect
 *
 */
DALI_IMPORT_API void SetShaderEffectRecursively( Actor actor, ShaderEffect effect );

/**
 * @DEPRECATED_1_1.11
 * @brief Removes the shader effect from all ImageActors in a tree of Actors.
 *
 * @SINCE_1_0.0
 * @param [in] actor root of a tree of actors.
 * @see ImageActor::RemoveShaderEffect
 *
 */
DALI_IMPORT_API void RemoveShaderEffectRecursively( Actor actor );

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_IMAGE_ACTOR_H__

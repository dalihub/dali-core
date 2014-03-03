#ifndef __DALI_INTERNAL_UV_RECT_H__
#define __DALI_INTERNAL_UV_RECT_H__

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

namespace Dali
{

namespace Internal
{

/**
 * UV co-ordinates for a rectangle
 */
struct UvRect
  {
    float     u0;         ///< u texture coordinate (x) -bottom left
    float     v0;         ///< v texture coordinate (y) -bottom left
    float     u2;         ///< u texture coordinate (x) -top right
    float     v2;         ///< v texture coordinate (y) -top right

    /**
     * Constructor.
     */
    UvRect()
    : u0(0.0f), v0(0.0f),
      u2(1.0f), v2(1.0f)
    {
    }

    /**
     * Reset the UV coordinates to default values
     */
    void Reset()
    {
       u0 = 0.0f;
       v0 = 0.0f;
       u2 = 1.0f;
       v2 = 1.0f;
     }

    /**
     * Assignment operator
     */
    UvRect& operator= (const UvRect& rhs)
    {
      if (this != &rhs)
      {
        u0 = rhs.u0;
        v0 = rhs.v0;
        u2 = rhs.u2;
        v2 = rhs.v2;
      }
      return *this;
    }

    /**
     * Adjusts the uv coordinates, relative to the atlas uv coordinates.
     * Use for displaying part of a bitmap, which is held in an atlas.
     * For example:
     * \code
     *
     * (0,0)
     * |-----------------------------|
     * | Atlas                       |
     * |                             |
     * |  /----------------------\   |
     * |  |  Bitmap              |   |
     * |  |                      |   |
     * |  |  /---------\         |   |
     * |  |  |         |         |   |
     * |  |  | Area to |         |   |
     * |  |  | display |         |   |
     * |  |  \---------/         |   |
     * |  \______________________/   |
     * |                             |
     * |_____________________________|
     *                               (1,1)
     *
     * \endcode
     */
    void AdjustToAtlasUV(const UvRect &atlasUV)
    {
      // this = Area To Display
      // atlasUV = UV coordinates of bitmap in the atlas.

      // 1. calculate the bitmap uv width / height in the atlas
      float parentWidth = atlasUV.u2 - atlasUV.u0;
      float parentHeight =  atlasUV.v2 - atlasUV.v0;

      // algorithm = bitmap.start + displayArea.position * bitmap.width or height

      u0 = atlasUV.u0 + u0 * parentWidth;
      v0 = atlasUV.v0 + v0 * parentHeight;
      u2 = atlasUV.u0 + u2 * parentWidth;
      v2 = atlasUV.v0 + v2 * parentHeight;

    }

  }; // struct UV


} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_UV_H__

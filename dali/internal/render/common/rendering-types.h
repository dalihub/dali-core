#ifndef __DALI_INTERNAL_RENDERING_TYPES_H__
#define __DALI_INTERNAL_RENDERING_TYPES_H__

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

/*
 * Paint style used for certain nodes
 */
struct PaintStyle
{
  /**
   * Default constructor, initializes everything to zero
   */
  PaintStyle()
  : mStyleMode( STYLE_1x1 ),
    mFadeIn(false),
    mFadeInTime(0),
    mBorderX0( 0.0f ), mBorderY0( 0.0f ), mBorderX1( 0.0f ), mBorderY1( 0.0f ),
    mClipX0( 0.0f ), mClipY0( 0.0f ), mClipX1( 0.0f ), mClipY1( 0.0f )
  { }

  enum StyleMode
  {
     STYLE_1x1,
     STYLE_3x1,
     STYLE_1x3,
     STYLE_3x3,
     STYLE_GRID,
     STYLE_BORDER_1x1
  };

  StyleMode   mStyleMode;
  bool        mFadeIn;                      ///< whether to fade the image in
  unsigned int mFadeInTime;                 ///< how quickly to fade an image in, in milliseconds

  float       mBorderX0;  // Border for 3x3 grid
  float       mBorderY0;
  float       mBorderX1;
  float       mBorderY1;

  float       mClipX0;  // clip region for centre rectangle
  float       mClipY0;
  float       mClipX1;
  float       mClipY1;
};

} // namespace Dali

} // namespace Internal

#endif // __DALI_INTERNAL_RENDERING_TYPES_H__

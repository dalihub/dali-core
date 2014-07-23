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
 */

#include "culling-algorithms.h"

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

bool Is2dBoxOutsideClipSpace(const Matrix& modelMatrix,
                             const Matrix& modelViewProjectionMatrix,
                             const Rect<float>& boundingBox )
{
  // First, calculate if the center is inside clip space:

  // Downside is mvp matrix calc per renderer per frame
  // and up to 4 matrix * vector calls.
  const Matrix& mvp = modelViewProjectionMatrix;
  const Vector4 translation = mvp.GetTranslation();

  // Upside is point test is very simple:
  if( -translation.w <= translation.x  &&  translation.x <= translation.w &&
      -translation.w <= translation.y  &&  translation.y <= translation.w &&
      -translation.w <= translation.z  &&  translation.z <= translation.w)
  {
    // Definitely inside clip space - don't do any more processing
    return false;
  }

  // Transform oriented bounding box to clip space:
  Vector4 topLeft(    boundingBox.x, boundingBox.y, 0.0f, 1.0f);
  Vector4 topRight(   boundingBox.x + boundingBox.width, boundingBox.y, 0.0f, 1.0f);
  Vector4 bottomLeft( boundingBox.x, boundingBox.y + boundingBox.height, 0.0f, 1.0f);
  Vector4 bottomRight(boundingBox.x + boundingBox.width, boundingBox.y + boundingBox.height, 0.0f, 1.0f);

  Vector4 topLeftClip = mvp * topLeft;
  if( -topLeftClip.w <= topLeftClip.x && topLeftClip.x <= topLeftClip.w &&
      -topLeftClip.w <= topLeftClip.y && topLeftClip.y <= topLeftClip.w &&
      -topLeftClip.w <= topLeftClip.z && topLeftClip.z <= topLeftClip.w )
  {
    // Definitely inside clip space - don't do any more processing
    return false;
  }

  Vector4 bottomRightClip = mvp * bottomRight;
  if( -bottomRightClip.w <= bottomRightClip.x && bottomRightClip.x <= bottomRightClip.w &&
      -bottomRightClip.w <= bottomRightClip.y && bottomRightClip.y <= bottomRightClip.w &&
      -bottomRightClip.w <= bottomRightClip.z && bottomRightClip.z <= bottomRightClip.w )
  {
    // Definitely inside clip space - don't do any more processing
    return false;
  }

  Vector4 topRightClip = mvp * topRight;
  if( -topRightClip.w <= topRightClip.x && topRightClip.x <= topRightClip.w &&
      -topRightClip.w <= topRightClip.y && topRightClip.y <= topRightClip.w &&
      -topRightClip.w <= topRightClip.z && topRightClip.z <= topRightClip.w )
  {
    // Definitely inside clip space - don't do any more processing
    return false;
  }

  Vector4 bottomLeftClip = mvp * bottomLeft;
  if( -bottomLeftClip.w <= bottomLeftClip.x && bottomLeftClip.x <= bottomLeftClip.w &&
      -bottomLeftClip.w <= bottomLeftClip.y && bottomLeftClip.y <= bottomLeftClip.w &&
      -bottomLeftClip.w <= bottomLeftClip.z && bottomLeftClip.z <= bottomLeftClip.w )
  {
    // Definitely inside clip space - don't do any more processing
    return false;
  }

  // Check to see if all four points are outside each plane

  unsigned int insideLeftPlaneCount=0;
  unsigned int insideRightPlaneCount=0;
  unsigned int insideTopPlaneCount=0;
  unsigned int insideBottomPlaneCount=0;

  if(-topLeftClip.w <= topLeftClip.x) { insideLeftPlaneCount++; }
  if(-topRightClip.w <= topRightClip.x){ insideLeftPlaneCount++; }
  if(-bottomRightClip.w <= bottomRightClip.x) {insideLeftPlaneCount++;}
  if(-bottomLeftClip.w <= bottomLeftClip.x) {insideLeftPlaneCount++;}

  if( insideLeftPlaneCount == 0 )
  {
    return true;
  }

  if(topLeftClip.x <= topLeftClip.w) { insideRightPlaneCount++;}
  if(topRightClip.x <= topRightClip.w) { insideRightPlaneCount++; }
  if(bottomRightClip.x <= bottomRightClip.w) { insideRightPlaneCount++; }
  if(bottomLeftClip.x <= bottomLeftClip.w ) { insideRightPlaneCount++; }

  if( insideRightPlaneCount == 0 )
  {
    return true;
  }

  if(-topLeftClip.w <= topLeftClip.y ) {insideTopPlaneCount++; }
  if(-topRightClip.w <= topRightClip.y) {insideTopPlaneCount++; }
  if(-bottomRightClip.w <= bottomRightClip.y) {insideTopPlaneCount++;}
  if(-bottomLeftClip.w <= bottomLeftClip.y) { insideTopPlaneCount++;}

  if( insideTopPlaneCount == 0 )
  {
    return true;
  }

  if(topLeftClip.y <= topLeftClip.w) { insideBottomPlaneCount++; }
  if(topRightClip.y <= topRightClip.w) { insideBottomPlaneCount++; }
  if(bottomRightClip.y <= bottomRightClip.w) { insideBottomPlaneCount++; }
  if(bottomLeftClip.y <= bottomLeftClip.w) { insideBottomPlaneCount++; }

  if( insideBottomPlaneCount == 0 )
  {
    return true;
  }

  // Test if any planes are bisected, if they are, then there is likely to
  // be an intersection into clip space.

  if( insideLeftPlaneCount < 4 )
  {
    return false;
  }
  if( insideRightPlaneCount < 4 )
  {
    return false;
  }
  if( insideTopPlaneCount < 4 )
  {
    return false;
  }
  if( insideBottomPlaneCount < 4 )
  {
    return false;
  }

  return true;
}


} // SceneGraph
} // Internal
} // Dali

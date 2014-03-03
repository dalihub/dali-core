#ifndef __DALI_INTERNAL_TEXT_VERTEX_2D_H__
#define __DALI_INTERNAL_TEXT_VERTEX_2D_H__

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
 * A 2D vertex with position, texture coordinate
 * and texture coordinate for distance field drop shadow adjustment
 */
struct TextVertex2D
{
  float mX;
  float mY;
  float mU;
  float mV;
  float mU1;
  float mV1;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_TEXT_VERTEX_2D_H__


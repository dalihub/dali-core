#ifndef DALI_INTERNAL_PROJECTION_H
#define DALI_INTERNAL_PROJECTION_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

namespace Dali
{

struct Vector4;
struct Vector2;

class Matrix;

namespace Internal
{

bool XyPlaneIntersect(const Dali::Vector4& pointA,
                      const Dali::Vector4& pointB,
                      Dali::Vector4& intersect);

bool UnprojectFull(const Dali::Vector4& windowPos,
                   const Matrix& modelView,
                   const Matrix& projection,
                   float viewportWidth,
                   float viewportHeight,
                   Dali::Vector4& objectPos);

bool Unproject(const Dali::Vector4& windowPos,
               const Matrix& inverseMvp,
               float viewportWidth,
               float viewportHeight,
               Dali::Vector4& objectPos);

bool ProjectFull( const Vector4& position,
                  const Matrix& modelView,
                  const Matrix& projection,
                  float viewportX,
                  float viewportY,
                  float viewportWidth,
                  float viewportHeight,
                  Vector4& windowPos );

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_PROJECTION_H


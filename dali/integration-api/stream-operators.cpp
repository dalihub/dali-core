/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <dali/integration-api/stream-operators.h>
#include <dali/integration-api/string-utils.h>

#include <string_view>

namespace Dali
{

std::ostream& operator<<(std::ostream& outStream, const String& string)
{
  outStream << Integration::ToStdStringView(string);
  return outStream;
}

std::ostream& operator<<(std::ostream& outStream, const StringView& stringView)
{
  outStream << Integration::ToStdStringView(stringView);
  return outStream;
}

std::ostream& operator<<(std::ostream& o, const Rect<int32_t>& rectangle)
{
  return o << "[" << rectangle.x << ", " << rectangle.y << ", " << rectangle.width << ", " << rectangle.height << "]";
}

std::ostream& operator<<(std::ostream& o, const Rect<float>& rectangle)
{
  return o << "[" << rectangle.x << ", " << rectangle.y << ", " << rectangle.width << ", " << rectangle.height << "]";
}

std::ostream& operator<<(std::ostream& o, const Vector2& vector)
{
  return o << "[" << vector.x << ", " << vector.y << "]";
}

std::ostream& operator<<(std::ostream& o, const Vector3& vector)
{
  return o << "[" << vector.x << ", " << vector.y << ", " << vector.z << "]";
}

std::ostream& operator<<(std::ostream& o, const Vector4& vector)
{
  return o << "[" << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << "]";
}

std::ostream& operator<<(std::ostream& o, const Quaternion& quaternion)
{
  Vector3 axis;
  Radian  angleRadians;

  quaternion.ToAxisAngle(axis, angleRadians);
  Degree degrees(angleRadians);

  return o << "[ Axis: [" << axis.x << ", " << axis.y << ", " << axis.z << "], Angle: " << degrees.degree << " degrees ]";
}

std::ostream& operator<<(std::ostream& o, const Matrix3& matrix)
{
  const float* elements = matrix.AsFloat();
  return o << "[ " << elements[0] << ", " << elements[1] << ", " << elements[2] << ", "
           << elements[3] << ", " << elements[4] << ", " << elements[5] << ", "
           << elements[6] << ", " << elements[7] << ", " << elements[8] << " ]";
}

std::ostream& operator<<(std::ostream& o, const Matrix& matrix)
{
  const float* elements = matrix.AsFloat();
  return o << "[ " << elements[0] << ", " << elements[1] << ", " << elements[2] << ", " << elements[3] << ", "
           << elements[4] << ", " << elements[5] << ", " << elements[6] << ", " << elements[7] << ", "
           << elements[8] << ", " << elements[9] << ", " << elements[10] << ", " << elements[11] << ", "
           << elements[12] << ", " << elements[13] << ", " << elements[14] << ", " << elements[15] << " ]";
}

std::ostream& operator<<(std::ostream& o, const Dali::AngleAxis& angleAxis)
{
  return o << "[ Axis: [" << angleAxis.axis.x << ", " << angleAxis.axis.y << ", " << angleAxis.axis.z << "], Angle: " << Degree(angleAxis.angle).degree << " degrees ]";
}

std::ostream& operator<<(std::ostream& stream, const Extents& extents)
{
  return stream << "[" << extents.start << ", " << extents.end << ", " << extents.top << ", " << extents.bottom << "]";
}

} //namespace Dali

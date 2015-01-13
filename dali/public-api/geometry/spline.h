#ifndef __DALI_SPLINE_H__
#define __DALI_SPLINE_H__

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


// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/matrix.h>

namespace Dali
{

class Matrix;
struct Vector3;

/**
 * @brief The Spline class is used to interpolate through a series of points
 * (or knots) which are open (the end points do not connect).
 *
 * In Hermite splines, each knot has an in-tangent and an
 * out-tangent associated with it. In the case of Bezier splines,
 * there are equivalent control points.
 *
 * Once the spline is initialized with AddKnot(), SetInTangent() and
 * SetOutTangent(), the client can choose to use either parametric
 * interpolation (using GetPoint()), or calculate a Y value from an X
 * value (using GetYFromMonotonicX()). The latter requires that the
 * curve is monotonic along X (i.e. for any given X value there is
 * exactly 1 Y value). Thus, the initial points and tangents have to
 * be chosen carefully. (Most useful for animation curves)
 *
 */
class DALI_IMPORT_API Spline
{
public:
  /**
   * @brief Constructor.
   */
  Spline();

  /**
   * @brief Destructor.
   */
  virtual ~Spline();

  /**
   * @brief Add a knot to the spline.
   *
   * @param[in] knot - the point or knot to add.
   */
  void AddKnot(Vector3 knot);

  /**
   * @brief Get the knot at the given index.
   *
   * @param[in] knotIndex the index of the knot
   * @return the knot vector
   */
  Vector3 GetKnot(size_t knotIndex);

  /**
   * @brief Get the in tangent (or in control point) at the given index.
   *
   * @param[in] knotIndex the index of the knot
   * @return the in tangent (or in control point)
   */
  Vector3 GetInTangent(size_t knotIndex);

  /**
   * @brief Get the out tangent (or out control point) at the given index.
   *
   * @param[in] knotIndex the index of the knot
   * @return the out tangent (or out control point)
   */
  Vector3 GetOutTangent(size_t knotIndex);

  /**
   * @brief Set an in-tangent for the given knot.
   *
   * @param[in] knotIndex - the index of the knot
   * @param[in] inTangent - The in tangent or control point, depending on the spline type
   */
  void SetInTangent(size_t knotIndex, Vector3 inTangent);

  /**
   * @brief Set an out-tangent for the given knot.
   *
   * @param[in] knotIndex - the index of the knot
   * @param[in] outTangent - The out tangent or control point, depending on the spline type
   */
  void SetOutTangent(size_t knotIndex, Vector3 outTangent);

  /**
   * @brief For the given set of knot points, generate control points which result in a smooth join between the splines of each segment.
   *
   * The generating algorithm is as follows:
   * For a given knot point K[N], find the vector that bisects K[N-1],[N] and [N],[N+1].
   * Calculate the tangent vector by taking the normal of this bisector.
   * The in control point is the length of the preceding segment back along this bisector multiplied by the curvature
   * The out control point is the length of the succceding segment forward along this bisector multiplied by the curvature
   *
   * @param[in] curvature The curvature of the spline. 0 gives straight lines between the knots,
   *                      negative values means the spline contains loops, positive values up to
   *                      0.5 result in a smooth curve, positive values between 0.5 and 1 result
   *                      in looped curves where the loops are not distinct (i.e. the curve appears
   *                      to be non-continuous), positive values higher than 1 result in looped curves.
   */
  void GenerateControlPoints(float curvature=0.25f);

  /**
   * @brief Get the number of segments.
   *
   * This is the number of points - 1, or zero if there are fewer than 2 points.
   * @return the number of segments
   */
  unsigned int GetNumberOfSegments() const;

  /**
   * @brief Given an alpha value 0.0-1.0, return the associated point on the entire
   * spline.
   *
   * The alpha value is split into each segment equally, so doesn't give a
   * constant speed along the spline.  An alpha value of 0.0 returns the first knot
   * and an alpha value of 1.0 returns the last knot.
   *
   * @param[in] alpha - the overall parameter value.
   * @return point - the interpolated point.
   */
  const Vector3 GetPoint(float alpha) const;

  /**
   * @brief Given a parameter 0.0 - 1.0, return the associated point
   * on the spline given the relevant segment.
   *
   * Note, the parameter does not correspond to an axis, neither is it
   * necessarily equally spaced along the curve.  If the parameter is
   * 0.0 or 1.0, then the first or second knot point is returned.
   *
   * @param[in] segmentIndex the segment of the spline to interpolation.
   * @param[in] parameter    the parameter to use in the parametric formula
   * @return point           the interpolated point.
   */
  const Vector3 GetPoint(unsigned int segmentIndex, float parameter) const;

  /**
   * @brief Given a value X on the spline (between the first and last knot inclusively),
   * calculate the corresponding Y value. Requires that there is only 1 Y per X.
   *
   * @param[in] x The X
   * @return the Y value
   */
  float GetYFromMonotonicX(float x) const;

  /**
   * @brief For a 2 dimensional curve (z=0), given a parameter 0.0 - 1.0, return the
   * associated Y value on the spline given the relevant segment.
   *
   * Note, the parameter does not correspond to an axis, neither is it
   * necessarily equally spaced along the curve.  If the parameter is
   * 0.0 or 1.0, then the first or second knot point's Y value is
   * returned.
   *
   * @param[in] segmentIndex - the segment of the spline to interpolation.
   * @param[in] parameter    - the parameter to use in the parametric formula
   * @return y value    - the interpolated point's Y value
   */
  float GetY(unsigned int segmentIndex, float parameter) const;


private:
  /**
   * @brief FindSegment searches through the control points to find the appropriate
   * segment.
   *
   * @param[in]  x            - the X value to search for
   * @param[out] segmentIndex - the returned segment index
   * @return True if the segment was found.
   */
  bool FindSegment(float x, int& segmentIndex) const;

  /**
   * @brief Given two knots and control points, interpolate to find the
   * parameter that will give the required X value.
   *
   * Requires that X is monotonic.
   *
   * @param[in] atX  the X value to search for
   * @param[in] P0_X the starting knot's X
   * @param[in] C0_X the X of the control point for the first knot
   * @param[in] C1_X the X of the control point for the second knot
   * @param[in] P1_X the ending knot's X.
   * @return         the corresponding parameter.
   */
  float ApproximateCubicBezierParameter (float atX, float P0_X, float C0_X, float C1_X, float P1_X ) const;

private:
  std::vector<Vector3>   mKnots;          ///< The set of knot points
  std::vector<Vector3>   mOutTangents;    ///< The set of out tangents (matching to mKnots)
  std::vector<Vector3>   mInTangents;     ///< The set of in tangents  (matching to mKnots)

  Matrix                 mBasis;          ///< The basis matrix to use

  static const float     BezierBasisA[];  ///< The basis matrix for Bezier splines
};

} //namespace Dali

#endif //__DALI_SPLINE_H__

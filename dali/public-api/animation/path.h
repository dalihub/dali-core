#ifndef __DALI_PATH_H__
#define __DALI_PATH_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/property-index-ranges.h>

namespace Dali
{
/**
 * @addtogroup dali_core_animation
 * @{
 */

namespace Internal DALI_INTERNAL
{
class Path;
}

/**
 * @brief A 3D parametric curve
 *
 * Paths can be used to animate position and orientation of actors using Dali::Animate()
 *
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API Path : public Handle
{
public:

  /**
   * @brief An enumeration of properties belonging to the Path class.
   * @SINCE_1_0.0
   */
  struct Property
  {
    /**
     * @brief An enumeration of properties belonging to the Path class.
     * @SINCE_1_0.0
     */
    enum
    {
      POINTS         = DEFAULT_OBJECT_PROPERTY_START_INDEX, ///< name "points",         type Vector3 @SINCE_1_0.0
      CONTROL_POINTS,                                       ///< name "controlPoints",  type Vector3 @SINCE_1_0.0
    };
  };

  /**
   * @brief Create an initialized Path handle.
   *
   * @SINCE_1_0.0
   * @return A handle to a newly allocated Dali resource.
   */
  static Path New();

  /**
   * @brief Downcast a handle to Path handle.
   *
   * If handle points to a Path object the downcast produces
   * valid handle. If not the returned handle is left uninitialized.
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return Handle to a Path object or an uninitialized handle
   */
  static Path DownCast( BaseHandle handle );

  /**
   * @brief Create an uninitialized Path handle.
   *
   * This can be initialized with Path::New().
   * Calling member functions with an uninitialized Path handle is not allowed.
   * @SINCE_1_0.0
   */
  Path();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~Path();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param [in] handle A reference to the copied handle
   */
  Path(const Path& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  Path& operator=(const Path& rhs);

  /**
   * @brief Add an interpolation point.
   *
   * @SINCE_1_0.0
   * @param[in] point The new interpolation point to be added
   */
  void AddPoint(const Vector3& point );

  /**
   * @brief Add a control point.
   *
   * @SINCE_1_0.0
   * @param[in] point The new control point to be added
   */
  void AddControlPoint(const Vector3& point );

  /**
   * @brief Automatic generation of control points. Generated control points which result in a smooth join between the splines of each segment.
   *
   * The generating algorithm is as follows:
   * For a given knot point K[N], find the vector that bisects K[N-1],[N] and [N],[N+1].
   * Calculate the tangent vector by taking the normal of this bisector.
   * The in control point is the length of the preceding segment back along this bisector multiplied by the curvature.
   * The out control point is the length of the succeeding segment forward along this bisector multiplied by the curvature.
   *
   * @SINCE_1_0.0
   * @param[in] curvature The curvature of the spline. 0 gives straight lines between the knots,
   *                      negative values means the spline contains loops, positive values up to
   *                      0.5 result in a smooth curve, positive values between 0.5 and 1 result
   *                      in looped curves where the loops are not distinct (i.e. the curve appears
   *                      to be non-continuous), positive values higher than 1 result in looped curves.
   * @pre There are at least two points in the path ( one segment ).
   *
   */
  void GenerateControlPoints( float curvature );

  /**
   * @brief Sample path at a given progress. Calculates position and tangent at that point of the curve
   *
   * @SINCE_1_0.0
   * @param[in]  progress  A floating point value between 0.0 and 1.0.
   * @param[out] position The interpolated position at that progress.
   * @param[out] tangent The interpolated tangent at that progress.
   */
  void Sample( float progress, Vector3& position, Vector3& tangent ) const;

  /**
   * @brief Accessor for the interpolation points.
   *
   * @SINCE_1_0.0
   * @param[in] index The index of the interpolation point.
   * @return A reference to the interpolation point.
   */
  Vector3& GetPoint( size_t index );

  /**
   * @brief Accessor for the control points.
   *
   * @SINCE_1_0.0
   * @param[in] index The index of the control point.
   * @return A reference to the control point.
   */
  Vector3& GetControlPoint( size_t index );

  /**
   * @brief Get the number of interpolation points in the path
   *
   * @SINCE_1_0.0
   * @return The number of interpolation points in the path
   */
  size_t GetPointCount() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief This constructor is used by Path::New() methods.
   *
   * @SINCE_1_0.0
   * @param[in] path A pointer to an internal path resource
   */
  explicit DALI_INTERNAL Path(Internal::Path* path);
  /// @endcond
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_KEY_FRAMES_H__

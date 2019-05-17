#ifndef DALI_INTERNAL_PATH_H
#define DALI_INTERNAL_PATH_H

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

// INTERNAL INCLUDES
#include <dali/public-api/animation/path.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/internal/event/common/object-impl.h>

namespace Dali
{

namespace Internal
{
typedef IntrusivePtr<Path> PathPtr;

/**
 * A 3D path
 */
class Path : public Object
{
public:

  /**
   * Construct a new path
   */
  static Path* New();

  /**
   * Constructor
   */
  Path();


protected:
  /**
   * virtual destructor
   */
  virtual ~Path();

private:

  /**
   * @copydoc Dali::Internal::Object::SetDefaultProperty()
   */
  virtual void SetDefaultProperty(Property::Index index, const Property::Value& propertyValue);

   /**
   * @copydoc Dali::Internal::Object::GetDefaultProperty()
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const;

public:

  /**
   * Returns a clone to the given path
   */
  static Path* Clone(const Path& path);

  /**
   * @copydoc Dali::Path::AddPoint
   */
  void AddPoint(const Vector3& point );

  /**
   * @copydoc Dali::Path::AddControlPoint
   */
  void AddControlPoint(const Vector3& point );

  /**
   * @copydoc Dali::Path::GenerateControlPoints
   */
  void GenerateControlPoints( float curvature );

  /**
   * @copydoc Dali::Path::Sample
   */
  void Sample( float t, Vector3& position, Vector3& tangent ) const;

  /**
   * @brief Sample path at a given progress. Calculates position and tangent at that point of the curve
   *
   * @param[in]  progress  A floating point value between 0.0 and 1.0.
   * @param[out] position The interpolated position at that progress.
   * @param[out] tangent The interpolated tangent at that progress.
   * @return true if Sample could be calculated
   */
  bool SampleAt( float t, Vector3& position, Vector3& tangent ) const;

  /**
   * Sample position at point t.
   *
   * @param[in] progress  A floating point value between 0.0 and 1.0.
   * @param[out] position The interpolated position at that progress.
   * @return true if sample could be calculated
   */
  bool SamplePosition( float t, Vector3& position ) const;

  /**
   * @brief Sample tangent at point t.
   *
   * @param[in] progress  A floating point value between 0.0 and 1.0.
   * @param[out] tangent The interpolated tangent at that progress.
   * @return true if sample could be calculated
   */
  bool SampleTangent( float t, Vector3& tangent ) const;

  /**
   * @copydoc Dali::Path::GetPoint
   */
  Vector3& GetPoint( uint32_t index );

  /**
   * @copydoc Dali::Path::GetControlPoint
   */
  Vector3& GetControlPoint( uint32_t index );

  /**
   * @copydoc Dali::Path::GetPointCount
   */
  uint32_t GetPointCount() const;

  /**
   * Clears the points of the path
   */
  void ClearPoints();

  /**
   * Clears the control points of the path
   */
  void ClearControlPoints();

  /**
   * @brief Get mPoint property
   *
   * @return A const reference to mPoint vector
   */
  const Dali::Vector<Vector3>& GetPoints() const{ return mPoint; }

  /*
   * @brief Set mPoint
   *
   * @param[in] p New value for mPoint property
   */
  void SetPoints( const Dali::Vector<Vector3>& p ){ mPoint = p; }

  /**
   * @brief Get mCotrolPoint property
   *
   * @return A const reference to mControlPoint vector
   */
  const Dali::Vector<Vector3>& GetControlPoints() const{ return mControlPoint; }

  /*
   * @brief Set mControlPoint property
   *
   * @param[in] p New value for mControlPoint property
   */
  void SetControlPoints( const Dali::Vector<Vector3>& p ){ mControlPoint = p; }

private:

  /**
   * Undefined
   */
  Path(const Path& p);

  /**
   * Undefined
   */
  Path& operator=(const Path& rhs);

  /**
   * Helper function to calculate the segment and local progress in that segment
   * given a progress
   *
   * @param[in] t Progress
   * @param[out] segment Segment for t
   * @param[out] tLocal Local progress in the segment
   *
   */
  void FindSegmentAndProgress( float t, uint32_t& segment, float& tLocal ) const;

  /**
   * Helper function to calculate to number of segments in the path
   */
  uint32_t GetNumberOfSegments() const;

  Dali::Vector<Vector3> mPoint;            ///< Interpolation points
  Dali::Vector<Vector3> mControlPoint;     ///< Control points
};

} // Internal

// Get impl of handle
inline Internal::Path& GetImplementation(Dali::Path& path)
{
  DALI_ASSERT_ALWAYS( path && "Path handle is empty" );
  Dali::RefObject& object = path.GetBaseObject();
  return static_cast<Internal::Path&>(object);
}

inline const Internal::Path& GetImplementation(const Dali::Path& path)
{
  DALI_ASSERT_ALWAYS( path && "Path handle is empty" );
  const Dali::RefObject& object = path.GetBaseObject();
  return static_cast<const Internal::Path&>(object);
}


} // Dali

#endif // DALI_INTERNAL_PATH_H

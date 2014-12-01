#ifndef __DALI_INTERNAL_PATH_H__
#define __DALI_INTERNAL_PATH_H__

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
#include <dali/public-api/animation/path.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/internal/event/common/proxy-object.h>

namespace Dali
{

namespace Internal
{
typedef IntrusivePtr<Path> PathPtr;

/**
 * A 3D path
 */
class Path : public ProxyObject
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
   * @copydoc Dali::Internal::ProxyObject::Observer::SceneObjectAdded()
   */
  virtual void SceneObjectAdded(ProxyObject& proxy){}

  /**
   * @copydoc Dali::Internal::ProxyObject::Observer::SceneObjectAdded()
   */
  virtual void SceneObjectRemoved(ProxyObject& proxy){}

  /**
   * @copydoc Dali::Internal::ProxyObject::Observer::ProxyDestroyed()
   */
  virtual void ProxyDestroyed(ProxyObject& proxy){}

  /**
   * @copydoc Dali::Internal::ProxyObject::IsSceneObjectRemovable()
   */
  virtual bool IsSceneObjectRemovable() const{ return false;}

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyCount()
   */
  virtual unsigned int GetDefaultPropertyCount() const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyIndices()
   */
  virtual void GetDefaultPropertyIndices( Property::IndexContainer& indices ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyName()
   */
  virtual const char* GetDefaultPropertyName(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyIndex()
   */
  virtual Property::Index GetDefaultPropertyIndex(const std::string& name) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::IsDefaultPropertyWritable()
   */
  virtual bool IsDefaultPropertyWritable(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::IsDefaultPropertyAnimatable()
   */
  virtual bool IsDefaultPropertyAnimatable(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::IsDefaultPropertyAConstraintInput()
   */
  virtual bool IsDefaultPropertyAConstraintInput( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyType()
   */
  virtual Property::Type GetDefaultPropertyType(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::SetDefaultProperty()
   */
  virtual void SetDefaultProperty(Property::Index index, const Property::Value& propertyValue);

   /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultProperty()
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::InstallSceneObjectProperty()
   */
  virtual void InstallSceneObjectProperty( SceneGraph::PropertyBase& newProperty, const std::string& name, unsigned int index ){}

  /**
   * @copydoc Dali::Internal::ProxyObject::GetSceneObject()
   */
  virtual const SceneGraph::PropertyOwner* GetSceneObject() const{ return NULL; }

  /**
   * @copydoc Dali::Internal::ProxyObject::GetSceneObjectAnimatableProperty()
   */
  virtual const SceneGraph::PropertyBase* GetSceneObjectAnimatableProperty( Property::Index index ) const{ return NULL; }

  /**
   * @copydoc Dali::Internal::ProxyObject::GetSceneObjectInputProperty()
   */
  virtual const PropertyInputImpl* GetSceneObjectInputProperty( Property::Index index ) const{ return NULL; }

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
   * Sample position at point t.
   *
   * @param[in] progress  A floating point value between 0.0 and 1.0.
   * @param[out] position The interpolated position at that progress.
   */
  Vector3 SamplePosition( float t ) const;

  /**
   * @brief Sample tangent at point t.
   *
   * @param[in] progress  A floating point value between 0.0 and 1.0.
   * @param[out] tangent The interpolated tangent at that progress.
   */
  Vector3 SampleTangent( float t ) const;

  /**
   * @copydoc Dali::Path::GetPoint
   */
  Vector3& GetPoint( size_t index );

  /**
   * @copydoc Dali::Path::GetControlPoint
   */
  Vector3& GetControlPoint( size_t index );

  /**
   * @copydoc Dali::Path::GetPointCount
   */
  size_t GetPointCount() const;

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
  void FindSegmentAndProgress( float t, unsigned int& segment, float& tLocal ) const;

  /**
   * Helper function to calculate to number of segments in the path
   */
  unsigned int GetNumberOfSegments() const;

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

#endif //__DALI_INTERNAL_KEY_FRAMES_H__

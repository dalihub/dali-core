#ifndef __DALI_INTERNAL_PATH_CONSTRAINT_H__
#define __DALI_INTERNAL_PATH_CONSTRAINT_H__

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
#include <dali/internal/event/animation/path-impl.h>
#include <dali/internal/event/common/object-impl.h>
#include <dali/public-api/animation/path-constraint.h>

namespace Dali
{

namespace Internal
{

typedef IntrusivePtr<PathConstraint>  PathConstraintPtr;
typedef Dali::Vector<Object*>         ObjectContainer;
typedef ObjectContainer::Iterator     ObjectIter;

/**
 * @brief Constraint functor to constraint properties to paths.
 *
 * Vector3 properties will be constrained to the position of the path and
 * Rotation properties will be constrained to follow the tangent of the path
 * given a forward vector in object's local space.
 */
struct PathConstraintFunctor
{
  /**
   * @brief Constructor.
   *
   * @param[in] path The path used in the constraint
   * @param[in] range The range of values in the input property which will be mapped to 0..1
   */

  PathConstraintFunctor(PathPtr path, const Vector2& range ):mPath(path),mRange(range){}

  /**
   * @brief Constructor.
   *
   * @param[in] path The path used in the constraint
   * @param[in] range The range of values in the input property which will be mapped to 0..1
   * @param[in] forward Vector in object space which will be aligned with the tangent of the path
   */
  PathConstraintFunctor(PathPtr path, const Vector2& range,const Vector3& forward ):mPath(path),mForward(forward),mRange(range){}

  /**
   * @brief Functor operator for Vector3 properties
   *
   * @param[in] current Current value of the property
   * @param[in] property The input property used as the parameter for the path
   *
   * @return The position of the path at the given parameter.
   */
  Vector3 operator()(const Vector3& current,
                     const PropertyInput& property)
  {
    float t = ( property.GetFloat() - mRange.x ) / (mRange.y-mRange.x);
    Vector3 position, tangent;
    mPath->Sample( t, position, tangent );
    return position;
  }

  /**
   * @brief Functor operator for Quaternion properties
   *
   * @param[in] current Current value of the property
   * @param[in] property The input property used as the parameter for the path
   *
   * @return The rotation which will align the forward vector and the tangent of the path at the given parameter.
   */
  Quaternion operator()( const Quaternion& current,
                         const PropertyInput& property)
  {
    float t = ( property.GetFloat() - mRange.x ) / (mRange.y-mRange.x);
    Vector3 position, tangent;
    mPath->Sample( t, position, tangent );
    return Quaternion( mForward, tangent );
  }

  PathPtr     mPath;      ///< The path used
  Vector3     mForward;   ///< Vector in object space which will be aligned with the tangent of the path
  Vector2     mRange;     ///< The range of values in the input property which will be mapped to 0..1
};

/**
 * @brief A PathConstraint used to constraint properties to a path
 */
class PathConstraint : public Object, public Object::Observer
{
public:

  /**
   * Create a new PathConstraint
   * @param[in] path The path used in the constraint
   * @param[in] range The range of values in the input property which will be mapped to 0..1
   * @return A smart-pointer to the newly allocated PathConstraint.
   */
  static PathConstraint* New( Path& path, const Vector2& range );



protected:
  /**
   * virtual destructor
   */
  virtual ~PathConstraint();

private:
  /**
   * @copydoc Dali::Internal::Object::Observer::SceneObjectAdded()
   */
  virtual void SceneObjectAdded(Object& object){}

  /**
   * @copydoc Dali::Internal::Object::Observer::SceneObjectAdded()
   */
  virtual void SceneObjectRemoved(Object& object){}

  /**
   * @copydoc Dali::Internal::Object::Observer::ObjectDestroyed()
   */
  virtual void ObjectDestroyed(Object& object);

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyCount()
   */
  virtual unsigned int GetDefaultPropertyCount() const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyIndices()
   */
  virtual void GetDefaultPropertyIndices( Property::IndexContainer& indices ) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyName()
   */
  virtual const char* GetDefaultPropertyName(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyIndex()
   */
  virtual Property::Index GetDefaultPropertyIndex(const std::string& name) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyWritable()
   */
  virtual bool IsDefaultPropertyWritable(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyAnimatable()
   */
  virtual bool IsDefaultPropertyAnimatable(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyAConstraintInput()
   */
  virtual bool IsDefaultPropertyAConstraintInput( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyType()
   */
  virtual Property::Type GetDefaultPropertyType(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::SetDefaultProperty()
   */
  virtual void SetDefaultProperty(Property::Index index, const Property::Value& propertyValue);

   /**
   * @copydoc Dali::Internal::Object::GetDefaultProperty()
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObject()
   */
  virtual const SceneGraph::PropertyOwner* GetSceneObject() const{ return NULL; }

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectAnimatableProperty()
   */
  virtual const SceneGraph::PropertyBase* GetSceneObjectAnimatableProperty( Property::Index index ) const{ return NULL; }

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectInputProperty()
   */
  virtual const PropertyInputImpl* GetSceneObjectInputProperty( Property::Index index ) const{ return NULL; }

public:

  /**
   * @copydoc Dali::PathConstraint::Apply
   */
  void Apply( Property source, Property target, const Vector3& forward );

  /**
   * @copydoc Dali::PathConstraint::Remove
   */
  void Remove( Dali::Handle& target );


private:
  /**
   * Constructor
   * @param[in] path The path used in the constraint
   * @param[in] range The range of values in the input property which will be mapped to 0..1
   */
  PathConstraint( Path& path, const Vector2& range );

  // Undefined
  PathConstraint();

  // Undefined
  PathConstraint(const PathConstraint&);

  // Undefined
  PathConstraint& operator=(const PathConstraint& rhs);

  PathPtr           mPath;              ///< The path used to constrain objects
  ObjectContainer   mObservedObjects;   ///< The list of object which have been constrained by the PathConstraint
  Vector2           mRange;             ///< The range of values in the input property which will be mapped to 0..1
};

} // Internal

// Get impl of handle
inline Internal::PathConstraint& GetImplementation(Dali::PathConstraint& pathConstraint)
{
  DALI_ASSERT_ALWAYS( pathConstraint && "PathConstraint handle is empty" );
  Dali::RefObject& object = pathConstraint.GetBaseObject();
  return static_cast<Internal::PathConstraint&>(object);
}

inline const Internal::PathConstraint& GetImplementation(const Dali::PathConstraint& pathConstraint)
{
  DALI_ASSERT_ALWAYS( pathConstraint && "PathConstraint handle is empty" );
  const Dali::RefObject& object = pathConstraint.GetBaseObject();
  return static_cast<const Internal::PathConstraint&>(object);
}

} // Dali

#endif //__DALI_INTERNAL_KEY_FRAMES_H__

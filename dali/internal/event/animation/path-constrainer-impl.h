#ifndef __DALI_INTERNAL_PATH_CONSTRAINER_H__
#define __DALI_INTERNAL_PATH_CONSTRAINER_H__

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
#include <dali/internal/event/animation/constrainer.h>
#include <dali/public-api/animation/path-constrainer.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/internal/event/animation/path-impl.h>

namespace Dali
{

namespace Internal
{

typedef IntrusivePtr<PathConstrainer> PathConstrainerPtr;

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
   * @param[in] range The range of values in the input property which will be mapped to [0,1]
   * @param[in] wrap Wrapping domain. Input property value will be wrapped in the domain [wrap.x,wrap.y] before mapping to [0,1]
   */
  PathConstraintFunctor(PathPtr path, const Vector2& range, const Vector2& wrap ):mPath(path),mRange(range),mWrap(wrap){}

  /**
   * @brief Constructor.
   *
   * @param[in] path The path used in the constraint
   * @param[in] range The range of values in the input property which will be mapped to 0..1
   * @param[in] forward Vector in object space which will be aligned with the tangent of the path
   * @param[in] wrap Wrapping domain. Input property value will be wrapped in the domain [wrap.x,wrap.y] before mapping to [0,1]
   */
  PathConstraintFunctor(PathPtr path, const Vector2& range,const Vector3& forward, const Vector2& wrap ):mPath(path),mForward(forward),mRange(range),mWrap(wrap){}

  /**
   * @brief Functor operator for Vector3 properties
   *
   * @param[in,out] position Current value of the property
   * @param[in]     inputs Contains the input property used as the parameter for the path
   *
   * @return The position of the path at the given parameter.
   */
  void operator()( Vector3& position,
                   const PropertyInputContainer& inputs)
  {
    float inputWrapped = inputs[0]->GetFloat();
    if( inputWrapped < mWrap.x || inputWrapped > mWrap.y )
    {
      inputWrapped = WrapInDomain(inputWrapped, mWrap.x, mWrap.y);
    }

    float t = ( inputWrapped - mRange.x ) / ( mRange.y-mRange.x );

    Vector3 tangent;
    mPath->Sample( t, position, tangent );
  }

  /**
   * @brief Functor operator for Quaternion properties
   *
   * @param[in,out] current Current value of the property
   * @param[in]     inputs Contains the input property used as the parameter for the path
   *
   * @return The rotation which will align the forward vector and the tangent of the path at the given parameter.
   */
  void operator()( Quaternion& current,
                   const PropertyInputContainer& inputs)
  {
    float inputWrapped = inputs[0]->GetFloat();
    if( inputWrapped < mWrap.x || inputWrapped > mWrap.y )
    {
      inputWrapped = WrapInDomain(inputWrapped, mWrap.x, mWrap.y);
    }

    float t = ( inputWrapped - mRange.x ) / ( mRange.y-mRange.x );

    Vector3 position, tangent;
    mPath->Sample( t, position, tangent );
    current = Quaternion( mForward, tangent );
  }

  PathPtr     mPath;      ///< The path used
  Vector3     mForward;   ///< Vector in object space which will be aligned with the tangent of the path
  Vector2     mRange;     ///< The range of values in the input property which will be mapped to 0..1
  Vector2     mWrap;      ///< Wrapping domain. Input property will be wrapped in this domain before being mapped to [0,1]
};

/**
 * @brief A PathConstrainer used to constraint properties to a path
 */
class PathConstrainer : public Constrainer
{
public:

  /**
   * Create a new PathConstrainer
   * @return A smart-pointer to the newly allocated PathConstrainer.
   */
  static PathConstrainer* New();

protected:

  /**
   * virtual destructor
   */
  virtual ~PathConstrainer();

private:

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

public:

  /**
   * @copydoc Dali::PathConstrainer::Apply
   */
  void Apply( Property target, Property source, const Vector2& range, const Vector2& wrap );

private:

  //Constructor
  PathConstrainer();

  // Undefined
  PathConstrainer(const PathConstrainer&);

  // Undefined
  PathConstrainer& operator=(const PathConstrainer& rhs);

  PathPtr mPath;    ///< The path used in the constraints
  Vector3 mForward; ///< Vector in object space which will be aligned with the tangent of the path
};

} // Internal

// Get impl of handle
inline Internal::PathConstrainer& GetImplementation(Dali::PathConstrainer& pathConstrainer)
{
  DALI_ASSERT_ALWAYS( pathConstrainer && "PathConstrainer handle is empty" );
  Dali::RefObject& object = pathConstrainer.GetBaseObject();
  return static_cast<Internal::PathConstrainer&>(object);
}

inline const Internal::PathConstrainer& GetImplementation(const Dali::PathConstrainer& pathConstrainer)
{
  DALI_ASSERT_ALWAYS( pathConstrainer && "PathConstrainer handle is empty" );
  const Dali::RefObject& object = pathConstrainer.GetBaseObject();
  return static_cast<const Internal::PathConstrainer&>(object);
}

} // Dali

#endif //__DALI_INTERNAL_PATH_CONSTRAINER_H__

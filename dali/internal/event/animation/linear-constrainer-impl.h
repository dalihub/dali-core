#ifndef __DALI_INTERNAL_LINEAR_CONSTRAINER_H__
#define __DALI_INTERNAL_LINEAR_CONSTRAINER_H__

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
#include <dali/public-api/animation/linear-constrainer.h>
#include <dali/public-api/math/math-utils.h>

namespace Dali
{

namespace Internal
{

typedef IntrusivePtr<LinearConstrainer> LinearConstrainerPtr;

/**
 * @brief Constraint functor to constraint properties given a linear map.
 */
struct LinearConstraintFunctor
{
  /**
   * @brief Constructor.
   *
   * @param[in] value The list of values for the linear map (f(x) of the linear map)
   * @param[in] progress Progress for each of the values normalized to [0,1] ( x of the linear map)
   * @param[in] range The range of values in the input property which will be mapped to [0,1]
   * @param[in] wrap Wrapping domain. Input property value will be wrapped in the domain [wrap.x,wrap.y] before mapping to [0,1]
   *
   * @note If progress is an empty vector, the values will be assumed to be equally spaced in the x-axis
   */
  LinearConstraintFunctor( Dali::Vector<float>& value, Dali::Vector<float>& progress, const Vector2& range, const Vector2& wrap )
  :mValue(value),
   mProgress(progress),
   mRange(range),
   mWrap(wrap)
  {}

  /**
   * @brief Functor operator for float properties
   *
   * @param[in,out] position Current value of the property
   * @param[in]     inputs Contains the input property used as the parameter for the path
   *
   * @return The value of the linear map at the given parameter.
   */
  void operator()( float& value,
                   const PropertyInputContainer& inputs)
  {
    size_t valueCount(  mValue.Size() );
    if( valueCount == 0 )
    {
      //No values.
    }
    else if(valueCount == 1 )
    {
      value = mValue[0];
    }
    else
    {
      float inputWrapped = inputs[0]->GetFloat();
      if( inputWrapped < mWrap.x || inputWrapped > mWrap.y )
      {
        inputWrapped = WrapInDomain(inputWrapped, mWrap.x, mWrap.y);
      }

      float t = (( inputWrapped - mRange.x ) / ( mRange.y-mRange.x ));

      //Find min and max values and local t between them
      size_t min(0);
      size_t max(0);
      float tLocal(0.0f);
      if( mProgress.Size() < valueCount )
      {
        float step = 1.0f / (valueCount-1.0f);
        float tLocation = t/step;
        if( tLocation < 0)
        {
          min = 0;
          max = 1;
        }
        else if( tLocation >= valueCount-1 )
        {
          min = max = valueCount-1;
        }
        else
        {
          min = static_cast<size_t>(tLocation);
          max = min+1;
        }

        tLocal =(t - min*step) / step;
      }
      else
      {
        while( t >= mProgress[min] && min < valueCount-1 )
        {
          min++;
        }

        min--;
        max = min+1;

        if( min >= valueCount-1)
        {
          min = max = valueCount-1;
          tLocal = 0.0f;
        }
        else
        {
          tLocal =(t - mProgress[min]) / ( mProgress[max]-mProgress[min]);
        }
      }

      //Linear interpolation
      value = (mValue[max]-mValue[min])*tLocal + mValue[min];
    }
  }


  Dali::Vector<float> mValue;     ///< values for the linear map
  Dali::Vector<float> mProgress;  ///< Progress for each of the values normalized to [0,1]
  Vector2             mRange;     ///< The range of values in the input property which will be mapped to 0..1
  Vector2             mWrap;      ///< Wrapping domain. Input property will be wrapped in this domain before being mapped to [0,1]
};

/**
 * @brief A LinearConstrainer used to constraint properties given a linear map
 */
class LinearConstrainer : public Constrainer
{
public:

  /**
   * Create a new LinearConstrainer
   * @return A smart-pointer to the newly allocated LinearConstrainer.
   */
  static LinearConstrainer* New();

protected:

  /**
   * virtual destructor
   */
  virtual ~LinearConstrainer();

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
  LinearConstrainer();

  // Undefined
  LinearConstrainer(const LinearConstrainer&);

  // Undefined
  LinearConstrainer& operator=(const LinearConstrainer& rhs);

  Dali::Vector<float> mValue;     ///< values for the linear map
  Dali::Vector<float> mProgress;  ///< Progress for each of the values normalized to [0,1]
};

} // Internal

// Get impl of handle
inline Internal::LinearConstrainer& GetImplementation(Dali::LinearConstrainer& linearConstrainer)
{
  DALI_ASSERT_ALWAYS( linearConstrainer && "LinearConstrainer handle is empty" );
  Dali::RefObject& object = linearConstrainer.GetBaseObject();
  return static_cast<Internal::LinearConstrainer&>(object);
}

inline const Internal::LinearConstrainer& GetImplementation(const Dali::LinearConstrainer& linearConstrainer)
{
  DALI_ASSERT_ALWAYS( linearConstrainer && "LinearConstrainer handle is empty" );
  const Dali::RefObject& object = linearConstrainer.GetBaseObject();
  return static_cast<const Internal::LinearConstrainer&>(object);
}

} // Dali

#endif //__DALI_INTERNAL_PATH_CONSTRAINER_H__

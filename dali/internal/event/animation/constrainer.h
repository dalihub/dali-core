#ifndef __DALI_INTERNAL_CONSTRAINER_H__
#define __DALI_INTERNAL_CONSTRAINER_H__

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
#include <dali/internal/event/common/object-impl.h>
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{

namespace Internal
{

typedef Dali::Vector<Object*>         ObjectContainer;
typedef ObjectContainer::Iterator     ObjectIter;

/**
 * An abstract base class for constrainers.
 * Constrainer base class is responsible or observing constrained objects and remove all the constraints created
 * when it is destroyed
 */
class Constrainer : public Object, public Object::Observer
{
public:

  /**
   * Constructor.
   */
  Constrainer();

  /**
   * Virtual destructor.
   */
  virtual ~Constrainer();

public: // Object methods

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyCount()
   */
  virtual unsigned int GetDefaultPropertyCount() const{return 0;}

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyIndices()
   */
  virtual void GetDefaultPropertyIndices( Property::IndexContainer& indices ) const{}

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyName()
   */
  virtual const char* GetDefaultPropertyName( Property::Index index ) const{return 0;}

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyIndex()
   */
  virtual Property::Index GetDefaultPropertyIndex( const std::string& name ) const{return  Property::INVALID_INDEX;}

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyWritable()
   */
  virtual bool IsDefaultPropertyWritable( Property::Index index ) const{return false;}

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyAnimatable()
   */
  virtual bool IsDefaultPropertyAnimatable( Property::Index index ) const{return false;}

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyAConstraintInput()
   */
  virtual bool IsDefaultPropertyAConstraintInput( Property::Index index ) const{return false;}

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyType()
   */
  virtual Property::Type GetDefaultPropertyType( Property::Index index ) const{return Property::NONE;}

  /**
   * @copydoc Dali::Internal::Object::SetDefaultProperty()
   */
  virtual void SetDefaultProperty( Property::Index index, const Property::Value& propertyValue ){}

  /**
   * @copydoc Dali::Internal::Object::GetDefaultProperty()
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const{return Property::Value();}

  /**
   * @copydoc Dali::Internal::Object::GetSceneObject()
   */
  virtual const SceneGraph::PropertyOwner* GetSceneObject() const{return 0;}

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectAnimatableProperty()
   */
  virtual const SceneGraph::PropertyBase* GetSceneObjectAnimatableProperty( Property::Index index ) const{return 0;}

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectInputProperty()
   */
  virtual const PropertyInputImpl* GetSceneObjectInputProperty( Property::Index index ) const{return 0;}

public: // Object::Observer methods

  /**
   * @copydoc Object::Observer::SceneObjectAdded()
   */
  virtual void SceneObjectAdded( Object& object ){}

  /**
   * @copydoc Object::Observer::SceneObjectRemoved()
   */
  virtual void SceneObjectRemoved( Object& object ){}

  /**
   * @copydoc Object::Observer::ObjectDestroyed()
   */
  virtual void ObjectDestroyed( Object& object );

public:

  /**
   * @brief Applies the constraint to the target property

   * @param[in] target Property to be constrained
   * @param[in] source Property used as parameter for the path
   * @param[in] range The range of values in the source property which will be mapped to [0,1]
   * @param[in] wrap Wrapping domain. Source property will be wrapped in the domain [wrap.x,wrap.y] before mapping to [0,1]
   */
  virtual void Apply( Property target, Property source, const Vector2& range, const Vector2& wrap) = 0;

  /**
   * @brief Removes the constraint in the target object
   *
   * @param[in] target A handle to an object constrained by the Constrainer
   */
  void Remove( Dali::Handle& target );

protected:

  /**
   * @brief Adds an object to the list of observed objects
   *
   * @param[in] handle A handle to the object to be observed
   */
  void Observe( Dali::Handle& handle );

private:

  ObjectContainer   mObservedObjects;   ///< The list of object which have been constrained by the Constrainer
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_CONSTRAINER_H__

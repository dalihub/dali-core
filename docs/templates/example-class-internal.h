#ifndef __DALI_INTERNAL_EXAMPLE_CLASS_H__
#define __DALI_INTERNAL_EXAMPLE_CLASS_H__

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

// EXTERNAL INCLUDES
#include <list>
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/core/actors/actor.h>

namespace Dali
{

// Forward declarations for the Dali namespace
class XXXX;

namespace Internal
{

// Forward declarations for the Dali::Internal namespace
class YYYY;

/**
 * TODO - Rewrite this comment, to explain the purpose of your class
 */
class ExampleClass : public ExampleBaseClass
{
public: // Types

  typedef std::list<XXXX> ExampleContainer;

  enum ExampleEnum
  {
    FirstValue  = 0, ///< First value description
    SecondValue = 1  ///< Second value description
  };

public: // Construction

  /**
   * Create an ExampleClass.
   */
  ExampleClass();

  /**
   * Create an ExampleClass with a name.
   * @param[in] The name of the example class.
   */
  ExampleClass(const std::string& name);

  /**
   * Virtual destructor; ExampleClass is intended as a base class.
   */
  virtual ~ExampleClass();

public: // Public members

  /**
   * Set the name of an ExampleClass.
   * @pre TODO - Edit this example comment.
   * @param[in] The new name.
   * @post TODO - Edit this example comment.
   */
  void SetName();

  /**
   * Retreive the name of an ExampleClass.
   * @pre TODO - Edit this example comment.
   * @return The name of the example class.
   * @post TODO - Edit this example comment.
   */
  std::string GetName() const { return mName; }

protected: // Protected members

  /**
   * Example helper method, which is accessible by derived classes.
   * @param [in] exampleParam An an example parameter.
   */
  void HelperMember(ExampleEnum exampleParam);

private: // Private members

  /**
   * Undefined copy constructor; declaring this prevents accidental copying.
   * @param[in] A reference to the ExampleClass to copy.
   */
  ExampleClass(const ExampleClass&);

  /**
   * Undefined assignment operator; declaring this prevents accidental copying.
   * @param[in] A reference to the ExampleClass to copy.
   * @return A reference to the copied ExampleClass.
   */
  ExampleClass& operator=(const ExampleClass& rhs);

  /**
   * Example pure virtual member.
   * The intention here is to avoid making the public SetName() virtual.
   * When the name changes, derived classes receive this notification.
   */
  virtual void OnNameSet() = 0;

protected: // Protected data

  ExampleContainer mContainer;

private: // Private data

  std::string mName;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::ExampleClass& GetImplementation(Dali::ExampleClass& example)
{
  DALI_ASSERT(example);

  BaseObject& handle = example.GetBaseObject();

  return static_cast<Internal::ExampleClass&>(handle);
}

inline const Internal::ExampleClass& GetImplementation(const Dali::ExampleClass& example)
{
  DALI_ASSERT(example);

  const BaseObject& handle = example.GetBaseObject();

  return static_cast<const Internal::ExampleClass&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_EXAMPLE_CLASS_H__


#ifndef DALI_INTERNAL_SHADER_H
#define DALI_INTERNAL_SHADER_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-common.h> // DALI_ASSERT_ALWAYS
#include <dali/public-api/common/intrusive-ptr.h> // Dali::IntrusivePtr
#include <dali/public-api/rendering/shader.h> // Dali::Shader
#include <dali/devel-api/rendering/shader-devel.h> // Dali::Shader
#include <dali/internal/event/common/object-connector.h> // Dali::Internal::ObjectConnector
#include <dali/internal/event/common/object-impl.h> // Dali::Internal::Object
#include <dali/internal/common/shader-data.h> // ShaderPtr

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class Shader;
}

class Shader;
typedef IntrusivePtr<Shader> ShaderPtr;

/**
 * Shader is an object that contains an array of structures of values that
 * can be accessed as properties.
 */
class Shader : public Object
{
public:

  /**
   * @copydoc Dali::Shader::New()
   */
  static ShaderPtr New( const std::string& vertexShader,
                        const std::string& fragmentShader,
                        Dali::Shader::Hint::Value hints );

  /**
   * Create a new shader object
   *
   * @param[in] vertexShader
   * @param[in] fragmentShader
   * @param[in] language
   * @param[in] specializationConstants
   * @param[in] hints Hints to define the geometry of the rendered object
   * @return
   */
  static ShaderPtr New( std::vector<char>& vertexShader,
                        std::vector<char>& fragmentShader,
                        DevelShader::ShaderLanguage language,
                        const Property::Map& specializationConstants,
                        Dali::Shader::Hint::Value hints );

  /**
   * @brief Get the shader scene object
   *
   * @return the shader scene object
   */
  const SceneGraph::Shader* GetShaderSceneObject() const;

  /**
   * Retrieve the scene-graph shader added by this object.
   * @return A pointer to the shader.
   */
  SceneGraph::Shader* GetShaderSceneObject();

public: // Default property extensions from Object

  /**
   * @copydoc Dali::Internal::Object::SetDefaultProperty()
   */
  virtual void SetDefaultProperty(Property::Index index, const Property::Value& propertyValue);

  /**
   * @copydoc Dali::Internal::Object::SetSceneGraphProperty()
   */
  virtual void SetSceneGraphProperty( Property::Index index, const PropertyMetadata& entry, const Property::Value& value );

  /**
   * @copydoc Dali::Internal::Object::GetDefaultProperty()
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyCurrentValue()
   */
  virtual Property::Value GetDefaultPropertyCurrentValue( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetPropertyOwner()
   */
  virtual const SceneGraph::PropertyOwner* GetPropertyOwner() const;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObject()
   */
  virtual const SceneGraph::PropertyOwner* GetSceneObject() const;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectAnimatableProperty()
   */
  virtual const SceneGraph::PropertyBase* GetSceneObjectAnimatableProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectInputProperty()
   */
  virtual const PropertyInputImpl* GetSceneObjectInputProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetPropertyComponentIndex()
   */
  virtual int GetPropertyComponentIndex( Property::Index index ) const;

private: // implementation
  Shader();

  /**
   * Second stage initialization
   */
  void Initialize( const std::string& vertexShader, const std::string& fragmentShader, Dali::Shader::Hint::Value hints );

  /**
   *
   * Second stage initialization, devel initialiser
   *
   *
   * @param vertexShader
   * @param fragmentShader
   * @param language
   * @param specializationConstants
   * @param[in] hints Hints to define the geometry of the rendered object
   */
  void Initialize( std::vector<char>& vertexShader,
                   std::vector<char>& fragmentShader,
                   DevelShader::ShaderLanguage language,
                   const Property::Map& specializationConstants,
                   Dali::Shader::Hint::Value hints );

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Shader();

private: // unimplemented methods
  Shader( const Shader& );
  Shader& operator=( const Shader& );

private:
  SceneGraph::Shader* mSceneObject;
  Internal::ShaderDataPtr mShaderData;
};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Internal::Shader& GetImplementation( Dali::Shader& handle )
{
  DALI_ASSERT_ALWAYS(handle && "Shader handle is empty");

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::Shader&>(object);
}

inline const Internal::Shader& GetImplementation( const Dali::Shader& handle )
{
  DALI_ASSERT_ALWAYS(handle && "Shader handle is empty");

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::Shader&>(object);
}

} // namespace Dali

#endif // DALI_INTERNAL_SHADER_H

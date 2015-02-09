#ifndef __DALI_INTERNAL_SHADER_EFFECT_H__
#define __DALI_INTERNAL_SHADER_EFFECT_H__

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
#include <dali/public-api/images/image.h>
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/internal/event/effects/shader-declarations.h>
#include <dali/internal/event/common/proxy-object.h>
#include <dali/internal/event/resources/resource-ticket.h>
#include <dali/internal/render/shaders/shader.h>


namespace Dali
{

namespace Internal
{

namespace SceneGraph
{
class UpdateManager;
}

/**
 * An abstract base class for a shader effect proxy.
 * The corresponding scene-graph object is a collection of shader programs,
 * which can apply the same effect to different geometry types.
 */
class ShaderEffect : public ProxyObject
{
public:
  typedef Dali::ShaderEffect::UniformCoordinateType UniformCoordinateType;

  enum GeometryState
  {
    DOESNT_MODIFY_GEOMETRY,
    MODIFIES_GEOMETRY
  };

  /**
   * Create a new ShaderEffect with no programs
   * @param hints GeometryHints to define the geometry of the rendered object
   * @return A smart-pointer to a newly allocated shader effect.
   */
  static ShaderEffectPtr New( Dali::ShaderEffect::GeometryHints hints = Dali::ShaderEffect::HINT_NONE );

  /**
   * @copydoc Dali::ShaderEffect::SetEffectImage
   */
  void SetEffectImage( Dali::Image image );

  /**
   * @copydoc Dali::ShaderEffect::SetUniform( const std::string& name, Property::Value value, UniformCoordinateType uniformCoordinateType )
   */
  void SetUniform( const std::string& name,
                   Property::Value value,
                   UniformCoordinateType uniformCoordinateType );

  /**
   * @copydoc Dali::ShaderEffect::AttachExtension()
   */
  void AttachExtension( Dali::ShaderEffect::Extension *object );

  /**
   * @copydoc Dali::ShaderEffect::GetExtension()
   */
  Dali::ShaderEffect::Extension& GetExtension();

  /**
   * @copydoc Dali::ShaderEffect::GetExtension() const
   */
  const Dali::ShaderEffect::Extension& GetExtension() const;

  /**
   * Add a GeometryType specific default program to this ShaderEffect
   * @param[in] geometryType    The GeometryType rendered by the shader program
   * @param[in] vertexSource    The source code for the vertex shader
   * @param[in] fragmentSource  The source code for the fragment shader
   */
  void SetPrograms( GeometryType geometryType, const std::string& vertexSource, const std::string& fragmentSource );

  /**
   * Add a GeometryType specific default program to this ShaderEffect.
   * This overload allows the optional prefixing for both the vertex and fragment shader.
   * A useful prefix may be shader \#defines for conditional compilation.
   * @param[in] geometryType    The GeometryType rendered by the shader program
   * @param[in] vertexPrefix    The prefix source code for the vertex shader
   * @param[in] fragmentPrefix  The prefix source code for the fragment shader
   * @param[in] vertexSource    The source code for the vertex shader
   * @param[in] fragmentSource  The source code for the fragment shader
   */
  void SetPrograms( GeometryType geometryType,
                    const std::string& vertexPrefix, const std::string& fragmentPrefix,
                    const std::string& vertexSource, const std::string& fragmentSource );

  /**
   * Send shader program to scene-graph object.
   * @param[in] geometryType     The GeometryType rendered by the shader program
   * @param[in] subType          The subtype, one of ShaderSubTypes.
   * @param[in] vertexSource     The source code for the vertex shader
   * @param[in] fragmentSource   The source code for the fragment shader
   * @param[in] modifiesGeometry True if the shader modifies geometry
   */
  void SendProgramMessage( GeometryType geometryType, ShaderSubTypes subType,
                           const std::string& vertexSource, const std::string& fragmentSource,
                           bool modifiesGeometry );

  /**
   * Notify ShaderEffect that it's being used by an Actor.
   */
  void Connect();

  /**
   * Notify ShaderEffect that an Actor is no longer using it.
   */
  void Disconnect();

public: // Default property extensions from ProxyObject

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
  virtual const char* GetDefaultPropertyName( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyIndex()
   */
  virtual Property::Index GetDefaultPropertyIndex( const std::string& name ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::IsDefaultPropertyWritable()
   */
  virtual bool IsDefaultPropertyWritable( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::IsDefaultPropertyAnimatable()
   */
  virtual bool IsDefaultPropertyAnimatable( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::IsDefaultPropertyAConstraintInput()
   */
  virtual bool IsDefaultPropertyAConstraintInput( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyType()
   */
  virtual Property::Type GetDefaultPropertyType( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::SetDefaultProperty()
   */
  virtual void SetDefaultProperty( Property::Index index, const Property::Value& propertyValue );

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultProperty()
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::NotifyScenePropertyInstalled()
   */
  virtual void NotifyScenePropertyInstalled( const SceneGraph::PropertyBase& newProperty, const std::string& name, unsigned int index );

  /**
   * @copydoc Dali::Internal::ProxyObject::GetSceneObject()
   */
  virtual const SceneGraph::PropertyOwner* GetSceneObject() const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetSceneObjectAnimatableProperty()
   */
  virtual const SceneGraph::PropertyBase* GetSceneObjectAnimatableProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetSceneObjectInputProperty()
   */
  virtual const PropertyInputImpl* GetSceneObjectInputProperty( Property::Index index ) const;

protected:

  /**
   * Protected constructor.
   */
  ShaderEffect( SceneGraph::UpdateManager& updateManager, Dali::ShaderEffect::GeometryHints hints );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ShaderEffect();

private:

  // No copying allowed, thus these are undefined
  ShaderEffect( const ShaderEffect& );
  ShaderEffect& operator=( const ShaderEffect& rhs );

private: // Data

  SceneGraph::UpdateManager& mUpdateManager;///< reference to the update manager
  SceneGraph::Shader* mSceneObject;         ///< pointer to the scene shader, should not be changed on this thread
  Dali::Image mImage;                       ///< Client-side handle for the effect image
  IntrusivePtr<Dali::ShaderEffect::Extension> mExtension;
  std::vector<ResourceTicketPtr>  mTickets; ///< Collection of shader program tickets
  unsigned int  mConnectionCount;           ///< number of on-stage ImageActors using this shader effect
  Dali::ShaderEffect::GeometryHints  mGeometryHints; ///< shader geometry hints for building the geometry
  Dali::Vector< UniformCoordinateType > mCoordinateTypes; ///< cached to avoid sending tons of unnecessary messages

};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::ShaderEffect& GetImplementation(Dali::ShaderEffect& effect)
{
  DALI_ASSERT_ALWAYS(effect && "ShaderEffect handle is empty");

  BaseObject& handle = effect.GetBaseObject();

  return static_cast<Internal::ShaderEffect&>(handle);
}

inline const Internal::ShaderEffect& GetImplementation(const Dali::ShaderEffect& effect)
{
  DALI_ASSERT_ALWAYS(effect && "ShaderEffect handle is empty");

  const BaseObject& handle = effect.GetBaseObject();

  return static_cast<const Internal::ShaderEffect&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_SHADER_EFFECT_H__

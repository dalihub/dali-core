#ifndef DALI_INTERNAL_SCENE_GRAPH_NODE_MESSAGES_H
#define DALI_INTERNAL_SCENE_GRAPH_NODE_MESSAGES_H

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
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/message.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/manager/update-manager.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

// Messages for Node

class NodePropertyMessageBase : public MessageBase
{
public:

  /**
   * Create a message.
   */
  NodePropertyMessageBase(UpdateManager& updateManager);

  /**
   * Virtual destructor
   */
  ~NodePropertyMessageBase() override;

private:

  // Undefined
  NodePropertyMessageBase(const NodePropertyMessageBase&);
  NodePropertyMessageBase& operator=(const NodePropertyMessageBase& rhs);

protected:

  UpdateManager& mUpdateManager;
};

/**
 * Templated message which bakes a Node property.
 */
template< typename P >
class NodePropertyMessage : public NodePropertyMessageBase
{
public:
  using MemberFunction = void ( AnimatableProperty<P>::* )( BufferIndex, typename ParameterType<P>::PassingType );

  /**
   * Create a message.
   * @note The node is expected to be const in the thread which sends this message.
   * However it can be modified when Process() is called in a different thread.
   * @param[in] eventThreadServices The object used to send messages to the scene graph
   * @param[in] node The node.
   * @param[in] property The property to bake.
   * @param[in] member The member function of the object.
   * @param[in] value The new value of the property.
   */
  static void Send( EventThreadServices& eventThreadServices,
                    const Node* node,
                    const AnimatableProperty<P>* property,
                    MemberFunction member,
                    typename ParameterType< P >::PassingType value )
  {
    // Reserve some memory inside the message queue
    uint32_t* slot = eventThreadServices.ReserveMessageSlot( sizeof( NodePropertyMessage ) );

    // Construct message in the message queue memory; note that delete should not be called on the return value
    new (slot) NodePropertyMessage( eventThreadServices.GetUpdateManager(), node, property, member, value );
  }

  /**
   * Virtual destructor
   */
  ~NodePropertyMessage() override = default;

  /**
   * @copydoc MessageBase::Process
   */
  void Process( BufferIndex updateBufferIndex ) override
  {
    (mProperty->*mMemberFunction)( updateBufferIndex, mParam );
  }

private:

  /**
   * Create a message.
   * @note The node is expected to be const in the thread which sends this message.
   * However it can be modified when Process() is called in a different thread.
   * @param[in] updateManager The update-manager.
   * @param[in] node The node.
   * @param[in] property The property to bake.
   * @param[in] member The member function of the object.
   * @param[in] value The new value of the property.
   */
  NodePropertyMessage( UpdateManager& updateManager,
                       const Node* node,
                       const AnimatableProperty<P>* property,
                       MemberFunction member,
                       typename ParameterType< P >::PassingType value )
  : NodePropertyMessageBase( updateManager ),
    mNode( const_cast< Node* >( node ) ),
    mProperty( const_cast< AnimatableProperty<P>* >( property ) ),
    mMemberFunction( member ),
    mParam( value )
  {
  }

private:

  Node* mNode;
  AnimatableProperty<P>* mProperty;
  MemberFunction mMemberFunction;
  typename ParameterType< P >::HolderType mParam;
};

/**
 * Templated message which bakes a Node property.
 */
template< typename P >
class NodePropertyComponentMessage : public NodePropertyMessageBase
{
public:
  using MemberFunction = void ( AnimatableProperty<P>::* )( BufferIndex, float );

  /**
   * Send a message.
   * @note The node is expected to be const in the thread which sends this message.
   * However it can be modified when Process() is called in a different thread.
   * @param[in] eventThreadServices The object used to send messages to the scene graph
   * @param[in] node The node.
   * @param[in] property The property to bake.
   * @param[in] member The member function of the object.
   * @param[in] value The new value of the X,Y,Z or W component.
   */
  static void Send( EventThreadServices& eventThreadServices,
                    const Node* node,
                    const AnimatableProperty<P>* property,
                    MemberFunction member,
                    float value )
  {
    // Reserve some memory inside the message queue
    uint32_t* slot = eventThreadServices.ReserveMessageSlot( sizeof( NodePropertyComponentMessage ) );

    // Construct message in the message queue memory; note that delete should not be called on the return value
    new (slot) NodePropertyComponentMessage( eventThreadServices.GetUpdateManager(), node, property, member, value );
  }

  /**
   * Virtual destructor
   */
  ~NodePropertyComponentMessage() override = default;

  /**
   * @copydoc MessageBase::Process
   */
  void Process( BufferIndex updateBufferIndex ) override
  {
    (mProperty->*mMemberFunction)( updateBufferIndex, mParam );
  }

private:

  /**
   * Create a message.
   * @note The node is expected to be const in the thread which sends this message.
   * However it can be modified when Process() is called in a different thread.
   * @param[in] updateManager The update-manager.
   * @param[in] node The node.
   * @param[in] property The property to bake.
   * @param[in] member The member function of the object.
   * @param[in] value The new value of the X,Y,Z or W component.
  */
  NodePropertyComponentMessage( UpdateManager& updateManager,
                                const Node* node,
                                const AnimatableProperty<P>* property,
                                MemberFunction member,
                                float value )
  : NodePropertyMessageBase( updateManager ),
    mNode( const_cast< Node* >( node ) ),
    mProperty( const_cast< AnimatableProperty<P>* >( property ) ),
    mMemberFunction( member ),
    mParam( value )
  {
  }

private:

  Node* mNode;
  AnimatableProperty<P>* mProperty;
  MemberFunction mMemberFunction;
  float mParam;
};


template <typename P>
class NodeTransformPropertyMessage : public NodePropertyMessageBase
{
public:
  using MemberFunction = void ( TransformManagerPropertyHandler<P>::* )( BufferIndex, const P& );

  /**
   * Create a message.
   * @note The node is expected to be const in the thread which sends this message.
   * However it can be modified when Process() is called in a different thread.
   * @param[in] eventThreadServices The object used to send messages to the scene graph
   * @param[in] node The node.
   * @param[in] property The property to bake.
   * @param[in] member The member function of the object.
   * @param[in] value The new value of the property.
   */
  static void Send( EventThreadServices& eventThreadServices,
                    const Node* node,
                    const TransformManagerPropertyHandler<P>* property,
                    MemberFunction member,
                    const P& value )
  {
    // Reserve some memory inside the message queue
    uint32_t* slot = eventThreadServices.ReserveMessageSlot( sizeof( NodeTransformPropertyMessage ) );

    // Construct message in the message queue memory; note that delete should not be called on the return value
    new (slot) NodeTransformPropertyMessage( eventThreadServices.GetUpdateManager(), node, property, member, value );
  }

  /**
   * Virtual destructor
   */
  ~NodeTransformPropertyMessage() override = default;

  /**
   * @copydoc MessageBase::Process
   */
  void Process( BufferIndex updateBufferIndex ) override
  {
    (mProperty->*mMemberFunction)( updateBufferIndex, mParam );
  }

private:

  /**
   * Create a message.
   * @note The node is expected to be const in the thread which sends this message.
   * However it can be modified when Process() is called in a different thread.
   * @param[in] updateManager The update-manager.
   * @param[in] node The node.
   * @param[in] property The property to bake.
   * @param[in] member The member function of the object.
   * @param[in] value The new value of the property.
   */
  NodeTransformPropertyMessage( UpdateManager& updateManager,
                       const Node* node,
                       const TransformManagerPropertyHandler<P>* property,
                       MemberFunction member,
                       const P& value )
  : NodePropertyMessageBase( updateManager ),
    mNode( const_cast< Node* >( node ) ),
    mProperty( const_cast< TransformManagerPropertyHandler<P>* >( property ) ),
    mMemberFunction( member ),
    mParam( value )
  {
  }

private:

  Node* mNode;
  TransformManagerPropertyHandler<P>* mProperty;
  MemberFunction mMemberFunction;
  P mParam;
};


template <typename P>
class NodeTransformComponentMessage : public NodePropertyMessageBase
{
public:
  using MemberFunction = void ( TransformManagerPropertyHandler<P>::* )( BufferIndex, float );

  /**
   * Send a message.
   * @note The node is expected to be const in the thread which sends this message.
   * However it can be modified when Process() is called in a different thread.
   * @param[in] eventThreadServices The object used to send messages to the scene graph
   * @param[in] node The node.
   * @param[in] property The property to bake.
   * @param[in] member The member function of the object.
   * @param[in] value The new value of the X,Y,Z or W component.
   */
  static void Send( EventThreadServices& eventThreadServices,
                    const Node* node,
                    const TransformManagerPropertyHandler<P>* property,
                    MemberFunction member,
                    float value )
  {
    // Reserve some memory inside the message queue
    uint32_t* slot = eventThreadServices.ReserveMessageSlot( sizeof( NodeTransformComponentMessage ) );

    // Construct message in the message queue memory; note that delete should not be called on the return value
    new (slot) NodeTransformComponentMessage( eventThreadServices.GetUpdateManager(), node, property, member, value );
  }

  /**
   * Virtual destructor
   */
  ~NodeTransformComponentMessage() override = default;

  /**
   * @copydoc MessageBase::Process
   */
  void Process( BufferIndex updateBufferIndex ) override
  {
    (mProperty->*mMemberFunction)( updateBufferIndex, mParam );
  }

private:

  /**
   * Create a message.
   * @note The node is expected to be const in the thread which sends this message.
   * However it can be modified when Process() is called in a different thread.
   * @param[in] updateManager The update-manager.
   * @param[in] node The node.
   * @param[in] property The property to bake.
   * @param[in] member The member function of the object.
   * @param[in] value The new value of the X,Y,Z or W component.
  */
  NodeTransformComponentMessage( UpdateManager& updateManager,
                                const Node* node,
                                const TransformManagerPropertyHandler<P>* property,
                                MemberFunction member,
                                float value )
  : NodePropertyMessageBase( updateManager ),
    mNode( const_cast< Node* >( node ) ),
    mProperty( const_cast< TransformManagerPropertyHandler<P>* >( property ) ),
    mMemberFunction( member ),
    mParam( value )
  {
  }

private:

  Node* mNode;
  TransformManagerPropertyHandler<P>* mProperty;
  MemberFunction mMemberFunction;
  float mParam;
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_NODE_MESSAGES_H

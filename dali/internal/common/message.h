#ifndef __DALI_INTERNAL_MESSAGE_H__
#define __DALI_INTERNAL_MESSAGE_H__

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
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/type-abstraction.h>
#include <dali/internal/update/common/scene-graph-buffers.h>

namespace Dali
{

namespace Internal
{

/**
 * An abstract base class for messages queued across threads.
 * Messages are only allowed to contain value objects, either copies of the parameters or pointers
 * If message parameter type is & or const& the message will try to take a copy of the actual type
 */
class MessageBase
{
public:

  /**
   * Construct the message base.
   */
  MessageBase( )
  {
  }

  /**
   * Virtual destructor
   */
  virtual ~MessageBase()
  {
  }

  /**
   * Called to process the message.
   * @param [in] bufferIndex The current update/render buffer index (depending on which thread processes the message).
   */
  virtual void Process( BufferIndex bufferIndex ) = 0;

private:
};

/**
 * Templated message which calls a member function of an object.
 * This allows nodes etc. to be modified in a thread-safe manner, when the update occurs in a separate thread.
 * The object lifetime must controlled i.e. not destroyed before the message is processed.
 */
template< typename T >
class Message : public MessageBase
{
public:

  typedef void(T::*MemberFunction)();

  /**
   * Create a message.
   * @note The object is expected to be const in the thread which sends this message.
   * However it can be modified when Process() is called in a different thread.
   * @param[in] obj The object to be updated in a separate thread.
   * @param[in] member The member function of the object.
   */
  Message( const T* obj, MemberFunction member )
  : MessageBase(),
    object( const_cast< T* >( obj ) ),
    memberFunction( member )
  {
  }

  /**
   * Virtual destructor
   */
  virtual ~Message()
  {
  }

  /**
   * @copydoc MessageBase::Process
   */
  virtual void Process( BufferIndex /*bufferIndex*/ )
  {
    DALI_ASSERT_DEBUG( object && "Message does not have an object" );
    (object->*memberFunction)();
  }

private:

  T* object;
  MemberFunction memberFunction;

};

/**
 * Templated message which calls a member function of an object.
 * This overload passes one value-type parameter.
 * Template parameters need to match the MemberFunction!
 * The message will contain copy of the value (in case of & or const&)
 */
template< typename T, typename P >
class MessageValue1 : public MessageBase
{
public:

  typedef void(T::*MemberFunction)( typename ParameterType< P >::PassingType );

  /**
   * Create a message.
   * @note The object is expected to be const in the thread which sends this message.
   * However it can be modified when Process() is called in a different thread.
   * @param[in] obj The object.
   * @param[in] member The member function of the object.
   * @param[in] p1 The first value-type parameter to pass to the member function.
   */
  MessageValue1( const T* obj,
                 MemberFunction member,
                 typename ParameterType< P >::PassingType p1 )
  : MessageBase(),
    object( const_cast< T* >( obj ) ),
    memberFunction( member ),
    param1( p1 )
  {
  }

  /**
   * Virtual destructor
   */
  virtual ~MessageValue1()
  {
  }

  /**
   * @copydoc MessageBase::Process
   */
  virtual void Process( BufferIndex /*bufferIndex*/ )
  {
    DALI_ASSERT_DEBUG( object && "Message does not have an object" );
    (object->*memberFunction)( ParameterType< P >::PassObject( param1 ) );
  }

private:

  T* object;
  MemberFunction memberFunction;
  typename ParameterType< P >::HolderType param1;

};

/**
 * Templated message which calls a member function of an object.
 * This overload passes two value-type parameters.
 * Template parameters need to match the MemberFunction!
 * The message will contain copy of the value (in case of & or const&)
 */

template< typename T, typename P1, typename P2 >
class MessageValue2 : public MessageBase
{
public:

  typedef void(T::*MemberFunction)(
      typename ParameterType< P1 >::PassingType,
      typename ParameterType< P2 >::PassingType );

  /**
   * Create a message.
   * @note The object is expected to be const in the thread which sends this message.
   * However it can be modified when Process() is called in a different thread.
   * @param[in] obj The object.
   * @param[in] member The member function of the object.
   * @param[in] p1 The first parameter to pass to the member function.
   * @param[in] p2 The second parameter to pass to the member function.
   */
  MessageValue2( const T* obj,
                 MemberFunction member,
                 typename ParameterType< P1 >::PassingType p1,
                 typename ParameterType< P2 >::PassingType p2 )
  : MessageBase(),
    object( const_cast< T* >( obj ) ),
    memberFunction( member ),
    param1( p1 ),
    param2( p2 )
  {
  }

  /**
   * Virtual destructor
   */
  virtual ~MessageValue2()
  {
  }

  /**
   * @copydoc MessageBase::Process
   */
  virtual void Process( BufferIndex /*bufferIndex*/ )
  {
    DALI_ASSERT_DEBUG( object && "Message does not have an object" );
    (object->*memberFunction)(
        ParameterType< P1 >::PassObject( param1 ),
        ParameterType< P2 >::PassObject( param2 ) );
  }

private:

  T* object;
  MemberFunction memberFunction;
  typename ParameterType< P1 >::HolderType param1;
  typename ParameterType< P2 >::HolderType param2;

};

/**
 * Templated message which calls a member function of an object.
 * This overload passes three value-type parameters.
 * Template parameters need to match the MemberFunction!
 * The message will contain copy of the value (in case of & or const&)
 */
template< typename T, typename P1, typename P2, typename P3 >
class MessageValue3 : public MessageBase
{
public:

  typedef void(T::*MemberFunction)(
      typename ParameterType< P1 >::PassingType,
      typename ParameterType< P2 >::PassingType,
      typename ParameterType< P3 >::PassingType );

  /**
   * Create a message.
   * @note The object is expected to be const in the thread which sends this message.
   * However it can be modified when Process() is called in a different thread.
   * @param[in] obj The object.
   * @param[in] member The member function of the object.
   * @param[in] p1 The first parameter to pass to the member function.
   * @param[in] p2 The second parameter to pass to the member function.
   * @param[in] p3 The third parameter to pass to the member function.
   */
  MessageValue3( const T* obj,
                 MemberFunction member,
                 typename ParameterType< P1 >::PassingType p1,
                 typename ParameterType< P2 >::PassingType p2,
                 typename ParameterType< P3 >::PassingType p3 )
  : MessageBase(),
    object( const_cast< T* >( obj ) ),
    memberFunction( member ),
    param1( p1 ),
    param2( p2 ),
    param3( p3 )
  {
  }

  /**
   * Virtual destructor
   */
  virtual ~MessageValue3()
  {
  }

  /**
   * @copydoc MessageBase::Process
   */
  virtual void Process( BufferIndex /*bufferIndex*/ )
  {
    DALI_ASSERT_DEBUG( object && "Message does not have an object" );
    (object->*memberFunction)(
        ParameterType< P1 >::PassObject( param1 ),
        ParameterType< P2 >::PassObject( param2 ),
        ParameterType< P3 >::PassObject( param3 ) );
  }

private:

  T* object;
  MemberFunction memberFunction;
  typename ParameterType< P1 >::HolderType param1;
  typename ParameterType< P2 >::HolderType param2;
  typename ParameterType< P3 >::HolderType param3;

};

/**
 * Templated message which calls a member function of an object.
 * This overload passes four value-type parameters.
 * Template parameters need to match the MemberFunction!
 * The message will contain copy of the value (in case of & or const&)
 */
template< typename T, typename P1, typename P2, typename P3, typename P4 >
class MessageValue4 : public MessageBase
{
public:

  typedef void(T::*MemberFunction)(
      typename ParameterType< P1 >::PassingType,
      typename ParameterType< P2 >::PassingType,
      typename ParameterType< P3 >::PassingType,
      typename ParameterType< P4 >::PassingType );

  /**
   * Create a message.
   * @note The object is expected to be const in the thread which sends this message.
   * However it can be modified when Process() is called in a different thread.
   * @param[in] obj The object.
   * @param[in] member The member function of the object.
   * @param[in] p1 The first parameter to pass to the member function.
   * @param[in] p2 The second parameter to pass to the member function.
   * @param[in] p3 The third parameter to pass to the member function.
   * @param[in] p4 The fourth parameter to pass to the member function.
   */
  MessageValue4( const T* obj,
                 MemberFunction member,
                 typename ParameterType< P1 >::PassingType p1,
                 typename ParameterType< P2 >::PassingType p2,
                 typename ParameterType< P3 >::PassingType p3,
                 typename ParameterType< P4 >::PassingType p4 )
  : MessageBase(),
    object( const_cast< T* >( obj ) ),
    memberFunction( member ),
    param1( p1 ),
    param2( p2 ),
    param3( p3 ),
    param4( p4 )
  {
  }

  /**
   * Virtual destructor
   */
  virtual ~MessageValue4()
  {
  }

  /**
   * @copydoc MessageBase::Process
   */
  virtual void Process( BufferIndex /*bufferIndex*/ )
  {
    DALI_ASSERT_DEBUG( object && "Message does not have an object" );
    (object->*memberFunction)(
        ParameterType< P1 >::PassObject( param1 ),
        ParameterType< P2 >::PassObject( param2 ),
        ParameterType< P3 >::PassObject( param3 ),
        ParameterType< P4 >::PassObject( param4 ) );
  }

private:

  T* object;
  MemberFunction memberFunction;
  typename ParameterType< P1 >::HolderType param1;
  typename ParameterType< P2 >::HolderType param2;
  typename ParameterType< P3 >::HolderType param3;
  typename ParameterType< P4 >::HolderType param4;

};

/**
 * Templated message which calls a member function of an object.
 * This overload passes five value-type parameters.
 * Template parameters need to match the MemberFunction!
 * The message will contain copy of the value (in case of & or const&)
 */
template< typename T, typename P1, typename P2, typename P3, typename P4, typename P5 >
class MessageValue5 : public MessageBase
{
public:

  typedef void(T::*MemberFunction)(
      typename ParameterType< P1 >::PassingType,
      typename ParameterType< P2 >::PassingType,
      typename ParameterType< P3 >::PassingType,
      typename ParameterType< P4 >::PassingType,
      typename ParameterType< P5 >::PassingType );

  /**
   * Create a message.
   * @note The object is expected to be const in the thread which sends this message.
   * However it can be modified when Process() is called in a different thread.
   * @param[in] obj The object.
   * @param[in] member The member function of the object.
   * @param[in] p1 The first parameter to pass to the member function.
   * @param[in] p2 The second parameter to pass to the member function.
   * @param[in] p3 The third parameter to pass to the member function.
   * @param[in] p4 The fourth parameter to pass to the member function.
   * @param[in] p5 The fifth parameter to pass to the member function.
   */
  MessageValue5( const T* obj,
                 MemberFunction member,
                 typename ParameterType< P1 >::PassingType p1,
                 typename ParameterType< P2 >::PassingType p2,
                 typename ParameterType< P3 >::PassingType p3,
                 typename ParameterType< P4 >::PassingType p4,
                 typename ParameterType< P5 >::PassingType p5 )
  : MessageBase(),
    object( const_cast< T* >( obj ) ),
    memberFunction( member ),
    param1( p1 ),
    param2( p2 ),
    param3( p3 ),
    param4( p4 ),
    param5( p5 )
  {
  }

  /**
   * Virtual destructor
   */
  virtual ~MessageValue5()
  {
  }

  /**
   * @copydoc MessageBase::Process
   */
  virtual void Process( BufferIndex /*bufferIndex*/ )
  {
    DALI_ASSERT_DEBUG( object && "Message does not have an object" );
    (object->*memberFunction)(
        ParameterType< P1 >::PassObject( param1 ),
        ParameterType< P2 >::PassObject( param2 ),
        ParameterType< P3 >::PassObject( param3 ),
        ParameterType< P4 >::PassObject( param4 ),
        ParameterType< P5 >::PassObject( param5 ) );

  }

private:

  T* object;
  MemberFunction memberFunction;
  typename ParameterType< P1 >::HolderType param1;
  typename ParameterType< P2 >::HolderType param2;
  typename ParameterType< P3 >::HolderType param3;
  typename ParameterType< P4 >::HolderType param4;
  typename ParameterType< P5 >::HolderType param5;

};

/**
 * Templated message which calls a member function of an object.
 * This overload passes six value-type parameters.
 * Template parameters need to match the MemberFunction!
 * The message will contain copy of the value (in case of & or const&)
 */
template< typename T, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6 >
class MessageValue6 : public MessageBase
{
public:

  typedef void(T::*MemberFunction)(
      typename ParameterType< P1 >::PassingType,
      typename ParameterType< P2 >::PassingType,
      typename ParameterType< P3 >::PassingType,
      typename ParameterType< P4 >::PassingType,
      typename ParameterType< P5 >::PassingType,
      typename ParameterType< P6 >::PassingType );

  /**
   * Create a message.
   * @note The object is expected to be const in the thread which sends this message.
   * However it can be modified when Process() is called in a different thread.
   * @param[in] obj The object.
   * @param[in] member The member function of the object.
   * @param[in] p1 The first parameter to pass to the member function.
   * @param[in] p2 The second parameter to pass to the member function.
   * @param[in] p3 The third parameter to pass to the member function.
   * @param[in] p4 The fourth parameter to pass to the member function.
   * @param[in] p5 The fifth parameter to pass to the member function.
   * @param[in] p6 The sixth parameter to pass to the member function.
   */
  MessageValue6( const T* obj,
                 MemberFunction member,
                 typename ParameterType< P1 >::PassingType p1,
                 typename ParameterType< P2 >::PassingType p2,
                 typename ParameterType< P3 >::PassingType p3,
                 typename ParameterType< P4 >::PassingType p4,
                 typename ParameterType< P5 >::PassingType p5,
                 typename ParameterType< P6 >::PassingType p6 )
  : MessageBase(),
    object( const_cast< T* >( obj ) ),
    memberFunction( member ),
    param1( p1 ),
    param2( p2 ),
    param3( p3 ),
    param4( p4 ),
    param5( p5 ),
    param6( p6 )
  {
  }

  /**
   * Virtual destructor
   */
  virtual ~MessageValue6()
  {
  }

  /**
   * @copydoc MessageBase::Process
   */
  virtual void Process( BufferIndex /*bufferIndex*/ )
  {
    DALI_ASSERT_DEBUG( object && "Message does not have an object" );
    (object->*memberFunction)(
        ParameterType< P1 >::PassObject( param1 ),
        ParameterType< P2 >::PassObject( param2 ),
        ParameterType< P3 >::PassObject( param3 ),
        ParameterType< P4 >::PassObject( param4 ),
        ParameterType< P5 >::PassObject( param5 ),
        ParameterType< P6 >::PassObject( param6 ) );

  }

private:

  T* object;
  MemberFunction memberFunction;
  typename ParameterType< P1 >::HolderType param1;
  typename ParameterType< P2 >::HolderType param2;
  typename ParameterType< P3 >::HolderType param3;
  typename ParameterType< P4 >::HolderType param4;
  typename ParameterType< P5 >::HolderType param5;
  typename ParameterType< P6 >::HolderType param6;

};

/**
 * Templated message which calls a member function of an object.
 * This overload passes a value-type to set a double-buffered property.
 * Template parameters need to match the MemberFunction!
 * The message will contain copy of the value (in case of & or const&)
 */
template< typename T, typename P >
class MessageDoubleBuffered1 : public MessageBase
{
public:

  typedef void(T::*MemberFunction)(
      BufferIndex,
      typename ParameterType< P >::PassingType );

  /**
   * Create a message.
   * @note The object is expected to be const in the thread which sends this message.
   * However it can be modified when Process() is called in a different thread.
   * @param[in] obj The object.
   * @param[in] member The member function of the object.
   * @param[in] p The second parameter to pass.
   */
  MessageDoubleBuffered1( const T* obj,
                          MemberFunction member,
                          typename ParameterType< P >::PassingType p )
  : MessageBase(),
    object( const_cast< T* >( obj ) ),
    memberFunction( member ),
    param( p )
  {
  }

  /**
   * Virtual destructor
   */
  virtual ~MessageDoubleBuffered1()
  {
  }

  /**
   * @copydoc MessageBase::Process
   */
  virtual void Process( BufferIndex bufferIndex )
  {
    DALI_ASSERT_DEBUG( object && "Message does not have an object" );
    (object->*memberFunction)(
        bufferIndex,
        ParameterType< P >::PassObject( param ) );
  }

private:

  T* object;
  MemberFunction memberFunction;
  typename ParameterType< P >::HolderType param;

};

/**
 * Templated message which calls a member function of an object.
 * This overload passes two value-types to set double-buffered properties.
 * Template parameters need to match the MemberFunction!
 * The message will contain copy of the value (in case of & or const&)
 */
template< typename T, typename P2, typename P3 >
class MessageDoubleBuffered2 : public MessageBase
{
public:

  typedef void(T::*MemberFunction)(
      BufferIndex,
      typename ParameterType< P2 >::PassingType,
      typename ParameterType< P3 >::PassingType );

  /**
   * Create a message.
   * @note The object is expected to be const in the thread which sends this message.
   * However it can be modified when Process() is called in a different thread.
   * @param[in] obj The object.
   * @param[in] member The member function of the object.
   * @param[in] p2 The second parameter to pass to the function.
   * @param[in] p3 The third parameter to pass to the function.
   */
  MessageDoubleBuffered2( const T* obj,
                          MemberFunction member,
                          typename ParameterType< P2 >::PassingType p2,
                          typename ParameterType< P3 >::PassingType p3 )
  : MessageBase(),
    object( const_cast< T* >( obj ) ),
    memberFunction( member ),
    param2( p2 ),
    param3( p3 )
  {
  }

  /**
   * Virtual destructor
   */
  virtual ~MessageDoubleBuffered2()
  {
  }

  /**
   * @copydoc MessageBase::Process
   */
  virtual void Process( BufferIndex bufferIndex )
  {
    DALI_ASSERT_DEBUG( object && "Message does not have an object" );
    (object->*memberFunction)(
        bufferIndex,
        ParameterType< P2 >::PassObject( param2 ),
        ParameterType< P3 >::PassObject( param3 ) );
  }

private:

  T* object;
  MemberFunction memberFunction;
  typename ParameterType< P2 >::HolderType param2;
  typename ParameterType< P3 >::HolderType param3;

};


/**
 * Templated message which calls a member function of an object.
 * This overload passes three value-types to set double-buffered properties.
 * Template parameters need to match the MemberFunction!
 * The message will contain copy of the value (in case of & or const&)
 */
template< typename T, typename P2, typename P3, typename P4 >
class MessageDoubleBuffered3 : public MessageBase
{
public:

  typedef void(T::*MemberFunction)(
      BufferIndex,
      typename ParameterType< P2 >::PassingType,
      typename ParameterType< P3 >::PassingType,
      typename ParameterType< P4 >::PassingType );

  /**
   * Create a message.
   * @note The object is expected to be const in the thread which sends this message.
   * However it can be modified when Process() is called in a different thread.
   * @param[in] obj The object.
   * @param[in] member The member function of the object.
   * @param[in] p2 The second parameter to pass.
   * @param[in] p3 The third parameter to pass.
   * @param[in] p4 The forth parameter to pass.
   */
  MessageDoubleBuffered3( const T* obj,
                          MemberFunction member,
                          typename ParameterType< P2 >::PassingType p2,
                          typename ParameterType< P3 >::PassingType p3,
                          typename ParameterType< P4 >::PassingType p4 )
  : MessageBase(),
    object( const_cast< T* >( obj ) ),
    memberFunction( member ),
    param2( p2 ),
    param3( p3 ),
    param4( p4 )
  {
  }

  /**
   * Virtual destructor
   */
  virtual ~MessageDoubleBuffered3()
  {
  }

  /**
   * @copydoc MessageBase::Process
   */
  virtual void Process( BufferIndex bufferIndex )
  {
    DALI_ASSERT_DEBUG( object && "Message does not have an object" );
    (object->*memberFunction)(
        bufferIndex,
        ParameterType< P2 >::PassObject( param2 ),
        ParameterType< P3 >::PassObject( param3 ),
        ParameterType< P4 >::PassObject( param4 ) );
  }

private:

  T* object;
  MemberFunction memberFunction;
  typename ParameterType< P2 >::HolderType param2;
  typename ParameterType< P3 >::HolderType param3;
  typename ParameterType< P4 >::HolderType param4;

};

/**
 * Templated message which calls a member function of an object.
 * This overload passes four value-types to set double-buffered properties.
 * Template parameters need to match the MemberFunction!
 * The message will contain copy of the value (in case of & or const&)
 */
template< typename T, typename P2, typename P3, typename P4, typename P5 >
class MessageDoubleBuffered4 : public MessageBase
{
public:

  typedef void(T::*MemberFunction)(
      BufferIndex,
      typename ParameterType< P2 >::PassingType,
      typename ParameterType< P3 >::PassingType,
      typename ParameterType< P4 >::PassingType,
      typename ParameterType< P5 >::PassingType );

  /**
   * Create a message.
   * @note The object is expected to be const in the thread which sends this message.
   * However it can be modified when Process() is called in a different thread.
   * @param[in] obj The object.
   * @param[in] member The member function of the object.
   * @param[in] p2 The second parameter to pass.
   * @param[in] p3 The third parameter to pass.
   * @param[in] p4 The forth parameter to pass.
   * @param[in] p5 The fifth parameter to pass.
   */
  MessageDoubleBuffered4( const T* obj,
                          MemberFunction member,
                          typename ParameterType< P2 >::PassingType p2,
                          typename ParameterType< P3 >::PassingType p3,
                          typename ParameterType< P4 >::PassingType p4,
                          typename ParameterType< P5 >::PassingType p5 )
  : MessageBase(),
    object( const_cast< T* >( obj ) ),
    memberFunction( member ),
    param2( p2 ),
    param3( p3 ),
    param4( p4 ),
    param5( p5 )
  {
  }

  /**
   * Virtual destructor
   */
  virtual ~MessageDoubleBuffered4()
  {
  }

  /**
   * @copydoc MessageBase::Process
   */
  virtual void Process( BufferIndex bufferIndex )
  {
    DALI_ASSERT_DEBUG( object && "Message does not have an object" );
    (object->*memberFunction)(
        bufferIndex,
        ParameterType< P2 >::PassObject( param2 ),
        ParameterType< P3 >::PassObject( param3 ),
        ParameterType< P4 >::PassObject( param4 ),
        ParameterType< P5 >::PassObject( param5 ) );
  }

private:

  T* object;
  MemberFunction memberFunction;
  typename ParameterType< P2 >::HolderType param2;
  typename ParameterType< P3 >::HolderType param3;
  typename ParameterType< P4 >::HolderType param4;
  typename ParameterType< P5 >::HolderType param5;

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_MESSAGE_H__

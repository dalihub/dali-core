#ifndef __DALI_INTERNAL_MESSAGE_BUFFER_H__
#define __DALI_INTERNAL_MESSAGE_BUFFER_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// EXTERNAL INCLUDES
#include <cstddef>

namespace Dali
{

namespace Internal
{

/**
 * Utility class to reserve a buffer for storing messages.
 */
class MessageBuffer
{
public:

  /**
   * Create a new MessageBuffer
   * @param[in] The smallest capacity which the buffer will allocate, with respect to the size of type "char".
   * @note The buffer will not allocate memory until the first call to ReserveMessageSlot().
   */
  MessageBuffer( std::size_t initialCapacity );

  /**
   * Non-virtual destructor; not suitable as a base class
   */
  ~MessageBuffer();

  /**
   * Reserve space for another message in the buffer.
   * @pre size is greater than zero.
   * @param[in] The message size with respect to the size of type "char".
   * @return A pointer to the address allocated for the message.
   */
  unsigned int* ReserveMessageSlot( std::size_t size );

  /**
   * Query the capacity of the message buffer.
   * @return The capacity with respect to the size of type "char".
   */
  std::size_t GetCapacity() const;

  /**
   * Used to iterate though the messages in the buffer.
   */
  class Iterator
  {
  public:

    // Constructor
    Iterator(unsigned int* current);

    // Inlined for performance
    bool IsValid()
    {
      // Valid until end marker has been found
      return 0 != mMessageSize;
    }

    // Inlined for performance
    unsigned int* Get()
    {
      return ( 0 != mMessageSize ) ? mCurrent : NULL;
    }

    // Inlined for performance
    void Next()
    {
      // Jump to next object and read size
      mCurrent += mMessageSize;
      mMessageSize = *mCurrent++;
    }

    // Copy constructor
    Iterator(const Iterator& copy);

  private:

    // Undefined
    Iterator& operator=(const Iterator& rhs);

  private:

    unsigned int* mCurrent;
    unsigned int mMessageSize;
  };

  /**
   * Returns an iterator to the first message in the buffer.
   * There is no past-the-end iterator; use Iterator::IsValid() to determine when the has been reached.
   * @note Adding more messages with ReserveMessageSlot() may corrupt this iterator.
   * @return The iterator.
   */
  Iterator Begin() const;

  /**
   * Sets the size of the buffer to zero (does not deallocate memory)
   */
  void Reset();

private:

  // Undefined
  MessageBuffer(const MessageBuffer&);

  // Undefined
  MessageBuffer& operator=(const MessageBuffer& rhs);

  /**
   * Helper to increase the capacity of the buffer.
   * @pre The newCapacity is greater than mCapacity.
   * @param[in] The newCapacity
   */
  void IncreaseCapacity( std::size_t newCapacity );

private:

  std::size_t mInitialCapacity; ///< The capacity to allocate during first call to ReserveMessageSlot

  unsigned int* mData;     ///< The data allocated for the message buffer
  unsigned int* mNextSlot; ///< The next free location in the buffer

  std::size_t mCapacity; ///< The memory allocated with respect to sizeof(unsigned int)
  std::size_t mSize;     ///< The memory reserved for messages with respect to sizeof(unsigned int)
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_MESSAGE_BUFFER_H__

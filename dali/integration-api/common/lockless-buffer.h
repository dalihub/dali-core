#ifndef __DALI_INTEGRATION_LOCKLESS_BUFFER_H__
#define __DALI_INTEGRATION_LOCKLESS_BUFFER_H__

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

// INTERNAL INCLUDES
#include <cstring>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

namespace Integration
{

/**
 * The LocklessBuffer class implements double buffering eligible for multi-(two) threaded use,
 * where it's possible to read from one thread and write from another
 * without requiring a mutex lock to avoid performance hit.
 * It's intended to be used for reading bitmap data in render thread
 * while still possible to write data in another thread.
 *
 * Ideally Write() and Read() calls should be alternating, otherwise written data might be thrown away.
 *
 * The buffers are swapped in the reading thread, just before reading begins.
 * In case the other thread is writing at that moment, buffers are not swapped and previously available data is read.
 * Similarly if Write() is called before a Read() has finished the previous write buffer is overwritten.
 */
class DALI_IMPORT_API LocklessBuffer
{

public:
  /**
   * Constructor.
   * @param[in] size The size of buffers in bytes.
   */
  LocklessBuffer( size_t size );

  /**
   * Destructor.
   */
  ~LocklessBuffer();

  /**
   * Write data to buffer.
   * @param[in] src data source
   * @param[in] size size of data in bytes
   */
  void Write( const unsigned char *src, size_t size );

  /**
   * Try to swap buffers and read data.
   * @note returned value only valid until Read() is called again or object is destroyed
   * @return current read buffer contents
   */
  const unsigned char* Read();

  /**
   * @return the buffer size in bytes
   */
  unsigned int GetSize() const;

private:
  /**
   * Atomically set state.
   * We're always writing to one buffer and reading from the other.
   * Write() sets WRITING bit when started and unsets it when finished.
   */
  enum BufferState
  {
    R0W1    = 0, ///< Read from buffer 0 write to buffer 1
    R1W0    = 1, ///< Read from buffer 1 write to buffer 0
    WRITING = 2, ///< Currently writing to buffer
    UPDATED = 4, ///< Swapping buffer required; there is new data available
    WRITE_BUFFER_MASK = 1, ///< indicates which buffer to write to
    WRITING_MASK      = 2, ///< indicates whether currently writing
    UPDATED_MASK      = 4  ///< indicates whether new data is available
  };

private:
  LocklessBuffer(); ///< undefined default constructor, need to give size on construction
  LocklessBuffer( const LocklessBuffer& );            ///< undefined copy constructor
  LocklessBuffer& operator=( const LocklessBuffer& ); ///< undefined assignment operator

private:
  unsigned char* mBuffer[2];     ///< bitmap buffers
  BufferState volatile mState;   ///< readbuffer number and whether we're currently writing into writebuffer or not
  size_t mSize;                  ///< size of buffers
};

} // Internal

} // Dali

#endif // __DALI_INTEGRATION_LOCKLESS_H__

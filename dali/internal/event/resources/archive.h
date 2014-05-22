#if !defined(__DALI_INTERNAL_ARCHIVE_H__)
#define __DALI_INTERNAL_ARCHIVE_H__

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
#include <string>
#include <iostream>
#include <stack>

namespace Dali
{
struct Vector2;
struct Vector3;
struct Vector4;
class Matrix;
class Quaternion;

namespace Internal
{

/**
 * Archive class. Serializes data to a Tag-Length-Value archive
 */
class Archive
{
public:
  typedef std::pair<unsigned int, std::streampos> ChunkHeader;

public:
  /**
   * Constructor
   * @param[in] buf An open streambuf
   */
  Archive(std::streambuf& buf);

  /**
   * Destructor
   */
  virtual ~Archive();

  /**
   * Get archive version
   */
  unsigned int GetVersion() const
  {
    return mVersion;
  }

  /**
   * Set archive version
   * @param[in] version the version number
   */
  void SetVersion(const unsigned int version)
  {
    mVersion = version;
  }

  /**
   * Returns the result of the archiving operation
   * The result status is initialised to true on construction
   * Any failure will set it to false
   * @return the archiving result, true signals success.
   */
  bool GetResult() const
  {
    return mResult;
  }

  /**
   * Set the archive status to failed
   */
  void SetResultFailed()
  {
    mResult = false;
  }

  /**
   * Write a bytestream to the archive
   * @param[in] data    A pointer to the data
   * @param[in] length  The length of the data in bytes
   * @return            true if the data was successfully written
   */
  virtual bool Write(const char* data, const unsigned int length)
  {
    return false;
  }

  /**
   * Read a bytestream from the archive
   * @param[in] data    A pointer to a buffer to store the data
   * @param[in] length  The length of the data in bytes
   * @return            true if the data was successfully read
   */
  virtual bool Read(char* data, const unsigned int length)
  {
    return false;
  }

  /**
   * Open a new chunk
   * @param[in] tag The FourCC tag for the chunk
   * @return true if success.
   */
  virtual bool OpenChunk(const unsigned int tag) = 0;

  /**
   * Skip an entire chunk
   * @param[in] tag The FourCC tag for the chunk
   */
  virtual void SkipChunk(const unsigned int tag)
  {
  }

  /**
   * Close the current chunk
   * The chunk length is written to the archive
   */
  virtual void CloseChunk() = 0;

  /**
   * Peek at the tag of the next chunk.
   * This will move the file pointer to the next even byte,
   * then read the next four bytes
   * @result The FourCC tag for the next chunk.
   */
  virtual unsigned int PeekChunk()
  {
    return 0;
  }

protected:
  unsigned int            mVersion;
  std::iostream           mStream;
  std::stack<ChunkHeader> mChunkStack;
  bool                    mResult;
}; // class Archive

/**
 * Archive specialization. Performs serialization to an Archive
 */
class OutputArchive : public Archive
{
public:
  OutputArchive(std::streambuf& buf, const unsigned int version);
  virtual ~OutputArchive();

public: // from Archive
  /**
   * @copydoc Archive::Write()
   */
  virtual bool Write(const char* data, const unsigned int length);

  /**
   * @copydoc Archive::OpenChunk()
   * @param[in] tag The chunk tag
   */
  virtual bool OpenChunk(const unsigned int tag);

  /**
   * @copydoc Archive::CloseChunk()
   */
  virtual void CloseChunk();

}; // class OutputArchive

/**
 * Archive specialization. Performs serialization from an Archive
 */
class InputArchive : public Archive
{
public:
  InputArchive(std::streambuf& buf, const unsigned int version);
  virtual ~InputArchive();

public: // from Archive
  /**
   * @copydoc Archive::Read()
   */
  virtual bool Read(char* data, const unsigned int length);

  /**
   * @copydoc Archive::OpenChunk()
   * @param[in] tag The chunk tag
   */
  virtual bool OpenChunk(const unsigned int tag);

  /**
   * @copydoc Archive::SkipChunk()
   * @param[in] tag The chunk tag
   */
  virtual void SkipChunk(const unsigned int tag);

  /**
   * @copydoc Archive::CloseChunk()
   */
  virtual void CloseChunk();

  /**
   * @copydoc Archive::PeekChunk()
   */
  virtual unsigned int PeekChunk();

public:
  /**
   * Set the archive version as read from the archive
   * @param[in] version The archives version number
   */
  void SetFileVersion(unsigned int version)
  {
    mFileVersion = version;
  }

  /**
   * Get the archive version number read from the archive
   */
  const unsigned int GetFileVersion() const
  {
    return mFileVersion;
  }
private:
  unsigned int  mFileVersion;

}; // class InputArchive

namespace Serialize
{

Archive& operator<< (Archive&, const char&);
Archive& operator>> (Archive&, char&);
Archive& operator<< (Archive&, const unsigned char&);
Archive& operator>> (Archive&, unsigned char&);
Archive& operator<< (Archive&, const bool&);
Archive& operator>> (Archive&, bool&);
Archive& operator<< (Archive&, const short&);
Archive& operator>> (Archive&, short&);
Archive& operator<< (Archive&, const unsigned short&);
Archive& operator>> (Archive&, unsigned short&);
Archive& operator<< (Archive&, const int&);
Archive& operator>> (Archive&, int&);
Archive& operator<< (Archive&, const unsigned int&);
Archive& operator>> (Archive&, unsigned int&);
Archive& operator<< (Archive&, const float&);
Archive& operator>> (Archive&, float&);

Archive& operator<< (Archive&, const std::string&);
Archive& operator>> (Archive&, std::string&);

// math
Archive& operator<< (Archive&, const Vector2&);
Archive& operator>> (Archive&, Vector2&);
Archive& operator<< (Archive&, const Vector3&);
Archive& operator>> (Archive&, Vector3&);
Archive& operator<< (Archive&, const Vector4&);
Archive& operator>> (Archive&, Vector4&);
Archive& operator<< (Archive&, const Quaternion&);
Archive& operator>> (Archive&, Quaternion&);
Archive& operator<< (Archive&, const Matrix&);
Archive& operator>> (Archive&, Matrix&);

} // namespace Serialize
} // Internal

} // Dali

#endif // !defined(__DALI_INTERNAL_ARCHIVE_H__)



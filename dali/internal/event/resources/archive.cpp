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

// CLASS HEADER
#include <dali/internal/event/resources/archive.h>

// EXTERNAL INCLUDES
#include <fstream>
#include <ios>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/quaternion.h>

namespace Dali
{

namespace Internal
{

/*--------- Archive ---------*/

Archive::Archive(std::streambuf& buf)
: mVersion(0),
  mStream(&buf),
  mResult(true)
{
}

Archive::~Archive()
{
  DALI_ASSERT_DEBUG(mChunkStack.empty());
}


/*--------- OutputArchive ---------*/

OutputArchive::OutputArchive(std::streambuf& buf, const unsigned int version)
: Archive(buf)
{
  SetVersion(version);
}

OutputArchive::~OutputArchive()
{
}

bool OutputArchive::Write(const char* data, const unsigned int length)
{
  if( mResult )
  {
    mStream.write(data, length);
    if( mStream.bad() )
    {
      mResult = false;
    }
  }
  return mResult;
}

bool OutputArchive::OpenChunk(const unsigned int fourCC)
{
  const unsigned int zero = 0;

  // Get current position in stream
  std::streampos currentPos = mStream.tellp();

  // Ensure chunk will start at an even byte
  // (necessary for nested chunks)
  if( currentPos & 1 )
  {
    const char padding = 0;
    Write(&padding, sizeof(char));
  }

  // Write chunk header to stream
  Write(reinterpret_cast<const char*>(&fourCC), sizeof(unsigned int));

  // Push chunkHeader information to chunk LIFO stack
  mChunkStack.push(std::make_pair(fourCC, mStream.tellp()));

  // Write zero for chunk length temporarily. This will be overwritten in CloseChunk
  Write(reinterpret_cast<const char*>(&zero), sizeof(unsigned int));

  return mResult;
}

void OutputArchive::CloseChunk()
{
  // Get current position in stream
  std::streampos currentPos = mStream.tellp();

  // retrieve chunk header
  ChunkHeader chunkHeader = mChunkStack.top();
  mChunkStack.pop();

  // seek to chunk header in stream
  mStream.seekp(chunkHeader.second);

  // calculate and write chunk size
  unsigned int chunkLength = static_cast<unsigned int>(currentPos - chunkHeader.second) - sizeof(unsigned int);
  Write(reinterpret_cast<const char*>(&chunkLength), sizeof(unsigned int));

  // return to current position in stream
  mStream.seekp(currentPos);

  // Ensure next chunk will start at on even byte
  if( currentPos & 1 )
  {
    const char padding = 0;
    Write(&padding, 1);
  }
}


/*--------- InputArchive ---------*/

InputArchive::InputArchive(std::streambuf& buf, const unsigned int version)
: Archive(buf),
  mFileVersion(0)
{
  SetVersion(version);
}

InputArchive::~InputArchive()
{
}

bool InputArchive::Read(char* data, const unsigned int length)
{
  if( mResult )
  {
    mStream.read(data, length);
    if( mStream.bad() )
    {
      mResult = false;
    }
  }
  return mResult;
}

bool InputArchive::OpenChunk(const unsigned int fourCC)
{
  if( PeekChunk() != fourCC )
  {
    // trying to open incorrect chunk, mark archive error
    mResult = false;
    return mResult;
  }

  union
  {
    unsigned int fourCC_int;
    char fourCC_char[4];
  };
  fourCC_int = 0;

  if( mStream.tellg() & 1 )
  {
    mStream.seekg(1, std::ios_base::cur);
  }

  Read(fourCC_char, sizeof(unsigned int));

  // Push chunkHeader information to chunk LIFO stack
  mChunkStack.push(std::make_pair(fourCC_int, mStream.tellg()));

# if defined(DEBUG_ENABLED)

  unsigned int chunkLength;
  Read(reinterpret_cast<char*>(&chunkLength), sizeof(unsigned int));

  DALI_LOG_INFO(Debug::Filter::gModel, Debug::Verbose, "Enter: %c%c%c%c(%d)\n",
                fourCC_char[0], fourCC_char[1], fourCC_char[2], fourCC_char[3], chunkLength);

# else  // defined(DEBUG_ENABLED)

  // skip chunk length
  mStream.seekg(sizeof(unsigned int), std::ios_base::cur);

# endif // defined(DEBUG_ENABLED)

  return mResult;
}

void InputArchive::SkipChunk(const unsigned int fourCC)
{
  // Ensure the next chunk is the correct one
  if( PeekChunk() != fourCC )
  {
    return;
  }

  union
  {
    unsigned int fourCC_int;
    char fourCC_char[4];
  };
  unsigned int chunkLength;

  if( mStream.tellg() & 1 )
  {
    mStream.seekg(1, std::ios_base::cur);
  }

  Read(fourCC_char, sizeof(unsigned int));
  Read(reinterpret_cast<char*>(&chunkLength), sizeof(unsigned int));

  DALI_LOG_INFO(Debug::Filter::gModel, Debug::Verbose, "Enter: %c%c%c%c(%d)\n",
                fourCC_char[0], fourCC_char[1], fourCC_char[2], fourCC_char[3], chunkLength);

  if( chunkLength & 1 )
  {
    ++chunkLength;
  }

  mStream.seekg(chunkLength, std::ios_base::cur);
}

void InputArchive::CloseChunk()
{
  ChunkHeader chunkHeader = mChunkStack.top();
  mChunkStack.pop();

  mStream.seekg(chunkHeader.second);

  unsigned int chunkLength;
  Read(reinterpret_cast<char*>(&chunkLength), sizeof(unsigned int));

  if( chunkLength & 1)
  {
    ++chunkLength;
  }
  mStream.seekg(chunkLength, std::ios_base::cur);
}

unsigned int InputArchive::PeekChunk()
{
  union
  {
    unsigned int fourCC_int;
    char fourCC_char[4];
  };
  fourCC_int = 0;

  // Get current position in stream
  std::streampos currentPos = mStream.tellg();

  // Ensure next read will be from an even byte
  if( currentPos & 1 )
  {
    mStream.seekg(1, std::ios_base::cur);
  }

  // Read tag
  Read(fourCC_char, sizeof(unsigned int));

# if defined(DEBUG_ENABLED)
  unsigned int chunkLength;
  Read(reinterpret_cast<char*>(&chunkLength), sizeof(unsigned int));

  DALI_LOG_INFO(Debug::Filter::gModel, Debug::Verbose, "Peek: %c%c%c%c(%d)\n",
                fourCC_char[0], fourCC_char[1], fourCC_char[2], fourCC_char[3], chunkLength);
# endif // defined(DEBUG_ENABLED)

  // return to original position in stream
  mStream.seekg(currentPos);

  return fourCC_int;
}

namespace Serialize
{
/*------ serialization overrides of operator<< () and operator>> () ------*/

Archive& operator<< (Archive& ar, const char& t)
{
  ar.Write((char*)&t, sizeof(t));

  return ar;
}

Archive& operator>> (Archive& ar, char& t)
{
  ar.Read((char*)&t, sizeof(t));

  return ar;
}

Archive& operator<< (Archive& ar, const unsigned char& t)
{
  ar.Write((char*)&t, sizeof(t));
  return ar;
}

Archive& operator>> (Archive& ar, unsigned char& t)
{
  ar.Read((char*)&t, sizeof(t));

  return ar;
}

Archive& operator<< (Archive& ar, const bool& t)
{
  char value = t ? 1 : 0;
  ar.Write(&value, sizeof(value));
  return ar;
}

Archive& operator>> (Archive& ar, bool& t)
{
  char value;
  ar.Read(&value, sizeof(value));
  t = value != 0;

  return ar;
}

Archive& operator<< (Archive& ar, const short& t)
{
  ar.Write((char*)&t, sizeof(t));

  return ar;
}

Archive& operator>> (Archive& ar, short& t)
{
  ar.Read((char*)&t, sizeof(t));

  return ar;
}

Archive& operator<< (Archive& ar, const unsigned short& t)
{
  ar.Write((char*)&t, sizeof(t));

  return ar;
}

Archive& operator>> (Archive& ar, unsigned short& t)
{
  ar.Read((char*)&t, sizeof(t));

  return ar;
}

Archive& operator<< (Archive& ar, const int& t)
{
  ar.Write((char*)&t, sizeof(t));

  return ar;
}

Archive& operator>> (Archive& ar, int& t)
{
  ar.Read((char*)&t, sizeof(t));

  return ar;
}

Archive& operator<< (Archive& ar, const unsigned int& t)
{
  ar.Write((char*)&t, sizeof(t));

  return ar;
}

Archive& operator>> (Archive& ar, unsigned int& t)
{
  ar.Read((char*)&t, sizeof(t));

  return ar;
}

// float
Archive& operator<< (Archive& ar, const float& t)
{
  ar.Write((char*)&t, sizeof(t));

  return ar;
}

Archive& operator>> (Archive& ar, float& t)
{
  ar.Read((char*)&t, sizeof(t));

  return ar;
}

// Vector2
Archive& operator<< (Archive& ar, const Vector2& t)
{
  ar << t.x << t.y;
  return ar;
}

Archive& operator>> (Archive& ar, Vector2& t)
{
  ar >> t.x >> t.y;
  return ar;
}

// Vector3
Archive& operator<< (Archive& ar, const Vector3& t)
{
  ar << t.x << t.y << t.z;
  return ar;
}

Archive& operator>> (Archive& ar, Vector3& t)
{
  ar >> t.x >> t.y >> t.z;
  return ar;
}

// Vector4
Archive& operator<< (Archive& ar, const Vector4& t)
{
  ar << t.x << t.y << t.z << t.w;
  return ar;
}

Archive& operator>> (Archive& ar, Vector4& t)
{
  ar >> t.x >> t.y >> t.z >> t.w;
  return ar;
}

// Quaternion
Archive& operator<< (Archive& ar, const Quaternion& t)
{
  ar << t.mVector;

  return ar;
}

Archive& operator>> (Archive& ar, Quaternion& t)
{
  ar >> t.mVector;

  return ar;
}

// Matrix
Archive& operator<< (Archive& ar, const Matrix& t)
{
  const float* data = t.AsFloat();

  for( unsigned i = 0; i < sizeof(Matrix) / sizeof(float); ++i)
  {
    ar << *data;
    ++data;
  }

  return ar;
}

Archive& operator>> (Archive& ar, Matrix& t)
{
  float* data = t.AsFloat();

  for( unsigned i = 0; i < sizeof(Matrix) / sizeof(float); ++i)
  {
    ar >> *data;
    ++data;
  }

  return ar;
}

/*---- std::string ----*/
Archive& operator<< (Archive& ar, const std::string& t)
{
  unsigned int length = t.size();
  ar << length;
  ar.Write(t.data(), length);

  return ar;
}

Archive& operator>> (Archive& ar, std::string& t)
{
  unsigned int length = 0;
  char* data = NULL;

  ar >> length;
  data = new char[ length ];
  ar.Read(data, length);
  t.assign(data, length);
  delete [] data;

  return ar;
}

} // namespace Serialize

} // Internal

} // Dali


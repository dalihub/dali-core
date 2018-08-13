/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/images/distance-field.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <math.h>
#include <stdio.h>
#include <time.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/math-utils.h>

namespace Dali
{

namespace
{

float Interpolate( float a, float b, float factor )
{
  return a * (1.0f - factor) + b * factor;
}

float Bilinear( float a, float b, float c, float d, float dx, float dy )
{
  return Interpolate( Interpolate( a, b, dx), Interpolate( c, d, dx ), dy );
}

void ScaleField( int width, int height, float* in, int targetWidth, int targetHeight, float* out )
{
  float xScale = static_cast< float >(width) / targetWidth;
  float yScale = static_cast< float >(height) / targetHeight;

  // for each row in target
  for(int y = 0; y < targetHeight; ++y)
  {
    const int sampleY = static_cast< int >( yScale * y );
    const int otherY = std::min( sampleY + 1, height - 1 );
    const float dy = (yScale * y ) - sampleY;

    // for each column in target
    for (int x = 0; x < targetWidth; ++x)
    {
      const int sampleX = static_cast< int >( xScale * x );
      const int otherX = std::min( sampleX + 1, width - 1 );
      const float dx = (xScale * x) - sampleX;

      float value = Bilinear( in[ sampleY * width + sampleX ],
                              in[ sampleY * width + otherX ],
                              in[ otherY * width + sampleX ],
                              in[ otherY * width + otherX ],
                              dx, dy );

      out[y * targetWidth + x] = std::min( value, 1.0f );
    }
  }
}

#define SQUARE(a) ((a) * (a))
const float MAX_DISTANCE( 1e20 );

/**
 * Distance transform of 1D function using squared distance
 */
void DistanceTransform( float *source, float* dest, unsigned int length )
{
  int parabolas[length];    // Locations of parabolas in lower envelope
  float edge[length + 1];   // Locations of boundaries between parabolas
  int rightmost(0);         // Index of rightmost parabola in lower envelope

  parabolas[0] = 0;
  edge[0] = -MAX_DISTANCE;
  edge[1] = +MAX_DISTANCE;
  for( unsigned int i = 1; i <= length - 1; i++ )
  {
    const float initialDistance( source[i] + SQUARE( i ) );
    int parabola = parabolas[rightmost];
    float newDistance( (initialDistance - (source[parabola] + SQUARE( parabola ))) / (2 * i - 2 * parabola) );
    while( rightmost > 0 && newDistance <= edge[rightmost] )
    {
      rightmost--;
      parabola = parabolas[rightmost];
      newDistance = (initialDistance - (source[parabola] + SQUARE( parabola ))) / (2 * i - 2 * parabola);
    }

    rightmost++;
    parabolas[rightmost] = i;
    edge[rightmost] = newDistance;
    edge[rightmost + 1] = MAX_DISTANCE;
  }

  rightmost = 0;
  for( unsigned int i = 0; i <= length - 1; ++i )
  {
    while( edge[rightmost + 1] < i )
    {
      ++rightmost;
    }
    dest[i] = SQUARE( static_cast< int >( i ) - parabolas[rightmost] ) + source[parabolas[rightmost]];
  }
}

/**
 * Distance transform of 2D function using squared distance
 */
void DistanceTransform( float* data, unsigned int width, unsigned int height, float* sourceBuffer, float* destBuffer )
{
  // transform along columns
  for( unsigned int x = 0; x < width; ++x )
  {
    for( unsigned int y = 0; y < height; ++y )
    {
      sourceBuffer[y] = data[ y * width + x ];
    }

    DistanceTransform( sourceBuffer, destBuffer, height );

    for( unsigned int y = 0; y < height; y++ )
    {
      data[y * width + x] = destBuffer[y];
    }
  }

  // transform along rows
  for( unsigned int y = 0; y < height; ++y )
  {
    for( unsigned int x = 0; x < width; ++x )
    {
      sourceBuffer[x] = data[ y * width + x ];
    }

    DistanceTransform( sourceBuffer, destBuffer, width );

    for( unsigned int x = 0; x < width; x++ )
    {
      data[y * width + x] = destBuffer[x];
    }
  }
}

} // namespace

void GenerateDistanceFieldMap(const unsigned char* const imagePixels, const Size& imageSize,
                              unsigned char* const distanceMap, const Size& distanceMapSize,
                              const float fieldRadius, const unsigned int fieldBorder, bool highQuality)
{
  GenerateDistanceFieldMap( imagePixels, imageSize, distanceMap, distanceMapSize, fieldBorder, imageSize, highQuality );
}

void GenerateDistanceFieldMap(const unsigned char* const imagePixels, const Size& imageSize,
                              unsigned char* const distanceMap, const Size& distanceMapSize,
                              const unsigned int fieldBorder,
                              const Vector2& maxSize,
                              bool highQuality)
{
  // constants to reduce redundant calculations
  const int originalWidth( static_cast<int>(imageSize.width) );
  const int originalHeight( static_cast<int>(imageSize.height) );
  const int paddedWidth( originalWidth + (fieldBorder * 2 ) );
  const int paddedHeight( originalHeight + (fieldBorder * 2 ) );
  const int scaledWidth( static_cast<int>(distanceMapSize.width) );
  const int scaledHeight( static_cast<int>(distanceMapSize.height) );
  const int maxWidth( static_cast<int>(maxSize.width) + (fieldBorder * 2 ));
  const int maxHeight( static_cast<int>(maxSize.height) + (fieldBorder * 2 ) );

  const int bufferLength( std::max( maxWidth, std::max(paddedWidth, scaledWidth) ) *
                          std::max( maxHeight, std::max(paddedHeight, scaledHeight) ) );

  std::vector<float> outsidePixels( bufferLength, 0.0f );
  std::vector<float> insidePixels( bufferLength, 0.0f );

  float* outside( outsidePixels.data() );
  float* inside( insidePixels.data() );

  for( int y = 0; y < paddedHeight; ++y )
  {
    for ( int x = 0; x < paddedWidth; ++x)
    {
      if( y < static_cast< int >( fieldBorder ) || y >= ( paddedHeight - static_cast< int >( fieldBorder ) ) ||
          x < static_cast< int >( fieldBorder ) || x >= ( paddedWidth - static_cast< int >( fieldBorder ) ) )
      {
        outside[ y * paddedWidth + x ] = MAX_DISTANCE;
        inside[ y * paddedWidth + x ] = 0.0f;
      }
      else
      {
        unsigned int pixel( imagePixels[ (y - fieldBorder) * originalWidth + (x - fieldBorder) ] );
        outside[ y * paddedWidth + x ] = (pixel == 0) ? MAX_DISTANCE : SQUARE((255 - pixel) / 255.0f);
        inside[ y * paddedWidth + x ] = (pixel == 255) ? MAX_DISTANCE : SQUARE(pixel / 255.0f);
      }
    }
  }

  // perform distance transform if high quality requested, else use original figure
  if( highQuality )
  {
    // create temporary buffers for DistanceTransform()
    const int tempBufferLength( std::max(paddedWidth, paddedHeight) );
    std::vector<float> tempSourceBuffer( tempBufferLength, 0.0f );
    std::vector<float> tempDestBuffer( tempBufferLength, 0.0f );

    // Perform distance transform for pixels 'outside' the figure
    DistanceTransform( outside, paddedWidth, paddedHeight, tempSourceBuffer.data(), tempDestBuffer.data() );

    // Perform distance transform for pixels 'inside' the figure
    DistanceTransform( inside, paddedWidth, paddedHeight, tempSourceBuffer.data(), tempDestBuffer.data() );
  }

  // distmap = outside - inside; % Bipolar distance field
  for( int y = 0; y < paddedHeight; ++y)
  {
    for( int x = 0; x < paddedWidth; ++x )
    {
      const int offset( y * paddedWidth + x );
      float pixel( sqrtf(outside[offset]) - sqrtf(inside[offset]) );
      pixel = 128.0f + pixel * 16.0f;
      pixel = Clamp( pixel, 0.0f, 255.0f );
      outside[offset] = (255.0f - pixel) / 255.0f;
    }
  }

  // scale the figure to the distance field tile size
  ScaleField( paddedWidth, paddedHeight, outside, scaledWidth, scaledHeight, inside );

  // convert from floats to integers
  for( int y = 0; y < scaledHeight; ++y )
  {
    for( int x = 0; x < scaledWidth; ++x )
    {
      float pixel( inside[ y * scaledWidth + x ] );
      distanceMap[y * scaledWidth + x ] = static_cast< unsigned char >(pixel * 255.0f);
    }
  }
}

} // namespace Dali

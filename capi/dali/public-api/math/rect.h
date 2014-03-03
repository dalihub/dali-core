#ifndef __DALI_RECT_H__
#define __DALI_RECT_H__

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

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// EXTERNAL INCLUDES
#include <math.h>

// INTERNAL INCLUDES
#include <dali/public-api/math/math-utils.h>

namespace Dali DALI_IMPORT_API
{

template< typename T = float >
struct Rect
{
// Methods

  /**
   * Constructor
   */
  Rect()
  : x(0),
    y(0),
    width(0),
    height(0)
  {
  }

  /**
   * Constructor
   * @param [in] x       x coordinate
   * @param [in] y       y coordinate
   * @param [in] width   width
   * @param [in] height  height
   */
  Rect(T x, T y, T width, T height)
  : x(x),
    y(y),
    width(width),
    height(height)
  {
  }

  /**
   * Copy constructor
   * @param [in] rhs  The original object
   */
  Rect(const Rect<T>& rhs)
  {
    x = rhs.x;
    y = rhs.y;
    width = rhs.width;
    height = rhs.height;
  }

   /**
    * Assignment operator
    * @param [in] rhs  The original object
    */
   Rect<T>& operator= (const Rect<T>& rhs)
   {
     if (this != &rhs)
     {
       x = rhs.x;
       y = rhs.y;
       width = rhs.width;
       height = rhs.height;
     }

     return *this;
   }

   /**
    * Assignment from individual values
    * @param[in] newX      x coordinate
    * @param[in] newY      y coordinate
    * @param[in] newWidth  width
    * @param[in] newHeight height
    */
  void Set(T newX, T newY, T newWidth, T newHeight)
  {
     x = newX;
     y = newY;
     width = newWidth;
     height = newHeight;
  }

   /**
    * Determines whether or not this Rectangle is empty
    * @return true if width or height are zero
    */
   bool IsEmpty() const
   {
     return width  == 0 ||
            height == 0;
   }

   /**
    * Get the left of the rectangle
    * return The left edge of the rectangle
    */
   T Left() const
   {
     return x;
   }
   /**
    * Get the right of the rectangle
    * return The right edge of the rectangle
    */
   T Right() const
   {
     return x + width;
   }

   /**
    * Get the top of the rectangle
    * return The top of the rectangle
    */
   T Top() const
   {
     return y;
   }

   /**
    * Get the bottom of the rectangle
    * return The bottom of the rectangle
    */
   T Bottom() const
   {
     return y + height;
   }

   /**
    * Get the area of the rectangle
    * @return The area of the rectangle
    */
   T Area() const
   {
     return width * height;
   }

   /**
    * Determines whether or not this rectangle and the specified rectangle intersect
    * @param[in] other  The other rectangle to test against this rectangle
    * @return           true if the rectangles intersect
    */
   bool Intersects(const Rect<T>& other) const
   {
     return (other.x + other.width)  > x           &&
             other.x                 < (x + width) &&
            (other.y + other.height) > y           &&
             other.y                 < (y + height);
   }

   /**
    * Determines whether or not this Rectangle contains the specified rectangle.
    * @param[in] other  The other rectangle to test against this rectangle
    * @return           true if the specified rectangle is contained
    */
   bool Contains(const Rect<T>& other) const
   {
     return other.x                  >= x           &&
            (other.x + other.width)  <= (x + width) &&
            other.y                  >= y           &&
            (other.y + other.height) <= (y + height);
   }

public:   // Data

   T x;
   T y;
   T width;
   T height;
}; // struct template<typename T>Rect

/**
 * Equality operator
 * @return true if boxes are exactly same
 */
template< typename T >
inline bool operator==( const Rect<T>& lhs, const Rect<T>& rhs )
{
  return ( lhs.x == rhs.x )&&
         ( lhs.y == rhs.y )&&
         ( lhs.width == rhs.width )&&
         ( lhs.height == rhs.height );
}

/**
 * Inequality operator
 */
template< typename T >
inline bool operator!=( const Rect<T>& lhs, const Rect<T>& rhs )
{
  return !(lhs == rhs);
}

/**
 * Equality operator specialization for float
 * @return true if boxes are exactly same
 */
template<>
inline bool operator==( const Rect<float>& lhs, const Rect<float>& rhs )
{
  return ( fabsf( lhs.x - rhs.x ) < GetRangedEpsilon(lhs.x, rhs.x) )&&
         ( fabsf( lhs.y - rhs.y ) < GetRangedEpsilon(lhs.y, rhs.y) )&&
         ( fabsf( lhs.width - rhs.width ) < GetRangedEpsilon(lhs.width, rhs.width) )&&
         ( fabsf( lhs.height - rhs.height ) < GetRangedEpsilon(lhs.height, rhs.height) );
}

/**
 * IsEmpty specialization for float
 */
template<>
inline bool Rect<float>::IsEmpty() const
{
  return (fabsf(width)  <= GetRangedEpsilon(width, width)
          ||
          fabsf(height) <= GetRangedEpsilon(height, height));
}

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_RECT_H__

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

// FILE HEADER
#include "text-vertex-generator.h"

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <dali/internal/event/text/glyph-status/glyph-status.h>
#include <dali/internal/event/text/special-characters.h>

// EXTERNAL INCLUDES
#include <cmath>  // for std::sin

namespace Dali
{

namespace Internal
{

namespace // unnamed namespace
{

typedef std::vector<TextVertex2D> VertexBuffer;

void RepositionData( VertexBuffer& buffer, Vector2 offset )
{
  /*
   *
   * As 0,0 is the middle of the actor, text will be displayed like this
   *
   *  |-------------------------------|
   *  |            Actor              |
   *  |                               |
   *  |                               |
   *  |          (0,0)|----------     | (x)
   *  |               | Hello World   |
   *  |               |               |
   *  |               |               |
   *  |-------------------------------|
   *                 (y)
   *
   *  Below it is repositioned to the centre of the actor
   *  |-------------------------------|
   *  |            Actor              |
   *  |                               |
   *  |                               |
   *  |          Hello World------    | (x)
   *  |               |               |
   *  |               |               |
   *  |               |               |
   *  |-------------------------------|
   */

  // move the vertices so 0,0 is the centre of the text string.
  offset.x/=2.0f;
  offset.y/=2.0f;

  for (std::size_t i=0, size = buffer.size() ; i< size; ++i)
  {
      buffer[i].mX -= offset.x;
      buffer[i].mY -= offset.y;
  }
}

void AddVertex( VertexBuffer& vertexBuffer,
                const float xPos,
                const float yPos,
                const float charWidth,
                const float charHeight,
                const UvRect& uv,
                const Vector2& uvShadow )
{
  /*
   * Create 4 vertices
   * 1 --- 2
   * |    /|
   * |  A  |
   * | /   |
   * 0 --- 3
   *
   * 2 triangles with clock wise winding: 0->1->2 and 0->2->3
   */

  TextVertex2D v;

  // set U1,V1 for all vertices
  v.mU1 = uvShadow.x;
  v.mV1 = uvShadow.y;

  // bottom left, 0
  v.mX = xPos;
  v.mY = yPos;
  v.mU = uv.u0;
  v.mV = uv.v0;
  vertexBuffer.push_back(v);

  // top left, 1
  v.mX = xPos;
  v.mY = yPos + charHeight;
  v.mU = uv.u0;
  v.mV = uv.v2;
  vertexBuffer.push_back(v);

  // top right, 2
  v.mX = xPos + charWidth;
  v.mY = yPos + charHeight;
  v.mU = uv.u2;
  v.mV = uv.v2;
  vertexBuffer.push_back(v);

  // bottom right, 3
  v.mX = xPos + charWidth;
  v.mY = yPos;
  v.mU = uv.u2;
  v.mV = uv.v0;
  vertexBuffer.push_back(v);
}

/**
 * Adjust the vertex data for italics.
 * Skews the vertices by a value
 */
void AdjustForItalics( VertexBuffer&  vertexBuffer,
                       const float italicsTopDisplacement,
                       const float italicsBottomDisplacement)
{

  std::size_t index = vertexBuffer.size()-4;
  TextVertex2D &v1 = vertexBuffer.at( index );
  v1.mX+= italicsBottomDisplacement;


  // top left
  index++;
  TextVertex2D &v2 = vertexBuffer.at( index );
  v2.mX+= italicsTopDisplacement;


  // top right
  index++;
  TextVertex2D &v3 = vertexBuffer.at( index );
  v3.mX+= italicsTopDisplacement;

  // bottom right
  index++;
  TextVertex2D &v4 = vertexBuffer.at( index );
  v4.mX+= italicsBottomDisplacement;
}

void AddUnderline( VertexBuffer& vertexBuffer,
                   const float totalWidth,
                   const float thickness,
                   const float yPosition,
                   const UvRect& uv )
{
  /*
   *  Add an underline to a string of text.
   *
   *
   *  A thin vertical slice of the underline character is stretched to the
   *  length of the string.
   *
   *  If we stretch the entire underline character (not a thin slice) then
   *  the rounded edges will be stretched as well, giving inconsistent results.
   *
   *   Underline glyph                     Only use a thin slice for texturing
   *
   *  |-------------------|           (u0,v2)|--------|X|---------|(u2,v2)
   *  |                   |                  |        |X|         |
   *  |   /-----------\   |                  |   /----|X|-----\   |
   *  |  |  underline  |  |                  |  |     |X|     |   |
   *  |   \___________/   |                  |   \____|X|_____/   |
   *  |                   |                  |        |X|         |
   *  |                   |                  |        |X|         |
   *  |-------------------|                  |--------|X|---------|
   *
   *                                      (u0,v0)    halfU       (u2,v0)
   *
   *  In calculation below
   *  HalfU = half way between u0 and u2. This gives a thin slice.
   *  So we use the texture-coordinates from  (halfU, v0) -> (halfU, v2).
   *
   *  End result is: A solid edge on the left / right side of the underline:
   *                 A smooth (anti-aliased) edge on the top / bottom of the underline
   */

  TextVertex2D v;

  // set U1,V1 for all vertices
  v.mU1 = 1.0f;
  v.mV1 = 1.0f;

  float halfU = (uv.u0 + uv.u2)/2.0f;

  /*
   * Create 4 vertices
   * 1 --- 2
   * |    /|
   * |  A  |
   * | /   |
   * 0 --- 3
   */

  // 0
  v.mX = 0.0f;
  v.mY = yPosition;
  v.mU = halfU;
  v.mV = uv.v2;
  vertexBuffer.push_back(v);

  // 1
  v.mX = 0.0f;
  v.mY = yPosition + thickness;
  v.mU = halfU;
  v.mV = uv.v0;
  vertexBuffer.push_back(v);

  // 2
  v.mX = totalWidth;
  v.mY = yPosition + thickness;
  v.mU = halfU;
  v.mV = uv.v0;
  vertexBuffer.push_back(v);

  // 3
  v.mX = totalWidth;
  v.mY = yPosition;
  v.mU = halfU;
  v.mV = uv.v2;
  vertexBuffer.push_back(v);
}

void GetAdjustedSize(float &charWidth,
                     float &charHeight,
                     float &left,
                     float &top,
                     float padAdjustX,
                     float padAdjustY,
                     float scalar,
                     const GlyphMetric& glyph)
{
  charWidth  = (glyph.GetWidth()  + padAdjustX * 2.0f) * scalar;
  charHeight = (glyph.GetHeight() + padAdjustY * 2.0f) * scalar;
  left = (glyph.GetLeft() - padAdjustX) * scalar;
  top  = (glyph.GetTop()  + padAdjustY) * scalar;
}

#ifdef DEBUG_VERTS

void DebugVertexBuffer( VertexBuffer& buffer )
{
  for (std::size_t i = 0, size = buffer.size(); i< size ; ++i)
  {
    TextVertex2D &v = buffer.at( i );
    printf("%d: xyuv =, %f , %f, %f, %f  \n", (unsigned int) i, v.mX,v.mY, v.mU, v.mV);
  }
}
#endif

} // unnamed namespace

TextVertexBuffer* TextVertexGenerator::Generate(const TextArray& text,
                           const TextFormat& format,
                           const FontMetricsInterface& metrics,
                           const AtlasUvInterface& uvInterface,
                           const FontId fontId)

{
  TextVertexBuffer* textVertexBuffer = new TextVertexBuffer;
  VertexBuffer &vertexBuffer(textVertexBuffer->mVertices);

  const GlyphMetric* glyph( NULL );
  float xPos( 0.0f );
  float yPos( 0.0f );
  float underlineWidth( 0.0f );
  float totalWidth( 0.0f );
  float charWidth( 0.0f );
  float charHeight( 0.0f );
  float left(0.0f);
  float top(0.0f);

  float scalar = metrics.GetUnitsToPixels( format.GetPointSize() );

  // Italics displacement
  // the text is rendered upside down
  const float sinAngle = format.IsItalic() ? std::sin( format.GetItalicsAngle() ) : 0.0f;

  // get the line height and ascender from the font
  const float lineHeight( metrics.GetLineHeight() * scalar );
  const float ascender( metrics.GetAscender() * scalar );
  const float padAdjustX( metrics.GetPadAdjustX() );
  const float padAdjustY( metrics.GetPadAdjustY() );
  const float tileWidth( metrics.GetMaxWidth() * scalar );
  const float tileHeight( metrics.GetMaxHeight() * scalar );
  unsigned int textSize = text.size();

  for (unsigned int i = 0; i < textSize; ++i)
  {
    // buffer is always filled starting from the first vector position. However text characters are visited from left to right or from right to left.
    uint32_t charIndex = text[ ( format.IsLeftToRight() ? i : ( textSize - 1 - i ) ) ];

    glyph = metrics.GetGlyph( charIndex );

    if (charIndex >= SpecialCharacters::FIRST_VISIBLE_CHAR && glyph )
    {
      // get char size and offset adjusted for padding in the atlas
      GetAdjustedSize(charWidth, charHeight, left, top, padAdjustX, padAdjustY, scalar, *glyph );

      yPos = (ascender - top);
      xPos += left;

      // a combination of character index and font id is used to uniquely identify the character
      unsigned int encodedChar = GlyphStatus::GetEncodedValue( charIndex, fontId );
      UvRect uv = uvInterface.GetUvCoordinates( encodedChar );

      const Vector2 uvShadow( tileWidth / charWidth, tileHeight / charHeight );

      AddVertex( vertexBuffer, xPos, yPos, charWidth, charHeight, uv, uvShadow );

      if( format.IsItalic() )
      {
        float italicsTopDisplacement = ( top - charHeight ) * sinAngle;
        float italicsBottomDisplacement = top * sinAngle;
        AdjustForItalics( vertexBuffer, italicsTopDisplacement, italicsBottomDisplacement);
      }

      xPos -= left;
    }

    if( glyph )
    {
      underlineWidth = std::max( underlineWidth, xPos + glyph->GetXAdvance() * scalar );
      xPos += glyph->GetXAdvance() * scalar;
      totalWidth = std::max(totalWidth, xPos);
    }
  } // for

  if( format.IsUnderLined() )
  {
    unsigned int encodedChar = GlyphStatus::GetEncodedValue( SpecialCharacters::UNDERLINE_CHARACTER, fontId );
    UvRect uv( uvInterface.GetUvCoordinates( encodedChar ));

    glyph = metrics.GetGlyph( SpecialCharacters::UNDERLINE_CHARACTER );

    if( glyph )
    {
      // Adjust uv coordinates for scaling within atlas tile
      GetAdjustedSize(charWidth, charHeight, left, top, padAdjustX, padAdjustY, scalar, *glyph );

      // Get underline thickness and position.
      // These values could be retrieved from the text-format, set to the text-actor through text-style,
      // or retrieved directly from the font metrics.
      float thickness = 0.f;
      float position = 0.f;

      if( fabs( format.GetUnderlineThickness() ) > Math::MACHINE_EPSILON_0 )
      {
        // Thickness and position retrieved from the format, which are passed to the
        // text-actor through the text-style, it adds the vertical pad adjust used to fit some effects like glow or shadow..
        thickness = -format.GetUnderlineThickness();
        position = format.GetUnderlinePosition();
      }
      else
      {
        // Thickness and position retrieved from the font metrics.
        // It adds the vertical pad adjust ( padAdjustY ) used to fit some effects like glow or shadow.
        thickness = -( metrics.GetUnderlineThickness() + 2.f * padAdjustY ) * scalar;
        position = ascender - ( metrics.GetUnderlinePosition() - padAdjustY ) * scalar;
      }
      AddUnderline( vertexBuffer, underlineWidth, thickness, position, uv );
    }
  }

  textVertexBuffer->mVertexMax = Vector2(totalWidth,lineHeight);

  RepositionData( vertexBuffer, textVertexBuffer->mVertexMax );

#ifdef DEBUG_VERTS
  DebugVertexBuffer( vertexBuffer );
#endif

  return textVertexBuffer;
}

} // namespace Internal

} // namespace Dali

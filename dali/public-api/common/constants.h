#ifndef DALI_CONSTANTS_H
#define DALI_CONSTANTS_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <math.h> // M_PI

// INTERNAL INCLUDES
#include <dali/public-api/math/compile-time-math.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>

namespace Dali
{
/**
 * @addtogroup dali_core_common
 * @{
 */

/**
 * @brief ParentOrigin constants.
 *
 * Note that for 2D layouting, the typical Z position is 0.5f (not 0.0f).
 * @SINCE_1_0.0
 */
namespace ParentOrigin
{
static constexpr float   TOP    = 0.0f;
static constexpr float   BOTTOM = 1.0f;
static constexpr float   LEFT   = 0.0f;
static constexpr float   RIGHT  = 1.0f;
static constexpr float   MIDDLE = 0.5f;
static constexpr Vector3 TOP_LEFT(0.0f, 0.0f, 0.5f);
static constexpr Vector3 TOP_CENTER(0.5f, 0.0f, 0.5f);
static constexpr Vector3 TOP_RIGHT(1.0f, 0.0f, 0.5f);
static constexpr Vector3 CENTER_LEFT(0.0f, 0.5f, 0.5f);
static constexpr Vector3 CENTER(0.5f, 0.5f, 0.5f);
static constexpr Vector3 CENTER_RIGHT(1.0f, 0.5f, 0.5f);
static constexpr Vector3 BOTTOM_LEFT(0.0f, 1.0f, 0.5f);
static constexpr Vector3 BOTTOM_CENTER(0.5f, 1.0f, 0.5f);
static constexpr Vector3 BOTTOM_RIGHT(1.0f, 1.0f, 0.5f);
static constexpr Vector3 DEFAULT(TOP_LEFT);
} // namespace ParentOrigin

/**
 * @brief AnchorPoint constants.
 *
 * Note that for 2D layouting, the typical Z position is 0.5f (not 0.0f).
 * @SINCE_1_0.0
 */
namespace AnchorPoint
{
static constexpr float   TOP    = 0.0f;
static constexpr float   BOTTOM = 1.0f;
static constexpr float   LEFT   = 0.0f;
static constexpr float   RIGHT  = 1.0f;
static constexpr float   MIDDLE = 0.5f;
static constexpr Vector3 TOP_LEFT(0.0f, 0.0f, 0.5f);
static constexpr Vector3 TOP_CENTER(0.5f, 0.0f, 0.5f);
static constexpr Vector3 TOP_RIGHT(1.0f, 0.0f, 0.5f);
static constexpr Vector3 CENTER_LEFT(0.0f, 0.5f, 0.5f);
static constexpr Vector3 CENTER(0.5f, 0.5f, 0.5f);
static constexpr Vector3 CENTER_RIGHT(1.0f, 0.5f, 0.5f);
static constexpr Vector3 BOTTOM_LEFT(0.0f, 1.0f, 0.5f);
static constexpr Vector3 BOTTOM_CENTER(0.5f, 1.0f, 0.5f);
static constexpr Vector3 BOTTOM_RIGHT(1.0f, 1.0f, 0.5f);
static constexpr Vector3 DEFAULT(CENTER);
} // namespace AnchorPoint

/**
 * @brief Color Constants.
 *
 * Color is represented by the Vector4 class (see vector4.h).
 * @SINCE_1_0.0
 */
namespace Color
{
// Subset of X11 Colors (CSS colors)
// https://www.w3.org/TR/css-color-3
static constexpr Vector4 ALICE_BLUE(240.0f, 248.0f / 255.0f, 1.0f, 1.0f);                            //#F0F8FF
static constexpr Vector4 ANTIQUE_WHITE(250.0f, 235.0f / 255.0f, 215.0f / 255.0f, 1.0f);              //#FAEBD7
static constexpr Vector4 AQUA(1.0f, 1.0f, 1.0f, 1.0f);                                               //#00FFFF
static constexpr Vector4 AQUA_MARINE(127.0f / 255.0f, 1.0f, 212.0f / 255.0f, 1.0f);                  //#7FFFD4
static constexpr Vector4 AZURE(240.0f, 1.0f, 1.0f, 1.0f);                                            //#F0FFFF
static constexpr Vector4 BEIGE(245.0f / 255.0f, 245.0f / 255.0f, 220.0f, 1.0f);                      //#F5F5DC
static constexpr Vector4 BISQUE(1.0f, 228.0f / 255.0f, 196.0f / 255.0f, 1.0f);                       //#FFE4C4
static constexpr Vector4 BLACK(0.0f, 0.0f, 0.0f, 1.0f);                                              //#000000
static constexpr Vector4 BLANCHE_DALMOND(1.0f, 235.0f / 255.0f, 205.0f / 255.0f, 1.0f);              //#FFEBCD
static constexpr Vector4 BLUE(0.0f, 0.0f, 1.0f, 1.0f);                                               //#0000FF
static constexpr Vector4 BLUE_VIOLET(138.0f / 255.0f, 43.0f / 255.0f, 226.0f / 255.0f, 1.0f);        //#8A2BE2
static constexpr Vector4 BROWN(165.0f / 255.0f, 42.0f / 255.0f, 42.0f / 255.0f, 1.0f);               //#A52A2A
static constexpr Vector4 BURLY_WOOD(222.0f / 255.0f, 184.0f / 255.0f, 135.0f / 255.0f, 1.0f);        //#DEB887
static constexpr Vector4 CADET_BLUE(95.0f / 255.0f, 158.0f / 255.0f, 160.0f, 1.0f);                  //#5F9EA0
static constexpr Vector4 CHARTREUSE(127.0f / 255.0f, 1.0f, 0.0f, 1.0f);                              //#7FFF00
static constexpr Vector4 CHOCOLATE(210.0f, 105.0f / 255.0f, 30.0f, 1.0f);                            //#D2691E
static constexpr Vector4 CORAL(1.0f, 127.0f / 255.0f, 80.0f, 1.0f);                                  //#FF7F50
static constexpr Vector4 CORNFLOWER_BLUE(100.0f, 149.0f / 255.0f, 237.0f / 255.0f, 1.0f);            //#6495ED
static constexpr Vector4 CORNSILK(1.0f, 248.0f / 255.0f, 220.0f, 1.0f);                              //#FFF8DC
static constexpr Vector4 CRIMSON(220.0f, 20.0f, 60.0f, 1.0f);                                        //#DC143C
static constexpr Vector4 CYAN(0.0f, 1.0f, 1.0f, 1.0f);                                               //#00FFFF
static constexpr Vector4 DARK_BLUE(0.0f, 0.0f, 139.0f / 255.0f, 1.0f);                               //#00008B
static constexpr Vector4 DARK_CYAN(0.0f, 139.0f / 255.0f, 139.0f / 255.0f, 1.0f);                    //#008B8B
static constexpr Vector4 DARK_GOLDENROD(184.0f / 255.0f, 134.0f / 255.0f, 11.0f / 255.0f, 1.0f);     //#B8860B
static constexpr Vector4 DARK_GRAY(169.0f / 255.0f, 169.0f / 255.0f, 169.0f / 255.0f, 1.0f);         //#A9A9A9
static constexpr Vector4 DARK_GREEN(0.0f, 100.0f, 0.0f, 1.0f);                                       //#006400
static constexpr Vector4 DARK_GREY(169.0f / 255.0f, 169.0f / 255.0f, 169.0f / 255.0f, 1.0f);         //#A9A9A9
static constexpr Vector4 DARK_KHAKI(189.0f / 255.0f, 183.0f / 255.0f, 107.0f / 255.0f, 1.0f);        //#BDB76B
static constexpr Vector4 DARK_MAGENTA(139.0f / 255.0f, 0.0f, 139.0f / 255.0f, 1.0f);                 //#8B008B
static constexpr Vector4 DARK_OLIVE_GREEN(85.0f / 255.0f, 107.0f / 255.0f, 47.0f / 255.0f, 1.0f);    //#556B2F
static constexpr Vector4 DARK_ORANGE(1.0f, 140.0f, 0.0f, 1.0f);                                      //#FF8C00
static constexpr Vector4 DARK_ORCHID(153.0f / 255.0f, 50.0f, 204.0f / 255.0f, 1.0f);                 //#9932CC
static constexpr Vector4 DARK_RED(139.0f / 255.0f, 0.0f, 0.0f, 1.0f);                                //#8B0000
static constexpr Vector4 DARK_SALMON(233.0f / 255.0f, 150.0f, 122.0f / 255.0f, 1.0f);                //#E9967A
static constexpr Vector4 DARK_SEA_GREEN(143.0f / 255.0f, 188.0f / 255.0f, 143.0f / 255.0f, 1.0f);    //#8FBC8F
static constexpr Vector4 DARK_SLATE_BLUE(72.0f / 255.0f, 61.0f / 255.0f, 139.0f / 255.0f, 1.0f);     //#483D8B
static constexpr Vector4 DARK_SLATE_GRAY(47.0f / 255.0f, 79.0f / 255.0f, 79.0f / 255.0f, 1.0f);      //#2F4F4F
static constexpr Vector4 DARK_SLATE_GREY(47.0f / 255.0f, 79.0f / 255.0f, 79.0f / 255.0f, 1.0f);      //#2F4F4F
static constexpr Vector4 DARK_TURQUOISE(0.0f, 206.0f / 255.0f, 209.0f / 255.0f, 1.0f);               //#00CED1
static constexpr Vector4 DARK_VIOLET(148.0f / 255.0f, 0.0f, 211.0f / 255.0f, 1.0f);                  //#9400D3
static constexpr Vector4 DEEP_PINK(1.0f, 20.0f, 147.0f / 255.0f, 1.0f);                              //#FF1493
static constexpr Vector4 DEEP_SKY_BLUE(0.0f, 191.0f / 255.0f, 1.0f, 1.0f);                           //#00BFFF
static constexpr Vector4 DIM_GRAY(105.0f / 255.0f, 105.0f / 255.0f, 105.0f / 255.0f, 1.0f);          //#696969
static constexpr Vector4 DIM_GREY(105.0f / 255.0f, 105.0f / 255.0f, 105.0f / 255.0f, 1.0f);          //#696969
static constexpr Vector4 DODGER_BLUE(30.0f, 144.0f / 255.0f, 1.0f, 1.0f);                            //#1E90FF
static constexpr Vector4 FIRE_BRICK(178.0f / 255.0f, 34.0f / 255.0f, 34.0f / 255.0f, 1.0f);          //#B22222
static constexpr Vector4 FLORAL_WHITE(1.0f, 250.0f, 240.0f, 1.0f);                                   //#FFFAF0
static constexpr Vector4 FOREST_GREEN(34.0f / 255.0f, 139.0f / 255.0f, 34.0f / 255.0f, 1.0f);        //#228B22
static constexpr Vector4 FUCHSIA(1.0f, 0.0f, 1.0f, 1.0f);                                            //#FF00FF
static constexpr Vector4 GAINSBORO(220.0f, 220.0f, 220.0f, 1.0f);                                    //#DCDCDC
static constexpr Vector4 GHOST_WHITE(248.0f / 255.0f, 248.0f / 255.0f, 1.0f, 1.0f);                  //#F8F8FF
static constexpr Vector4 GOLD(1.0f, 215.0f / 255.0f, 0.0f, 1.0f);                                    //#FFD700
static constexpr Vector4 GOLDEN_ROD(218.0f / 255.0f, 165.0f / 255.0f, 32.0f / 255.0f, 1.0f);         //#DAA520
static constexpr Vector4 GRAY(190.0f / 255.0f, 190.0f / 255.0f, 190.0f / 255.0f, 1.0f);              //#BEBEBE
static constexpr Vector4 GREEN(0.0f, 1.0f, 0.0f, 1.0f);                                              //#00FF00
static constexpr Vector4 GREEN_YELLOW(173.0f / 255.0f, 1.0f, 47.0f / 255.0f, 1.0f);                  //#ADFF2F
static constexpr Vector4 GREY(128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f);              //#808080
static constexpr Vector4 HONEYDEW(240.0f, 1.0f, 240.0f, 1.0f);                                       //#F0FFF0
static constexpr Vector4 HOT_PINK(1.0f, 105.0f / 255.0f, 180.0f, 1.0f);                              //#FF69B4
static constexpr Vector4 INDIANRED(205.0f / 255.0f, 92.0f / 255.0f, 92.0f / 255.0f, 1.0f);           //#CD5C5C
static constexpr Vector4 INDIGO(75.0f / 255.0f, 0.0f, 130.0f, 1.0f);                                 //#4B0082
static constexpr Vector4 IVORY(1.0f, 1.0f, 240.0f, 1.0f);                                            //#FFFFF0
static constexpr Vector4 KHAKI(240.0f, 230.0f, 140.0f, 1.0f);                                        //#F0E68C
static constexpr Vector4 LAVENDER(230.0f, 230.0f, 250.0f, 1.0f);                                     //#E6E6FA
static constexpr Vector4 LAVENDER_BLUSH(1.0f, 240.0f, 245.0f / 255.0f, 1.0f);                        //#FFF0F5
static constexpr Vector4 LAWN_GREEN(124.0f / 255.0f, 252.0f / 255.0f, 0.0f, 1.0f);                   //#7CFC00
static constexpr Vector4 LEMON_CHIFFON(1.0f, 250.0f, 205.0f / 255.0f, 1.0f);                         //#FFFACD
static constexpr Vector4 LIGHT_BLUE(173.0f / 255.0f, 216.0f / 255.0f, 230.0f, 1.0f);                 //#ADD8E6
static constexpr Vector4 LIGHT_CORAL(240.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f);                //#F08080
static constexpr Vector4 LIGHT_CYAN(224.0f / 255.0f, 1.0f, 1.0f, 1.0f);                              //#E0FFFF
static constexpr Vector4 LIGHT_GOLDEN_ROD_YELLOW(250.0f, 250.0f, 210.0f, 1.0f);                      //#FAFAD2
static constexpr Vector4 LIGHT_GRAY(211.0f / 255.0f, 211.0f / 255.0f, 211.0f / 255.0f, 1.0f);        //#D3D3D3
static constexpr Vector4 LIGHT_GREEN(144.0f / 255.0f, 238.0f / 255.0f, 144.0f / 255.0f, 1.0f);       //#90EE90
static constexpr Vector4 LIGHT_GREY(211.0f / 255.0f, 211.0f / 255.0f, 211.0f / 255.0f, 1.0f);        //#D3D3D3
static constexpr Vector4 LIGHT_PINK(1.0f, 182.0f / 255.0f, 193.0f / 255.0f, 1.0f);                   //#FFB6C1
static constexpr Vector4 LIGHT_SALMON(1.0f, 160.0f, 122.0f / 255.0f, 1.0f);                          //#FFA07A
static constexpr Vector4 LIGHT_SEA_GREEN(32.0f / 255.0f, 178.0f / 255.0f, 170.0f, 1.0f);             //#20B2AA
static constexpr Vector4 LIGHT_SKY_BLUE(135.0f / 255.0f, 206.0f / 255.0f, 250.0f, 1.0f);             //#87CEFA
static constexpr Vector4 LIGHT_SLATE_GRAY(119.0f / 255.0f, 136.0f / 255.0f, 153.0f / 255.0f, 1.0f);  //#778899
static constexpr Vector4 LIGHT_SLATE_GREY(119.0f / 255.0f, 136.0f / 255.0f, 153.0f / 255.0f, 1.0f);  //#778899
static constexpr Vector4 LIGHT_STEEL_BLUE(176.0f / 255.0f, 196.0f / 255.0f, 222.0f / 255.0f, 1.0f);  //#B0C4DE
static constexpr Vector4 LIGHT_YELLOW(1.0f, 1.0f, 224.0f / 255.0f, 1.0f);                            //#FFFFE0
static constexpr Vector4 LIME(0.0f, 1.0f, 0.0f, 1.0f);                                               //#00FF00
static constexpr Vector4 LIME_GREEN(50.0f, 205.0f / 255.0f, 50.0f, 1.0f);                            //#32CD32
static constexpr Vector4 LINEN(250.0f, 240.0f, 230.0f, 1.0f);                                        //#FAF0E6
static constexpr Vector4 MAGENTA(1.0f, 0.0f, 1.0f, 1.0f);                                            //#FF00FF
static constexpr Vector4 MAROON(176.0f / 255.0f, 48.0f / 255.0f, 96.0f / 255.0f, 1.0f);              //#B03060
static constexpr Vector4 MEDIUM_AQUA_MARINE(102.0f / 255.0f, 205.0f / 255.0f, 170.0f, 1.0f);         //#66CDAA
static constexpr Vector4 MEDIUM_BLUE(0.0f, 0.0f, 205.0f / 255.0f, 1.0f);                             //#0000CD
static constexpr Vector4 MEDIUM_ORCHID(186.0f / 255.0f, 85.0f / 255.0f, 211.0f / 255.0f, 1.0f);      //#BA55D3
static constexpr Vector4 MEDIUM_PURPLE(147.0f / 255.0f, 112.0f / 255.0f, 219.0f / 255.0f, 1.0f);     //#9370DB
static constexpr Vector4 MEDIUM_SEA_GREEN(60.0f, 179.0f / 255.0f, 113.0f / 255.0f, 1.0f);            //#3CB371
static constexpr Vector4 MEDIUM_SLATE_BLUE(123.0f / 255.0f, 104.0f / 255.0f, 238.0f / 255.0f, 1.0f); //#7B68EE
static constexpr Vector4 MEDIUM_SPRING_GREEN(0.0f, 250.0f, 154.0f / 255.0f, 1.0f);                   //#00FA9A
static constexpr Vector4 MEDIUM_TURQUOISE(72.0f / 255.0f, 209.0f / 255.0f, 204.0f / 255.0f, 1.0f);   //#48D1CC
static constexpr Vector4 MEDIUM_VIOLETRED(199.0f / 255.0f, 21.0f / 255.0f, 133.0f / 255.0f, 1.0f);   //#C71585
static constexpr Vector4 MIDNIGHT_BLUE(25.0f / 255.0f, 25.0f / 255.0f, 112.0f / 255.0f, 1.0f);       //#191970
static constexpr Vector4 MINT_CREAM(245.0f / 255.0f, 1.0f, 250.0f, 1.0f);                            //#F5FFFA
static constexpr Vector4 MISTY_ROSE(1.0f, 228.0f / 255.0f, 225.0f / 255.0f, 1.0f);                   //#FFE4E1
static constexpr Vector4 MOCCASIN(1.0f, 228.0f / 255.0f, 181.0f / 255.0f, 1.0f);                     //#FFE4B5
static constexpr Vector4 NAVAJO_WHITE(1.0f, 222.0f / 255.0f, 173.0f / 255.0f, 1.0f);                 //#FFDEAD
static constexpr Vector4 NAVY(0.0f, 0.0f, 128.0f / 255.0f, 1.0f);                                    //#000080
static constexpr Vector4 OLD_LACE(253.0f / 255.0f, 245.0f / 255.0f, 230.0f, 1.0f);                   //#FDF5E6
static constexpr Vector4 OLIVE(128.0f / 255.0f, 128.0f / 255.0f, 0.0f, 1.0f);                        //#808000
static constexpr Vector4 OLIVE_DRAB(107.0f / 255.0f, 142.0f / 255.0f, 35.0f / 255.0f, 1.0f);         //#6B8E23
static constexpr Vector4 ORANGE(1.0f, 165.0f / 255.0f, 0.0f, 1.0f);                                  //#FFA500
static constexpr Vector4 ORANGE_RED(1.0f, 69.0f / 255.0f, 0.0f, 1.0f);                               //#FF4500
static constexpr Vector4 ORCHID(218.0f / 255.0f, 112.0f / 255.0f, 214.0f / 255.0f, 1.0f);            //#DA70D6
static constexpr Vector4 PALE_GOLDEN_ROD(238.0f / 255.0f, 232.0f / 255.0f, 170.0f, 1.0f);            //#EEE8AA
static constexpr Vector4 PALE_GREEN(152.0f / 255.0f, 251.0f / 255.0f, 152.0f / 255.0f, 1.0f);        //#98FB98
static constexpr Vector4 PALE_TURQUOISE(175.0f / 255.0f, 238.0f / 255.0f, 238.0f / 255.0f, 1.0f);    //#AFEEEE
static constexpr Vector4 PALE_VIOLET_RED(219.0f / 255.0f, 112.0f / 255.0f, 147.0f / 255.0f, 1.0f);   //#DB7093
static constexpr Vector4 PAPAYA_WHIP(1.0f, 239.0f / 255.0f, 213.0f / 255.0f, 1.0f);                  //#FFEFD5
static constexpr Vector4 PEACH_PUFF(1.0f, 218.0f / 255.0f, 185.0f / 255.0f, 1.0f);                   //#FFDAB9
static constexpr Vector4 PERU(205.0f / 255.0f, 133.0f / 255.0f, 63.0f / 255.0f, 1.0f);               //#CD853F
static constexpr Vector4 PINK(1.0f, 192.0f / 255.0f, 203.0f / 255.0f, 1.0f);                         //#FFC0CB
static constexpr Vector4 PLUM(221.0f / 255.0f, 160.0f, 221.0f / 255.0f, 1.0f);                       //#DDA0DD
static constexpr Vector4 POWDER_BLUE(176.0f / 255.0f, 224.0f / 255.0f, 230.0f, 1.0f);                //#B0E0E6
static constexpr Vector4 PURPLE(160.0f / 255.0f, 32.0f, 240.0f / 255.0f, 1.0f);                      //#A020F0
static constexpr Vector4 RED(1.0f, 0.0f, 0.0f, 1.0f);                                                //#FF0000
static constexpr Vector4 ROSY_BROWN(188.0f / 255.0f, 143.0f / 255.0f, 143.0f / 255.0f, 1.0f);        //#BC8F8F
static constexpr Vector4 ROYAL_BLUE(65.0f / 255.0f, 105.0f / 255.0f, 225.0f / 255.0f, 1.0f);         //#4169E1
static constexpr Vector4 SADDLE_BROWN(139.0f / 255.0f, 69.0f / 255.0f, 19.0f / 255.0f, 1.0f);        //#8B4513
static constexpr Vector4 SALMON(250.0f, 128.0f / 255.0f, 114.0f / 255.0f, 1.0f);                     //#FA8072
static constexpr Vector4 SANDY_BROWN(244.0f / 255.0f, 164.0f / 255.0f, 96.0f / 255.0f, 1.0f);        //#F4A460
static constexpr Vector4 SEA_GREEN(46.0f / 255.0f, 139.0f / 255.0f, 87.0f / 255.0f, 1.0f);           //#2E8B57
static constexpr Vector4 SEA_SHELL(1.0f, 245.0f / 255.0f, 238.0f / 255.0f, 1.0f);                    //#FFF5EE
static constexpr Vector4 SIENNA(160.0f, 82.0f / 255.0f, 45.0f / 255.0f, 1.0f);                       //#A0522D
static constexpr Vector4 SILVER(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f, 1.0f);            //#C0C0C0
static constexpr Vector4 SKY_BLUE(135.0f / 255.0f, 206.0f / 255.0f, 235.0f / 255.0f, 1.0f);          //#87CEEB
static constexpr Vector4 SLATE_BLUE(106.0f / 255.0f, 90.0f, 205.0f / 255.0f, 1.0f);                  //#6A5ACD
static constexpr Vector4 SLATE_GRAY(112.0f / 255.0f, 128.0f / 255.0f, 144.0f / 255.0f, 1.0f);        //#708090
static constexpr Vector4 SLATE_GREY(112.0f / 255.0f, 128.0f / 255.0f, 144.0f / 255.0f, 1.0f);        //#708090
static constexpr Vector4 SNOW(1.0f, 250.0f, 250.0f, 1.0f);                                           //#FFFAFA
static constexpr Vector4 SPRING_GREEN(0.0f, 1.0f, 127.0f / 255.0f, 1.0f);                            //#00FF7F
static constexpr Vector4 STEEL_BLUE(70.0f, 130.0f, 180.0f, 1.0f);                                    //#4682B4
static constexpr Vector4 TAN(210.0f, 180.0f, 140.0f, 1.0f);                                          //#D2B48C
static constexpr Vector4 TEAL(0.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f);                         //#008080
static constexpr Vector4 THISTLE(216.0f / 255.0f, 191.0f / 255.0f, 216.0f / 255.0f, 1.0f);           //#D8BFD8
static constexpr Vector4 TOMATO(1.0f, 99.0f / 255.0f, 71.0f / 255.0f, 1.0f);                         //#FF6347
static constexpr Vector4 TRANSPARENT(0.0f, 0.0f, 0.0f, 0.0f);                                        //#000000
static constexpr Vector4 TURQUOISE(64.0f / 255.0f, 224.0f / 255.0f, 208.0f / 255.0f, 1.0f);          //#40E0D0
static constexpr Vector4 VIOLET(238.0f / 255.0f, 130.0f, 238.0f / 255.0f, 1.0f);                     //#EE82EE
static constexpr Vector4 WHEAT(245.0f / 255.0f, 222.0f / 255.0f, 179.0f / 255.0f, 1.0f);             //#F5DEB3
static constexpr Vector4 WHITE(1.0f, 1.0f, 1.0f, 1.0f);                                              //#FFFFFF
static constexpr Vector4 WHITE_SMOKE(245.0f / 255.0f, 245.0f / 255.0f, 245.0f / 255.0f, 1.0f);       //#F5F5F5
static constexpr Vector4 YELLOW(1.0f, 1.0f, 0.0f, 1.0f);                                             //#FFFF00
static constexpr Vector4 YELLOW_GREEN(154.0f / 255.0f, 205.0f / 255.0f, 50.0f, 1.0f);                //#9ACD32

} // namespace Color

/**
 * @brief Math constants.
 * @SINCE_1_0.0
 */
namespace Math
{
static constexpr float MACHINE_EPSILON_0     = Epsilon<0>::value;
static constexpr float MACHINE_EPSILON_1     = Epsilon<1>::value;
static constexpr float MACHINE_EPSILON_10    = Epsilon<10>::value;
static constexpr float MACHINE_EPSILON_100   = Epsilon<100>::value;
static constexpr float MACHINE_EPSILON_1000  = Epsilon<1000>::value;
static constexpr float MACHINE_EPSILON_10000 = Epsilon<10000>::value;

// float is preferred to double for performance on ARM targets
static constexpr float PI            = static_cast<float>(M_PI);   ///< Pi
static constexpr float PI_2          = static_cast<float>(M_PI_2); ///< Pi/2
static constexpr float PI_4          = static_cast<float>(M_PI_4); ///< Pi/4
static constexpr float PI_OVER_180   = Dali::Math::PI / 180.0f;    ///< Constant used to convert degree to radian
static constexpr float ONE80_OVER_PI = 180.0f / Dali::Math::PI;    ///< Constant used to convert radian to degree

} // namespace Math

// background colors
static constexpr Vector4 DEFAULT_BACKGROUND_COLOR = Color::BLACK;
static constexpr Vector4 DEBUG_BACKGROUND_COLOR(0.2f, 0.5f, 0.2f, 1.0f); ///< Dark green, useful for debugging visuals (texture / shader issues / culling etc.).

/**
 * @}
 */
} // namespace Dali

#endif // DALI_CONSTANTS_H

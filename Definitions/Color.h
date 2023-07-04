//*****************************************************************************
/// \copyright (c) 2016,2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \file Color.h
//*****************************************************************************
/// Color declarations
//******************************************************************************
#pragma once

#pragma region Includes
#pragma endregion Includes

namespace SvDef
{
#pragma region Declarations
typedef DWORD   COLORREF;

constexpr COLORREF Black = RGB(0, 0, 0); //0x00000000
constexpr COLORREF White = RGB(255, 255, 255); //0x00FFFFFF
constexpr COLORREF DarkBlue = RGB(0, 0, 192); //0x00C00000
constexpr COLORREF Blue = RGB(0, 0, 255); //0x00FF0000
constexpr COLORREF MediumBlue = RGB(0, 128, 255); //0x00FF8000
constexpr COLORREF LightBlue = RGB(0, 192, 255); //0x00FFC000
constexpr COLORREF Aqua = RGB(0, 255, 255); //0x00FFFF00
constexpr COLORREF DarkGreen = RGB(0, 128, 0); //0x00008000
constexpr COLORREF Green = RGB(0, 200, 0); //0x0000C800
constexpr COLORREF LightGreen = RGB(0, 255, 0); //0x0000FF00
constexpr COLORREF Cyan = RGB(0, 192, 192); //0x00C0C000
constexpr COLORREF LightCyan = RGB(0, 255, 255); //0x00FFFF00
constexpr COLORREF Red = RGB(192, 0, 0); //0x000000C0
constexpr COLORREF LightRed = RGB(255, 0, 0); //0x000000FF
constexpr COLORREF Magenta = RGB(192, 0, 192); //0x00C000C0
constexpr COLORREF LightMagenta = RGB(255, 0, 255); //0x00FF00FF
constexpr COLORREF Brown = RGB(143, 71, 71); //0x0047478F
constexpr COLORREF Yellow = RGB(255, 255, 0); //0x0000FFFF
constexpr COLORREF DarkYellow = RGB(200, 200, 0); //0x00C8C800
constexpr COLORREF Gold = RGB(255, 215, 0); //0x00FFD700
constexpr COLORREF DarkGray = RGB(128, 128, 128); //0x00808080
constexpr COLORREF MiddleGray = RGB(150, 150, 150); //0x00969696
constexpr COLORREF LightGray = RGB(192, 192, 192); //0x00C0C0C0
constexpr COLORREF WhiteSmoke = RGB(241, 241, 241); //0x00F1F1F1
constexpr COLORREF Orange = RGB(255, 112, 0); //0x000070FF
constexpr COLORREF Crimson = RGB(237, 24, 61);
constexpr COLORREF Plum = RGB(221, 160, 221);
constexpr COLORREF Slate = RGB(106, 90, 205);
constexpr COLORREF Aquamarine = RGB(127, 255, 212);
constexpr COLORREF Azure = RGB(240, 255, 255);
constexpr COLORREF Beige = RGB(245, 245, 220);
constexpr COLORREF Bisque = RGB(255, 228, 196);
constexpr COLORREF Charcoal = RGB(64, 64, 64);
constexpr COLORREF Chartreuse = RGB(127, 255, 0);
constexpr COLORREF Chocolate = RGB(210, 105, 30);
constexpr COLORREF Coral = RGB(255, 127, 80);
constexpr COLORREF Cornsilk = RGB(255, 248, 220);
constexpr COLORREF Fuchsia = RGB(255, 0, 255);
constexpr COLORREF Gainsboro = RGB(220, 220, 220);
constexpr COLORREF Indigo = RGB(75, 0, 130);
constexpr COLORREF Ivory = RGB(250, 255, 250);
constexpr COLORREF Khaki = RGB(240, 230, 140);
constexpr COLORREF Lavender = RGB(230, 230, 250);
constexpr COLORREF Lime = RGB(0, 255, 0);
constexpr COLORREF Linen = RGB(250, 240, 230);
constexpr COLORREF Maroon = RGB(128, 0, 0);
constexpr COLORREF Moccasin = RGB(255, 228, 181);
constexpr COLORREF Navy = RGB(0, 0, 128);
constexpr COLORREF Olive = RGB(128, 128, 0);
constexpr COLORREF Orchid = RGB(218, 112, 214);
constexpr COLORREF Peru = RGB(205, 133, 63);
constexpr COLORREF Pink = RGB(255, 192, 203);
constexpr COLORREF Purple = RGB(128, 0, 128);
constexpr COLORREF Salmon = RGB(250, 128, 114);
constexpr COLORREF Sienna = RGB(160, 82, 45);
constexpr COLORREF Tan = RGB(210, 180, 140);
constexpr COLORREF Teal = RGB(0, 128, 128);
constexpr COLORREF Thistle = RGB(216, 191, 216);
constexpr COLORREF Tomato = RGB(255, 99, 71);
constexpr COLORREF Turquoise = RGB(64, 224, 208);
constexpr COLORREF Violet = RGB(238, 130, 238);
constexpr COLORREF Wheat = RGB(245, 222, 179);
constexpr COLORREF PaleYellow = RGB(255, 255, 75);

constexpr COLORREF KoerberGray = RGB(204, 204, 204);
constexpr COLORREF StatusBarBackground = RGB(240, 240, 240);

constexpr COLORREF DefaultTextColor = Black;
constexpr COLORREF DefaultInactiveColor = Blue;
constexpr COLORREF DefaultFailedColor = LightRed;
constexpr COLORREF DefaultWarnedColor = DarkYellow;
constexpr COLORREF DefaultGoodColor = Green;
constexpr COLORREF DefaultSubFunctionColor1 = LightMagenta;
constexpr COLORREF DefaultSubFunctionColor2 = LightCyan;
constexpr COLORREF DefaultWhiteColor = White;
constexpr COLORREF DefaultBlackColor = Black;
constexpr COLORREF DefaultDisabledColor = DarkGray;
#pragma endregion Declarations
} //namespace SvDef

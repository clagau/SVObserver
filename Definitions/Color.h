//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file Color.h
//*****************************************************************************
/// Color declarations
//******************************************************************************
#pragma once

#pragma region Includes
#include <WinDef.h>
#pragma endregion Includes

namespace SvDef
{  
#pragma region Declarations
	const COLORREF Black        = RGB(  0,   0,   0); //0x00000000
	const COLORREF White        = RGB(255, 255, 255); //0x00FFFFFF
	const COLORREF DarkBlue     = RGB(  0,   0, 192); //0x00C00000
	const COLORREF Blue         = RGB(  0,   0, 255); //0x00FF0000
	const COLORREF MediumBlue   = RGB(  0, 128, 255); //0x00FF8000
	const COLORREF LightBlue    = RGB(  0, 192, 255); //0x00FFC000
	const COLORREF Aqua         = RGB(  0, 255, 255); //0x00FFFF00
	const COLORREF DarkGreen    = RGB(  0, 128,   0); //0x00008000
	const COLORREF Green        = RGB(  0, 200,   0); //0x0000C800
	const COLORREF LightGreen   = RGB(  0, 255,   0); //0x0000FF00
	const COLORREF Cyan         = RGB(  0, 192, 192); //0x00C0C000
	const COLORREF LightCyan    = RGB(  0, 255, 255); //0x00FFFF00
	const COLORREF Red          = RGB(192,   0,   0); //0x000000C0
	const COLORREF LightRed     = RGB(255,   0,   0); //0x000000FF
	const COLORREF Magenta      = RGB(192,   0, 192); //0x00C000C0
	const COLORREF LightMagenta = RGB(255,   0, 255); //0x00FF00FF
	const COLORREF Brown        = RGB(143,  71,  71); //0x0047478F
	const COLORREF Yellow       = RGB(255, 255,   0); //0x0000FFFF
	const COLORREF DarkYellow	= RGB(200, 200,   0); //0x00C8C800
	const COLORREF Gold			= RGB(255, 215,   0); //0x00FFD700
	const COLORREF DarkGray     = RGB(128, 128, 128); //0x00808080
	const COLORREF MiddleGray	= RGB(150, 150, 150); //0x00969696
	const COLORREF LightGray    = RGB(192, 192, 192); //0x00C0C0C0
	const COLORREF WhiteSmoke   = RGB(241, 241, 241); //0x00F1F1F1
	const COLORREF Orange       = RGB(255, 112,   0); //0x000070FF
	const COLORREF Crimson		= RGB(237,  24,  61);
	const COLORREF Plum			= RGB(221, 160, 221);
	const COLORREF Slate		= RGB(106,  90, 205);
	const COLORREF Aquamarine	= RGB(127, 255, 212);
	const COLORREF Azure		= RGB(240, 255, 255);
	const COLORREF Beige		= RGB(245, 245, 220);
	const COLORREF Bisque		= RGB(255, 228, 196);
	const COLORREF Charcoal		= RGB( 64,  64,  64);
	const COLORREF Chartreuse	= RGB(127, 255,   0);
	const COLORREF Chocolate	= RGB(210, 105,  30);
	const COLORREF Coral		= RGB(255, 127,  80);
	const COLORREF Cornsilk		= RGB(255, 248, 220);
	const COLORREF Fuchsia		= RGB(255,   0, 255);
	const COLORREF Gainsboro	= RGB(220, 220, 220);
	const COLORREF Indigo		= RGB( 75,   0, 130);
	const COLORREF Ivory		= RGB(250, 255, 250);
	const COLORREF Khaki		= RGB(240, 230, 140);
	const COLORREF Lavender		= RGB(230, 230, 250);
	const COLORREF Lime			= RGB(  0, 255,   0);
	const COLORREF Linen		= RGB(250, 240, 230);
	const COLORREF Maroon		= RGB(128,   0,   0);
	const COLORREF Moccasin		= RGB(255, 228, 181);
	const COLORREF Navy			= RGB(  0,   0, 128);
	const COLORREF Olive		= RGB(128, 128,   0);
	const COLORREF Orchid		= RGB(218, 112, 214);
	const COLORREF Peru			= RGB(205, 133,  63);
	const COLORREF Pink			= RGB(255, 192, 203);
	const COLORREF Purple		= RGB(128,   0, 128);
	const COLORREF Salmon		= RGB(250, 128, 114);
	const COLORREF Sienna		= RGB(160, 82,   45);
	const COLORREF Tan			= RGB(210, 180, 140);
	const COLORREF Teal			= RGB(  0, 128, 128);
	const COLORREF Thistle		= RGB(216, 191, 216);
	const COLORREF Tomato		= RGB(255,  99,  71);
	const COLORREF Turquoise	= RGB( 64, 224, 208);
	const COLORREF Violet		= RGB(238, 130, 238);
	const COLORREF Wheat		= RGB(245, 222, 179);
	const COLORREF PaleYellow	= RGB(255, 255, 75);

	const COLORREF KoerberGray	= RGB(204, 204, 204);

	const COLORREF DefaultTextColor			= Black;
	const COLORREF DefaultInactiveColor		= Blue;
	const COLORREF DefaultFailedColor		= LightRed;
	const COLORREF DefaultWarnedColor		= DarkYellow;
	const COLORREF DefaultGoodColor			= Green;
	const COLORREF DefaultSubFunctionColor1 = LightMagenta;
	const COLORREF DefaultSubFunctionColor2 = LightCyan;
	const COLORREF DefaultWhiteColor		= White;
	const COLORREF DefaultBlackColor		= Black;
	const COLORREF DefaultDisabledColor		= DarkGray;
#pragma endregion Declarations
} //namespace SvDef

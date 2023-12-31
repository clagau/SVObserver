//*****************************************************************************
/// \copyright COPYRIGHT (c) 2016, 2016 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// Contains implementations of utility functions for image file operations
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <fstream>
#include "ImageFileUtilities.h"
#pragma endregion Includes

bool CreateBmpFileFromHdc(const char* FilePath, HDC Context, RECT Area, uint16_t BitsPerPixel)
{
	bool bRetVal = false;
	uint32_t Width = Area.right - Area.left;
	uint32_t Height = Area.bottom - Area.top;
	BITMAPINFO Info;
	BITMAPFILEHEADER Header;
	memset(&Info, 0, sizeof(Info));
	memset(&Header, 0, sizeof(Header));
	Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	Info.bmiHeader.biWidth = Width;
	Info.bmiHeader.biHeight = Height;
	Info.bmiHeader.biPlanes = 1;
	Info.bmiHeader.biBitCount = BitsPerPixel;
	Info.bmiHeader.biCompression = BI_RGB;
	Info.bmiHeader.biSizeImage = Width * Height * (BitsPerPixel > 24 ? 4 : 3);
	Header.bfType = 0x4D42;
	Header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	char* Pixels = nullptr;
	HDC MemDC = CreateCompatibleDC(Context);
	HBITMAP Section = CreateDIBSection(Context, &Info, DIB_RGB_COLORS, (void**)&Pixels, 0, 0);
	if (Section)
	{
		DeleteObject(SelectObject(MemDC, Section));
		BitBlt(MemDC, 0, 0, Width, Height, Context, Area.left, Area.top, SRCCOPY);
		DeleteDC(MemDC);
		std::fstream hFile(FilePath, std::ios::out | std::ios::binary);
		if (hFile.is_open())
		{
			hFile.write((char*)&Header, sizeof(Header));
			hFile.write((char*)&Info.bmiHeader, sizeof(Info.bmiHeader));
			hFile.write(Pixels, (((BitsPerPixel * Width + 31) & ~31) / 8) * Height);
			hFile.close();
		
			bRetVal = true;
		}
		DeleteObject(Section);
	}
	return bRetVal;
}

//******************************************************************************
/// \copyright (c) 2018 by Seidenader Maschinenbau GmbH
/// \file Overlay.h
/// All Rights Reserved
//******************************************************************************
/// Function for Overlays
//******************************************************************************

#include "Overlay.h"

namespace SvPb
{
	unsigned char* generateBmpString(SvPb::OverlayShape& rShape, const RECT& rec)
	{
		long height{ rec.bottom - rec.top };
		long width{ rec.right - rec.left };
		auto& imageShape = *rShape.mutable_image();
		imageShape.mutable_x()->set_value(rec.top);
		imageShape.mutable_y()->set_value(rec.left);
		imageShape.mutable_w()->set_value(width);
		imageShape.mutable_h()->set_value(height);
		auto& image = *imageShape.mutable_image();
		return generateBmpString(image, width, height);
	}

	unsigned char* generateBmpString(SvPb::Image& image, long width, long height)
	{
		image.set_height(height);
		image.set_width(width);
		image.set_type("image/bmp");
		auto& bmp = *image.mutable_rgbdata();

		int bmpHeaderSize = 14;
		int infoHeaderSize = sizeof(BITMAPV5HEADER);
		int dataOffset = bmpHeaderSize + infoHeaderSize;
		int fileSize = bmpHeaderSize + infoHeaderSize + (width * height * 4);

		bmp.resize(fileSize, '\0');
		char* buf = const_cast<char*>(bmp.c_str());

		buf[0] = 'B';
		buf[1] = 'M';
		*reinterpret_cast<UINT32*>(buf + 2) = fileSize;
		*reinterpret_cast<UINT32*>(buf + 6) = 0;
		*reinterpret_cast<UINT32*>(buf + 10) = dataOffset;

		BITMAPV5HEADER& bmpheader = *reinterpret_cast<BITMAPV5HEADER*>(buf + bmpHeaderSize);
		bmpheader.bV5Size = sizeof(BITMAPV5HEADER);
		bmpheader.bV5Width = width;
		bmpheader.bV5Height = height;
		bmpheader.bV5Planes = 1;
		bmpheader.bV5BitCount = 32;
		bmpheader.bV5Compression = BI_BITFIELDS;
		bmpheader.bV5SizeImage = width * height * 4;
		bmpheader.bV5RedMask = 0x00FF0000;
		bmpheader.bV5GreenMask = 0x0000FF00;
		bmpheader.bV5BlueMask = 0x000000FF;
		bmpheader.bV5AlphaMask = 0xFF000000;
		bmpheader.bV5CSType = 0x57696e20; // LCS_WINDOWS_COLOR_SPACE
		bmpheader.bV5Intent = LCS_GM_BUSINESS;

		return reinterpret_cast<unsigned char*>(buf + dataOffset);
	}
}
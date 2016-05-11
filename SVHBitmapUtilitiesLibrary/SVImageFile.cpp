//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageFile
//* .File Name       : $Workfile:   SVImageFile.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:11:14  $
//******************************************************************************
#include "stdafx.h"
#include "SVImageFile.h"

SVImageFile::SVImageFile()
: m_bitmapHandle(0)
{
}

SVImageFile::~SVImageFile()
{
	Destroy();
}

HBITMAP SVImageFile::GetBitmapHandle() const
{
	return m_bitmapHandle;
}

SIZE SVImageFile::GetSize() const
{
	SIZE size = { 0, 0 };
	BITMAP bmp;
	if (::GetObject(m_bitmapHandle, sizeof(BITMAP), &bmp))
	{
		size.cx = bmp.bmWidth;
		size.cy = bmp.bmHeight;
	}
	return size;
}

int SVImageFile::GetBitDepth() const
{
	int bitDepth = 0;
	BITMAP bmp;
	if (::GetObject(m_bitmapHandle, sizeof(BITMAP), &bmp))
	{
		bitDepth = bmp.bmBitsPixel;
	}
	return bitDepth;
}

const unsigned char* SVImageFile::GetBits() const
{
	const unsigned char* pBits = nullptr;

	DIBSECTION dib;
	if (::GetObject(m_bitmapHandle, sizeof(DIBSECTION), &dib))
	{
		pBits = static_cast<const unsigned char*>(dib.dsBm.bmBits);
	}
	return pBits;
}

SVImageFile& SVImageFile::operator=(HBITMAP bmpHandle)
{
	if (bmpHandle != m_bitmapHandle)
	{
		Destroy();
		m_bitmapHandle = bmpHandle;
	}
	return *this;
}

SVImageFile::operator HBITMAP()
{
	return m_bitmapHandle;
}

void SVImageFile::Destroy()
{
	if (m_bitmapHandle)
	{
		::DeleteObject(m_bitmapHandle);
		m_bitmapHandle = 0;
	}
}


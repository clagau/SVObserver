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
	const unsigned char* pBits = NULL;

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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVHBitmapUtilitiesLibrary\SVImageFile.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:11:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:41:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
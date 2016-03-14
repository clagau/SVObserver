//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedImage
//* .File Name       : $Workfile:   SVSharedImage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:08:34  $
//******************************************************************************

#include "stdafx.h"
#include "SVSharedImage.h"

SVSharedImage::SVSharedImage(const void_allocator& rAlloc)
: m_Status(E_FAIL)
, m_Allocator(rAlloc)
, m_Filename("", rAlloc)
{
}

SVSharedImage::SVSharedImage(const char* Filename, int Status, const void_allocator& rAlloc)
: m_Status(Status)
, m_Allocator(rAlloc)
, m_Filename(Filename, rAlloc)
{
}

SVSharedImage::SVSharedImage(const SVSharedImage& rData)
: m_Status(rData.m_Status)
, m_Allocator(rData.m_Allocator)
, m_Filename(rData.m_Filename.c_str(), rData.m_Allocator)
{
}

const SVSharedImage& SVSharedImage::operator=(const SVSharedImage& rData)
{
	if (this != &rData)
	{
		m_Filename = rData.m_Filename;
		m_Status = rData.m_Status;
	}
	return *this;
}

std::string SVSharedImage::filename(const std::string& rName, long SlotNumber, img::type ImageType, bool bReject)
{
	static const char *ext[] = { ".bmp", ".png", ".jpg" };

	std::stringstream ret;
	if (bReject)
	{
		ret << rName << "." << SlotNumber << "." << "REJECT" << ext[ImageType];
	}
	else
	{
		ret << rName << "." << SlotNumber << ext[ImageType];
	}
	return ret.str();
}

img::type SVSharedImage::GetImageType(const std::string& rName)
{
	img::type imgType = img::bmp;
	std::string baseName = rName;
	// strip off all but last extension
	return imgType;
}


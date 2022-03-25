//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageFile
//* .File Name       : $Workfile:   SVImageFile.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:11:16  $
//******************************************************************************

#pragma once

class SVImageFile
{
private:
	HBITMAP m_bitmapHandle;

public:
	SVImageFile();
	~SVImageFile();

	HBITMAP GetBitmapHandle() const;
	SIZE GetSize() const;
	int GetBitDepth() const;
	const unsigned char* GetBits() const;

	SVImageFile& operator=(HBITMAP bmpHandle);
	operator HBITMAP();

	void Destroy();
};


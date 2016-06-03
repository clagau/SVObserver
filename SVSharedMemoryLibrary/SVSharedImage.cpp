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
#pragma region Includes
#include "stdafx.h"
#include "SVSharedImage.h"
#pragma endregion Includes

namespace Seidenader { namespace SVSharedMemoryLibrary
{
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


	void  SVSharedImage::BuildImageFileName(LPTSTR  filename, int size,  LPCTSTR name, long slotnumber, bool bReject, SVMatroxFileTypeEnum type )
	{
		TCHAR buffer[65];
		_itot_s(slotnumber,buffer,65, 10);
		_tcscpy_s(filename,size,name);
		_tcscat_s(filename,size, _T(".") );
		_tcscat_s(filename,size, buffer );
		if(bReject)
		{
			switch (type)
			{

			case  SVFileMIL:
				_tcscat_s( filename,size, _T(".REJECT.mil")  );
				break;
			case SVFileBitmap:
				_tcscat_s( filename, size,_T(".REJECT.bmp")  );
				break;
			case  SVFileTiff:
				_tcscat_s( filename, size,_T(".REJECT.tif")  );
				break;
			default:
				_tcscat_s( filename, size, _T(".REJECT.xxx")  );
				break;
			}
		}
		else
		{
			switch (type)
			{

			case SVFileMIL:
				_tcscat_s( filename, size, _T(".mil")  );
				break;
			case  SVFileBitmap:
				_tcscat_s( filename, size, _T(".bmp")  );
				break;
			case SVFileTiff:
				_tcscat_s( filename, size, _T(".tif")  );
				break;
			default:
				_tcscat_s( filename, size, _T(".xxx")  );
				break;
			}
		}
	}

} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/

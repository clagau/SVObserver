//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedImage
//* .File Name       : $Workfile:   SVSharedImage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:08:36  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVShared.h"
#include "SVMatroxLibrary\SVMatroxSimpleEnums.h"
#pragma endregion Includes

namespace Seidenader { namespace SVSharedMemoryLibrary
{
	struct SVSharedImage
	{
		char_string m_ElementName;
		char m_Filename[statics::max_result_size];
		long m_Status;
		void_allocator m_Allocator;

		SVSharedImage(const void_allocator& rAlloc);
	
		SVSharedImage(const SVSharedImage& rData);
		const SVSharedImage& operator=(const SVSharedImage& rData);
		void SetName(const std::string& NameName);
		void SetFileName(const std::string& Filename);
		void SetData(const std::string& Filename, int Status);
		
		//! Build a filename 
		//! \param filename [in,out] filename
		//! \param size [in] len of filename
		//! \param name [in] name 
		//! \param slotnumber [in]
		//! \param bReject [in]
		//! \param type [in]
		//! \returns void
		static void  BuildImageFileName(LPTSTR filename, int filenamelen, LPCTSTR name, long slotnumber, bool bReject = false, SVMatroxFileTypeEnum type = SVFileBitmap );
	};

typedef boost::interprocess::allocator<SVSharedImage, segment_manager_t> SVSharedImageAllocator;

} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/

namespace SvSml = Seidenader::SVSharedMemoryLibrary;

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
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

namespace Seidenader { namespace SVSharedMemoryLibrary
{
	//!This structure holds the path information which are stored in SharedMemory 
	struct SVSharedImage
	{
		SVSharedImage(const void_allocator& rAlloc);
		SVSharedImage(const SVSharedImage& rData);
		const SVSharedImage& operator=(const SVSharedImage& rData);
		void SetName(const SVString& NameName);
		
		void SetImageStoreProperties(LPCTSTR Name ,DWORD imageStoreIndex ,DWORD ImageIndex,DWORD slotindex,bool isreject);
		void SetImageStoreStatus( int Status );
	
		void SetFileName(LPCTSTR  Filename);
		void SetData(LPCTSTR Filename, int Status);
		
		//! Build a filename 
		//! \param filename [in,out] filename
		//! \param size [in] len of filename
		//! \param name [in] name 
		//! \param slotnumber [in]
		//! \param bReject [in]
		//! \param type [in]
		//! \returns void
		//static void  BuildImageFileName(LPTSTR filename, int filenamelen, LPCTSTR name, long slotnumber, bool bReject = false, SVMatroxFileTypeEnum type = SVFileBitmap );

	
		static LPCTSTR  BuildImageFileName(DWORD ImageIndex, DWORD ImageStoreIndex, DWORD SlotIndex, bool bReject);  

		bip_string m_ElementName;
		char m_Filename[statics::max_result_size];
		DWORD m_ImageIndex;
		DWORD m_ImageStoreIndex;
		bool m_IsReject;
		DWORD m_SlotIndex;
		long m_Status;
		void_allocator m_Allocator;
	};

typedef bip::allocator<SVSharedImage, segment_manager_t> SVSharedImageAllocator;
typedef bip::vector< SVSharedImage, SVSharedImageAllocator > SVSharedImageVector;
typedef bip::allocator< SVSharedImageVector, segment_manager_t > SVSharedImageVectorAllocator;

///only copy the status
void  CopySharedImages(SVSharedImageVector& rTo, const SVSharedImageVector& rFrom );

} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/

namespace SvSml = Seidenader::SVSharedMemoryLibrary;

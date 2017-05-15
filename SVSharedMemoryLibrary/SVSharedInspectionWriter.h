//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedInspectionWriter
//* .File Name       : $Workfile:   SVSharedInspectionWriter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:08:44  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <memory>
#include "SVUtilityLibrary/SVString.h"
#include "SVSharedLastInspectedCache.h"
#include "SVSharedRejectCache.h"
#include "SVSharedMemorySettings.h"
#include "SharedImageStore.h"


#pragma endregion Includes

#define 	BUFFER_REJECT_IMAGE_FILENAME_LEN 1024

namespace SvSml
{
	
	class SVSharedInspectionWriter // one writer per inspection
	{
	public:
		SVSharedInspectionWriter();
		~SVSharedInspectionWriter();

		HRESULT Create(const SVString& name, const GUID& guid, const SVSharedMemorySettings& rSettings, const long numProductSlots, const long numRejectSlots, const CreationInfo& rImagesInfo, const CreationInfo& rValuesInfo);
		void Destroy();

		const SVString& GetShareName() const;
		const GUID& GetGuid() const;

		const SVSharedData& GetLastInspectedSlot(long idx) const;
		SVSharedData& GetLastInspectedSlot(long idx);

		SVSharedData& GetRejectSlot(long idx);

		void ReleaseAll();

		HRESULT CopyLastInspectedToReject(long index, long rejectIndex);

		DWORD GetRejectCacheSize();
		DWORD GetLastInspectedCacheSize();
		


	private:
		void Init();
		void RemovePreviousImageFiles();

		void ClearHeldLastInspected();
		void ClearHeldRejects();

		std::shared_ptr<bip::managed_shared_memory> m_pManagedSharedMemory;

		SVString m_ShareName;
		SVSharedLastInspectedCache* m_pSharedLastInspectedCache;
		SVSharedRejectCache* m_pSharedRejectCache;

		
		
		GUID m_guid; //< Inspection Guid 

		TCHAR m_BufferRejectImageFileName[BUFFER_REJECT_IMAGE_FILENAME_LEN]; //< Buffer holds the full path of a RejectImage
		TCHAR* m_SecondPtrRejectImageFileName;   //<pointer to RejectImageFilename after the path 
		int  m_SecondPtrRejectImageFileNameLen;   //<pointer to RejectImageFilename after the path 


	};
} //namespace SvSml


//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedPPQWriter
//* .File Name       : $Workfile:   SVSharedPPQWriter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:09:54  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVSharedProductStore.h"
#include "SVSharedMonitorList.h"
#include "SVSharedMemorySettings.h"
#include "SVSharedInspectionWriter.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

namespace SvSml
{
	typedef std::pair<SVString, GUID> InspectionID;
	struct InspectionWriterCreationInfo
	{
		InspectionID inspectionID;
		CreationInfo imagesCreationInfo;
		CreationInfo valuesCreationInfo;

		InspectionWriterCreationInfo(const InspectionID& nameGuid, const CreationInfo& imagesInfo, const CreationInfo& valuesInfo)
		: inspectionID(nameGuid), imagesCreationInfo(imagesInfo), valuesCreationInfo(valuesInfo)
		{}
	};
	typedef std::vector<InspectionWriterCreationInfo> InspectionWriterCreationInfos;
	

	class SVSharedPPQWriter // one writer per ppq
	{
	public:
		SVSharedPPQWriter();
		~SVSharedPPQWriter();

		HRESULT Create( const SVString& name, const InspectionWriterCreationInfos& inspections, const SVSharedMemorySettings& p_rSettings, const long ProoductSlots, const long RejectSlots, size_t size );
		void Destroy();

		SVSharedProduct& RequestNextProductSlot(long& idx);
		SVSharedProduct& GetProductSlot(long idx);

		void ReleaseProduct(SVSharedProduct& product);
		SVSharedProduct& RequestNextRejectSlot(long& idx);
		void ReleaseReject(SVSharedProduct& product);

		SVSharedInspectionWriter& operator[](const SVString& shareName);
		SVSharedInspectionWriter& operator[](const GUID& guid);

		HRESULT CopyLastInspectedToReject(const SVSharedProduct& rProduct);

	private:
		void Init();
		long next_writable(SVSharedProductStore* share, bool bReject=false);

		void ClearHeld(SVSharedProductStore* share);
		void ReleaseAll();

		SVSharedProductStore* m_pProductStore; // product queue
		SVSharedProductStore* m_pRejectStore; // reject queue
		SVString m_ShareName;
		std::vector<SVSharedInspectionWriter> m_writers; //< one writer per inspection
		std::shared_ptr<bip::managed_shared_memory> m_pManagedSharedMemory;
	};

} //namespace SvSml

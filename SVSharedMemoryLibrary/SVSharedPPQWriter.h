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
#pragma endregion Includes

namespace Seidenader { namespace SVSharedMemoryLibrary
{
	typedef std::pair<std::string, GUID> InspectionID;
	typedef std::vector<InspectionID> InspectionIDs;
	typedef std::vector<SVSharedInspectionWriter> InspectionWriters;

	class SVSharedPPQWriter // one writer per ppq
	{
	public:
		SVSharedPPQWriter();
		~SVSharedPPQWriter();

		HRESULT Create( const std::string& name, const InspectionIDs & inspections, const SVSharedMemorySettings& p_rSettings );
		void Destroy();

		SVSharedProduct & RequestNextProductSlot(long & idx);
		SVSharedProduct & GetProductSlot(long idx);

		void ReleaseProduct(SVSharedProduct & product);
		SVSharedProduct & RequestNextRejectSlot(long & idx);
		void ReleaseReject(SVSharedProduct & product);

		SVSharedInspectionWriter & operator[](const std::string & shareName);
		SVSharedInspectionWriter & operator[](const GUID & guid);

		HRESULT CopyLastInspectedToReject(const SVSharedProduct& rProduct);

	private:
		void Init();
		long next_writable(SVSharedProductStore * share, bool bReject=false);

		void ClearHeld(SVSharedProductStore* share);
		void ReleaseAll();

		SVSharedProductStore * sh; // product queue
		SVSharedProductStore * rsh; // reject queue
		std::string m_ShareName;
		InspectionWriters m_writers;

		typedef std::shared_ptr< boost::interprocess::managed_shared_memory > managed_shared_memory_shared_ptr;
		managed_shared_memory_shared_ptr shm;
	};
} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/

namespace SvSml = Seidenader::SVSharedMemoryLibrary;


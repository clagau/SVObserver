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

#include <memory>
#include "SVSharedLastInspectedCache.h"
#include "SVSharedRejectCache.h"
#include "SVSharedMemorySettings.h"

namespace SeidenaderVision
{
	class SVSharedInspectionWriter // one writer per inspection
	{
	public:
		SVSharedInspectionWriter();
		~SVSharedInspectionWriter();

		HRESULT Create(const std::string& name, const GUID& guid, const SVSharedMemorySettings& rSettings);
		void Destroy();

		const std::string& GetShareName() const;
		const GUID& GetGuid() const;

		const SVSharedData & GetLastInspectedSlot(long idx) const;
		SVSharedData & GetLastInspectedSlot(long idx);

		SVSharedData & GetRejectSlot(long idx);

		void ReleaseAll();

		HRESULT CopyLastInspectedToReject(long index, long rejectIndex);

	private:
		typedef std::shared_ptr<boost::interprocess::managed_shared_memory> managed_shared_memory_shared_ptr;

		void Init();
		void RemovePreviousImageFiles();

		void ClearHeldLastInspected();
		void ClearHeldRejects();

		managed_shared_memory_shared_ptr shm;

		std::string m_ShareName;
		SVSharedLastInspectedCache* sh;
		SVSharedRejectCache* rsh;

		GUID m_guid;
	};
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSharedMemoryLibrary\SVSharedInspectionWriter.h_v  $
 * 
 *    Rev 1.0   14 Aug 2014 17:08:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Jan 2012 19:00:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Revised Create method to check for available disk space
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2011 13:40:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

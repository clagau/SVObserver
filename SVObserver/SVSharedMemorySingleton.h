//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedMemorySingleton
//* .File Name       : $Workfile:   SVSharedMemorySingleton.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   20 Aug 2014 17:43:20  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <map>
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVSharedMemoryLibrary/SVSharedPPQWriter.h"
#include "SVSharedMemoryLibrary/SVMonitorListWriter.h"
#include "SVSharedMemoryLibrary/SVShareControlHandler.h"
#include "SVSharedMemoryLibrary/SVSharedMemorySettings.h"
#pragma endregion Includes

class SVSharedMemorySingleton
{
public:
	static SVSharedMemorySingleton& Instance();

	~SVSharedMemorySingleton();

	HRESULT InsertPPQSharedMemory(const SVString& rName, const SVGUID& rGuid, const SeidenaderVision::InspectionIDs& rIDs);
	HRESULT ErasePPQSharedMemory(const SVGUID& rGuid);

	SeidenaderVision::SVSharedPPQWriter& GetPPQWriter(const SVGUID& rGuid);
	SeidenaderVision::SVSharedInspectionWriter& GetInspectionWriter(const SVGUID& rPPQGuid, const SVGUID& rGuid);
	SeidenaderVision::SVMonitorListWriter& GetMonitorListWriter();
	SeidenaderVision::SVShareControlHandler& GetIPCShare();

	std::string GetInspectionShareName(const SVGUID& rPPQGuid,const SVGUID& rGuid);
	const SVSharedMemorySettings& GetSettings() const;
	long GetRejectDepth() const;
	void SetRejectDepth(long rejectDepth);
	void SetProductDepth(long productDepth, long extra);

	static void Destroy();
protected:
	typedef std::map<SVGUID, SeidenaderVision::SVSharedPPQWriter> SVPPQSharedMemoryMap;

	SVSharedMemorySingleton();

	SVPPQSharedMemoryMap m_PPQSharedMemory;
	SeidenaderVision::SVMonitorListWriter m_monitorListWriter;
	SeidenaderVision::SVShareControlHandler m_shareControlHandler;
private:
	//Do not implement this method
	SVSharedMemorySingleton(const SVSharedMemorySingleton& p_rObject);

	//Do not implement this method
	const SVSharedMemorySingleton& operator=(const SVSharedMemorySingleton& p_rObject);

	void ReadSettings();
	void CheckDirectories();
	SVSharedMemorySettings m_settings;
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVSharedMemorySingleton.h_v  $
 * 
 *    Rev 1.1   20 Aug 2014 17:43:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised Destroy method to be static and to check for the existence of the Shared Memory Drive and Control Shared Memory file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Aug 2014 17:56:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2011 15:24:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in of new files associated with socket and shared memory functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/


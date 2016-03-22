//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedMemorySingleton
//* .File Name       : $Workfile:   SVSharedMemorySingleton.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   28 Aug 2014 18:48:26  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
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
	static HRESULT SetProductFilter(const SVString& listName, SVProductFilterEnum filter);

	static bool HasShares();
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


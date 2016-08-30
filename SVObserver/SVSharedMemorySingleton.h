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

	HRESULT InsertPPQSharedMemory(const SVString& rName, const SVGUID& rGuid, const long ProductSlots, const long RejectSlots, const SvSml::InspectionWriterCreationInfos& rCreationInfos);
	HRESULT ErasePPQSharedMemory(const SVGUID& rGuid);

	SvSml::SVSharedPPQWriter& GetPPQWriter(const SVGUID& rGuid);
	SvSml::SVSharedInspectionWriter& GetInspectionWriter(const SVGUID& rPPQGuid, const SVGUID& rGuid);
	SvSml::SVMonitorListWriter& GetMonitorListWriter();
	SvSml::SVShareControlHandler& GetIPCShare();

	std::string GetInspectionShareName(const SVGUID& rPPQGuid,const SVGUID& rGuid);
	const SvSml::SVSharedMemorySettings& GetSettings() const;
	static HRESULT SetProductFilter(const SVString& listName, SvSml::SVProductFilterEnum filter);

	static bool HasShares();
	static void Destroy();
protected:
	typedef std::map<SVGUID, SvSml::SVSharedPPQWriter> SVPPQSharedMemoryMap;

	SVSharedMemorySingleton();

	SVPPQSharedMemoryMap m_PPQSharedMemory;
	SvSml::SVMonitorListWriter m_monitorListWriter;
	SvSml::SVShareControlHandler m_shareControlHandler;
private:
	//Do not implement this method
	SVSharedMemorySingleton(const SVSharedMemorySingleton& p_rObject);

	//Do not implement this method
	const SVSharedMemorySingleton& operator=(const SVSharedMemorySingleton& p_rObject);

	void ReadSettings();
	void CheckDirectories();
	SvSml::SVSharedMemorySettings m_settings;
};


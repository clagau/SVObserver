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
#include "SVSharedMemoryLibrary/SVProductFilterEnum.h"
#pragma endregion Includes

namespace Seidenader { namespace SVSharedMemoryLibrary
{

class SVSharedMemorySingleton
{
public:
	static SVSharedMemorySingleton& Instance();

	~SVSharedMemorySingleton();

	HRESULT InsertPPQSharedMemory(const SVString& rName, const SVGUID& rGuid, const long ProductSlots, const long RejectSlots, const SvSml::InspectionWriterCreationInfos& rCreationInfos);
	HRESULT ErasePPQSharedMemory(const SVGUID& rGuid);

	SVSharedPPQWriter& GetPPQWriter(const SVGUID& rGuid);
	SVSharedInspectionWriter& GetInspectionWriter(const SVGUID& rPPQGuid, const SVGUID& rGuid);
	SVMonitorListWriter& GetMonitorListWriter();
	SVShareControlHandler& GetIPCShare();

	std::string GetInspectionShareName(const SVGUID& rPPQGuid,const SVGUID& rGuid);
	const SVSharedMemorySettings& GetSettings() const;
	static HRESULT SetProductFilter(const SVString& listName, SVProductFilterEnum filter);

	/// Stops the RRS reading Products. Function is moved from SVPPQObjects 
	static void QuiesceSharedMemory();
	/// Clears the PPQ part of the shared memory
	static void ClearPPQSharedMemory();
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

} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/

namespace SvSml = Seidenader::SVSharedMemoryLibrary;

//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOPPQObj
//* .File Name       : $Workfile:   SVOPPQObj.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:10:58  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <deque>
//Moved to precompiled header: #include <memory>
//Moved to precompiled header: #include <utility>
#include "SVImportedInspectionInfo.h"
#include "SVPPQConstants.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

typedef std::pair<std::string, uint32_t> SVNameObjectIdPair;
typedef std::deque<SVNameObjectIdPair> SVNameObjectIdPairList;

class SVOPPQObj  
{
public:
	SVOPPQObj() = default;

	void SetPPQName(LPCTSTR PPQName);
	const std::string& GetPPQName();

	void AttachTriggerToPPQ(LPCTSTR TriggerName);
	void DetachTriggerFromPPQ();
	const std::string& GetAttachedTriggerName() const;

	bool AttachCameraToPPQ(LPCTSTR CameraName);
	bool DetachCameraFromPPQ(LPCTSTR CameraName);
	int GetAttachedCameraCount() const;
	std::string GetAttachedCamera(int ipos) const;

	bool AttachInspectionToPPQ(LPCTSTR InspectName);
	bool DetachInspectionFromPPQ(LPCTSTR InspectName);
	int GetAttachedInspectionCount();
	std::string GetAttachedInspection(int iPos);

	//also have methods for properties
	void SetPPQMode(int iMode);
	void SetPPQLength(long lLength);
	void SetPPQOutputResetDelay(long lResetDelay);
	void SetPPQOutputDelayTime(long lDelayTime);
	void SetMaintainSourceImageProperty(bool bValue);
	void SetMaxProcessingOffsetProperty(int value);
	void SetInspectionTimeout( long lTimeoutMillisec );

	int GetPPQMode() const;
	long GetPPQLength() const;
	long GetPPQOutputResetDelay() const;
	long GetPPQOutputDelayTime() const;
	bool GetMaintainSourceImageProperty() const;
	int GetMaxProcessingOffsetProperty() const;
	long GetInspectionTimeout() const;

	void SetAvailableInputsForConditionalOutput(const SVNameObjectIdPairList& list);
	const SVNameObjectIdPairList& GetAvailableInputsForConditionalOutput() const;

	void SetConditionalOutputName(const std::string& name);
	const std::string& GetConditionalOutputName() const;
	bool IsConditionalOutputCameraInput() const;
	void RemoveCameraInputConditionalOutput();

	void SetImportedInspectionInfo(SVImportedInspectionInfo&& importedInspectionInfo) { m_importedInspectionInfo = std::move(importedInspectionInfo); };
	const SVImportedInspectionInfo& GetImportedInspectionInfo() const { return m_importedInspectionInfo; }
	void ClearImportedInspectionInfo() { m_importedInspectionInfo = {}; }

private:
	std::string m_PPQName {};
	std::string m_AttachedTrigger {};
	SvDef::StringVector m_AttachedCameraList;
	SvDef::StringVector m_AttachedInspectList;

	//properties
	int m_iPPQMode {0};
	long m_lPPQLength {2};
	long m_lPPQOutputResetDelay {0};
	long m_lPPQOutputDelayTime {100};
	bool m_bMaintainSrcImg {false};
	int m_MaxProcessingOffset {0};
	long m_lInspectionTimeout {0};
	std::string m_conditionalOutputName {PPQ_CONDITIONAL_OUTPUT_ALWAYS};
	SVNameObjectIdPairList m_availableInputs;
	SVImportedInspectionInfo m_importedInspectionInfo;
};

typedef std::shared_ptr< SVOPPQObj > SVOPPQObjPtr;



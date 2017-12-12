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
#include "SVImportedInputList.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

typedef std::pair<std::string, GUID> SVNameGuidPair;
typedef std::deque<SVNameGuidPair> SVNameGuidPairList;

class SVOPPQObj  
{
public:
	SVOPPQObj();
	virtual ~SVOPPQObj();

	SVOPPQObj& operator=(const SVOPPQObj &source);

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
	void SetInspectionTimeout( long lTimeoutMillisec );

	int GetPPQMode() const;
	long GetPPQLength() const;
	long GetPPQOutputResetDelay() const;
	long GetPPQOutputDelayTime() const;
	bool GetMaintainSourceImageProperty() const;
	long GetInspectionTimeout() const;

	void SetAvailableInputsForConditionalOutput(const SVNameGuidPairList& list);
	const SVNameGuidPairList& GetAvailableInputsForConditionalOutput() const;

	void SetConditionalOutputName(const std::string& name);
	const std::string& GetConditionalOutputName() const;
	bool IsConditionalOutputCameraInput() const;
	void RemoveCameraInputConditionalOutput();

	void SetImportedInputList(const SVImportedInputList& list);
	const SVImportedInputList& GetImportedInputList() const;
	void ClearImportedInputList();

private:
	std::string m_PPQName;
	std::string m_AttachedTrigger;
	SvDef::StringVector m_AttachedCameraList;
	SvDef::StringVector m_AttachedInspectList;

	//properties
	int m_iPPQMode;
	long m_lPPQLength;
	long m_lPPQOutputResetDelay;
	long m_lPPQOutputDelayTime;
	bool m_bMaintainSrcImg;
	long m_lInspectionTimeout;
	std::string m_conditionalOutputName;
	SVNameGuidPairList m_availableInputs;
	SVImportedInputList m_importedInputList;
};

typedef std::shared_ptr< SVOPPQObj > SVOPPQObjPtr;



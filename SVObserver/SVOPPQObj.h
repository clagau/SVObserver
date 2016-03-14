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
#include <deque>
#include <utility>
#include "SVUtilityLibrary/SVString.h"
#include "SVImportedInputList.h"
#include "SVUtilityLibrary\SVSharedPtr.h"
#pragma endregion Includes

typedef std::pair<SVString, GUID> SVNameGuidPair;
typedef std::deque<SVNameGuidPair> SVNameGuidPairList;

class SVOPPQObj  
{
public:
	SVOPPQObj();
	virtual ~SVOPPQObj();

	SVOPPQObj& operator=(const SVOPPQObj &source);

	void SetPPQName(CString sPPQName);
	CString GetPPQName();

	void AttachTriggerToPPQ(CString sTriggerName);
	void DetachTriggerFromPPQ();
	CString GetAttachedTriggerName() const;

	BOOL AttachCameraToPPQ(CString sCameraName);
	BOOL DetachCameraFromPPQ(CString sCameraName);
	int GetAttachedCameraCount() const;
	CString GetAttachedCamera(int ipos) const;

	BOOL AttachInspectionToPPQ(CString sInspectName);
	BOOL DetachInspectionFromPPQ(CString sInspectName);
	int GetAttachedInspectionCount();
	CString GetAttachedInspection(int iPos);

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

	void SetConditionalOutputName(const SVString& name);
	const SVString& GetConditionalOutputName() const;
	bool IsConditionalOutputCameraInput() const;
	void RemoveCameraInputConditionalOutput();

	void SetImportedInputList(const SVImportedInputList& list);
	const SVImportedInputList& GetImportedInputList() const;
	void ClearImportedInputList();

private:
	CString m_sPPQName;
	CString m_sAttachedTrigger;
	CStringList m_slAttachedCameraList;
	CStringList m_slAttachedInspectList;

	//properties
	int m_iPPQMode;
	long m_lPPQLength;
	long m_lPPQOutputResetDelay;
	long m_lPPQOutputDelayTime;
	bool m_bMaintainSrcImg;
	long m_lInspectionTimeout;
	SVString m_conditionalOutputName;
	SVNameGuidPairList m_availableInputs;
	SVImportedInputList m_importedInputList;
};

typedef SVSharedPtr< SVOPPQObj > SVOPPQObjPtr;



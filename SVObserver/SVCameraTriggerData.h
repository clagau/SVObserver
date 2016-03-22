//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCameraTriggerData
//* .File Name       : $Workfile:   SVCameraTriggerData.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:52:48  $
//******************************************************************************

#pragma once

#include "SVObjectLibrary/SVObjectClass.h"
#include "SVDoubleValueObjectClass.h"
#include "SVBoolValueObjectClass.h"
#include "SVIOEntryHostStruct.h"

class SVCameraTriggerData : public SVObjectClass 
{
public:
	typedef  std::map<SVString, _variant_t> NameVariantMap;

private:
	SVDoubleValueObjectClass m_timestamp;
	SVBoolValueObjectClass m_lineInState;

public:
	SVCameraTriggerData();
	virtual ~SVCameraTriggerData();

	SVIOEntryHostStructPtr SetupLineStateInput(long objectDepth);
	SVIOEntryHostStructPtr SetupTimestampInput(long objectDepth);

	SVString GetTimestampName() const;
	SVString GetLineStateName() const;

	void Set(long index, const NameVariantMap& rSettings);

private:
	void init();
	void destroy();
	void SetLineState(long index, double timeStamp, bool state);
};


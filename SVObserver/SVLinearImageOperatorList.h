//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLinearImageOperatorList
//* .File Name       : $Workfile:   SVLinearImageOperatorList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:54:06  $
//******************************************************************************

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes which are necessary to declare or define types, constants and macros
////////////////////////////////////////////////////////////////////////////////

#include "SVUnaryImageOperatorList.h"

class SVLinearImageOperatorListClass : public SVStdImageOperatorListClass 
{
	SV_DECLARE_CLASS( SVLinearImageOperatorListClass )

public:
	SVLinearImageOperatorListClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVLINEARIMAGEOPERATORLIST );
	virtual ~SVLinearImageOperatorListClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual HRESULT ResetObject();

	virtual BOOL OnValidate();
	virtual BOOL Run( SVRunStatusClass& RRunStatus );

protected:
	SVBoolValueObjectClass *getUseRotationAngle();
	SVEnumerateValueObjectClass *getInputProfileOrientation();

	SVInObjectInfoStruct inputUseRotationAngle;
	SVInObjectInfoStruct inputProfileOrientation;

	SVDoubleValueObjectClass m_svLinearData;

	SVDoubleValueObjectClass m_svMinThreshold;
	SVDoubleValueObjectClass m_svMaxThreshold;

	SVDataBufferClass m_svProfileResultData;

	unsigned long m_ulLineLength;
	SVMatroxLongArray m_aulLineData;

	std::vector<double> m_svArray;

private:
	void init();

	HRESULT UpdateLineExtentData();
};


//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLUTOperator
//* .File Name       : $Workfile:   SVLUTOperator.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 Aug 2013 10:08:54  $
//******************************************************************************

#pragma once

#include "SVUnaryImageOperatorClass.h"
#include "SVValueObjectImpl.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"

class SVLUTOperatorClass : public SVUnaryImageOperatorClass
{
	SV_DECLARE_CLASS( SVLUTOperatorClass );

public:
	SVLUTOperatorClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVLUTOPERATOR );
	virtual ~SVLUTOperatorClass();

private:
	void init();

public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual HRESULT ResetObject();

	BOOL RecalcLUT( SVRunStatusClass& RRunStatus );

	virtual BOOL OnValidate();

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );
	SVByteValueObjectClass* getInputLUTVectorResult();
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

protected:

	// MIL LUT Buffer ID
	SVMatroxBuffer m_lutBufID;

	// Number of elements in LUT ( should not exceed 256 )
	long m_lutElementNumber;

	// Embeddeds:

	// Contains the 256 LUT elements...
	SVByteValueObjectClass  m_lutVector;

	// Is LUT activated...
	SVBoolValueObjectClass	 m_useLUT;

	// Shall LUT be recalulated on every run...
	SVBoolValueObjectClass	 m_continuousRecalcLUT;

	// LUT Mode ( i.e. Identity, Clip, Formula... )
	SVEnumerateValueObjectClass  m_lutMode;

	// Used in Clip mode...
	SVLongValueObjectClass m_upperClip;
	// Used in Clip mode...
	SVLongValueObjectClass m_lowerClip;
	
	// Inputs:

	// LUT Vector Input, calculated by LUTEquation.
	//	( SVByteVectorObjectClass )
	SVInObjectInfoStruct	 m_inputLUTVectorResult;

	bool m_bForceLUTRecalc;
};


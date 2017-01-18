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

#pragma region Includes
#include "SVOCore/SVUnaryImageOperatorClass.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#pragma endregion Includes

class SVLUTOperatorClass : public SVUnaryImageOperatorClass
{
	SV_DECLARE_CLASS( SVLUTOperatorClass );

public:
	SVLUTOperatorClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLUTOPERATOR );
	virtual ~SVLUTOperatorClass();

private:
	void init();

public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;
	virtual BOOL CloseObject() override;

	virtual bool resetAllObjects( bool shouldNotifyFriends, bool silentReset ) override;
	virtual HRESULT ResetObject() override;

	BOOL RecalcLUT( SVRunStatusClass& RRunStatus );

	virtual BOOL OnValidate() override;

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus ) override;
	SVByteValueObjectClass* getInputLUTVectorResult();

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


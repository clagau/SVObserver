//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVUnaryImageOperatorClass.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "SVObjectLibrary/InputObject.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVByteValueObjectClass.h"
#include "SVValueObjectLibrary/SVEnumerateValueObjectClass.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{

class SVLUTOperator : public SVUnaryImageOperatorClass
{
	SV_DECLARE_CLASS

public:
	SVLUTOperator( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLUTOPERATOR );
	virtual ~SVLUTOperator();

private:
	void init();

public:
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool CloseObject() override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	bool RecalcLUT( SvIe::RunStatus& rRunStatus );

protected:
	virtual bool onRun(bool First, SvOi::SVImageBufferHandlePtr RInputImageHandle, SvOi::SVImageBufferHandlePtr ROutputImageHandle, SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr) override;

protected:

	// MIL LUT Buffer ID
	SVMatroxBuffer m_lutBufID;

	// Number of elements in LUT ( should not exceed 256 )
	long m_lutElementNumber;

	// Embeddeds:

	// Contains the 256 LUT elements...
	SvVol::SVByteValueObjectClass  m_lutVector;

	// Is LUT activated...
	SvVol::SVBoolValueObjectClass	 m_useLUT;

	// Shall LUT be recalulated on every run...
	SvVol::SVBoolValueObjectClass	 m_continuousRecalcLUT;

	// LUT Mode ( i.e. Identity, Clip, Formula... )
	SvVol::SVEnumerateValueObjectClass  m_lutMode;

	// Used in Clip mode...
	SvVol::SVLongValueObjectClass m_upperClip;
	// Used in Clip mode...
	SvVol::SVLongValueObjectClass m_lowerClip;

	// used in Stretch mode...
	SvVol::SVLongValueObjectClass m_minInput;
	SvVol::SVLongValueObjectClass m_maxInput;
	SvVol::SVLongValueObjectClass m_minOutput;
	SvVol::SVLongValueObjectClass m_maxOutput;

	// Inputs:

	// LUT Vector Input, calculated by LUTEquation.
	//	( SVByteVectorObjectClass )
	SvOl::InputObject	 m_inputLUTVectorResult;

	bool m_bForceLUTRecalc;

	SVTaskObjectClass* m_pLUTEquation {nullptr};
};

} //namespace SvOp

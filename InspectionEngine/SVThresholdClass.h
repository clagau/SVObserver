//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVThresholdClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Aug 2013 16:30:08  $
//******************************************************************************

#pragma once

#pragma region Incudes
#include "SVUnaryImageOperatorClass.h"
#include "SVMatroxLibrary/SVMatroxTypedefs.h"
#pragma endregion Incudes

class SVThresholdClass : public SVUnaryImageOperatorClass
{
	SV_DECLARE_CLASS( SVThresholdClass );
	friend class SVToolAdjustmentDialogThresholdPageClass;
public:
	SVThresholdClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTHRESHOLD );
	virtual ~SVThresholdClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool CloseObject() override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	SVBoolValueObjectClass& GetThresholdActivateAttribute();

	bool Rebuild();

	const SVMatroxLongArray & GetHistogramValues() const
	{
		return m_HistValueArray;
	}

protected:
	virtual bool onRun( bool First, SvOi::SVImageBufferHandlePtr RInputImageHandle, SvOi::SVImageBufferHandlePtr ROutputImageHandle, SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	SVDoubleValueObjectClass* getExternalUT();
	SVDoubleValueObjectClass* getExternalLT();
	SVDoubleValueObjectClass* getExternalATM();

	SVMatroxIdentifier m_histResultID = M_NULL;
	SVMatroxLongArray  m_HistValueArray;
	int    m_histValueArraySize;
	__int64 m_pixelNumber;


	SVDoubleValueObjectClass m_dAutoThresholdMultiplier;
	SVBoolValueObjectClass   m_useExternalATM;	// use external auto threshold multiplier
	SVInObjectInfoStruct	 m_inputATM;


	SVLongValueObjectClass	 m_upperThresh;
	SVBoolValueObjectClass   m_useExternalUT;		// use external Upper Threshold Value
	SVInObjectInfoStruct	 m_inputUT;

	SVLongValueObjectClass	 m_lowerThresh;
	SVBoolValueObjectClass   m_useExternalLT;		// use external Lower Threshold Value
	SVInObjectInfoStruct	 m_inputLT;

	
	SVBoolValueObjectClass	 m_threshActivate;
	SVBoolValueObjectClass	 m_upperThreshActivate;
	SVBoolValueObjectClass	 m_lowerThreshActivate;
	SVBoolValueObjectClass	 m_autoThreshold;
	SVBoolValueObjectClass	 m_blackBackground;

private:
	void init();
public:
	bool saveHistogram;

};

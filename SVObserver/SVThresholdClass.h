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
#include "SVOCore/SVUnaryImageOperatorClass.h"
#include "SVMatroxLibrary/SVMatroxTypedefs.h"
#include "SVMatroxLibrary/SVMatroxImageResult.h"
#pragma endregion Incudes

class SVThresholdClass : public SVUnaryImageOperatorClass
{
	SV_DECLARE_CLASS( SVThresholdClass );
	friend class SVToolAdjustmentDialogThresholdPageClass;
public:
	SVThresholdClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTHRESHOLD );
	virtual ~SVThresholdClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;
	virtual BOOL CloseObject() override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	SVBoolValueObjectClass& GetThresholdActivateAttribute();

	bool Rebuild();

	const SVMatroxLongArray & GetHistogramValues() const
	{
		return m_HistValueArray;
	}

protected:
	virtual bool onRun( bool First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	SVDoubleValueObjectClass* getExternalUT();
	SVDoubleValueObjectClass* getExternalLT();
	SVDoubleValueObjectClass* getExternalATM();

	SVMatroxImageResult m_histResultID;
	SVMatroxLongArray  m_HistValueArray;
	int    m_histValueArraySize;

	// Contents the current pixel number, call
	// BOOL SVThresholdClass::Resize( SVExtentClass& RExtent );
	// to (re-)calculate...
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


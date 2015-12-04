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
#include "SVMatroxLibrary/SVMatroxImageResult.h"
#pragma endregion Incudes

class SVThresholdClass : public SVUnaryImageOperatorClass
{
	SV_DECLARE_CLASS( SVThresholdClass );
	friend class SVToolAdjustmentDialogThresholdPageClass;
public:
	SVThresholdClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVTHRESHOLD );
	virtual ~SVThresholdClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual HRESULT ResetObject();

	SVBoolValueObjectClass& GetThresholdActivateAttribute();

	virtual HRESULT Rebuild();
	virtual BOOL OnValidate();

	const SVMatroxLongArray & GetHistogramValues() const
	{
		return m_HistValueArray;
	}

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );

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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVThresholdClass.h_v  $
 * 
 *    Rev 1.0   12 Aug 2013 16:30:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Created from SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

//******************************************************************************
//* COPYRIGHT (c) 2013 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include <mil.h>
#include "SVUnaryImageOperatorClass.h"
#include "SVObjectLibrary/InputObject.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#pragma endregion Incudes

namespace SvOp
{

class SVThresholdClass : public SVUnaryImageOperatorClass
{
	SV_DECLARE_CLASS
	friend class TADlgThresholdPage;
public:
	SVThresholdClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTHRESHOLD );
	virtual ~SVThresholdClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool CloseObject() override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	SvVol::SVBoolValueObjectClass& GetThresholdActivateAttribute();

	bool Rebuild();

	const std::vector<long> & GetHistogramValues() const
	{
		return m_HistValueArray;
	}

protected:
	virtual bool onRun( bool First, SvOi::SVImageBufferHandlePtr RInputImageHandle, SvOi::SVImageBufferHandlePtr ROutputImageHandle, SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	__int64 m_histResultID = M_NULL;
	std::vector<long>  m_HistValueArray;
	int    m_histValueArraySize;
	__int64 m_pixelNumber;


	SvVol::SVDoubleValueObjectClass m_dAutoThresholdMultiplier;
	SvVol::SVBoolValueObjectClass   m_useExternalATM;	// use external auto threshold multiplier
	SvOl::InputObject	 m_inputATM;


	SvVol::SVLongValueObjectClass	 m_upperThresh;
	SvVol::SVBoolValueObjectClass   m_useExternalUT;		// use external Upper Threshold Value
	SvOl::InputObject	 m_inputUT;

	SvVol::SVLongValueObjectClass	 m_lowerThresh;
	SvVol::SVBoolValueObjectClass   m_useExternalLT;		// use external Lower Threshold Value
	SvOl::InputObject	 m_inputLT;

	
	SvVol::SVBoolValueObjectClass	 m_threshActivate;
	SvVol::SVBoolValueObjectClass	 m_upperThreshActivate;
	SvVol::SVBoolValueObjectClass	 m_lowerThreshActivate;
	SvVol::SVBoolValueObjectClass	 m_autoThreshold;
	SvVol::SVBoolValueObjectClass	 m_blackBackground;

	SVTaskObjectClass* m_pUTEquation {nullptr};
	SVTaskObjectClass* m_pLTEquation {nullptr};
	SVTaskObjectClass* m_pATEquation {nullptr};

private:
	void init();
public:
	bool saveHistogram;

};

} //namespace SvOp

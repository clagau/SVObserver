//*****************************************************************************
/// \copyright (c) 2003,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file PixelAnalyzer.h
/// All Rights Reserved 
//*****************************************************************************
/// This class is the TaskObjectList-Implementation of the PixelAnalyzer 
//*****************************************************************************
#pragma once

#pragma region Includes
#include "ImageAnalyzer.h"
#include "SVValueObjectLibrary/SVByteValueObjectClass.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#pragma endregion Includes

namespace SvAo
{

class PixelAnalyzer : public ImageAnalyzer
{
	SV_DECLARE_CLASS

public:
	explicit PixelAnalyzer( LPCSTR ObjectName /* = "Pixel Analyzer" */ );
	PixelAnalyzer(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVPIXELANALYZER );

	virtual ~PixelAnalyzer();
	virtual bool CloseObject() override;

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	virtual void addParameterForMonitorList(SvStl::MessageContainerVector& rMessages, std::back_insert_iterator<SvOi::ParametersForML> inserter) const override;

private:
	void init();

//******************************************************************************
// Data Element(s):
//******************************************************************************
public:
	SvVol::SVLongValueObjectClass m_pixelCount;
	SvVol::SVByteValueObjectClass m_pixelCountColor;

protected:
	virtual bool onRun(SvIe::RunStatus &rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	__int64		m_contextID = M_NULL;
	__int64		m_ResultID = M_NULL;
};

} //namespace SvAo

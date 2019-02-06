//*****************************************************************************
/// \copyright (c) 2003,2018 by Seidenader Maschinenbau GmbH
/// \file SVPixelAnalyzer.h
/// All Rights Reserved 
//*****************************************************************************
/// This class is the TaskObjectList-Implementation of the PixelAnalyzer 
//*****************************************************************************
#pragma once

#pragma region Includes
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVImageAnalyzerClass.h"
#pragma endregion Includes

namespace SvAo
{

class SVPixelAnalyzerClass : public SVImageAnalyzerClass
{
	SV_DECLARE_CLASS( SVPixelAnalyzerClass );

public:
	SVPixelAnalyzerClass( LPCSTR ObjectName /* = "Pixel Analyzer" */ );
	SVPixelAnalyzerClass(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVPIXELANALYZER );

	virtual ~SVPixelAnalyzerClass();
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
	virtual bool onRun(SVRunStatusClass &rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	SVMatroxIdentifier		m_contextID = M_NULL;
	SVMatroxIdentifier		m_ResultID = M_NULL;
};

} //namespace SvAo

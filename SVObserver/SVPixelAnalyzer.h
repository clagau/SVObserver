//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : PixelAnalyzer
//* .File Name       : $Workfile:   SVPixelAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:22:46  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVAnalyzer.h"     // Required by PixelAnalyzer.h
#pragma endregion Includes

////////////////////////////////////////////////////////////////////////////////
// .Title       : Class SVPixelAnalyzerClass
// -----------------------------------------------------------------------------
// .Description : e.g. This base class capsules an Image and provides its ...
////////////////////////////////////////////////////////////////////////////////
class SVPixelAnalyzerClass : public SVImageAnalyzerClass
{
	SV_DECLARE_CLASS( SVPixelAnalyzerClass );

public:
	SVPixelAnalyzerClass( LPCSTR ObjectName /* = "Pixel Analyzer" */ );
	SVPixelAnalyzerClass(BOOL BCreateDefaultTaskList = FALSE, 
                         SVObjectClass* POwner = nullptr,
                         int StringResourceID = IDS_CLASSNAME_SVPIXELANALYZER );

	virtual ~SVPixelAnalyzerClass();
	virtual BOOL CloseObject();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL OnValidate();

private:
	void init();

//******************************************************************************
// Data Element(s):
//******************************************************************************
public:
	long m_svlHistValueArraySize;
	SVLongValueObjectClass m_pixelCount;
	SVByteValueObjectClass m_pixelCountColor;

protected:
	virtual BOOL onRun(SVRunStatusClass &RRunStatus);

	SVMatroxImageResult		m_histResultID;
	SVMatroxLongArray		m_alHistValues;
};



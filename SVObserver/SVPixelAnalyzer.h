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

#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVAnalyzer.h"     // Required by PixelAnalyzer.h

////////////////////////////////////////////////////////////////////////////////
// .Title       : Class SVPixelAnalyzerClass
// -----------------------------------------------------------------------------
// .Description : e.g. This base class capsules an Image and provides its ...
//              :
//              :
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
//  :
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :dd.mm.yyyy				First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
class SVPixelAnalyzerClass : public SVImageAnalyzerClass
{

	SV_DECLARE_CLASS( SVPixelAnalyzerClass );

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVPixelAnalyzerClass( LPCSTR ObjectName /* = "Pixel Analyzer" */ );
	SVPixelAnalyzerClass(BOOL BCreateDefaultTaskList = FALSE, 
                         SVObjectClass* POwner = NULL ,
                         int StringResourceID = IDS_CLASSNAME_SVPIXELANALYZER );

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	virtual ~SVPixelAnalyzerClass();
	virtual BOOL CloseObject();

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

private:
	void init();

//******************************************************************************
// Operation(s) Of Process:
//******************************************************************************
public:
	virtual BOOL OnValidate();

//******************************************************************************
// Data Element(s):
//******************************************************************************
   long             msvlHistValueArraySize;
	SVLongValueObjectClass	pixelCount;
	SVByteValueObjectClass	pixelCountColor;


protected:
	virtual BOOL onRun(SVRunStatusClass &RRunStatus);

	SVMatroxImageResult		histResultID;
	SVMatroxLongArray		m_alHistValues;

};



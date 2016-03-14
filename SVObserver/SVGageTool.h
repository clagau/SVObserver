//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGageTool
//* .File Name       : $Workfile:   SVGageTool.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   09 Dec 2014 09:17:56  $
//******************************************************************************

#ifndef SVGAGETOOL_H
#define SVGAGETOOL_H

#include "SVTool.h"
#include "SVImageClass.h"

////////////////////////////////////////////////////////////////////////////////
// .Title       : Class SVGageToolClass
// -----------------------------------------------------------------------------
// .Description : This class provides the functionality of a gage
//              : processing Tool like find a special edge and ...
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :20.08.1999 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
class SVGageToolClass : public SVToolClass
{
	SV_DECLARE_CLASS( SVGageToolClass );

public:
	SVGageToolClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVGAGETOOL );
	virtual ~SVGageToolClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual BOOL OnValidate();

	virtual bool DoesObjectHaveExtents() const override;

	virtual BOOL SetObjectDepth( int NewObjectDepth );
	virtual BOOL SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex );

	virtual HRESULT ResetObject();

	virtual HRESULT IsInputImage( SVImageClass *p_psvImage );

	SVImageClass* getInputImage();

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

	void init();

	SVInObjectInfoStruct		inputImageObjectInfo;

	SVImageClass				gageImage;

	bool m_bToolRegistered;
	
};




//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	// SVGAGETOOL_H


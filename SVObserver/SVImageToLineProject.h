//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageToLineProject
//* .File Name       : $Workfile:   SVImageToLineProject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:44:06  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVImageLibrary/SVExtentLineStruct.h"

#include "SVLine.h"
#include "SVTaskObject.h"
#pragma endregion Includes

class SVToolClass;

class SVImageToLineProjectClass : public SVTaskObjectClass
{
	SV_DECLARE_CLASS( SVImageToLineProjectClass )

public:
	SVImageToLineProjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVIMAGETOLINEPROJECT );

	virtual ~SVImageToLineProjectClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual HRESULT ResetObject();

	virtual BOOL OnValidate();

	SVLineClass* getOutputLine();
	SVImageClass* getInputImage();

	HRESULT GetThresholdBars( SVExtentMultiLineStruct& p_rsvMiltiLine );
	HRESULT GetHistogram( SVExtentMultiLineStruct& p_rsvMiltiLine );

protected:
	SVDoubleValueObjectClass* getInputProjectAngle();

	virtual BOOL onRun( SVRunStatusClass& RRunStatus );
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
	virtual HRESULT onCollectOverlays(SVImageClass *p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray );
	virtual HRESULT UpdateLineExtentData();

	SVInObjectInfoStruct inputImageObjectInfo;
	SVInObjectInfoStruct inputProjectAngle;

	// Embedded Object:
	SVLineClass outputLineObject; // Embedded

	// Contains Project Data...
	SVDataBufferClass dataBuffer;

	// Calculated Min, Max Values from the projection
	SVDWordValueObjectClass thresholdMin;
	SVDWordValueObjectClass thresholdMax;
};


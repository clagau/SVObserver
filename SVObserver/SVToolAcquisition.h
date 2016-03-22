//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAcquisition
//* .File Name       : $Workfile:   SVToolAcquisition.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 13:10:48  $
//******************************************************************************

#pragma once

#include "SVTool.h"
#include "SVMainImageClass.h"

class SVImageClass;

class SVAcquisitionToolClass : public SVToolClass
{
	SV_DECLARE_CLASS( SVAcquisitionToolClass );

public:
	SVAcquisitionToolClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVACQUISITIONTOOL );

	virtual ~SVAcquisitionToolClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual HRESULT ResetObject();

	virtual BOOL OnValidate();

	virtual bool DoesObjectHaveExtents() const override;

	SVCameraImageTemplate* GetMainImageClass();

	virtual SVStaticStringValueObjectClass* GetInputImageNames( ) override;

protected:
	virtual void init();

	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

	// Embedded Objects:
	SVMainImageClass				mainImageObject;	// Embedded

	// String value object for Source Image Names
	SVStaticStringValueObjectClass m_svSourceImageNames;

};


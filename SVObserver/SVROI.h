//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVROI
//* .File Name       : $Workfile:   SVROI.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:40:08  $
//******************************************************************************

#pragma once

#include "SVTaskObject.h"

class SVImageClass;
class SVToolClass;

class SVROIClass : public SVTaskObjectClass
{
	SV_DECLARE_CLASS( SVROIClass )

public:
// Standard constructors
	SVROIClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVROI );
	virtual ~SVROIClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	SVImageClass* getInputImage();

	virtual BOOL OnValidate();

protected:
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

	SVInObjectInfoStruct		inputImageObjectInfo;

};


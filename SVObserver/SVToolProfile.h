//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVProfileTool
//* .File Name       : $Workfile:   SVToolProfile.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   09 Dec 2014 10:23:34  $
//******************************************************************************

#ifndef SVTOOLPROFILE_H
#define SVTOOLPROFILE_H

#include "SVTool.h"
#include "SVValueObject.h"

class SVProfileToolClass : public SVToolClass
{
	SV_DECLARE_CLASS( SVProfileToolClass );

public:
	SVProfileToolClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVPROFILETOOL );

	virtual ~SVProfileToolClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual HRESULT ResetObject();

	virtual bool DoesObjectHaveExtents() const override;

	// Embedded Object...
	SVDoubleValueObjectClass	projectAngle;

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

	virtual void init();

};

#endif


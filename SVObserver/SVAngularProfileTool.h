//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAngularProfileTool
//* .File Name       : $Workfile:   SVAngularProfileTool.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   09 Dec 2014 09:17:56  $
//******************************************************************************

#ifndef SVANGULARPROFILETOOL_H
#define SVANGULARPROFILETOOL_H

#include "SVTool.h"
#include "SVImageClass.h"

class SVLineClass;

class SVAngularProfileToolClass : public SVToolClass
{
	SV_DECLARE_CLASS( SVAngularProfileToolClass );

public:
	SVAngularProfileToolClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVPROFILETOOL );

	virtual ~SVAngularProfileToolClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual HRESULT ResetObject();

	virtual HRESULT IsInputImage( SVImageClass *p_psvImage );

	virtual SVTaskObjectClass *GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint );
	virtual bool DoesObjectHaveExtents() const override;

	SVObjectClass* getImageToLineProject();
	SVLineClass* getOutputLine();
	SVImageClass* getInputImage();
	SVImageClass* getOutputImage();

	SVToolPropertyEntryStruct* GetSpecialPropertyList( int& RCount );

	virtual SVStaticStringValueObjectClass* GetInputImageNames() override;

	virtual BOOL OnValidate();
protected:
	virtual void init();
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
	BOOL onRun( SVRunStatusClass& RRunStatus );

	SVDoubleValueObjectClass m_svRotationAngle;
	SVDoubleValueObjectClass m_svRotationPointX;
	SVDoubleValueObjectClass m_svRotationPointY;

	// Embedded Object...
	SVDoubleValueObjectClass	projectAngle;

	// Image for rotation
	SVImageClass	profileImage;

	// Input Image required
	SVInObjectInfoStruct  inputImageObjectInfo;

private:
	// String value object for Source Image Names
	SVStaticStringValueObjectClass m_svSourceImageNames;

};

#endif


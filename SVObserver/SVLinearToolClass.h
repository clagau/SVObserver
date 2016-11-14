// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearToolClass.h
// * .File Name       : $Workfile:   SVLinearToolClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   09 Dec 2014 09:47:26  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVTool.h"
#include "SVImageClass.h"
#pragma endregion Includes

#pragma region Declarations
const LPCSTR g_strOrientationEnums = _T( "Horizontal=0,Vertical=90" );
#pragma endregion Declarations

class SVLinearToolClass : public SVToolClass
, public AllowResizeToParent
{
	SV_DECLARE_CLASS( SVLinearToolClass );

public:
#pragma region Constructor
	SVLinearToolClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLINEARTOOL );
	virtual ~SVLinearToolClass();
#pragma endregion Constructor

#pragma region Public Methods
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual HRESULT ResetObject();

	virtual HRESULT SetImageExtentToParent( unsigned long p_ulIndex );
	virtual SVTaskObjectClass *GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint );
	virtual bool DoesObjectHaveExtents() const override;

	CString GetProfileOrientation();
	BOOL GetRotation();

	//************************************
	//! return Flag with enabled Autosize
	//! \returns  EnableSize  | EnablePosition
	//************************************
	virtual EAutoSize GetAutoSizeEnabled() override;
	BOOL IsToolRotated();

	virtual SVStaticStringValueObjectClass* GetInputImageNames() override;

	virtual BOOL IsValid() override;

#pragma endregion Public Methods

	//Embedded Object : Profile Orientation
	SVEnumerateValueObjectClass m_voProfileOrientation; // @WARNING:  bad practice making members public
	SVBoolValueObjectClass m_voUseProfileRotation; // @WARNING:  bad practice making members public

protected:
#pragma region Protected Methods
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );
#pragma endregion Protected Methods

	SVDoubleValueObjectClass m_svRotationAngle;
	SVDoubleValueObjectClass m_svRotationPointX;
	SVDoubleValueObjectClass m_svRotationPointY;

private:
#pragma region Private Methods
	virtual void init();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	SVStaticStringValueObjectClass m_svSourceImageNames;
#pragma endregion Member Variables
};


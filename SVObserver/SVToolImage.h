//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolImage
//* .File Name       : $Workfile:   SVToolImage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   07 May 2013 08:29:14  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVTool.h"
#pragma region Includes

class SVImageToolClass : public SVToolClass, public AllowResizeToParent
{
	SV_DECLARE_CLASS( SVImageToolClass );

public:
	SVImageToolClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVIMAGETOOL );
	virtual ~SVImageToolClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

	virtual SVTaskObjectClass *GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint );
	virtual bool DoesObjectHaveExtents() const override;
	virtual HRESULT SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent );

	virtual BOOL SetDefaultFormulas();

	virtual HRESULT ResetObject();

	virtual SVStaticStringValueObjectClass* GetInputImageNames() override;

	virtual HRESULT SetImageExtentToParent( unsigned long p_ulIndex );

	virtual BOOL IsValid() override;

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

	virtual HRESULT UpdateTranslation();

	virtual void init();

	SVBoolValueObjectClass		outputEnableOffsetA;
	SVPointValueObjectClass		outputOffsetAPoint;
	SVBoolValueObjectClass		outputEnableOffsetB;
	SVPointValueObjectClass		outputOffsetBPoint;
	SVLongValueObjectClass		outputOperator;
	// String value object for Source Image Names
	SVStaticStringValueObjectClass m_svSourceImageNames;
};


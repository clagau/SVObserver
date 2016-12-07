// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVThickeningFilter
// * .File Name       : $Workfile:   SVThickeningFilter.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   13 Aug 2013 10:37:30  $
// ******************************************************************************
#pragma once

#pragma region Includes
#include "SVFilterClass.h"
#pragma endregion Includes

class SVThickeningFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVThickeningFilterClass );
public:

	SVThickeningFilterClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTHICKENINGFILTER );
	virtual ~SVThickeningFilterClass();
	
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;

#pragma region virtual method (IFilter)
	virtual bool shouldResetInspection() const override { return true; }
#pragma region virtual method (IFilter)

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus ) override;

private:
	void init();

	SVLongValueObjectClass m_lvoItterations;
	SVBoolValueObjectClass m_bvoGrayOn;	
};


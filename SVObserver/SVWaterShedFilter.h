// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVWaterShedFilter
// * .File Name       : $Workfile:   SVWaterShedFilter.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   13 Aug 2013 10:32:08  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#include "SVFilterClass.h"
#pragma endregion Includes

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVWatershedFilterClass
// -----------------------------------------------------------------------------
// .Description : Base class for all kinds of Skeleton filters
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
////////////////////////////////////////////////////////////////////////////////
class SVWatershedFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVWatershedFilterClass );

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
// Standard constructor
	SVWatershedFilterClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVWATERSHEDFILTER );
	virtual ~SVWatershedFilterClass();
	
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

#pragma region virtual method (IFilter)
	virtual bool shouldResetInspection() const override { return true; }
#pragma region virtual method (IFilter)

private:
	void init();

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );
	virtual BOOL OnValidate();
	
private:
	SVInObjectInfoStruct m_MarkerImageInfo;
	SVBoolValueObjectClass m_bvoUseMarker ;
	SVLongValueObjectClass m_lvoMinVariation;
	SVLongValueObjectClass m_lvoControlFlag;
};


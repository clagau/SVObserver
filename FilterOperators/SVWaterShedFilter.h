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
#include "SVObjectLibrary/SVGetObjectDequeByTypeVisitor.h"
#include "SVFilterClass.h"
#pragma endregion Includes

class SVWatershedFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVWatershedFilterClass );

public:
	SVWatershedFilterClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVWATERSHEDFILTER );
	virtual ~SVWatershedFilterClass();
	
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

#pragma region virtual method (IFilter)
	virtual bool shouldResetInspection() const override { return true; }
#pragma region virtual method (IFilter)
protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus ) override;

private:
	void init();
	bool ValidateLocal( SvStl::MessageContainerVector * pErrorMessages ) const;


private:
	SVInObjectInfoStruct m_MarkerImageInfo;
	SVBoolValueObjectClass m_bvoUseMarker;
	SVLongValueObjectClass m_lvoMinVariation;
	SVLongValueObjectClass m_lvoControlFlag;
};


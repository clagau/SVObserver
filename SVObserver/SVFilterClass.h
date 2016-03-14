//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVFilterClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 09:54:04  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "ObjectInterfaces\ISVFilter.h"
#include "SVUnaryImageOperatorClass.h"
#pragma endregion Includes

class SVFilterClass : public SvOi::ISVFilter, public SVUnaryImageOperatorClass
{
	SV_DECLARE_CLASS( SVFilterClass );
public:
	SVFilterClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVFILTER );

	//************************************
	// Description: Creates the object
	// Parameter: pCreateStructure <in> The create structure
	// Return: True on success
	//************************************
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* pCreateStructure ) override;

#pragma region virtual methods (IFilter)
	virtual bool shouldResetInspection() const override { return false; };
#pragma endregion virtual methods (IFilter)

private:
	void init();
};



//******************************************************************************
//* COPYRIGHT (c) 2013 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "ObjectInterfaces/ISVFilter.h"
#include "Operators/SVUnaryImageOperatorClass.h"


#pragma endregion Includes

class SVFilterClass : public SvOi::ISVFilter, public SvOp::SVUnaryImageOperatorClass
{
	///This class does not need to call SV_DECLARE_CLASS as it is a base class and only derived classes are instantiated
	//SV_DECLARE_CLASS
public:
	SVFilterClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVFILTER );
	virtual ~SVFilterClass();

	//************************************
	// Description: Creates the object
	// Parameter: rCreateStructure <in> The create structure
	// Return: True on success
	//************************************
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

#pragma region virtual methods (IFilter)
	virtual bool shouldResetInspection() const override { return false; };
#pragma endregion virtual methods (IFilter)

private:
	void init();
};

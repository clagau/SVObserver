//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVBinaryOperatorClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 09:51:40  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "InspectionEngine/SVOperator.h"
#pragma endregion Includes

class SVBinaryOperatorClass : public SVOperatorClass
{
	SV_DECLARE_CLASS( SVBinaryOperatorClass );
public:
	SVBinaryOperatorClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVBINARYOPERATOR );
	virtual ~SVBinaryOperatorClass();

private:
	void init();
};


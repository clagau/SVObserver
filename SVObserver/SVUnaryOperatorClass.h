//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVUnaryOperatorClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Aug 2013 16:30:54  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOperator.h"
#include "SVObjectLibrary/SVObjectLibrary.h"
#pragma endregion Includes

class SVUnaryOperatorClass : public SVOperatorClass
{
	SV_DECLARE_CLASS( SVUnaryOperatorClass );
public:
	SVUnaryOperatorClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVUNARYOPERATOR );
	virtual ~SVUnaryOperatorClass();
	
private:
	void init();
};


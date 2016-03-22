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

#include "SVOperator.h"

class SVBinaryOperatorClass : public SVOperatorClass
{
	SV_DECLARE_CLASS( SVBinaryOperatorClass );
public:
	SVBinaryOperatorClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVBINARYOPERATOR );
	
private:
	void init();
};


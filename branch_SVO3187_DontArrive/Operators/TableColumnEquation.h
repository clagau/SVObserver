//*****************************************************************************
/// \copyright (c) 2016,2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file TableColumnEquation.h
/// All Rights Reserved 
//*****************************************************************************
/// Equation class for the column formula in table tool.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVEquation.h"
#pragma endregion Includes

namespace SvOp
{

class TableColumnEquation : public SVEquation
{
	SV_DECLARE_CLASS

#pragma region Constructor
public:
	TableColumnEquation( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_TABLECOLUMN_EQUATION );
	virtual ~TableColumnEquation();
#pragma endregion Constructor

#pragma region Public Methods
public:

#pragma endregion Public Methods

#pragma region Private Methods
private:
	void init();
#pragma endregion Private Methods

#pragma region Member Variables
private:

#pragma endregion Member Variables
};

} //namespace SvOp

//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
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

class TableColumnEquation : public SVEquationClass
{
	SV_DECLARE_CLASS( TableColumnEquation );

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
//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file TableAddColumnEquation.h
/// All Rights Reserved 
//*****************************************************************************
/// Equation class for the add column formula in table analyzer tool.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVEquation.h"
#include "SVValueObjectLibrary\DoubleSortValueObject.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

class TableAddColumnEquation : public SVEquationClass
{
	SV_DECLARE_CLASS(TableAddColumnEquation);

#pragma region Constructor
public:
	TableAddColumnEquation(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_TABLEADDCOLUMN_EQUATION);
	virtual ~TableAddColumnEquation();
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages = nullptr) override;
	virtual void SetName(LPCTSTR Name) override;

	void setResultColumn(DoubleSortValuePtr pResultColumn) { m_pResultColumn = pResultColumn; };
	void setIndexObject(SVLongValueObjectClass* pIndex) { m_pIndex = pIndex; };
#pragma endregion Public Methods

protected:
	virtual bool onRun(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr) override;

#pragma region Private Methods
private:
	void init();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	DoubleSortValuePtr m_pResultColumn;
	SVLongValueObjectClass	*m_pIndex;
#pragma endregion Member Variables
};

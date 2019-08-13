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

namespace SvVol
{
class SVLongValueObjectClass;
}

namespace SvOp
{

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

	void setResultColumn(SvVol::DoubleSortValuePtr pResultColumn) { m_pResultColumn = pResultColumn; };
	void setIndexObject(SvVol::SVLongValueObjectClass* pIndex) { m_pIndex = pIndex; };
#pragma endregion Public Methods

protected:
	virtual bool onRun(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr) override;

#pragma region Private Methods
private:
	void init();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	SvVol::DoubleSortValuePtr m_pResultColumn;
	SvVol::SVLongValueObjectClass* m_pIndex = nullptr;
#pragma endregion Member Variables
};

} //namespace SvOp

//*****************************************************************************
/// \copyright (c) 2018,2018 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file IndexEquation.h
/// All Rights Reserved 
//*****************************************************************************
/// Equation class to fill a DoubleSortValueObject (an array) by using an index value (e.g. used for the add column formula in table analyzer tool or custom feature in BlobAnalyzer2).
//******************************************************************************
#pragma once

#pragma region Includes
#include "EquationArray.h"
#include "SVValueObjectLibrary\DoubleSortValueObject.h"
#pragma endregion Includes

namespace SvVol
{
class SVLongValueObjectClass;
}

namespace SvOp
{

class IndexEquation : public EquationArray
{
	SV_DECLARE_CLASS

#pragma region Constructor
public:
	IndexEquation(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_INDEX_EQUATION);
	virtual ~IndexEquation();
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages = nullptr) override;
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName) override;

	void setResultColumn(SvVol::DoubleSortValuePtr pResultColumn) { m_pResultColumn = pResultColumn; };
	void setIndexObject(SvVol::SVLongValueObjectClass* pIndex) { m_pIndex = pIndex; };
#pragma endregion Public Methods

protected:
	virtual bool onRun(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr) override;

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

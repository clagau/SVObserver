//*****************************************************************************
/// \copyright (c) 2018,2018 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file TableAddColumnAnalyzer.h
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated the analyzer for adding a new column.
//******************************************************************************
#pragma once

#pragma region Includes
#include "InspectionEngine/SVTaskObject.h"
#include "SVObjectLibrary/InputObject.h"
#include "SVValueObjectLibrary/DoubleSortValueObject.h"
#pragma endregion Includes



namespace SvAo
{

class TableAddColumnAnalyzer : public SvIe::SVTaskObjectClass
{
#pragma region Constructor
	SV_DECLARE_CLASS
public:
	explicit TableAddColumnAnalyzer(LPCTSTR ObjectName);
	TableAddColumnAnalyzer(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_TABLEANALYZER_ADDCOLUMN);

	virtual ~TableAddColumnAnalyzer();
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages = nullptr) override;
#pragma endregion Public Methods

#pragma region Private Methods
private:
	void Initialize();
#pragma endregion Private Methods

#pragma region Member Variables
protected:
	SvOl::InputObject m_newColumnInput;
	SvVol::DoubleSortValuePtr m_pNewColumn;
#pragma endregion Member Variables
};

} //namespace SvAo

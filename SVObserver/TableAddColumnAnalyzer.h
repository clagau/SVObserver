//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file TableAddColumnAnalyzer.h
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated the analyzer for adding a new column.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectLibrary\SVObjectLibrary.h"
#include "InspectionEngine/SVTaskObject.h"
#include "SVValueObjectLibrary/DoubleSortValueObject.h"
#include "InspectionEngine/TableAddColumnEquation.h"
#pragma endregion Includes


#pragma region Declarations

#pragma endregion Declarations

class TableAddColumnAnalyzer : public SVTaskObjectClass
{
#pragma region Constructor
	SV_DECLARE_CLASS(TableAddColumnAnalyzer);
public:
	TableAddColumnAnalyzer(LPCTSTR ObjectName);
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
	TableAddColumnEquation* m_pColumnEquation;
	SvOl::SVInObjectInfoStruct m_newColumnObjectInfo;
	DoubleSortValuePtr m_pNewColumn;
#pragma endregion Member Variables
};

//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file TableAddColumnAnalyzer.h
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated the analyzer for adding a new column.
//******************************************************************************
#pragma once

#pragma region Includes
#include "InspectionEngine/SVTaskObject.h"
#include "SVValueObjectLibrary/DoubleSortValueObject.h"
#pragma endregion Includes


namespace SvAo
{

class TableAddColumnAnalyzer : public SvIe::SVTaskObjectClass
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
	SvOl::SVInObjectInfoStruct m_newColumnObjectInfo;
	SvVol::DoubleSortValuePtr m_pNewColumn;
#pragma endregion Member Variables
};

} //namespace SvAo

//*****************************************************************************
/// \copyright (c) 2018,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file TableDeleteColumnAnalyzer.h
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated the analyzer for delete a old column.
//******************************************************************************
#pragma once

#pragma region Includes
#include "InspectionEngine/SVTaskObject.h"
#include "SVObjectLibrary/InputObject.h"
#pragma endregion Includes



namespace SvAo
{

class TableDeleteColumnAnalyzer : public SvIe::SVTaskObjectClass
{
#pragma region Constructor
	SV_DECLARE_CLASS
public:
	explicit TableDeleteColumnAnalyzer(LPCTSTR ObjectName);
	TableDeleteColumnAnalyzer(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_TABLEANALYZER_DELETECOLUMN);

	virtual ~TableDeleteColumnAnalyzer();
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual bool ResetObject(SvStl::MessageContainerVector* pErrorMessages = nullptr) override;
#pragma endregion Public Methods

#pragma region Private Methods
private:
	void Initialize();
#pragma endregion Private Methods

#pragma region Member Variables
protected:
	SvOl::InputObject m_deleteColumnInput;
#pragma endregion Member Variables
};

} //namespace SvAo

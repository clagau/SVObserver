//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TableLimitAnalyzer.h
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated the analyzer for limiting the rows of the table.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectLibrary\SVObjectLibrary.h"
#include "SVOCore/SVTaskObject.h"
#include "SVValueObjectLibrary\LinkedValue.h"
#pragma endregion Includes

#pragma region Declarations

#pragma endregion Declarations

class TableLimitAnalyzer : public SVTaskObjectClass
{
#pragma region Constructor
	SV_DECLARE_CLASS( TableLimitAnalyzer );
public:
	TableLimitAnalyzer( LPCTSTR ObjectName );
	TableLimitAnalyzer( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_TABLEANALYZER_LIMIT );

	virtual ~TableLimitAnalyzer();
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* pCreateStructure ) override;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual BOOL onRun( SVRunStatusClass& rRunStatus ) override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void Initialize();

	/// Build the embedded object list for this task.
	void	BuildEmbeddedObjectList();

	/// Build the input object list for this task.
	void BuildInputObjectList ();
#pragma endregion Private Methods

#pragma region Member Variables
protected:
	LinkedValue m_LimitValue;
#pragma endregion Member Variables
};

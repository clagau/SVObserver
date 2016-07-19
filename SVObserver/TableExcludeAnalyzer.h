//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TableExcludeAnalyzer.h
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated the analyzer for excluding rows of the table.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectLibrary\SVObjectLibrary.h"
#include "SVTaskObject.h"
#include "DoubleSortValueObject.h"
#include "LinkedValue.h"
#pragma endregion Includes

#pragma region Declarations

#pragma endregion Declarations

class TableExcludeAnalyzer : public SVTaskObjectClass
{
#pragma region Constructor
	SV_DECLARE_CLASS( TableExcludeAnalyzer );
public:
	TableExcludeAnalyzer( LPCTSTR ObjectName );
	TableExcludeAnalyzer( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_TABLEANALYZER_EXCLUDE );

	virtual ~TableExcludeAnalyzer();
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* pCreateStructure ) override;
	virtual HRESULT ResetObject() override;
	virtual BOOL Validate() override;
	virtual BOOL OnValidate() override;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual bool ValidateOfflineParameters () override;
	virtual BOOL onRun( SVRunStatusClass& rRunStatus ) override;
	virtual DWORD_PTR processMessage(DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext) override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void Initialize();

	/// Build the embedded object list for this tool.
	void	BuildEmbeddedObjectList();

	void BuildInputObjectList ();
#pragma endregion Private Methods

#pragma region Member Variables
protected:
	SVInObjectInfoStruct m_excludeColumnObjectInfo;
	LinkedValue m_excludeHigh;
	LinkedValue m_excludeLow;
#pragma endregion Member Variables
};

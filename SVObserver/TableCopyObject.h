//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TableCopyObject.h
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated the table and copy the value from another table, used e.g. for TableAnaylzerTool
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectLibrary\SVObjectLibrary.h"
#include "TableObject.h"
#pragma endregion Includes

#pragma region Declarations

#pragma endregion Declarations

class TableCopyObject : public TableObject
{
#pragma region Constructor
	SV_DECLARE_CLASS( TableCopyObject );
public:
	TableCopyObject( LPCTSTR ObjectName );
	TableCopyObject( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_TABLE_OBJECT );

	virtual ~TableCopyObject();
#pragma endregion Constructor

#pragma region Public Methods
public:
	void setSourecTable(TableObject* sourceTable) { m_pSourceTable = sourceTable; };
	virtual HRESULT ResetObject() override;
	/// Set a new sort Container to this object and its column values.
	/// \param sortMap [in] the new container
	void setSortContainer(const ValueObjectSortContainer& sortMap, SVRunStatusClass& rRunStatus);
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual BOOL onRun( SVRunStatusClass& rRunStatus ) override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void Initialize();

#pragma endregion Private Methods

#pragma region Member Variables
private:
	TableObject* m_pSourceTable;
#pragma endregion Member Variables
};

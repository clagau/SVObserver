//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TableObject.h
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated the table use e.g. for TableTool
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectLibrary\SVObjectLibrary.h"
#include "SVTaskObject.h"
#include "DoubleSortValueObject.h"
#pragma endregion Includes

#pragma region Declarations

#pragma endregion Declarations

class TableObject : public SVTaskObjectClass
{
#pragma region Constructor
	SV_DECLARE_CLASS( TableObject );
public:
	TableObject( LPCTSTR ObjectName );
	TableObject( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_TABLE_OBJECT );

	virtual ~TableObject();
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* pCreateStructure ) override;
	virtual HRESULT ResetObject() override;

#pragma endregion Public Methods

#pragma region Protected Methods
protected:
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void Initialize();

	/// Build the embedded object list for this tool.
	void	BuildEmbeddedObjectList();
#pragma endregion Private Methods

#pragma region Member Variables
protected:
	int m_maxArray;
	ValueObjectSortContainer m_sortContainer;
	std::vector<DoubleSortValuePtr> m_ValueList;
	SVLongValueObjectClass m_NumberOfRows; 
#pragma endregion Member Variables
};

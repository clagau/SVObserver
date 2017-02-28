//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TableFillObject.h
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated the table and fill it with equation results e.g. for TableTool
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectLibrary\SVObjectLibrary.h"
#include "TableObject.h"
#include "TableColumnEquation.h"
#pragma endregion Includes

#pragma region Declarations

#pragma endregion Declarations

class TableFillObject : public TableObject
{
#pragma region Constructor
	SV_DECLARE_CLASS( TableFillObject );
public:
	TableFillObject( LPCTSTR ObjectName );
	TableFillObject( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_TABLE_OBJECT );

	virtual ~TableFillObject();
#pragma endregion Constructor

#pragma region Public Methods
public:
	/// Add, delete or rename value Objects for the column
	/// \param rColumEquationList [in] The list of the equation (every equation means one column)
	/// \param maxArray [in] Maximum of line in the table (size of the array in value object)
	void setColumnValueObjects(const std::vector<TableColumnEquation*>& rColumEquationList, int maxArray);
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void Initialize();

	/// Return the next unused embedded GUID for the columns
	/// \returns SVGUID
	SVGUID getNextFreeEmbeddedColumGUID();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	std::vector<TableColumnEquation*> m_equationList;
#pragma endregion Member Variables
};

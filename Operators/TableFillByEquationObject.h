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
#pragma endregion Includes

namespace SvOp
{
class TableColumnEquation;

class TableFillByEquationObject : public TableObject
{
#pragma region Constructor
	SV_DECLARE_CLASS( TableFillByEquationObject );
public:
	TableFillByEquationObject( LPCTSTR ObjectName );
	TableFillByEquationObject( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_TABLE_OBJECT );

	virtual ~TableFillByEquationObject();
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
#pragma endregion Private Methods

#pragma region Member Variables
private:
	std::vector<TableColumnEquation*> m_equationList;
#pragma endregion Member Variables
};

} //namespace SvOp

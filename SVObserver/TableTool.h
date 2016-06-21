//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This class perform the Table tool
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVTool.h"
#include "TableFillObject.h"
#pragma endregion Includes

#pragma region Declarations
class SVEquationClass;
class TableColumnEquation;
#pragma endregion Declarations

class TableTool : public SVToolClass
/// \brief This class perform the Table tool
{
	SV_DECLARE_CLASS (TableTool);

#pragma region Constructor
public:
	TableTool(	BOOL	bCreateDefaultTaskList = FALSE, 
		SVObjectClass*	pOwner = nullptr, 
		int	stringResourceID = IDS_CLASSNAME_TABLE_TOOL );

	virtual ~TableTool(void);

#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual BOOL	CreateObject( SVObjectLevelCreateStruct* pCreateStructure ) override;

	virtual bool DoesObjectHaveExtents() const override;

	virtual HRESULT ResetObject() override;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual BOOL onRun( SVRunStatusClass& rRunStatus ) override;
	virtual bool ValidateOfflineParameters () override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	/// Initialized the class.
	void	LocalInitialize();

	/// Build the input object list for this tool.
	void	BuildInputObjectList();

	/// Build the embedded object list for this tool.
	void	BuildEmbeddedObjectList();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	static const int c_defaultMaxRow = 50;
	//embedded parameter
	SVLongValueObjectClass m_MaxRow; 
	SVEquationClass* m_pClearEquation;
	std::vector<TableColumnEquation*> m_ColumnEquationList;
	TableFillObject* m_pTable;
#pragma endregion Member Variables
};

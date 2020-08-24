//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This class perform the Table tool
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVTool.h"
#include "Operators/TableFillByEquationObject.h"
#pragma endregion Includes

namespace SvOp
{
class SVEquation;
class TableColumnEquation;
}

namespace SvTo
{

class TableTool : public SVToolClass
/// \brief This class perform the Table tool
{
	SV_DECLARE_CLASS (TableTool);

#pragma region Constructor
public:
	TableTool(	SVObjectClass*	pOwner = nullptr, int stringResourceID = IDS_CLASSNAME_TABLE_TOOL );

	virtual ~TableTool(void);

#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual HRESULT GetOutputList( SVOutputInfoListClass& p_rOutputInfoList ) const override;

	virtual bool DoesObjectHaveExtents() const override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	bool ValidateLocal( SvStl::MessageContainerVector * pErrorMessages ) const;

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
	SvVol::SVLongValueObjectClass m_MaxRow;
	SvOp::SVEquation* m_pClearEquation;
	std::vector<SvOp::TableColumnEquation*> m_ColumnEquationList;
	SvOp::TableFillByEquationObject* m_pTable;
#pragma endregion Member Variables
};

} //namespace SvTo

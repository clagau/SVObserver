//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TableAnaylzerTool.h
/// All Rights Reserved 
//*****************************************************************************
/// This class perform the Table Analyzer Tool
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVTool.h"
#include "SVObjectLibrary/InputObject.h"
#include "Operators/TableCopyObject.h"
#pragma endregion Includes

namespace SvOp
{
class SVLongResult;
}

namespace SvTo
{

class TableAnalyzerTool : public SVToolClass
/// \brief This class perform the Table Analyzer Tool
{
	SV_DECLARE_CLASS

#pragma region Constructor
public:
	TableAnalyzerTool( SVObjectClass* pOwner = nullptr, int stringResourceID = IDS_CLASSNAME_TABLEANALYZER_TOOL );

	virtual ~TableAnalyzerTool(void);

#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual bool DoesObjectHaveExtents() const override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	SvVol::ValueObjectSortContainer&  getSortContainer() { return m_pResultTable->getSortContainer();};
	void UpdateNumberOfRows() { return m_pResultTable->UpdateNumberOfRows(); };

	/// Add a new column to the table, which is add on to the copied column of the source table.
	/// \param name [in] Name of the new column.
	/// \param pAnalyzer [in] Analyzer which requested the new column.
	/// \returns DoubleSortValuePtr Pointer to the new column object.
	SvVol::DoubleSortValuePtr addNewColumn(LPCTSTR name, const SVTaskObjectClass* pAnalyzer);

	/// Set a column to the new column list at the result table object.
	/// \param pColumn [in] The object which should be set.
	/// \param pAnalyzer [in] Analyzer which requested the new column.
	/// \returns DoubleSortValuePtr Shared pointer of the set object.
	SvVol::DoubleSortValuePtr setNewColumn(const SVObjectClass* pColumn, const SVTaskObjectClass* pAnalyzer);

	/// Remove a add on column form the table.
	/// \param pColumn [in] Column object to be removed.
	void removeNewColumn(const SvVol::DoubleSortValuePtr pColumn);

	virtual void OnEmbeddedIDChanged(const SVObjectClass* pOwnerObject, SvPb::EmbeddedIdEnum oldEmbeddedID, SvPb::EmbeddedIdEnum newEmbeddedID) override;
	virtual bool areAuxExtentsAvailable() const override { return false; };
	virtual std::vector<std::string> getToolAdjustNameList() const override;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual bool useOverlayColorTool() const override { return false; };
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	bool ValidateLocal( SvStl::MessageContainerVector * pErrorMessages ) const;

	/// Initialized the class.
	void LocalInitialize();

	/// Build the input object list for this tool.
	void BuildInputObjectList();

	/// Calculate the position for the new column depending of the order of the taskObjects.
	/// \param pAnalyzer [in] Analyzer object for the new column.
	/// \returns int Position.
	int calcNewColumnPosition(const SVTaskObjectClass* pAnalyzer);
#pragma endregion Private Methods

#pragma region Member Variables
private:
	SvOp::TableCopyObject* m_pResultTable;
	SvOl::InputObject m_sourceTableInput;
	SvOp::SVLongResult* m_pResult = nullptr;
#pragma endregion Member Variables
};

} //namespace SvTo

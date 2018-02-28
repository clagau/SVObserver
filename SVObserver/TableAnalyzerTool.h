//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TableAnaylzerTool.h
/// All Rights Reserved 
//*****************************************************************************
/// This class perform the Table Analyzer Tool
//******************************************************************************
#pragma once

#pragma region Includes
#include "InspectionEngine/SVTool.h"
#include "TableCopyObject.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

class TableAnalyzerTool : public SVToolClass
/// \brief This class perform the Table Analyzer Tool
{
	SV_DECLARE_CLASS (TableAnalyzerTool);

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

	/// The method inserts a task object to this task object list. This method must be override for TableAnalyzer, 
	/// because if a Analyzer will be insert, the index will increased by one, because the first task is always the ResultTable.
	/// \param index <in> position where to insert.
	/// \param rObject <in> object to insert.
	/// \param count <in> Default value is 1.
	virtual void InsertAt(int index, ITaskObject& rObject, int count = 1) override;

	void setSortContainer(const ValueObjectSortContainer& sortMap, SVRunStatusClass& rRunStatus) {m_pResultTable->setSortContainer(sortMap, rRunStatus);};
	const ValueObjectSortContainer&  getSortContainer() const { return m_pResultTable->getSortContainer();};

	/// Add a new column to the table, which is add on to the copied column of the source table.
	/// \param name [in] Name of the new column.
	/// \returns DoubleSortValuePtr Pointer to the new column object.
	DoubleSortValuePtr addNewColumn(LPCTSTR name);

	/// Set a column to the new column list at the result table object.
	/// \param pColumn [in] The object which should be set.
	/// \returns DoubleSortValuePtr Shared pointer of the set object.
	DoubleSortValuePtr setNewColumn(const SVObjectClass* pColumn);

	/// Remove a add on column form the table.
	/// \param pColumn [in] Column object to be removed.
	void removeNewColumn(const DoubleSortValuePtr pColumn);

	virtual void OnEmbeddedIDChanged(const SVObjectClass* pOwnerObject, const SVGUID& rOldEmbeddedID, const SVGUID& rNewEmbeddedID) override;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	bool ValidateLocal( SvStl::MessageContainerVector * pErrorMessages ) const;

	/// Initialized the class.
	void LocalInitialize();

	/// Build the input object list for this tool.
	void BuildInputObjectList();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	TableCopyObject* m_pResultTable;
	SVInObjectInfoStruct m_sourceTableObjectInfo;
	// Index Counter...
	// Can be used in Equation for e.g. Add-Column-Analyzer.
	// NOTE:
	// If Equation supports vector calculations also in future time it can be replaced.
	SVLongValueObjectClass	m_Index;
#pragma endregion Member Variables
};

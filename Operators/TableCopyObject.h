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

namespace SvOp
{

class TableCopyObject : public TableObject
{
#pragma region Constructor
	SV_DECLARE_CLASS( TableCopyObject );
public:
	explicit TableCopyObject( LPCTSTR ObjectName );
	TableCopyObject( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_TABLE_OBJECT );

	virtual ~TableCopyObject();
#pragma endregion Constructor

#pragma region Public Methods
public:
	void setSourecTable(TableObject* sourceTable) { m_pSourceTable = sourceTable; };
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	/// Add a new column to the table, which is add on to the copied column of the source table.
	/// \param name [in] Name of the new column.
	/// \param pos [in] Add the column to the new position. If pos == -1, add it at the end.
	/// \returns DoubleSortValuePtr Pointer to the new column object.
	SvVol::DoubleSortValuePtr addNewColumn(LPCTSTR name, int pos = -1);

	/// Set a column to the new column list at the result table object.
	/// \param pColumn [in] The object which should be set.
	/// \param pos [in] Add the column to the new position. If pos == -1, add it at the end.
	/// \returns DoubleSortValuePtr Shared pointer of the set object.
	SvVol::DoubleSortValuePtr setNewColumn(const SVObjectClass* pColumn, int pos = -1);

	/// Remove a add on column form the table.
	/// \param pColumn [in] Column object to be removed.
	void removeNewColumn(const SvVol::DoubleSortValuePtr pColumn);

	/// Change the embeddedId of a copy column to a new ID.
	/// \param rOldId [in] Old embeddedID.
	/// \param rNewId [in] New embeddedID.
	void changeEmbeddedId(const SVGUID& rOldId, const SVGUID& rNewId);
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void Initialize();

	/// Remove all unused column objects.
	void RemoveUnusedColumn();

	/// Reset all column objects which are copy from source table.
	/// \returns int Position of the last copy object in the m_ValueList.
	int ResetCopyColumn();

	/// Reset all new column objects (which are not from the source table.)
	/// \param valueListPos [in] This is the position of the last entry before the new columns.
	/// \param arraySize [in] Array of the columns.
	/// \param pErrorMessages [in,out] Error message vector.
	/// \returns bool if successfully.
	bool ResetNewColumns(int valueListPos, int arraySize, SvStl::MessageContainerVector * pErrorMessages);

	/// Send to other tool which use this object a message that the embeddedID has changed.
	/// \param oldEmbeddedGuid [in] old embedded ID.
	/// \param newEmbeddedGuid [in] new embedded ID.
	void sendChangedEmbeddedIDToUser(SVGUID oldEmbeddedGuid, SVGUID newEmbeddedGuid);

	/// Add a column to the new value list a the required position.
	/// \param pos [in] Add position. If pos == -1, add it at the end.
	/// \param pColumn [in] The new Column.
	void AddEntryToNewValueList(int pos, SvVol::DoubleSortValuePtr pColumn);
#pragma endregion Private Methods

#pragma region Member Variables
private:
	TableObject* m_pSourceTable;
	std::vector<SvVol::DoubleSortValuePtr> m_NewValueList;
#pragma endregion Member Variables
};

} //namespace SvOp

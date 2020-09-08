//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TableObject.h
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated the table use e.g. for TableTool
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectLibrary/SVObjectLibrary.h"
#include "InspectionEngine/SVTaskObject.h"
#include "SVValueObjectLibrary/DoubleSortValueObject.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#pragma endregion Includes

namespace SvOp //< Operators
{

class TableObject : public SvIe::SVTaskObjectClass
{
#pragma region Constructor
	SV_DECLARE_CLASS( TableObject );
public:
	explicit TableObject( LPCTSTR ObjectName );
	TableObject( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_TABLE_OBJECT );

	virtual ~TableObject();
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	SVObjectClass* getNumberOfRowObject() const;

	const std::vector<SvVol::DoubleSortValuePtr>& getValueList() const { return m_ValueList; };
	SvVol::ValueObjectSortContainer& getSortContainer() ;
	/// Set a new sort Container to this object and its column values.
	/// \param sortMap [in] the new container
	void setSortContainer(const SvVol::ValueObjectSortContainer& sortMap, RunStatus& rRunStatus);

	void  setSortContainerDummy(const SvVol::DummySortContainer& rDummy);
	
	void UpdateNumberOfRows();

	/// Update (Name and array size) column if column (with embeddedID) exist, if not exist create column.
	/// \param embeddedID [in] emdedded Id.
	/// \param nameId [in] ID of the new name
	/// \param arraysize [in] New array size.
	/// \returns DoubleSortValueObject Pointer to the valueObject or nullptr if creation failed.
	SvVol::DoubleSortValuePtr updateOrCreateColumn(SvPb::EmbeddedIdEnum embeddedId, int nameId, int arraysize);
	/// Update (Name and array size) column if column (with embeddedID) exist, if not exist create column.
	/// \param embeddedID [in] emdedded Id.
	/// \param newname [in]  the new name
	/// \param arraysize [in] New array size.
	/// \returns DoubleSortValueObject Pointer to the valueObject or nullptr if creation failed.
	SvVol::DoubleSortValuePtr updateOrCreateColumn(SvPb::EmbeddedIdEnum embeddedId, const std::string& newName, int arraysize);

	/// Remove a column.
	/// \param embeddedId [in] emdedded Id of the value object.
	void removeColumn(SvPb::EmbeddedIdEnum embeddedId);

	/// Clear the table
	void clearTable();

	virtual SVObjectClass* OverwriteEmbeddedObject(uint32_t uniqueID, SvPb::EmbeddedIdEnum embeddedID) override;

	/// for rValue  a 2 dim SafeArray of double  is expected, otherwise false is returned
	bool setTableValues(const _variant_t& rValue);
	/// rValue contents a 2 dim SafeArray of double  
	void  getTableValues(_variant_t& rValue,long* pRowCount, long* pColoumnCount)const;
	/// rValue contents a 1 dim Safe array of BSTR with the columnames returnvalue is size
	unsigned  getColumNames(_variant_t& rValue) const;
	/// hide table in ResultPicker
	void Hide(bool hide);
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	/// Create a new column value Object. If creation failed, this method throw an exception.
	/// \param embeddedID [in] The EmbeddedId of the new object.
	/// \param name [in] The name of the new object.
	/// \param arraySize [in] The array size of the new object.
	SvVol::DoubleSortValuePtr createColumnObject(SvPb::EmbeddedIdEnum embeddedId, LPCTSTR name, int arraySize);

	/// Updated object-name and max number for the array of a columnValueObject.
	/// \param pos [in] Position in the m_ValueList.
	/// \param objectName [in] The new name.
	/// \param maxArray [in] The new maximum numbers of the array.
	void UpdateColumnValueObject(int pos, std::string objectName, int maxArray);

	/// Moved an columnValueObject in the m_ValueList and also in the embedded-list (this is necessary to keep the order after reloading)
	/// \param oldPos [in] The old position in the m_ValueList.
	/// \param newPos [in] The new position in the m_ValueList.
	void MoveValueColumn(int oldPos, int newPos);

	/// Return the next unused embedded Id for the columns
	/// \returns SvPb::EmbeddedIdEnum
	SvPb::EmbeddedIdEnum getNextFreeEmbeddedColumID();
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void Initialize();

	/// Build the embedded object list for this tool.
	void	BuildEmbeddedObjectList();
#pragma endregion Private Methods

#pragma region Member Variables
protected:
	SvVol::DummySortContainer m_DummySortContainer;
	SvVol::spValueObjectSortContainer m_spSortContainer {nullptr};
	std::vector<SvVol::DoubleSortValuePtr> m_ValueList;
	SvVol::SVLongValueObjectClass m_NumberOfRows;
#pragma endregion Member Variables
};

} //namespace SvOp

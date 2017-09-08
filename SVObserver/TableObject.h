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
#include "SVOCore/SVTaskObject.h"
#include "SVValueObjectLibrary/DoubleSortValueObject.h"
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
	virtual bool CreateObject( SVObjectLevelCreateStruct* pCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	SVObjectClass* getNumberOfRowObject() const;

	const std::vector<DoubleSortValuePtr>& getValueList() const { return m_ValueList; };
	const ValueObjectSortContainer& getSortContainer() const { return m_sortContainer; };
	/// Set a new sort Container to this object and its column values.
	/// \param sortMap [in] the new container
	void setSortContainer(const ValueObjectSortContainer& sortMap, SVRunStatusClass& rRunStatus);

	/// Update (Name and array size) column if column (with embeddedID) exist, if not exist create column.
	/// \param rEmbeddedID [in] Emdedded GUID.
	/// \param nameId [in] ID of the new name
	/// \param arraysize [in] New array size.
	/// \returns DoubleSortValueObject* Pointer to the valueObject or nullptr if creation failed.
	DoubleSortValueObject* updateOrCreateColumn(const GUID& rEmbeddedID, int nameId, int arraysize);

	/// Remove a column.
	/// \param rEmbeddedId [in] Emdedded GUID of the value object.
	void removeColumn(const GUID& rEmbeddedId);

	/// Clear the table
	void clearTable();

	virtual SVObjectClass* OverwriteEmbeddedObject(const GUID& uniqueID, const GUID& rEmbeddedID) override;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	/// Create a new column value Object. If creation failed, this method throw an exception.
	/// \param embeddedID [in] The EmbeddedId of the new object.
	/// \param name [in] The name of the new object.
	/// \param arraySize [in] The array size of the new object.
	DoubleSortValueObject* createColumnObject(SVGUID embeddedID, LPCTSTR name, int arraySize);
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void Initialize();

	/// Build the embedded object list for this tool.
	void	BuildEmbeddedObjectList();
#pragma endregion Private Methods

#pragma region Member Variables
protected:
	ValueObjectSortContainer m_sortContainer;
	std::vector<DoubleSortValuePtr> m_ValueList;
	SVLongValueObjectClass m_NumberOfRows; 
#pragma endregion Member Variables
};

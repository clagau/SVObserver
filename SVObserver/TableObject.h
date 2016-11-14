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
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* pCreateStructure ) override;
	virtual HRESULT ResetObject() override;

	SVObjectClass* getNumberOfRowObject() const;

	const std::vector<DoubleSortValuePtr>& getValueList() const { return m_ValueList; };
	const ValueObjectSortContainer& getSortContainer() const { return m_sortContainer; };

	virtual SVObjectClass* OverwriteEmbeddedObject(const GUID& uniqueID, const GUID& rEmbeddedID) override;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	/// Create a new column value Object. If creation failed, this method throw an exception.
	/// \param embeddedID [in] The EmbeddedId of the new object.
	/// \param name [in] The name of the new object.
	/// \param arraySize [in] The array size of the new object.
	void createColumnObject(SVGUID embeddedID, LPCTSTR name, int arraySize);
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

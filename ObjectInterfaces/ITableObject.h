//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for TableObject
//******************************************************************************

#pragma once

#pragma region Includes
#include <vector>
#include "SVValueObjectLibrary/DoubleSortValueObject.h"
#pragma endregion Includes


namespace SvOi
{
	//This class is a interface to allow access to TableObjects. 
	//It currently used only by ExternalTool GUIs (via Protobuf).
	//@todo: it may require rework before using it in business logic (add / remove methods)
	class ITableObject
	{
	public:
		virtual const std::vector<SvVol::DoubleSortValuePtr>& getValueList() const = 0;
		virtual void setSortContainer(const SvVol::ValueObjectSortContainer& sortMap) = 0;

		virtual void  setSortContainerDummy(const SvVol::DummySortContainer& rDummy) = 0;

		virtual void UpdateNumberOfRows() = 0;
		virtual SvVol::DoubleSortValuePtr updateOrCreateColumn(SvPb::EmbeddedIdEnum embeddedId, int nameId, int arraysize) = 0;
		virtual SvVol::DoubleSortValuePtr updateOrCreateColumn(SvPb::EmbeddedIdEnum embeddedId, const std::string& newName, int arraysize) = 0;
		virtual void removeColumn(SvPb::EmbeddedIdEnum embeddedId) = 0;
		virtual void clearTable() = 0;
		virtual bool setTableValues(const _variant_t& rValue) = 0;
		virtual void  getTableValues(_variant_t& rValue, long* pRowCount, long* pColoumnCount) const = 0;
		virtual unsigned  getColumNames(_variant_t& rValue) const = 0;
		virtual void Hide(bool hide) = 0;

	};
}
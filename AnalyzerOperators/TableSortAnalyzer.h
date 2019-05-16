//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TableSortAnalyzer.h
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated the analyzer for sorting the table.
//******************************************************************************
#pragma once

#pragma region Includes
#include "InspectionEngine/SVTaskObject.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#pragma endregion Includes

namespace SvAo
{

class TableSortAnalyzer : public SvIe::SVTaskObjectClass
{
#pragma region Constructor
	SV_DECLARE_CLASS(TableSortAnalyzer);
public:
	TableSortAnalyzer(LPCTSTR ObjectName);
	TableSortAnalyzer(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_TABLEANALYZER_SORT);

	virtual ~TableSortAnalyzer();
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages = nullptr) override;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual bool onRun(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr) override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void Initialize();

	/// Build the embedded object list for this task.
	void BuildEmbeddedObjectList();

	/// Build the input object list for this task.
	void BuildInputObjectList();

	/// Check if tmp-array is big enough and realloc if not.
	/// \param sizeTmp [in] The needed minimum size of the tmp-array
	void CheckAndResizeTmpArray(size_t sizeTmp);
#pragma endregion Private Methods

#pragma region Member Variables
protected:
	SvOl::SVInObjectInfoStruct m_sortColumnObjectInfo;
	SvVol::SVBoolValueObjectClass m_isASC;
	std::shared_ptr<double> m_tmpValues;
	size_t m_tmpArraySize;
#pragma endregion Member Variables
};

} //namespace SvAo

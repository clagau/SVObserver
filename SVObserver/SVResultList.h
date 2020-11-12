//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultList.h
//* .File Name       : $Workfile:   SVResultList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   07 Jan 2015 17:49:30  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <concrt.h>
//Moved to precompiled header: #include <vector>
#include "SVInfoStructs.h"
#include "ResultViewReferences.h"

#pragma endregion Includes


namespace SvIe
{
struct SVIPResultData;
class SVTaskObjectClass;
}
namespace SvOp
{
class SVResult;
}

class SVToolSet;
enum SVInspectionStateEnum;

class SVResultList
{
public:
#pragma region Constructor
	SVResultList();
	virtual ~SVResultList();	// no hierarchy; but so we can have RTTI
#pragma endregion Constructor

#pragma region Public Methods
	void Destroy();
	void Refresh(SvIe::SVTaskObjectClass* pRootObject);

	void SetToolSet(SVToolSet* pToolSet);

	SVProductInspectedState GetInspectionState();

	//************************************
	// Description:  Build SVIPResultData from the reference vector.  (Get the values of the variables.)
	// Parameter:  rResultData <out>:  object containing data from the class's variables.
	// Parameter:  getColor <in>:  true (default) if the color for the item in the result view should be fetched.
	//************************************
	void  GetResultData(SvIe::SVIPResultData& rResultData) const;
	std::vector <SvIe::IPResultTableData> getResultTableData(const SvOi::ITriggerRecordR& rTriggerRecord);

	uint32_t getTableId() const { return m_ResultViewReferences.getTableId(); }
	void setTableId(uint32_t id) { m_ResultViewReferences.setTableId(id); }

	//************************************
	//! Description:  Rebuild the reference vector from the list of IDs.
	//! \param pInspection [in] if not null the References will be checked if the are  a Disabled PPQVariables
	//! \returns void
	//************************************
	void RebuildReferenceVector(SVInspectionProcess* pInspection = nullptr);
	
	void Clear();
	
	//************************************
	// Description:  Build SVResultDefinitionVector from the reference vector.  (Defines the list of viewed variables.)
	// Parameter:  rDefinitions <out>:  object containing data from the class's variables.
	// Returns:  S_OK if no error occurs   
	//************************************
	HRESULT GetResultDefinitions( ResultViewReferences::SVResultDefinitionVector& rDefinitions ) const; 

	bool LoadViewedVariables(ResultViewReferences::SVTreeType& rTree, ResultViewReferences::SVTreeType::SVBranchHandle htiParent);
	
	const SVObjectReferenceVector& GetSelectedObjects() const;
	bool Insert(const std::string& rDottedName);
	double getUpdateTimeStamp();
	void Save(SvOi::IObjectWriter& rWriter);

#pragma endregion Public Methods

#pragma region Member Variables
protected:
	mutable Concurrency::critical_section m_Lock;
	ResultViewReferences  m_ResultViewReferences;
	SVToolSet* m_pToolSet;
	std::vector<SvOp::SVResult*> m_results;
#pragma endregion Member Variables
};


//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultList.cpp
//* .File Name       : $Workfile:   SVResultList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   07 Jan 2015 17:48:18  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVResultList.h"

#include "InspectionEngine/SVIPResultData.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "Operators/SVResult.h"
#include "SVInspectionProcess.h"
#include "SVToolSet.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
SVResultList::SVResultList()
: m_ResultViewReferences(SvXml::CTAG_VIEWEDRESULTS), m_pToolSet(nullptr)
{
}

SVResultList::~SVResultList()
{
	Destroy();
	m_results.clear();
}
#pragma endregion Constructor

void SVResultList::Destroy()
{
	m_ResultViewReferences.Clear();
}

void SVResultList::SetToolSet(SVToolSet* pToolSet)
{
	m_pToolSet = pToolSet;
}

void SVResultList::Refresh(SvIe::SVTaskObjectClass* pRootObject)
{
	std::lock_guard<std::mutex> lock(m_dataMutex);

	m_results.clear();

	// find all result classes
	// pRootObject is a pointer to SVToolSet at this time
	SvDef::SVObjectTypeInfoStruct info;
	info.m_ObjectType = SvPb::SVResultObjectType;

	std::vector<SvOi::IObjectClass*> list;
	fillObjectList(std::back_inserter(list), info, pRootObject->getObjectId());

	for (const auto pObject : list)
	{
		SvOp::SVResult* pResult = dynamic_cast<SvOp::SVResult*> (pObject);

		m_results.push_back( pResult );
	}
	
	m_ResultViewReferences.RebuildReferenceVector(dynamic_cast<SVInspectionProcess*>(m_pToolSet->GetInspection()));
}

double SVResultList::getUpdateTimeStamp()
{
	std::lock_guard<std::mutex> lock(m_dataMutex);

	return m_ResultViewReferences.getUpdateTimeStamp();
}

void SVResultList::Save(SvOi::IObjectWriter& rWriter)
{
	std::lock_guard<std::mutex> lock(m_dataMutex);

	m_ResultViewReferences.Save(rWriter);
}

bool SVResultList::LoadViewedVariables(ResultViewReferences::SVTreeType& rTree, ResultViewReferences::SVTreeType::SVBranchHandle htiParent)
{
	std::lock_guard<std::mutex> lock(m_dataMutex);

	m_ResultViewReferences.Clear();
	SVInspectionProcess* pInspec(nullptr);
	if (m_pToolSet)
	{
		pInspec = dynamic_cast<SVInspectionProcess*>(m_pToolSet->GetInspection());
	}
	
	bool SevenOneCfg(false);
	bool SevenTwoCfg(false);

	ResultViewReferences::SVTreeType::SVBranchHandle htiChild = nullptr;

	if (SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_VIEWEDVARIABLES, htiParent, htiChild))
	{
		SevenTwoCfg = m_ResultViewReferences.Load( rTree, htiChild );
		if ( false == SevenTwoCfg )
		{
			///older configurations may have a ViewedEnvVariables entry! 
			m_ResultViewReferences.Load( rTree, htiChild, SvXml::CTAG_VIEWEDENVARIABLES);
			SevenOneCfg = m_ResultViewReferences.Load( rTree, htiChild, SvXml::CTAG_VIEWEDTOOLVARIABLES);
		}
	}
	
	if ( !SevenTwoCfg )
	{
		m_ResultViewReferences.InsertFromPPQInputs(pInspec);
	}

	if ( !SevenTwoCfg && !SevenOneCfg )
	{
		///older configurations may have no ViewedToolVariables entry!
		m_ResultViewReferences.InsertFromOutputList(pInspec);
	}

	return true;
}

void SVResultList::RebuildReferenceVector(SVInspectionProcess* pInspection )
{
	std::lock_guard<std::mutex> lock(m_dataMutex);
	m_mustUpdate = false;
	return m_ResultViewReferences.RebuildReferenceVector(pInspection);
}

void SVResultList::updateIfRequired()
{
	if (m_mustUpdate)
	{
		RebuildReferenceVector(dynamic_cast<SVInspectionProcess*>(m_pToolSet->GetInspection()));
	}
}

void  SVResultList::GetResultData(SvIe::SVIPResultData& rResultData) const
{
	std::lock_guard<std::mutex> lock(m_dataMutex);

	m_ResultViewReferences.GetResultData( rResultData);
}

std::vector <SvIe::IPResultTableData> SVResultList::getResultTableData(const SvOi::ITriggerRecordR& rTriggerRecord)
{
	std::lock_guard<std::mutex> lock(m_dataMutex);
	return m_ResultViewReferences.getResultTableData(rTriggerRecord);
}

HRESULT SVResultList::GetResultDefinitions( ResultViewReferences::SVResultDefinitionVector& rDefinitions )  const
{
	std::lock_guard<std::mutex> lock(m_dataMutex);

	return m_ResultViewReferences.GetResultDefinitions(rDefinitions);
}



SVProductInspectedState SVResultList::GetInspectionState()
{
	// Only if both values are false the product is good !!!!
	bool masterFailed = false;
	bool masterWarned = false;

	for( auto pResult : m_results)
	{
		masterFailed |= pResult->IsFailed();
		masterWarned |= pResult->IsWarned();
	}

	if (masterFailed)
	{
		return(PRODUCT_INSPECTION_FAILED);
	}

	if (masterWarned)
	{
		return(PRODUCT_INSPECTION_WARNING);
	}

	return( PRODUCT_INSPECTION_PASSED );
}

void SVResultList::Clear()
{
	m_ResultViewReferences.Clear();
}

const SVObjectReferenceVector& SVResultList::GetSelectedObjects() const
{
	std::lock_guard<std::mutex> lock(m_dataMutex);

	return m_ResultViewReferences.GetSelectedObjects();
}

bool SVResultList::Insert( const std::string& rDottedName )
{
	std::lock_guard<std::mutex> lock(m_dataMutex);

	return m_ResultViewReferences.Insert( rDottedName );
}


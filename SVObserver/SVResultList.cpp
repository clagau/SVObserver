//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
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
SVResultListClass::SVResultListClass()
: m_ResultViewReferences(SvXml::CTAG_VIEWEDRESULTS)
{
}

SVResultListClass::~SVResultListClass()
{
	Destroy();
	m_results.clear();
}
#pragma endregion Constructor

void SVResultListClass::Destroy()
{
	m_ResultViewReferences.Clear();
}

void SVResultListClass::SetToolSet(SVToolSetClass* pToolSet)
{
	m_pToolSet = pToolSet;
}

void SVResultListClass::Refresh(SvIe::SVTaskObjectClass* pRootObject)
{
	Concurrency::critical_section::scoped_lock  AutoLock(m_Lock);

	m_results.clear();

	// find all result classes
	// pRootObject is a pointer to SVToolSetClass at this time
	SvDef::SVObjectTypeInfoStruct info;
	info.ObjectType = SvPb::SVResultObjectType;

	SVGetObjectDequeByTypeVisitor l_Visitor( info );

	SVObjectManagerClass::Instance().VisitElements( l_Visitor, pRootObject->GetUniqueObjectID() );

	SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

	for( l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end(); ++l_Iter )
	{
		SvOp::SVResultClass* pResult = dynamic_cast<SvOp::SVResultClass*> (const_cast<SVObjectClass*> (*l_Iter));

		m_results.push_back( pResult );
	}
	
	m_ResultViewReferences.RebuildReferenceVector(dynamic_cast<SVInspectionProcess*>(m_pToolSet->GetInspection()));
}

double SVResultListClass::getUpdateTimeStamp()
{
	Concurrency::critical_section::scoped_lock  AutoLock(m_Lock);

	return m_ResultViewReferences.getUpdateTimeStamp();
}

void SVResultListClass::Save(SvOi::IObjectWriter& rWriter)
{
	Concurrency::critical_section::scoped_lock  AutoLock(m_Lock);

	m_ResultViewReferences.Save(rWriter);
}

bool SVResultListClass::LoadViewedVariables(ResultViewReferences::SVTreeType& rTree, ResultViewReferences::SVTreeType::SVBranchHandle htiParent)
{
	Concurrency::critical_section::scoped_lock  AutoLock(m_Lock);

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

void SVResultListClass::RebuildReferenceVector(SVInspectionProcess* pInspection )
{
	Concurrency::critical_section::scoped_lock  AutoLock(m_Lock);

	return m_ResultViewReferences.RebuildReferenceVector(pInspection);
}

void  SVResultListClass::GetResultData(SvIe::SVIPResultData& rResultData) const
{
	Concurrency::critical_section::scoped_lock  AutoLock(m_Lock);

	m_ResultViewReferences.GetResultData( rResultData);
}

std::vector <SvIe::IPResultTableData> SVResultListClass::getResultTableData(SvTrc::ITriggerRecordRPtr pTriggerRecord)
{
	Concurrency::critical_section::scoped_lock  AutoLock(m_Lock);
	return m_ResultViewReferences.getResultTableData(pTriggerRecord);
}

HRESULT SVResultListClass::GetResultDefinitions( ResultViewReferences::SVResultDefinitionDeque& rDefinitions )  const
{
	Concurrency::critical_section::scoped_lock  AutoLock(m_Lock);

	return m_ResultViewReferences.GetResultDefinitions(rDefinitions);
}



SVProductInspectedState SVResultListClass::GetInspectionState()
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

void SVResultListClass::Clear()
{
	m_ResultViewReferences.Clear();
}

const SVObjectReferenceVector& SVResultListClass::GetSelectedObjects() const
{
	Concurrency::critical_section::scoped_lock  AutoLock(m_Lock);

	return m_ResultViewReferences.GetSelectedObjects();
}

bool SVResultListClass::Insert( const std::string& rDottedName )
{
	Concurrency::critical_section::scoped_lock  AutoLock(m_Lock);

	return m_ResultViewReferences.Insert( rDottedName );
}


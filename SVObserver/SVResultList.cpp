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

#include "SVObjectLibrary/GlobalConst.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#include "SVResult.h"
#include "SVInspectionProcess.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVToolSet.h"
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
	m_results.RemoveAll();
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

void SVResultListClass::Refresh(SVTaskObjectClass* pRootObject)
{
	m_results.RemoveAll();

	// find all result classes
	// pRootObject is a pointer to SVToolSetClass at this time
	SVObjectTypeInfoStruct info;
	info.ObjectType = SVResultObjectType;

	SVGetObjectDequeByTypeVisitor l_Visitor( info );

	SVObjectManagerClass::Instance().VisitElements( l_Visitor, pRootObject->GetUniqueObjectID() );

	SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

	for( l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end(); ++l_Iter )
	{
		SVResultClass* pResult = dynamic_cast< SVResultClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

		m_results.Add( pResult );
	}
	
	m_ResultViewReferences.RebuildReferenceVector(dynamic_cast<SVInspectionProcess*>(m_pToolSet->GetInspection()));
}

void SVResultListClass::Save(SVObjectWriter& rWriter)
{
	m_ResultViewReferences.Save(rWriter);
}

bool SVResultListClass::LoadViewedVariables(ResultViewReferences::SVTreeType& rTree, ResultViewReferences::SVTreeType::SVBranchHandle htiParent)
{
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
	return m_ResultViewReferences.RebuildReferenceVector(pInspection);
}

void  SVResultListClass::GetResultData( SVIPResultData& p_rResultData) const
{
	m_ResultViewReferences.GetResultData( p_rResultData);
	m_ResultViewReferences.GetResultTableData(p_rResultData);
}

HRESULT SVResultListClass::GetResultDefinitions( ResultViewReferences::SVResultDefinitionDeque& rDefinitions )  const
{
	return m_ResultViewReferences.GetResultDefinitions(rDefinitions);
}



SVProductInspectedState SVResultListClass::GetInspectionState()
{
	// Only if both values are FALSE the product is good !!!!
	BOOL masterFailed = FALSE;
	BOOL masterWarned = FALSE;

	for( int i = 0; i < m_results.GetSize(); ++i )
	{
		masterFailed |= m_results.GetAt( i )->IsFailed();
		masterWarned |= m_results.GetAt( i )->IsWarned();
	}

	if( masterFailed )
		return( PRODUCT_INSPECTION_FAILED );

	if( masterWarned )
		return( PRODUCT_INSPECTION_WARNING );

	return( PRODUCT_INSPECTION_PASSED );
}

void SVResultListClass::Clear()
{
	m_ResultViewReferences.Clear();
}

const SVObjectReferenceVector& SVResultListClass::GetSelectedObjects() const
{
	return m_ResultViewReferences.GetSelectedObjects();
}

bool SVResultListClass::Insert( const SVString& rDottedName )
{
	return m_ResultViewReferences.Insert( rDottedName );
}


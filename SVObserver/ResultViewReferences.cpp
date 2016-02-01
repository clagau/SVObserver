//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ResultViewReferences
//* .File Name       : $Workfile:   ResultViewReferences.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   07 Jan 2015 17:35:06  $
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVRunControlLibrary/SVRunControlLibrary.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "BasicValueObject.h"
#include "ResultViewReferences.h"
#include "SVIPResultData.h"
#include "SVIPResultItemDefinition.h"
#include "SVResultsWrapperClass.h"
#include "SVValueObjectReference.h"
#include "SVToolSet.h"
#include "SVInspectionProcess.h"
#pragma endregion Includes

#pragma region Constructor
ResultViewReferences::ResultViewReferences(LPCTSTR tagname)
	: m_LastUpdateTimeStamp(0),
	  m_TagName(tagname)
{
}

ResultViewReferences::~ResultViewReferences()
{
	m_ResultViewItemDefList.clear();
	m_ReferenceVector.clear();
}
#pragma endregion Constructor

SVClock::SVTimeStamp ResultViewReferences::getUpdateTimeStamp()
{
	return m_LastUpdateTimeStamp;
}

bool ResultViewReferences::IsViewable(const SVObjectReference& objectRef) const
{
	return (objectRef.ObjectAttributesAllowed() & SV_VIEWABLE);
}

bool ResultViewReferences::Load( SVTreeType& rTree, SVTreeType::SVBranchHandle hParent, LPCTSTR TagName )
{

	CString csTagName =  TagName == nullptr?  m_TagName: TagName;
	SVTreeType::SVBranchHandle hVariables = nullptr;
	bool bOK = SVNavigateTree::GetItemBranch(rTree, csTagName, hParent, hVariables);

	if (bOK)
	{
		SVTreeType::SVLeafHandle hVariable( rTree.getFirstLeaf(hVariables) );
		bool bContinue = rTree.isValidLeaf( hVariables, hVariable );

		while (bContinue)
		{
			bContinue = LoadResultViewItemDef(rTree, hVariable);
			hVariable  =rTree.getNextLeaf(hVariables, hVariable);
			bContinue = bContinue && rTree.isValidLeaf( hVariables, hVariable );
		}
	}
	m_LastUpdateTimeStamp = SVClock::GetTimeStamp();
	return bOK;
}

bool ResultViewReferences::LoadResultViewItemDef( SVTreeType& rTree, SVTreeType::SVLeafHandle htiLeaf )
{
	SVString Name( rTree.getLeafName( htiLeaf ) );

	bool bOK = ( Name.Compare( CTAG_COMPLETENAME ) == 0 );

	_variant_t svValue;
	svValue = rTree.getLeafData( htiLeaf );
	bOK = bOK && ( VT_EMPTY != svValue.vt);
	if ( bOK )
	{
		SVObjectReference objRef;
		CString dottedName = svValue;
		bOK = (SVObjectManagerClass::Instance().GetObjectByDottedName( dottedName, objRef ) == S_OK);
		if ( bOK && objRef.Object() )
		{
		
			if (IsViewable(objRef))
			{
				ResultViewItemDef itemDef(objRef);
				m_ResultViewItemDefList.push_back(itemDef);
				m_ReferenceVector.push_back(objRef);
			}
		}
	}
	return bOK;
}

bool ResultViewReferences::Insert( const SVString &dottedName )
{
	SVObjectReference objRef;
	bool bOK = ( S_OK == SVObjectManagerClass::Instance().GetObjectByDottedName( dottedName, objRef ) );

	if ( bOK && objRef.Object() )
	{

		ResultViewItemDef itemDef(objRef);
		m_ResultViewItemDefList.push_back(itemDef);
		m_ReferenceVector.push_back(objRef);
	}
	return bOK;
}

bool ResultViewReferences::Save(SVObjectWriter& rWriter)
{
	rWriter.StartElement(m_TagName);
	std::vector<SVObjectReference>::const_iterator it = m_ReferenceVector.begin();
	for( ; it != m_ReferenceVector.end(); ++it )
	{
		if( nullptr != it->Object() )
		{
			CString csObjectName = it->GetCompleteOneBasedObjectName();
			_variant_t var(csObjectName);
			rWriter.WriteAttribute(_T("CompleteName"), var);
			var.Clear();
		}
	}
	rWriter.EndElement();
	return true;
}

int ResultViewReferences::GetNameSet(SVStringSet& stringSet) const
{
	int res =0;
	std::vector<SVObjectReference>::const_iterator it = m_ReferenceVector.begin();
	for( ; it != m_ReferenceVector.end(); ++it)
	{
		if( nullptr != it->Object() )
		{
			SVString ObjectName = it->GetCompleteOneBasedObjectName();
			stringSet.insert(ObjectName);	
			++res;
		}
	}
	return res;
}

void ResultViewReferences::Clear()
{
	m_ReferenceVector.clear();
	m_ResultViewItemDefList.clear();
	m_LastUpdateTimeStamp = SVClock::GetTimeStamp();
}


void ResultViewReferences::RebuildReferenceVector( SVInspectionProcess* pIProcess)
{
	m_ReferenceVector.clear();
	std::list<ResultViewItemDef>::iterator it = m_ResultViewItemDefList.begin();
	while( it != m_ResultViewItemDefList.end() )
	{
		bool bInsert(false);
		SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( it->getGuid() );
		if( pObject != nullptr)
		{
			bInsert = true;
			if(pIProcess  != nullptr  && pIProcess->IsDisabledPPQVariable(dynamic_cast<SVValueObjectClass*>(pObject)) == true )
			{
				bInsert = false;
			}

		}
		if( bInsert )
		{
			SVObjectReference ObjectRef(pObject);
			if( it->hasIndex() )
			{
				ObjectRef.SetArrayIndex(it->getIndexValue());
			}
			m_ReferenceVector.push_back(ObjectRef);

			it++;
		}
		else
		{
			std::list<ResultViewItemDef>::iterator eit = it;
			it = m_ResultViewItemDefList.erase(eit);
		}
	}
	m_LastUpdateTimeStamp = SVClock::GetTimeStamp();
}

HRESULT  ResultViewReferences::GetResultData( SVIPResultData& p_rResultData) const
{
	std::vector<SVObjectReference>::const_iterator it = m_ReferenceVector.begin();
	for( ; it != m_ReferenceVector.end(); ++it)
	{
		if( nullptr == it->Object() )
			continue;

		SVIPResultItemDefinition itemDef;

		if( it->IsIndexPresent() )
		{
			itemDef = SVIPResultItemDefinition( it->Guid(), it->ArrayIndex() );
		}
		else
		{
			itemDef = SVIPResultItemDefinition( it->Guid() );
		}

		unsigned long Color = SV_DEFAULT_WHITE_COLOR;

		if( it->Object()->GetOwner() )
		{
			Color = it->Object()->GetOwner()->GetObjectColor();
		}


		CString csValue;
		BasicValueObject* bvo(nullptr); 
		SVValueObjectReference voref(*it);  // try to assign to value object
		if( voref.Object() )                // if successful
		{
			if( voref->GetObjectType() == SVStringValueObjectType)
			{
				CString l_strQuotes;
				CString l_strValue;
				voref.GetValue( l_strValue );
				// Wrap string in Quotes...
				l_strQuotes.Format(_T("\042%s\042"),l_strValue);
				csValue = l_strQuotes;
			}
			else
			{
				HRESULT hr = voref.GetValue( csValue );
				if ( hr == SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE )
				{
					csValue = _T("< ") + csValue + _T(" >");
				}
				else if ( hr != S_OK )
				{
					csValue = _T( "<Not Valid>" );
				}
			}
		}
		else  if(nullptr != (bvo = dynamic_cast<BasicValueObject*>(it->Object()))) 
		{

			Color = SV_DEFAULT_WHITE_COLOR;


			SVString value;
			HRESULT hr = bvo->getValue(value);

			if ( hr != S_OK )
			{
				csValue = _T( "<Not Valid>" );
			}
			else
			{
				csValue = value.c_str();
			}

		}
		p_rResultData.m_ResultData[ itemDef ] = SVIPResultItemData( static_cast< LPCTSTR >( csValue ), Color );

	}

	return S_OK;
}

HRESULT  ResultViewReferences::GetResultDefinitions( SVResultDefinitionDeque &rDefinitions ) const
{
	for( size_t i = 0; i < m_ReferenceVector.size(); ++i )
	{
		SVObjectReference ref = m_ReferenceVector.at(i);
		if( ref.Object() != nullptr )
		{
			SVIPResultItemDefinition Def;

			if( ref.IsIndexPresent() )
			{
				Def = SVIPResultItemDefinition( ref.Guid(), ref.ArrayIndex() );
			}
			else
			{
				Def = SVIPResultItemDefinition( ref.Guid() );
			}

			rDefinitions.push_back( Def );
		}
	}

	return S_OK;
}

int ResultViewReferences::AddResults(  SVResultsWrapperClass* pSVRWC, LPCTSTR lptitle )
{
	CString szCol1, szCol2, szCol3, szCol4;
	DWORD dwColor = 0;
	size_t i = 0;
	int iNumberOfItems = 0;
	int nType;

	for( i = 0; i < m_ReferenceVector.size(); ++i )
	{
		SVObjectReference ref = m_ReferenceVector.at(i);
		if( ref.Object() )
		{
			// Insert all items that are picked for viewing
			szCol1 = ref.GetName();

			dwColor = 0;

			SVValueObjectReference voref(ref);  // try to assign to value object
			if( voref.Object() )                // if successful
			{
				if( voref->GetObjectType() == SVStringValueObjectType)
				{
					CString l_strQuotes;
					CString l_strValue;
					voref.GetValue( l_strValue );
					// Wrap string in Quotes...
					l_strQuotes.Format(_T("\042%s\042"),l_strValue);
					szCol2 = l_strQuotes;
				}
				else
				{
					HRESULT hr = voref.GetValue( szCol2 );
					if ( hr == SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE )
						szCol2 = _T("< ") + szCol2 + _T(" >");
					else if ( hr != S_OK )
						szCol2 = _T( "<Not Valid>" );
				}

				// Set Color...
				dwColor = ref.Object()->GetObjectColor();
				if( ref.Object()->GetOwner() )
					dwColor = ref.Object()->GetOwner()->GetObjectColor();

				nType = ref.Object()->GetObjectType();
			}

			szCol3 = ref.GetCompleteObjectNameToObjectType( NULL, SVToolObjectType );
			szCol4.Format(_T("%d"),iNumberOfItems);

			CString szIPD = lptitle;
			szIPD += _T( ".ipd" );
			pSVRWC->AddData( iNumberOfItems, szIPD, szCol1, szCol2, szCol3, szCol4, dwColor, nType);

			++iNumberOfItems;
		}
	}

	return iNumberOfItems;
}// end AddResults



void ResultViewReferences::InsertFromOutputList(SVInspectionProcess* pInspection)
{
	SVToolSetClass* pToolSet = nullptr;
	if( nullptr != pInspection )
	{
		pToolSet = pInspection->GetToolSet();
	}

	if(nullptr == pToolSet)
	{
		ASSERT(FALSE);
		return;
	}



	SVObjectReferenceVector RefVector;

	// Find all outputs marked as selected for viewing
	SVOutputInfoListClass OutputList;
	pToolSet->GetOutputList( OutputList );
	OutputList.GetSetAttributesList( SV_VIEWABLE, RefVector );

	for(SVObjectReferenceVector::const_iterator iter = RefVector.begin(); iter != RefVector.end(); ++iter)
	{
		if( nullptr != iter->Object() && iter->ObjectAttributesSet() & SV_VIEWABLE )
		{
			m_ReferenceVector.push_back(*iter);
			ResultViewItemDef item(*iter);
			m_ResultViewItemDefList.push_back(item);
		}
	}

	m_LastUpdateTimeStamp = SVClock::GetTimeStamp();
}


void ResultViewReferences::InsertFromPPQInputs(SVInspectionProcess* pInspection)
{
	// Insert all PPQ input items that are picked for viewing
	
	size_t PpqInputSize = 0;
	if( nullptr != pInspection )
	{
		PpqInputSize = pInspection->m_PPQInputs.size();
		for( size_t l = 0; pInspection && l < PpqInputSize; ++l )
		{
			SVIOEntryStruct pIOEntry = pInspection->m_PPQInputs[l];
			SVObjectClass * object  = pIOEntry.m_IOEntryPtr->m_pValueObject;
			if( nullptr != object && object->ObjectAttributesSet() & SV_VIEWABLE )
			{
				SVObjectReference ref(object);
				ResultViewItemDef item(ref);
				m_ReferenceVector.push_back(ref);
				m_ResultViewItemDefList.push_back(item);
			}
		}
	}
	m_LastUpdateTimeStamp = SVClock::GetTimeStamp();
}



//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\ResultViewReferences.cpp_v  $
 * 
 *    Rev 1.0   07 Jan 2015 17:35:06   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  980
 * SCR Title:  Add Non-Inspection Objects to the Result View
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
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
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVRunControlLibrary/SVRunControlLibrary.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVValueObjectLibrary/BasicValueObject.h"
#include "ResultViewReferences.h"
#include "SVOCore/SVIPResultData.h"
#include "SVOCore/SVIPResultItemDefinition.h"
#include "SVToolSet.h"
#include "SVInspectionProcess.h"
#pragma endregion Includes

#pragma region Constructor
ResultViewReferences::ResultViewReferences(LPCTSTR tagname)
	: m_LastUpdateTimeStamp(0),
	  m_TagName(tagname)
	, m_resultTableGuid( SV_GUID_NULL )
	, m_resultTable(nullptr)
{
}

ResultViewReferences::~ResultViewReferences()
{
	m_ResultViewItemDefList.clear();
	m_ReferenceVector.clear();
}
#pragma endregion Constructor

bool ResultViewReferences::IsViewable(const SVObjectReference& objectRef) const
{
	return (objectRef.ObjectAttributesAllowed() & SvDef::SV_VIEWABLE);
}

bool ResultViewReferences::Load( SVTreeType& rTree, SVTreeType::SVBranchHandle hParent, LPCTSTR TagName )
{

	SVString BranchName =  (nullptr == TagName) ?  m_TagName: TagName;
	SVTreeType::SVBranchHandle hVariables = nullptr;
	bool bOK = SvXml::SVNavigateTree::GetItemBranch(rTree, BranchName.c_str(), hParent, hVariables);

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
	m_LastUpdateTimeStamp = SvTl::GetTimeStamp();
	return bOK;
}

bool ResultViewReferences::LoadResultViewItemDef( SVTreeType& rTree, SVTreeType::SVLeafHandle htiLeaf )
{
	SVString Name( rTree.getLeafName( htiLeaf ) );

	_variant_t Value;
	rTree.getLeafData( htiLeaf, Value );
	bool bOK = ( VT_EMPTY != Value.vt);
	if ( bOK )
	{
		if (0 == Name.compare(SvXml::CTAG_COMPLETENAME))
		{
			SVObjectReference objRef;
			SVString dottedName = SvUl_SF::createSVString(Value);
			bOK = (S_OK == SVObjectManagerClass::Instance().GetObjectByDottedName(dottedName, objRef));
			if (bOK && objRef.getObject())
			{
				if (IsViewable(objRef))
				{
					ResultViewItemDef itemDef(objRef);
					m_ResultViewItemDefList.push_back(itemDef);
					m_ReferenceVector.push_back(objRef);
				}
			}
		}
		else if (0 == Name.compare(SvXml::CTAG_VIEWEDTABLE))
		{
			SVObjectReference objRef;
			SVGUID tableGuid(Value);
			TableObject* table = dynamic_cast<TableObject*>(SVObjectManagerClass::Instance().GetObject(Value));
			if (nullptr != table)
			{
				m_resultTableGuid = tableGuid;
				m_resultTable = table;
			}
			else
			{
				bOK = false;
			}
		}
		else
		{
			bOK = false;
		}
	}
	return bOK;
}

bool ResultViewReferences::Insert( const SVString &rDottedName )
{
	SVObjectReference objRef;
	bool bOK = ( S_OK == SVObjectManagerClass::Instance().GetObjectByDottedName( rDottedName, objRef ) );

	if ( bOK && objRef.getObject() )
	{

		ResultViewItemDef itemDef(objRef);
		m_ResultViewItemDefList.push_back(itemDef);
		m_ReferenceVector.push_back(objRef);
	}
	return bOK;
}

bool ResultViewReferences::Save(SVObjectWriter& rWriter)
{
	rWriter.StartElement( m_TagName.c_str() );
	std::vector<SVObjectReference>::const_iterator it = m_ReferenceVector.begin();
	for( ; it != m_ReferenceVector.end(); ++it )
	{
		if( nullptr != it->getObject() )
		{
			_variant_t Value;
			Value.SetString( it->GetCompleteOneBasedObjectName().c_str() );
			rWriter.WriteAttribute(SvXml::CTAG_COMPLETENAME, Value);
		}
	}
	if (SV_GUID_NULL != m_resultTableGuid)
	{
		_variant_t Value;
		Value.SetString(m_resultTableGuid.ToBSTR());
		rWriter.WriteAttribute(SvXml::CTAG_VIEWEDTABLE, Value);
	}
	rWriter.EndElement();
	return true;
}

const SVObjectReferenceVector& ResultViewReferences::GetSelectedObjects() const
{
	return m_ReferenceVector;
}

void ResultViewReferences::Clear()
{
	m_ReferenceVector.clear();
	m_ResultViewItemDefList.clear();
	m_LastUpdateTimeStamp = SvTl::GetTimeStamp();
}


void ResultViewReferences::RebuildReferenceVector( SVInspectionProcess* pIProcess)
{
	m_ReferenceVector.clear();
	std::list<ResultViewItemDef>::iterator it = m_ResultViewItemDefList.begin();
	while( it != m_ResultViewItemDefList.end() )
	{
		bool bInsert(false);
		SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( it->getGuid() );
		if( nullptr != pObject )
		{
			bInsert = true;
			if( nullptr != pIProcess && true == pIProcess->IsDisabledPPQVariable(pObject) )
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

	m_resultTable = dynamic_cast<TableObject*>(SVObjectManagerClass::Instance().GetObject(m_resultTableGuid));

	m_LastUpdateTimeStamp = SvTl::GetTimeStamp();
}

void  ResultViewReferences::GetResultData( SVIPResultData& p_rResultData) const
{
	std::vector<SVObjectReference>::const_iterator it = m_ReferenceVector.begin();
	for( ; it != m_ReferenceVector.end(); ++it)
	{
		if( nullptr == it->getObject() )
			continue;

		SVIPResultItemDefinition itemDef;

		if( it->isArray() && -1 != it->ArrayIndex() )
		{
			itemDef = SVIPResultItemDefinition( it->Guid(), it->ArrayIndex() );
		}
		else
		{
			itemDef = SVIPResultItemDefinition( it->Guid() );
		}

		unsigned long Color = SV_DEFAULT_WHITE_COLOR;

		if( it->getObject()->GetOwner() )
		{
			Color = it->getObject()->GetOwner()->GetObjectColor();
		}


		SVString Value;
		if( it->getObject()->GetObjectSubType() == SVStringValueObjectType )
		{
			SVString ValueString;
			it->getValueObject()->getValue( ValueString, it->getValidArrayIndex() );
			// Wrap string in Quotes...
			Value = SvUl_SF::Format(_T("\042%s\042"), ValueString.c_str());
		}
		else
		{
			if( !it->isEntireArray() )
			{
				HRESULT hr = it->getValueObject()->getValue( Value, it->getValidArrayIndex() );
				if ( hr == SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE )
				{
					Value = _T("< ") + Value + _T(" >");
				}
				else if ( S_OK != hr )
				{
					Value = _T( "<Not Valid>" );
				}
			}
			else
			{
				HRESULT hr = it->getValueObject()->getValue( Value );
				if ( S_OK != hr )
				{
					Value = _T( "<Not Valid>" );
				}
			}
		}

		p_rResultData.m_ResultData[ itemDef ] = SVIPResultItemData( Value, Color );
	}
}


void ResultViewReferences::GetResultTableData(SVIPResultData &p_rResultData) const
{
	if (nullptr != m_resultTable)
	{
		const std::vector<DoubleSortValuePtr>& valueList = m_resultTable->getValueList();
		for (DoubleSortValuePtr valuePtr : valueList)
		{
			DoubleSortValueObject* valueObject = valuePtr.get();
			if (nullptr != valueObject)
			{
				IPResultTableData data = IPResultTableData();
				data.m_LastUpdateTimeStamp = m_LastUpdateTimeStamp;
				data.m_columnName = valueObject->GetName();
				valueObject->getValues(data.m_rowData);
				p_rResultData.m_ResultTableData.push_back(data);
			}
		}
	}
}


HRESULT  ResultViewReferences::GetResultDefinitions( SVResultDefinitionDeque &rDefinitions ) const
{
	for( size_t i = 0; i < m_ReferenceVector.size(); ++i )
	{
		SVObjectReference ObjectRef = m_ReferenceVector.at(i);
		if( nullptr != ObjectRef.getObject() )
		{
			SVIPResultItemDefinition Def;

			if( ObjectRef.isArray() && -1 != ObjectRef.ArrayIndex() )
			{
				Def = SVIPResultItemDefinition( ObjectRef.Guid(), ObjectRef.ArrayIndex() );
			}
			else
			{
				Def = SVIPResultItemDefinition( ObjectRef.Guid() );
			}

			rDefinitions.push_back( Def );
		}
	}

	return S_OK;
}




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
	OutputList.GetSetAttributesList( SvDef::SV_VIEWABLE, RefVector );

	for(SVObjectReferenceVector::const_iterator iter = RefVector.begin(); iter != RefVector.end(); ++iter)
	{
		if( nullptr != iter->getObject() && iter->ObjectAttributesSet() & SvDef::SV_VIEWABLE )
		{
			m_ReferenceVector.push_back(*iter);
			ResultViewItemDef item(*iter);
			m_ResultViewItemDefList.push_back(item);
		}
	}

	m_LastUpdateTimeStamp = SvTl::GetTimeStamp();
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
			SVObjectClass* pObject  = pIOEntry.m_IOEntryPtr->getObject();
			if( nullptr != pObject && pObject->ObjectAttributesSet() & SvDef::SV_VIEWABLE )
			{
				SVObjectReference ObjectRef(pObject);
				ResultViewItemDef item(ObjectRef);
				m_ReferenceVector.push_back(ObjectRef);
				m_ResultViewItemDefList.push_back(item);
			}
		}
	}
	m_LastUpdateTimeStamp = SvTl::GetTimeStamp();
}

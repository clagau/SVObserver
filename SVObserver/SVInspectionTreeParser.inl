	//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInspectionTreeParser
//* .File Name       : $Workfile:   SVInspectionTreeParser.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   19 Dec 2014 04:10:42  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/assign.hpp>
#include "SVInspectionTreeParser.h"
#include "SVXMLLibrary\SVNavigateTree.h"
#include "SVObjectLibrary\SVObjectBuilder.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVXMLLibrary\SVConfigurationTags.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVInspectionProcess.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVUtilityLibrary\SVGUID.h"
#pragma endregion Includes

static SVStringSet g_ObjectAttributeFilter = boost::assign::list_of< SVString >
( SVString(scObjectNameTag) )
( SVString(scClassIDTag) )
( SVString(scUniqueReferenceIDTag) )
( SVString(scEmbeddedIDTag) )
;

// The attribute object type table is needed to convert string type to point or double point type.
typedef std::map<SVString, SVObjectScriptDataObjectTypeEnum> SVObjectAttributeTypeMap;

static SVObjectAttributeTypeMap g_ObjectAttributeType = boost::assign::map_list_of<>
(_T( "RotationCenter" ), SV_POINT_Type)
(_T( "Translation" ), SV_POINT_Type)
;

template< typename SVTreeType >
SVInspectionTreeParser< SVTreeType >::SVInspectionTreeParser(SVTreeType& rTreeCtrl, typename SVTreeType::SVBranchHandle hItem, unsigned long parserHandle, const GUID& OwnerGuid, SVObjectClass* pOwnerObject, CWnd* pWnd)
: SVObjectScriptParserBase(parserHandle, OwnerGuid, pOwnerObject, pWnd)
, m_rTree(rTreeCtrl)
, m_rootItem(hItem)
, m_count(0)
, m_ReplaceUniqueID(true)
{
	m_totalSize = m_rTree.getCount();
}

template< typename SVTreeType >
SVInspectionTreeParser< SVTreeType >::~SVInspectionTreeParser() 
{ 
}


template< typename SVTreeType >
HRESULT SVInspectionTreeParser< SVTreeType >::DoParse()
{
	HRESULT hr = S_OK;
	m_count = 0;
	hr = Process(m_rootItem, m_OwnerGuid);
	return hr;
}

template< typename SVTreeType >
size_t SVInspectionTreeParser< SVTreeType >::GetTotal() const
{
	// get count of nodes
	return m_totalSize;
}

template< typename SVTreeType >
bool SVInspectionTreeParser< SVTreeType >::GetItemValue(const SVString& tag, typename SVTreeType::SVBranchHandle hItem, _variant_t& rValue)
{
	m_count++;
	return SvXml::SVNavigateTree::GetItem(m_rTree, tag.c_str(), hItem, rValue) ? true : false;
}

template< typename SVTreeType >
bool SVInspectionTreeParser< SVTreeType >::GetValues(typename SVTreeType::SVBranchHandle hItem, const SVString& tag, SVVariantList& rValueList)
{
	bool bRetVal = false;

	SVTreeType::SVBranchHandle hBranch = nullptr;
	SvXml::SVNavigateTree::GetItemBranch(m_rTree, tag.c_str(), hItem, hBranch);
	if(nullptr != hBranch)
	{
		bRetVal = true;
		SVTreeType::SVLeafHandle hValue;
		hValue = m_rTree.getFirstLeaf(hBranch);
		do
		{
			if( m_rTree.isValidLeaf(hBranch, hValue) )
			{
				m_count++;
				_variant_t Data;

				m_rTree.getLeafData(hValue, Data);
				rValueList.push_back( Data );

				hValue = m_rTree.getNextLeaf(hBranch, hValue);
			}
		} while( m_rTree.isValidLeaf(hBranch, hValue) );
	}
	return bRetVal;
}

template< typename SVTreeType >
HRESULT SVInspectionTreeParser< SVTreeType >::Process(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID)
{
	HRESULT hr = S_OK;

	SVString name( m_rTree.getBranchName(hItem) );
	m_count++;

	if (scAttributesSetTag != name)
	{
		_variant_t objectName;
		_variant_t classID;
		_variant_t uniqueID;
		
		SvXml::SVNavigateTree::GetItem(m_rTree, scObjectNameTag, hItem, objectName);
		m_count++;

		SvXml::SVNavigateTree::GetItem(m_rTree, scClassIDTag, hItem, classID);
		m_count++;

		if( m_ReplaceUniqueID )
		{
			SvXml::SVNavigateTree::GetItem(m_rTree, scUniqueReferenceIDTag, hItem, uniqueID);
		}
		m_count++;
		
	
		GUID objectID( GUID_NULL );
		if( m_ReplaceUniqueID )
		{
			objectID = SVGUID(uniqueID);
		}
		//Create this object only if it is not the same as the owner GUID
		if( ownerID == objectID )
		{
			//If the owner and object ID are the same the
			m_ReplaceUniqueID = false;
		}
		else
		{
			hr = SVObjectBuilder::CreateObject(SVGUID(classID), objectID, name, SvUl_SF::createSVString(objectName), ownerID);
		}
		if (S_OK == hr)
		{
			UpdateProgress(m_count, m_totalSize);

			// this will be different for embeddeds, it will use the owning object ID and the embedded object ID
			hr = ProcessAttributes(objectID, objectID, hItem);
		
			UpdateProgress(m_count, m_totalSize);

			hr = ProcessChildren(hItem, objectID);

			UpdateProgress(m_count, m_totalSize);
		}
	}
	return hr;
}

template< typename SVTreeType >
HRESULT SVInspectionTreeParser< SVTreeType >::ProcessChildren(typename SVTreeType::SVBranchHandle hParentItem, const GUID& ownerID)
{
	HRESULT hr = S_OK;

	_variant_t value;

	if (SvXml::SVNavigateTree::GetItem(m_rTree, scEquationBufferTag, hParentItem, value))
	{
		hr = ProcessEquation(ownerID, value);
	}

	if (SvXml::SVNavigateTree::GetItem(m_rTree, scMaskDataTag, hParentItem, value))
	{
		hr = ProcessMaskData(ownerID, value);
	}

	// iterate thru nodes and process
	SVTreeType::SVBranchHandle hItem( nullptr );
	hItem = m_rTree.getFirstBranch(hParentItem);
	do
	{
		if (hItem)
		{
			UpdateProgress(++m_count, m_totalSize);

			SVString Name = m_rTree.getBranchName(hItem);

			if (Name == scFriendsTag)
			{
				hr = ProcessFriends(hItem, ownerID);
			}
			else if (Name == scEmbeddedsTag)
			{
				hr = ProcessEmbeddeds(hItem, ownerID);
			}
			else if (Name == scInputsTag)
			{
				hr = ProcessInputs(hItem, ownerID);
			}
			else 
			{
				// process this node
				if (SvXml::SVNavigateTree::HasChildren(m_rTree, hItem))
				{
					hr = Process(hItem, ownerID);
				}
			}

			hItem = m_rTree.getNextBranch(hParentItem, hItem);
		}
	}
	while (hItem);

	return hr;
}

template< typename SVTreeType >
HRESULT SVInspectionTreeParser< SVTreeType >::ProcessFriends(typename SVTreeType::SVBranchHandle hFriends, const GUID& ownerID)
{
	HRESULT hr = S_OK;
	// Destroy Friends
	SVObjectBuilder::DestroyFriends(ownerID);

	SVTreeType::SVBranchHandle hItem( nullptr );
	hItem = m_rTree.getFirstBranch(hFriends);
	do
	{
		if (hItem)
		{
			UpdateProgress(++m_count, m_totalSize); 
			hr = ProcessFriend(hItem, ownerID);
			hItem = m_rTree.getNextBranch(hFriends, hItem);
		}
	} while (hItem);
	return hr;
}

template< typename SVTreeType >
HRESULT SVInspectionTreeParser< SVTreeType >::ProcessFriend(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID)
{
	HRESULT hr = S_OK;

	_variant_t objectName;
	_variant_t classID;
	_variant_t uniqueID;
	_variant_t attributesAllowed;

	SVString Name;
		
	Name = m_rTree.getBranchName(hItem);
	
	GetItemValue(scObjectNameTag, hItem, objectName);
	GetItemValue(scClassIDTag, hItem, classID);
	GetItemValue(scUniqueReferenceIDTag, hItem, uniqueID);

	UpdateProgress(m_count, m_totalSize);

	// Build the Object
	GUID objectID = SVGUID(uniqueID);
	hr = SVObjectBuilder::CreateFriendObject(SVGUID(classID), objectID, SvUl_SF::createSVString(objectName), ownerID);
	if (S_OK == hr)
	{
		// this will be different for embeddeds, it will use the owning object ID and the embedded object ID
		hr = ProcessAttributes(objectID, objectID, hItem);
		if (S_OK == hr)
		{
			hr = ProcessChildren(hItem, objectID);
		}
	}
	m_count++;
	UpdateProgress(m_count, m_totalSize);

	return hr;
}

template< typename SVTreeType >
HRESULT SVInspectionTreeParser< SVTreeType >::ProcessEmbeddeds(typename SVTreeType::SVBranchHandle hEmbeddeds, const GUID& ownerID)
{
	HRESULT hr = S_OK;
	SVTreeType::SVBranchHandle hItem( nullptr );
	hItem = m_rTree.getFirstBranch(hEmbeddeds);
	do
	{
		if (hItem)
		{
			UpdateProgress(++m_count, m_totalSize); 
			hr = ProcessEmbedded(hItem, ownerID);
			hItem = m_rTree.getNextBranch(hEmbeddeds, hItem);
		}
	} while (hItem);
	return hr;
}

template< typename SVTreeType >
HRESULT SVInspectionTreeParser< SVTreeType >::ProcessEmbedded(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID)
{
	HRESULT hr = S_OK;
	_variant_t objectName;
	_variant_t uniqueID;
	_variant_t embeddedID;
	_variant_t attributesAllowed;

	GetItemValue(scObjectNameTag, hItem, objectName);
	GetItemValue(scEmbeddedIDTag, hItem, embeddedID);
	GetItemValue(scUniqueReferenceIDTag, hItem, uniqueID);

	UpdateProgress(m_count, m_totalSize);

	GUID objectID = SVGUID(uniqueID);
	hr = SVObjectBuilder::OverwriteEmbeddedObject(SVGUID(embeddedID), objectID, SvUl_SF::createSVString(objectName), ownerID);
	if (S_OK == hr)
	{
		SVObjectScriptDataObjectTypeEnum dataType;
		if (S_OK == SVObjectBuilder::GetObjectDataType(ownerID, objectID, dataType))
		{
			hr = ProcessAttributes(ownerID, objectID, hItem);
		
			if (S_OK == hr)
			{
				hr = ProcessEmbeddedValues(hItem, ownerID, objectID, dataType);
			}
		}
		else
		{
			HRESULT l_Temp = ProcessLeafObjectValues(hItem, ownerID, objectID);

			hr = ProcessBranchObjectValues(hItem, ownerID, objectID);

			if( l_Temp != S_OK )
			{
				hr = l_Temp;
			}
		}
	}
	if (S_OK == hr)
	{
		_variant_t arraySize;
		if (GetItemValue(scArraySizeTag, hItem, arraySize))
		{
			hr = SVObjectBuilder::SetObjectValue(ownerID, objectID, scArraySizeTag, arraySize, SV_LONG_Type);
		}
	}
	m_count++;
	UpdateProgress(m_count, m_totalSize);

	return hr;
}

template< typename SVTreeType >
HRESULT SVInspectionTreeParser< SVTreeType >::ProcessEmbeddedValues(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID, const GUID& objectID, SVObjectScriptDataObjectTypeEnum dataType)
{
	HRESULT hr = S_OK;

	SVVariantList values;
	bool bVal = GetValues(hItem, scArrayElementsTag, values);
	if (bVal)
	{
		hr = SVObjectBuilder::SetObjectValue(ownerID, objectID, scArrayElementsTag, values, dataType);
	}

	return hr;
}

template< typename SVTreeType >
HRESULT SVInspectionTreeParser< SVTreeType >::ProcessBranchObjectValues(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID, const GUID& objectID)
{
	HRESULT hr = S_OK;

	if( nullptr != hItem && SvXml::SVNavigateTree::HasChildren( m_rTree, hItem ) )
	{
		SVTreeType::SVBranchHandle hValue( nullptr );

		hValue = m_rTree.getFirstBranch(hItem);

		while( S_OK == hr && m_rTree.isValidBranch( hValue ) )
		{
			SVVariantList values;

			SVString DataName;

			DataName = m_rTree.getBranchName(hValue);

			if( g_ObjectAttributeFilter.find( DataName.c_str() ) == g_ObjectAttributeFilter.end() )
			{
				if( SvXml::SVNavigateTree::HasChildren( m_rTree, hValue ) )
				{
					SVTreeType::SVLeafHandle hChildValue;

					hChildValue = m_rTree.getFirstLeaf(hValue);

					do
					{
						if (m_rTree.isValidLeaf(hValue, hChildValue))
						{
							_variant_t Data;

							m_rTree.getLeafData(hChildValue, Data);
							values.push_back(Data);

							hChildValue = m_rTree.getNextLeaf(hValue, hChildValue);
						}
					} while (m_rTree.isValidLeaf(hValue, hChildValue));
				}

				SVObjectScriptDataObjectTypeEnum l_Type = SV_UNKNOWN_Type;

				// The attribute object type table is needed to convert string type to point or double point type.
				// This functionality is not necessary for the RotationPoint and Translation elements because they are nto being persisted.
				// It is an example of what could be done to convert to a specific dastionation data type.
				SVObjectAttributeTypeMap::const_iterator l_Iter = g_ObjectAttributeType.find( DataName.c_str() );

				if( l_Iter != g_ObjectAttributeType.end() )
				{
					l_Type = l_Iter->second; 
				}
				//End of attribute object type section

				hr = SVObjectBuilder::SetObjectValue(objectID, objectID, DataName.c_str(), values, l_Type);
			}

			hValue = m_rTree.getNextBranch(hItem, hValue);
		}
	}
	else
	{
		hr = E_FAIL;
	}

	return hr;
}

template< typename SVTreeType >
HRESULT SVInspectionTreeParser< SVTreeType >::ProcessLeafObjectValues(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID, const GUID& objectID)
{
	HRESULT hr = S_OK;

	if( nullptr != hItem && SvXml::SVNavigateTree::HasChildren( m_rTree, hItem ) )
	{
		SVTreeType::SVLeafHandle hValue( m_rTree.getFirstLeaf(hItem) );

		while( S_OK == hr && m_rTree.isValidLeaf(hItem, hValue) )
		{
			SVVariantList ValueList;

			SVString DataName;

			DataName = m_rTree.getLeafName(hValue);

			if( g_ObjectAttributeFilter.find( DataName.c_str() ) == g_ObjectAttributeFilter.end() )
			{
				_variant_t Data;

				m_rTree.getLeafData(hValue, Data);
				ValueList.push_back(Data);

				SVObjectScriptDataObjectTypeEnum l_Type = SV_UNKNOWN_Type;

				// The attribute object type table is needed to convert string type to point or double point type.
				// This functionality is not necessary for the RotationPoint and Translation elements because they are nto being persisted.
				// It is an example of what could be done to convert to a specific dastionation data type.
				SVObjectAttributeTypeMap::const_iterator l_Iter = g_ObjectAttributeType.find( DataName.c_str() );

				if( l_Iter != g_ObjectAttributeType.end() )
				{
					l_Type = l_Iter->second; 
				}
				//End of attribute object type section

				hr = SVObjectBuilder::SetObjectValue(objectID, objectID, DataName.c_str(), ValueList, l_Type);
			}

			hValue = m_rTree.getNextLeaf(hItem, hValue);
		}
	}
	else
	{
		hr = E_FAIL;
	}

	return hr;
}

template< typename SVTreeType >
HRESULT SVInspectionTreeParser< SVTreeType >::ProcessAttributes(const GUID& ownerID, const GUID& objectID, typename SVTreeType::SVBranchHandle hItem)
{
	HRESULT hr = S_OK;

	SVVariantList attributes;
	bool bVal = GetValues(hItem, scAttributesSetTag, attributes);
	if (bVal)
	{			
		// process Attributes Set
		hr = SVObjectBuilder::SetObjectValue(ownerID, objectID, scAttributesSetTag, attributes, SV_DWORD_Type);
	}
	return hr;
}

template< typename SVTreeType >
HRESULT SVInspectionTreeParser< SVTreeType >::ProcessEquation(const GUID& ownerID, const _variant_t& equation)
{
	// process Equation Buffer
	HRESULT hr = SVObjectBuilder::SetObjectValue(ownerID, ownerID, scEquationBufferTag, equation, SV_STRING_Type);
	return hr;
}

template< typename SVTreeType >
HRESULT SVInspectionTreeParser< SVTreeType >::ProcessMaskData(const GUID& ownerID, const _variant_t& maskData)
{
	// process Mask Data Buffer
	HRESULT hr = SVObjectBuilder::SetObjectValue(ownerID, ownerID, scMaskDataTag, maskData, SV_STRING_Type);
	return hr;
}

template< typename SVTreeType >
HRESULT SVInspectionTreeParser< SVTreeType >::ProcessInputs(typename SVTreeType::SVBranchHandle hInputs, const GUID& objectID)
{
	HRESULT hr = S_OK;
	SVNameGuidList inputList;
	SVTreeType::SVBranchHandle hInput( nullptr );
	hInput = m_rTree.getFirstBranch(hInputs);
	do
	{
		if (hInput)
		{
			UpdateProgress(++m_count, m_totalSize);

			_variant_t name;
			_variant_t value;
			GetItemValue(scNameTag, hInput, name);
			GetItemValue(scValueTag, hInput, value);

			if (name.vt == VT_BSTR && value.vt == VT_BSTR)
			{
				GUID inputID = SVGUID(value);
				inputList.insert(std::make_pair(_bstr_t(name.bstrVal), inputID));
			}
			hInput = m_rTree.getNextBranch(hInputs, hInput);
		}
	} while (hInput);

	if (inputList.size())
	{
		hr = SVObjectBuilder::SetInputs(objectID, inputList);
	}
	return hr;
}

template< typename SVTreeType >
bool SVInspectionTreeParser< SVTreeType >::HasTag(typename SVTreeType::SVBranchHandle hItem, const SVString& tag)
{
	bool l_Status = true;

	SVTreeType::SVBranchHandle hValues( nullptr );

	hValues = m_rTree.findBranch(hItem, tag.c_str());

	if( nullptr == hValues )
	{
		SVTreeType::SVLeafHandle hLeaf;

		hLeaf = m_rTree.findLeaf(hItem, tag.c_str());
		if( !m_rTree.isValidLeaf(hItem, hLeaf) )
		{
			l_Status = false;
		}
	}
	else
	{
		l_Status = false;
	}

	return l_Status;
}

// This will need to change if/when SVInspectionprocess is converted to inhert from SVTaskObjectList
template< typename SVTreeType >
HRESULT SVInspectionTreeParser< SVTreeType >::CreateInspectionObject(GUID& inspectionGuid, SVTreeType& p_rTree, typename SVTreeType::SVBranchHandle hItem)
{
	HRESULT hr = S_OK;
	
	GUID ownerGuid = GUID_NULL;
	_variant_t objectName;
	_variant_t classID;
	_variant_t uniqueID;
	_variant_t newDisableMethod;
	_variant_t enableAuxiliaryExtent;
	SVString name;

	name = p_rTree.getBranchName(hItem);
	SvXml::SVNavigateTree::GetItem(p_rTree, scObjectNameTag, hItem, objectName);
	SvXml::SVNavigateTree::GetItem(p_rTree, scClassIDTag, hItem, classID);
	SvXml::SVNavigateTree::GetItem(p_rTree, scUniqueReferenceIDTag, hItem, uniqueID);
	SvXml::SVNavigateTree::GetItem(p_rTree, SvXml::CTAG_INSPECTION_NEW_DISABLE_METHOD, hItem, newDisableMethod);
	SvXml::SVNavigateTree::GetItem(p_rTree, SvXml::CTAG_INSPECTION_ENABLE_AUXILIARY_EXTENT, hItem, enableAuxiliaryExtent);

	SVString sNewDisableMethod = SvUl_SF::createSVString(newDisableMethod);
	SVString sEnableAuxiliaryExtent = SvUl_SF::createSVString(enableAuxiliaryExtent);

	hr = SVObjectBuilder::CreateObject(SVGUID(classID), SVGUID(uniqueID), name, SvUl_SF::createSVString(objectName), ownerGuid);
	if (S_OK == hr)
	{
		inspectionGuid = SVGUID(uniqueID);

		// Get Inspection Object
		SVObjectClass* pObject = nullptr;
		SVObjectManagerClass::Instance().GetObjectByIdentifier(inspectionGuid, pObject);
		SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(pObject);
		if ( nullptr != pInspection )
		{
			pInspection->SetNewDisableMethod( sNewDisableMethod == _T("1") );
			pInspection->SetEnableAuxiliaryExtent( sEnableAuxiliaryExtent == _T("1") );
		}
		else
		{
			SvStl::MessageMgrStd e( SvStl::LogOnly );
			e.setMessage( SVMSG_SVO_57_PARSERTREE_INSPECTIONCREATE_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10010_TreeParser_InspectionCreateFailed );
			hr = -SvStl::Err_10010_TreeParser_InspectionCreateFailed;
		}
	}
	return hr;
}


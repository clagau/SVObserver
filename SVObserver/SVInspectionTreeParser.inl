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
#include <boost/config.hpp>
#include <boost/assign.hpp>
#include "SVInspectionTreeParser.h"
#include "SVXMLLibrary\SVNavigateTree.h"
#include "SVObjectLibrary\SVObjectBuilder.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVConfigurationLibrary\SVConfigurationTags.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVInspectionProcess.h"
#include "SVStatusLibrary\MessageManager.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#pragma endregion Includes

typedef std::set<SVString> SVObjectAttributeFilterSet;

static SVObjectAttributeFilterSet g_ObjectAttributeFilter = boost::assign::list_of< SVString >
(static_cast< LPCTSTR >( scObjectNameTag ))
(static_cast< LPCTSTR >( scClassIDTag))
(static_cast< LPCTSTR >( scUniqueReferenceIDTag))
(static_cast< LPCTSTR >( scEmbeddedIDTag))
;

// The attribute object type table is needed to convert string type to point or double point type.
typedef std::map<SVString, SVObjectScriptDataObjectTypeEnum> SVObjectAttributeTypeMap;

static SVObjectAttributeTypeMap g_ObjectAttributeType = boost::assign::map_list_of<>
(_T( "RotationCenter" ), SV_POINT_Type)
(_T( "Translation" ), SV_POINT_Type)
;

static GUID StringToGUID(const SVString& guidStr)
{
	GUID guid;
	AfxGetClassIDFromString(guidStr.c_str(), &guid);
	return guid;
}

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
bool SVInspectionTreeParser< SVTreeType >::GetItemValue(const SVString& tag, typename SVTreeType::SVBranchHandle hItem, _variant_t& value)
{
	m_count++;
	return SVNavigateTree::GetItem(m_rTree, tag.c_str(), hItem, value) ? true : false;
}

template< typename SVTreeType >
bool SVInspectionTreeParser< SVTreeType >::GetValues(typename SVTreeType::SVBranchHandle hItem, const SVString& tag, SVVariantList& list)
{
	bool bRetVal = false;

	SVTreeType::SVBranchHandle hBranch = NULL;
	SVNavigateTree::GetItemBranch(m_rTree, tag.c_str(), hItem, hBranch);
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
				_variant_t value;

				value = m_rTree.getLeafData(hValue);
				list.push_back(value);

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

	if (scAttributesSetTag != name.c_str() )
	{
		_variant_t objectName;
		_variant_t classID;
		_variant_t uniqueID;
		
		SVNavigateTree::GetItem(m_rTree, scObjectNameTag, hItem, objectName);
		m_count++;

		SVNavigateTree::GetItem(m_rTree, scClassIDTag, hItem, classID);
		m_count++;

		if( m_ReplaceUniqueID )
		{
			SVNavigateTree::GetItem(m_rTree, scUniqueReferenceIDTag, hItem, uniqueID);
		}
		m_count++;
		
	
		GUID objectID( GUID_NULL );
		if( m_ReplaceUniqueID )
		{
			objectID = StringToGUID(uniqueID);
		}
		//Create this object only if it is not the same as the owner GUID
		if( ownerID == objectID )
		{
			//If the owner and object ID are the same the
			m_ReplaceUniqueID = false;
		}
		else
		{
			hr = SVObjectBuilder::CreateObject(StringToGUID(classID), objectID, name, objectName, ownerID);
		}
		if (hr == S_OK)
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

	if (SVNavigateTree::GetItem(m_rTree, scEquationBufferTag, hParentItem, value))
	{
		hr = ProcessEquation(ownerID, value);
	}

	if (SVNavigateTree::GetItem(m_rTree, scMaskDataTag, hParentItem, value))
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

			if (Name.c_str() == scFriendsTag)
			{
				hr = ProcessFriends(hItem, ownerID);
			}
			else if (Name.c_str() == scEmbeddedsTag)
			{
				hr = ProcessEmbeddeds(hItem, ownerID);
			}
			else if (Name.c_str() == scInputsTag)
			{
				hr = ProcessInputs(hItem, ownerID);
			}
			else 
			{
				// process this node
				if (SVNavigateTree::HasChildren(m_rTree, hItem))
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
	GUID objectID = StringToGUID(uniqueID);
	hr = SVObjectBuilder::CreateFriendObject(StringToGUID(classID), objectID, Name.c_str(), objectName, ownerID);
	if (hr == S_OK)
	{
		// this will be different for embeddeds, it will use the owning object ID and the embedded object ID
		hr = ProcessAttributes(objectID, objectID, hItem);
		if( hr == S_OK )
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
	_variant_t defaultValue;

	SVString Name;
		
	Name = m_rTree.getBranchName(hItem);
	
	GetItemValue(scObjectNameTag, hItem, objectName);
	GetItemValue(scEmbeddedIDTag, hItem, embeddedID);
	GetItemValue(scUniqueReferenceIDTag, hItem, uniqueID);

	UpdateProgress(m_count, m_totalSize);

	GUID objectID = StringToGUID(uniqueID);
	hr = SVObjectBuilder::CreateEmbeddedObject(StringToGUID(embeddedID), objectID, Name.c_str(), objectName, ownerID);
	if (hr == S_OK)
	{
		SVObjectScriptDataObjectTypeEnum dataType;
		if (SVObjectBuilder::GetObjectDataType(ownerID, objectID, dataType) == S_OK)
		{
			hr = ProcessAttributes(ownerID, objectID, hItem);
		
			if (hr == S_OK)
			{
				GetItemValue(scDefaultTag, hItem, defaultValue);
				hr = ProcessEmbeddedValues(hItem, ownerID, objectID, defaultValue, dataType);
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
	m_count++;
	UpdateProgress(m_count, m_totalSize);

	return hr;
}

template< typename SVTreeType >
HRESULT SVInspectionTreeParser< SVTreeType >::ProcessEmbeddedValues(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID, const GUID& objectID, const _variant_t& defaultValue, SVObjectScriptDataObjectTypeEnum dataType)
{
	HRESULT hr = S_OK;

	if (HasTag(hItem, scDefaultTag))
	{
		hr = SVObjectBuilder::SetObjectValue(ownerID, objectID, scDefaultTag, defaultValue, dataType);
	}
	if (hr == S_OK)
	{
		SVVariantList values;
		bool bVal = GetValues(hItem, scArrayElementsTag, values);
		if (bVal)
		{
			hr = SVObjectBuilder::SetObjectValue(ownerID, objectID, scArrayElementsTag, values, dataType);
		}
	}
	return hr;
}

template< typename SVTreeType >
HRESULT SVInspectionTreeParser< SVTreeType >::ProcessBranchObjectValues(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID, const GUID& objectID)
{
	HRESULT hr = S_OK;

	if( hItem != NULL && SVNavigateTree::HasChildren( m_rTree, hItem ) )
	{
		SVTreeType::SVBranchHandle hValue( nullptr );

		hValue = m_rTree.getFirstBranch(hItem);

		while( hr == S_OK && m_rTree.isValidBranch( hValue ) )
		{
			SVVariantList values;

			SVString DataName;

			DataName = m_rTree.getBranchName(hValue);

			if( g_ObjectAttributeFilter.find( DataName.c_str() ) == g_ObjectAttributeFilter.end() )
			{
				if( SVNavigateTree::HasChildren( m_rTree, hValue ) )
				{
					SVTreeType::SVLeafHandle hChildValue;

					hChildValue = m_rTree.getFirstLeaf(hValue);

					do
					{
						if (m_rTree.isValidLeaf(hValue, hChildValue))
						{
							_variant_t value;

							value = m_rTree.getLeafData(hChildValue);
							values.push_back(value);

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

	if( hItem != NULL && SVNavigateTree::HasChildren( m_rTree, hItem ) )
	{
		SVTreeType::SVLeafHandle hValue( m_rTree.getFirstLeaf(hItem) );

		while( hr == S_OK && m_rTree.isValidLeaf(hItem, hValue) )
		{
			SVVariantList values;

			SVString DataName;

			DataName = m_rTree.getLeafName(hValue);

			if( g_ObjectAttributeFilter.find( DataName.c_str() ) == g_ObjectAttributeFilter.end() )
			{
				_variant_t value;

				value = m_rTree.getLeafData(hValue);
				values.push_back(value);

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
				GUID inputID = StringToGUID(value.bstrVal);
				inputList.insert(std::make_pair(name.bstrVal, inputID));
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
	SVNavigateTree::GetItem(p_rTree, scObjectNameTag, hItem, objectName);
	SVNavigateTree::GetItem(p_rTree, scClassIDTag, hItem, classID);
	SVNavigateTree::GetItem(p_rTree, scUniqueReferenceIDTag, hItem, uniqueID);
	SVNavigateTree::GetItem(p_rTree, CTAG_INSPECTION_NEW_DISABLE_METHOD, hItem, newDisableMethod);
	SVNavigateTree::GetItem(p_rTree, CTAG_INSPECTION_ENABLE_AUXILIARY_EXTENT, hItem, enableAuxiliaryExtent);

	SVString sNewDisableMethod = newDisableMethod;
	SVString sEnableAuxiliaryExtent = enableAuxiliaryExtent;

	hr = SVObjectBuilder::CreateObject(StringToGUID(classID), StringToGUID(uniqueID), name.c_str(), objectName, ownerGuid);
	if (hr == S_OK)
	{
		inspectionGuid = StringToGUID(uniqueID);

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
			SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
			e.setMessage( SVMSG_SVO_57_PARSERTREE_INSPECTIONCREATE_ERROR, nullptr, StdMessageParams, SvOi::Err_10010_TreeParser_InspectionCreateFailed );
			hr = -SvOi::Err_10010_TreeParser_InspectionCreateFailed;
		}
	}
	return hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVInspectionTreeParser.inl_v  $
 * 
 *    Rev 1.1   19 Dec 2014 04:10:42   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  978
 * SCR Title:  Copy and Paste a Tool within an Inspection or Between Different Inspections
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Changed parser to be able to parse a single tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 11:01:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Oct 2012 10:54:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information based on the navigate tree class moving to the XML library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   28 Sep 2012 14:28:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Migrated object to templates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   25 Sep 2012 15:19:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   18 Sep 2012 18:21:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   12 Sep 2012 17:13:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update Process Shildren to handle the missing Mask Data processing and ass a new Process Mask Data method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Sep 2012 12:29:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added Object Attribute filter
 * 
 * Added Object Attribute type table
 * 
 * Added new Process Object Values
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 Sep 2012 10:00:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update Create Inspection Object method to include missing data from Inspection Process for export fucntionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Sep 2012 14:00:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in of new files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
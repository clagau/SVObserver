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

#ifndef INCL_SVINSPECTIONTREEPARSER_INL
#define INCL_SVINSPECTIONTREEPARSER_INL

#include <boost/config.hpp>
#include <boost/assign.hpp>
#include "SVInspectionTreeParser.h"
#include "SVXMLLibrary/SVNavigateTreeClass.h"
#include "SVObjectLibrary/SVObjectBuilder.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVObjectLibrary/SVToolsetScriptTags.h"
#include "SVInspectionProcess.h"
#include "SVStatusLibrary/ExceptionManager.h"
#include "ErrorNumbers.h"

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
	m_totalSize = m_rTree.GetCount();
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
	return SVNavigateTreeClass::GetItem(m_rTree, tag.c_str(), hItem, value) ? true : false;
}

template< typename SVTreeType >
bool SVInspectionTreeParser< SVTreeType >::GetValues(typename SVTreeType::SVBranchHandle hItem, const SVString& tag, SVVariantList& list)
{
	bool bRetVal = false;

	SVTreeType::SVBranchHandle hValues = NULL;
	SVNavigateTreeClass::GetItemBranch(m_rTree, tag.c_str(), hItem, hValues);
	if (hValues)
	{
		bRetVal = true;
		SVTreeType::SVLeafHandle hValue;
		m_rTree.GetFirstLeaf(hValues, hValue);
		do
		{
			if (m_rTree.IsValidLeaf(hValues, hValue) == S_OK)
			{
				m_count++;
				_variant_t value;

				m_rTree.GetLeafData(hValue, value);
				list.push_back(value);

				m_rTree.GetNextLeaf(hValues, hValue);
			}
		} while (m_rTree.IsValidLeaf(hValues, hValue) == S_OK);
	}
	return bRetVal;
}

template< typename SVTreeType >
HRESULT SVInspectionTreeParser< SVTreeType >::Process(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID)
{
	HRESULT hr = S_OK;

	_bstr_t name;

	m_rTree.GetBranchName(hItem, name.GetBSTR());
	m_count++;

	if (scAttributesSetTag != static_cast< LPCTSTR >( name ) )
	{
		_variant_t objectName;
		_variant_t classID;
		_variant_t uniqueID;
		_variant_t attributesAllowed;
		
		SVNavigateTreeClass::GetItem(m_rTree, scObjectNameTag, hItem, objectName);
		m_count++;

		SVNavigateTreeClass::GetItem(m_rTree, scClassIDTag, hItem, classID);
		m_count++;

		if( m_ReplaceUniqueID )
		{
			SVNavigateTreeClass::GetItem(m_rTree, scUniqueReferenceIDTag, hItem, uniqueID);
		}
		m_count++;
		
		SVNavigateTreeClass::GetItem(m_rTree, scAttributesAllowedTag, hItem, attributesAllowed);
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
			hr = ProcessAttributes(objectID, objectID, attributesAllowed, hItem);
		
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

	if (SVNavigateTreeClass::GetItem(m_rTree, scEquationBufferTag, hParentItem, value))
	{
		hr = ProcessEquation(ownerID, value);
	}

	if (SVNavigateTreeClass::GetItem(m_rTree, scMaskDataTag, hParentItem, value))
	{
		hr = ProcessMaskData(ownerID, value);
	}

	// iterate thru nodes and process
	SVTreeType::SVBranchHandle hItem = NULL;
	m_rTree.GetFirstBranch(hParentItem, hItem);
	do
	{
		if (hItem)
		{
			UpdateProgress(++m_count, m_totalSize);

			_bstr_t l_Temp;

			m_rTree.GetBranchName(hItem, l_Temp.GetBSTR());

			CString text = static_cast< LPCTSTR >( l_Temp );

			if (text == scFriendsTag)
			{
				hr = ProcessFriends(hItem, ownerID);
			}
			else if (text == scEmbeddedsTag)
			{
				hr = ProcessEmbeddeds(hItem, ownerID);
			}
			else if (text == scInputsTag)
			{
				hr = ProcessInputs(hItem, ownerID);
			}
			else 
			{
				// process this node
				if (SVNavigateTreeClass::HasChildren(m_rTree, hItem))
				{
					hr = Process(hItem, ownerID);
				}
			}

			m_rTree.GetNextBranch(hParentItem, hItem);
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

	SVTreeType::SVBranchHandle hItem = NULL;
	m_rTree.GetFirstBranch(hFriends, hItem);
	do
	{
		if (hItem)
		{
			UpdateProgress(++m_count, m_totalSize); 
			hr = ProcessFriend(hItem, ownerID);
			m_rTree.GetNextBranch(hFriends, hItem);
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

	_bstr_t name;
		
	m_rTree.GetBranchName(hItem, name.GetBSTR());
	
	GetItemValue(scObjectNameTag, hItem, objectName);
	GetItemValue(scClassIDTag, hItem, classID);
	GetItemValue(scUniqueReferenceIDTag, hItem, uniqueID);
	GetItemValue(scAttributesAllowedTag, hItem, attributesAllowed);

	UpdateProgress(m_count, m_totalSize);

	// Build the Object
	GUID objectID = StringToGUID(uniqueID);
	hr = SVObjectBuilder::CreateFriendObject(StringToGUID(classID), objectID, name, objectName, ownerID);
	if (hr == S_OK)
	{
		// this will be different for embeddeds, it will use the owning object ID and the embedded object ID
		hr = ProcessAttributes(objectID, objectID, attributesAllowed, hItem);
	}
	if (hr == S_OK)
	{
		hr = ProcessChildren(hItem, objectID);
	}
	m_count++;
	UpdateProgress(m_count, m_totalSize);

	return hr;
}

template< typename SVTreeType >
HRESULT SVInspectionTreeParser< SVTreeType >::ProcessEmbeddeds(typename SVTreeType::SVBranchHandle hEmbeddeds, const GUID& ownerID)
{
	HRESULT hr = S_OK;
	SVTreeType::SVBranchHandle hItem = NULL;
	m_rTree.GetFirstBranch(hEmbeddeds, hItem);
	do
	{
		if (hItem)
		{
			UpdateProgress(++m_count, m_totalSize); 
			hr = ProcessEmbedded(hItem, ownerID);
			m_rTree.GetNextBranch(hEmbeddeds, hItem);
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

	_bstr_t name;
		
	m_rTree.GetBranchName(hItem, name.GetBSTR());
	
	GetItemValue(scObjectNameTag, hItem, objectName);
	GetItemValue(scEmbeddedIDTag, hItem, embeddedID);
	GetItemValue(scUniqueReferenceIDTag, hItem, uniqueID);
	GetItemValue(scAttributesAllowedTag, hItem, attributesAllowed);

	UpdateProgress(m_count, m_totalSize);

	GUID objectID = StringToGUID(uniqueID);
	hr = SVObjectBuilder::CreateEmbeddedObject(StringToGUID(embeddedID), objectID, name, objectName, ownerID);
	if (hr == S_OK)
	{
		SVObjectScriptDataObjectTypeEnum dataType;
		if (SVObjectBuilder::GetObjectDataType(ownerID, objectID, dataType) == S_OK)
		{
			hr = ProcessAttributes(ownerID, objectID, attributesAllowed, hItem);
		
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

	if( hItem != NULL && SVNavigateTreeClass::HasChildren( m_rTree, hItem ) )
	{
		SVTreeType::SVBranchHandle hValue = NULL;

		m_rTree.GetFirstBranch(hItem, hValue);

		while( hr == S_OK && m_rTree.IsValidBranch( hValue ) == S_OK )
		{
			SVVariantList values;

			_bstr_t l_DataName;

			m_rTree.GetBranchName(hValue, l_DataName.GetBSTR());

			if( g_ObjectAttributeFilter.find( l_DataName ) == g_ObjectAttributeFilter.end() )
			{
				if( SVNavigateTreeClass::HasChildren( m_rTree, hValue ) )
				{
					SVTreeType::SVLeafHandle hChildValue;

					m_rTree.GetFirstLeaf(hValue, hChildValue);

					do
					{
						if (m_rTree.IsValidLeaf(hValue, hChildValue) == S_OK)
						{
							_variant_t value;

							m_rTree.GetLeafData(hChildValue, value );
							values.push_back(value);

							m_rTree.GetNextLeaf(hValue, hChildValue);
						}
					} while (m_rTree.IsValidLeaf(hValue, hChildValue) == S_OK);
				}

				SVObjectScriptDataObjectTypeEnum l_Type = SV_UNKNOWN_Type;

				// The attribute object type table is needed to convert string type to point or double point type.
				// This functionality is not necessary for the RotationPoint and Translation elements because they are nto being persisted.
				// It is an example of what could be done to convert to a specific dastionation data type.
				SVObjectAttributeTypeMap::const_iterator l_Iter = g_ObjectAttributeType.find( l_DataName );

				if( l_Iter != g_ObjectAttributeType.end() )
				{
					l_Type = l_Iter->second; 
				}
				//End of attribute object type section

				hr = SVObjectBuilder::SetObjectValue(objectID, objectID, l_DataName, values, l_Type);
			}

			m_rTree.GetNextBranch(hItem, hValue);
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

	if( hItem != NULL && SVNavigateTreeClass::HasChildren( m_rTree, hItem ) )
	{
		SVTreeType::SVLeafHandle hValue;

		m_rTree.GetFirstLeaf(hItem, hValue);

		while( hr == S_OK && m_rTree.IsValidLeaf(hItem, hValue) == S_OK )
		{
			SVVariantList values;

			_bstr_t l_DataName;

			m_rTree.GetLeafName(hValue, l_DataName.GetBSTR());

			if( g_ObjectAttributeFilter.find( l_DataName ) == g_ObjectAttributeFilter.end() )
			{
				_variant_t value;

				m_rTree.GetLeafData(hValue, value);
				values.push_back(value);

				SVObjectScriptDataObjectTypeEnum l_Type = SV_UNKNOWN_Type;

				// The attribute object type table is needed to convert string type to point or double point type.
				// This functionality is not necessary for the RotationPoint and Translation elements because they are nto being persisted.
				// It is an example of what could be done to convert to a specific dastionation data type.
				SVObjectAttributeTypeMap::const_iterator l_Iter = g_ObjectAttributeType.find( l_DataName );

				if( l_Iter != g_ObjectAttributeType.end() )
				{
					l_Type = l_Iter->second; 
				}
				//End of attribute object type section

				hr = SVObjectBuilder::SetObjectValue(objectID, objectID, l_DataName, values, l_Type);
			}

			m_rTree.GetNextLeaf(hItem, hValue);
		}
	}
	else
	{
		hr = E_FAIL;
	}

	return hr;
}

template< typename SVTreeType >
HRESULT SVInspectionTreeParser< SVTreeType >::ProcessAttributes(const GUID& ownerID, const GUID& objectID, const _variant_t& attributesAllowed, typename SVTreeType::SVBranchHandle hItem)
{
	HRESULT hr = S_OK;

	SVVariantList attributes;
	bool bVal = GetValues(hItem, scAttributesSetTag, attributes);
	if (bVal)
	{			
		// process Attributes Allowed
		hr = SVObjectBuilder::SetObjectValue(objectID, objectID, scAttributesAllowedTag, attributesAllowed, SV_DWORD_Type);
	}
	if (hr == S_OK)
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
	SVTreeType::SVBranchHandle hInput = NULL;
	m_rTree.GetFirstBranch(hInputs, hInput);
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
			m_rTree.GetNextBranch(hInputs, hInput);
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

	SVTreeType::SVBranchHandle hValues = NULL;

	l_Status = ( m_rTree.FindBranch(hItem, tag.ToBSTR(), hValues) == S_OK );

	if( !l_Status )
	{
		SVTreeType::SVLeafHandle hLeaf;

		l_Status = ( m_rTree.FindLeaf(hItem, tag.ToBSTR(), hLeaf) == S_OK );
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
	_bstr_t name;

	p_rTree.GetBranchName(hItem, name.GetBSTR());
	SVNavigateTreeClass::GetItem(p_rTree, scObjectNameTag, hItem, objectName);
	SVNavigateTreeClass::GetItem(p_rTree, scClassIDTag, hItem, classID);
	SVNavigateTreeClass::GetItem(p_rTree, scUniqueReferenceIDTag, hItem, uniqueID);
	SVNavigateTreeClass::GetItem(p_rTree, CTAG_INSPECTION_NEW_DISABLE_METHOD, hItem, newDisableMethod);
	SVNavigateTreeClass::GetItem(p_rTree, CTAG_INSPECTION_ENABLE_AUXILIARY_EXTENT, hItem, enableAuxiliaryExtent);

	SVString sNewDisableMethod = newDisableMethod;
	SVString sEnableAuxiliaryExtent = enableAuxiliaryExtent;

	hr = SVObjectBuilder::CreateObject(StringToGUID(classID), StringToGUID(uniqueID), name, objectName, ownerGuid);
	if (hr == S_OK)
	{
		inspectionGuid = StringToGUID(uniqueID);

		// Get Inspection Object
		SVObjectClass* pObject = nullptr;
		SVObjectManagerClass::Instance().GetObjectByIdentifier(inspectionGuid, pObject);
		SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(pObject);
		if (pInspection)
		{
			pInspection->SetNewDisableMethod( sNewDisableMethod == _T("1") );
			pInspection->SetEnableAuxiliaryExtent( sEnableAuxiliaryExtent == _T("1") );
		}
		else
		{
			SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
			e.setMessage( SVMSG_SVO_57_PARSERTREE_INSPECTIONCREATE_ERROR, nullptr, StdExceptionParams, Err_TreeParser_InspectionCreateFailed_2010 );
			hr = -Err_TreeParser_InspectionCreateFailed_2010;
		}
	}
	return hr;
}


#endif

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
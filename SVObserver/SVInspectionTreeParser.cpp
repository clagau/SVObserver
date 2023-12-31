	//******************************************************************************
//* COPYRIGHT (c) 2012 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInspectionTreeParser
//* .File Name       : $Workfile:   SVInspectionTreeParser.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   19 Dec 2014 04:10:42  $
//******************************************************************************

#pragma region Includes
#include "Stdafx.h"
#include "SVInspectionTreeParser.h"
#include "SVInspectionProcess.h"
#include "SVObjectLibrary/SVObjectBuilder.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVToolsetScriptTags.h"
#include "SVStatusLibrary/MessageManager.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#pragma endregion Includes

// The attribute object type table is needed to convert string type to point or double point type.
typedef std::map<std::string, SVObjectScriptDataObjectTypeEnum> SVObjectAttributeTypeMap;

static const SVObjectAttributeTypeMap cObjectAttributeType
{
	{_T("RotationCenter"),	SV_POINT_Type},
	{_T("Translation"),		SV_POINT_Type},
	{scDefaultTag,			SV_VARIANT_Type}
};

namespace
{
	static const SvDef::StringSet cObjectAttributeFilter
	{
		std::string(scObjectNameTag),
		std::string(scClassIDTag),
		std::string(scUniqueReferenceIDTag),
		std::string(scEmbeddedIDTag)
	};

	static const std::vector< const TCHAR *>   cExcludeTypes = {scDefaultTag, scArrayElementsTag};

	bool checkIfProcess(const std::string& DataName, bool excludeDefaultAndArray)
	{
		bool shouldProcess = (cObjectAttributeFilter.find(DataName.c_str()) == cObjectAttributeFilter.end());
		if (shouldProcess && excludeDefaultAndArray)
		{
			auto iter = std::ranges::find(cExcludeTypes, DataName);
			shouldProcess = (iter == cExcludeTypes.end());
		}
		return shouldProcess;
	}
}

SVInspectionTreeParser::SVInspectionTreeParser(SvXml::SVXMLMaterialsTree& rTreeCtrl, SvXml::SVXMLMaterialsTree::SVBranchHandle hItem, unsigned long parserHandle, uint32_t ownerId, SVObjectClass* pOwnerObject, CWnd* pWnd)
: SVObjectScriptParserBase(parserHandle, ownerId, pOwnerObject, pWnd)
, m_rTree(rTreeCtrl)
, m_rootItem(hItem)
, m_count(0)
, m_ReplaceUniqueID(true)
{
	m_totalSize = m_rTree.getCount();
}

SVInspectionTreeParser::~SVInspectionTreeParser() 
{ 
}


HRESULT SVInspectionTreeParser::DoParse()
{
	HRESULT hr = S_OK;
	m_count = 0;
	hr = Process(m_rootItem, m_OwnerId);
	return hr;
}

size_t SVInspectionTreeParser::GetTotal() const
{
	// get count of nodes
	return m_totalSize;
}

bool SVInspectionTreeParser::GetItemValue(const std::string& tag, SvXml::SVXMLMaterialsTree::SVBranchHandle hItem, _variant_t& rValue)
{
	m_count++;
	return SvXml::SVNavigateTree::GetItem(m_rTree, tag.c_str(), hItem, rValue) ? true : false;
}

bool SVInspectionTreeParser::GetValues(SvXml::SVXMLMaterialsTree::SVBranchHandle hItem, const std::string& tag, std::vector<_variant_t>& rValueList)
{
	bool bRetVal = false;

	SvXml::SVXMLMaterialsTree::SVBranchHandle hBranch = nullptr;
	SvXml::SVNavigateTree::GetItemBranch(m_rTree, tag.c_str(), hItem, hBranch);
	if(nullptr != hBranch)
	{
		bRetVal = true;
		SvXml::SVXMLMaterialsTree::SVLeafHandle hValue;
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

HRESULT SVInspectionTreeParser::Process(SvXml::SVXMLMaterialsTree::SVBranchHandle hItem, uint32_t ownerID)
{
	HRESULT hr = S_OK;

	std::string name( m_rTree.getBranchName(hItem) );
	m_count++;

	if (scAttributesSetTag != name)
	{
		_variant_t objectName;
		_variant_t classIDVariant;
		_variant_t uniqueID;
		
		SvXml::SVNavigateTree::GetItem(m_rTree, scObjectNameTag, hItem, objectName);
		m_count++;
		SvXml::SVNavigateTree::GetItem(m_rTree, scClassIDTag, hItem, classIDVariant);
		SvPb::ClassIdEnum classId = calcClassId(classIDVariant);
		if (SvPb::NoObjectClassId == classId)
		{
			const GUID SVRemoteInputToolGuid = { 0xef12c6da, 0xc0de, 0x4503,{ 0xac, 0x69, 0xc6, 0x96, 0xc, 0x6b, 0x9e, 0x9f } };
			SVGUID guid{ classIDVariant };
			if (guid == SVRemoteInputToolGuid)
			{
				//Ignore this classId
				return S_FALSE;
			}
		}
		m_count++;

		if( m_ReplaceUniqueID )
		{
			SvXml::SVNavigateTree::GetItem(m_rTree, scUniqueReferenceIDTag, hItem, uniqueID);
		}
		m_count++;
		
	
		uint32_t objectID(SvDef::InvalidObjectId);
		if( m_ReplaceUniqueID )
		{
			objectID = calcObjectId(uniqueID);
		}
		//Create this object only if it is not the same as the owner ID
		if( ownerID == objectID )
		{
			//If the owner and object ID are the same the
			m_ReplaceUniqueID = false;
		}
		else
		{
			hr = SVObjectBuilder::CreateObject(classId, objectID, name, SvUl::createStdString(objectName), ownerID);
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

HRESULT SVInspectionTreeParser::ProcessChildren(SvXml::SVXMLMaterialsTree::SVBranchHandle hParentItem, uint32_t ownerID)
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
	SvXml::SVXMLMaterialsTree::SVBranchHandle hItem( nullptr );
	hItem = m_rTree.getFirstBranch(hParentItem);
	do
	{
		if (hItem)
		{
			UpdateProgress(++m_count, m_totalSize);

			std::string Name = m_rTree.getBranchName(hItem);

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
			else if (Name == scHistoryTag)
			{
				hr = ProcessHistory(hItem, ownerID);
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

HRESULT SVInspectionTreeParser::ProcessFriends(SvXml::SVXMLMaterialsTree::SVBranchHandle hFriends, uint32_t ownerID)
{
	HRESULT hr = S_OK;
	// Destroy Friends
	SVObjectBuilder::DestroyFriends(ownerID);

	SvXml::SVXMLMaterialsTree::SVBranchHandle hItem( nullptr );
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

HRESULT SVInspectionTreeParser::ProcessFriend(SvXml::SVXMLMaterialsTree::SVBranchHandle hItem, uint32_t ownerID)
{
	HRESULT hr = S_OK;

	_variant_t objectName;
	_variant_t classIDVariant;
	_variant_t uniqueID;
	_variant_t attributesAllowed;

	GetItemValue(scObjectNameTag, hItem, objectName);
	GetItemValue(scClassIDTag, hItem, classIDVariant);
	SvPb::ClassIdEnum classId = calcClassId(classIDVariant);
	GetItemValue(scUniqueReferenceIDTag, hItem, uniqueID);

	UpdateProgress(m_count, m_totalSize);

	// Build the Object
	uint32_t objectID = calcObjectId(uniqueID);;
	hr = (nullptr != SVObjectBuilder::CreateFriendObject(classId, objectID, SvUl::createStdString(objectName), ownerID)) ? S_OK : E_FAIL;
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

HRESULT SVInspectionTreeParser::ProcessEmbeddeds(SvXml::SVXMLMaterialsTree::SVBranchHandle hEmbeddeds, uint32_t ownerID)
{
	HRESULT hr = S_OK;
	SvXml::SVXMLMaterialsTree::SVBranchHandle hItem( nullptr );
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

HRESULT SVInspectionTreeParser::setCommentToObject(SvXml::SVXMLMaterialsTree::SVBranchHandle hItem, uint32_t ownerID)
{
	std::vector<_variant_t> values;
	bool bVal = GetValues(hItem, scArrayElementsTag, values);
	if (bVal && 0 < values.size() && VT_BSTR == values[0].vt)
	{
		return SVObjectBuilder::SetObjectValue(ownerID, ownerID, scCommentTag, values[0], SV_STRING_Type);
	}
	return E_FAIL;
}

HRESULT SVInspectionTreeParser::ProcessEmbedded(SvXml::SVXMLMaterialsTree::SVBranchHandle hItem, uint32_t ownerID)
{
	HRESULT hr = S_OK;
	_variant_t objectName;
	_variant_t uniqueID;
	_variant_t embeddedVariantID;
	_variant_t attributesAllowed;

	GetItemValue(scObjectNameTag, hItem, objectName);
	GetItemValue(scEmbeddedIDTag, hItem, embeddedVariantID);
	SvPb::EmbeddedIdEnum embeddedId = calcEmbeddedId(embeddedVariantID, objectName);
	if (SvPb::ToolCommentTypeEId == embeddedId)
	{	//for configuration saved with 10.20 and older, the comment is saved in an own embedded object.
		return setCommentToObject(hItem, ownerID);
	}
	GetItemValue(scUniqueReferenceIDTag, hItem, uniqueID);

	UpdateProgress(m_count, m_totalSize);
	uint32_t objectID = calcObjectId(uniqueID);
	hr = SVObjectBuilder::OverwriteEmbeddedObject(embeddedId, objectID, SvUl::createStdString(objectName), ownerID);
	if (S_OK == hr)
	{
		SVObjectScriptDataObjectTypeEnum dataType;
		bool isDataTypeValid = (S_OK == SVObjectBuilder::GetObjectDataType(ownerID, objectID, dataType));
		if (isDataTypeValid)
		{
			hr = ProcessAttributes(ownerID, objectID, hItem);
		
			if (S_OK == hr)
			{
				hr = ProcessEmbeddedValues(hItem, ownerID, objectID, dataType);
			}
		}
		else
		{
			//special code to load indirect linked values from old configs (older than 8.20)
			SvPb::EmbeddedIdEnum linkedValueEmbeddedId = getEmbeddedIdFromIndirectLinked(embeddedId);
			if (SvDef::InvalidObjectId != linkedValueEmbeddedId)
			{
				std::vector<_variant_t> values;
				bool bVal = GetValues(hItem, scArrayElementsTag, values);
				if (bVal && 0 < values.size() && VT_BSTR == values[0].vt)
				{
					hr = SVObjectBuilder::SetIndirectStringToObject(ownerID, linkedValueEmbeddedId, values);
				}
			}
		}

		if (S_OK == hr)
		{
			HRESULT l_Temp = ProcessLeafObjectValues(hItem, ownerID, objectID, isDataTypeValid);

			hr = ProcessBranchObjectValues(hItem, ownerID, objectID, isDataTypeValid);

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

HRESULT SVInspectionTreeParser::ProcessEmbeddedChilds(SvXml::SVXMLMaterialsTree::SVBranchHandle hItem, uint32_t ownerID)
{
	HRESULT hr = S_OK;

	SvXml::SVXMLMaterialsTree::SVBranchHandle hChild = m_rTree.getFirstBranch(hItem);
	std::vector<uint32_t> objectIdList;
	while (hChild)
	{
		_variant_t uniqueID;
		GetItemValue(scUniqueReferenceIDTag, hChild, uniqueID);
		objectIdList.push_back(calcObjectId(uniqueID));

		hChild = m_rTree.getNextBranch(hItem, hChild);
	}
	if (false == objectIdList.empty())
	{
		hr = SVObjectBuilder::SetEmbeddedLinkedChildIds(ownerID, objectIdList);
	}
	return hr;
}

HRESULT SVInspectionTreeParser::ProcessEmbeddedValues(SvXml::SVXMLMaterialsTree::SVBranchHandle hItem, uint32_t ownerID, uint32_t objectID, SVObjectScriptDataObjectTypeEnum dataType)
{
	HRESULT hr = S_OK;

	if (HasTag(hItem, scDefaultTag))
	{
		_variant_t defaultValue;
		GetItemValue(scDefaultTag, hItem, defaultValue);
		hr = SVObjectBuilder::SetObjectValue(ownerID, objectID, scDefaultTag, defaultValue, dataType);
	}
	if (S_OK == hr)
	{
		std::vector<_variant_t> values;
		bool bVal = GetValues(hItem, scArrayElementsTag, values);
		if (bVal)
		{
			hr = SVObjectBuilder::SetObjectValue(ownerID, objectID, scArrayElementsTag, values, dataType);
		}
	}

	return hr;
}

HRESULT SVInspectionTreeParser::ProcessBranchObjectValues(SvXml::SVXMLMaterialsTree::SVBranchHandle hItem, uint32_t , uint32_t objectID, bool excludeDefaultAndArray)
{
	HRESULT hr = S_OK;

	if( nullptr != hItem && SvXml::SVNavigateTree::HasChildren( m_rTree, hItem ) )
	{
		SvXml::SVXMLMaterialsTree::SVBranchHandle hValue( nullptr );

		hValue = m_rTree.getFirstBranch(hItem);

		while( S_OK == hr && m_rTree.isValidBranch( hValue ) )
		{
			std::string DataName = m_rTree.getBranchName(hValue);
			if (DataName == scEmbeddedsTag)
			{
				hr = ProcessEmbeddeds(hValue, objectID);
			}
			else if (DataName == scLinkedChildsTag)
			{
				hr = ProcessEmbeddedChilds(hValue, objectID);
			}
			else if (checkIfProcess(DataName, excludeDefaultAndArray))
			{
				std::vector<_variant_t> values;
				if (SvXml::SVNavigateTree::HasChildren(m_rTree, hValue))
				{
					SvXml::SVXMLMaterialsTree::SVLeafHandle hChildValue = m_rTree.getFirstLeaf(hValue);

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
				SVObjectAttributeTypeMap::const_iterator l_Iter = cObjectAttributeType.find(DataName.c_str());

				if (l_Iter != cObjectAttributeType.end())
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

HRESULT SVInspectionTreeParser::ProcessLeafObjectValues(SvXml::SVXMLMaterialsTree::SVBranchHandle hItem, uint32_t, uint32_t objectID, bool excludeDefaultAndArray)
{
	HRESULT hr = S_OK;

	if( nullptr != hItem && SvXml::SVNavigateTree::HasChildren( m_rTree, hItem ) )
	{
		SvXml::SVXMLMaterialsTree::SVLeafHandle hValue( m_rTree.getFirstLeaf(hItem) );

		while( S_OK == hr && m_rTree.isValidLeaf(hItem, hValue) )
		{
			std::string DataName = m_rTree.getLeafName(hValue);
						
			if(checkIfProcess(DataName, excludeDefaultAndArray))
			{
				_variant_t Data;
				m_rTree.getLeafData(hValue, Data);

				SVObjectScriptDataObjectTypeEnum l_Type = SV_UNKNOWN_Type;

				// The attribute object type table is needed to convert string type to point or double point type.
				// This functionality is not necessary for the RotationPoint and Translation elements because they are nto being persisted.
				// It is an example of what could be done to convert to a specific dastionation data type.
				SVObjectAttributeTypeMap::const_iterator l_Iter = cObjectAttributeType.find( DataName.c_str() );
				if( l_Iter != cObjectAttributeType.end() )
				{
					l_Type = l_Iter->second; 
				}

				hr = SVObjectBuilder::SetObjectValue(objectID, objectID, DataName.c_str(), Data, l_Type);
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

HRESULT SVInspectionTreeParser::ProcessAttributes(uint32_t ownerID, uint32_t objectID, SvXml::SVXMLMaterialsTree::SVBranchHandle hItem)
{
	HRESULT hr = S_OK;

	_variant_t value;
	if (GetItemValue(scCommentTag, hItem, value))
	{
		hr = SVObjectBuilder::SetObjectValue(ownerID, objectID, scCommentTag, value, SV_STRING_Type);
	}

	if (GetItemValue(scModuleGuidTag, hItem, value))
	{
		hr = SVObjectBuilder::SetObjectValue(ownerID, objectID, scModuleGuidTag, value, SV_STRING_Type);
	}

	std::vector<_variant_t> attributes;
	bool bVal = GetValues(hItem, scAttributesSetTag, attributes);
	if (bVal)
	{			
		// process Attributes Set
		hr = SVObjectBuilder::SetObjectValue(ownerID, objectID, scAttributesSetTag, attributes, SV_DWORD_Type);
	}
	return hr;
}

HRESULT SVInspectionTreeParser::ProcessEquation(uint32_t ownerID, const _variant_t& equation)
{
	// process Equation Buffer
	HRESULT hr = SVObjectBuilder::SetObjectValue(ownerID, ownerID, scEquationBufferTag, equation, SV_STRING_Type);
	return hr;
}

HRESULT SVInspectionTreeParser::ProcessMaskData(uint32_t ownerID, const _variant_t& maskData)
{
	// process Mask Data Buffer
	HRESULT hr = SVObjectBuilder::SetObjectValue(ownerID, ownerID, scMaskDataTag, maskData, SV_STRING_Type);
	return hr;
}

HRESULT SVInspectionTreeParser::ProcessInputs(SvXml::SVXMLMaterialsTree::SVBranchHandle hInputs, uint32_t ownerID)
{
	HRESULT hr = S_OK;
	SvDef::StringPairVector InputPairVector;
	SvXml::SVXMLMaterialsTree::SVBranchHandle hInput{ m_rTree.getFirstBranch(hInputs) };
	do
	{
		if (hInput)
		{
			UpdateProgress(++m_count, m_totalSize);

			std::string branchName = m_rTree.getBranchName(hInput);
			if (branchName == scInputTag)
			{ //old version
				_variant_t name;
				_variant_t value;
				GetItemValue(scNameTag, hInput, name);
				GetItemValue(scValueTag, hInput, value);

				if (name.vt == VT_BSTR && value.vt == VT_BSTR)
				{
					SvPb::EmbeddedIdEnum embeddedId{ calcInputEmbeddedId(name) };
					uint32_t connectId{ calcObjectId(value) };
					if (SvPb::NoEmbeddedId == embeddedId || SvDef::InvalidObjectId == connectId)
					{
						SvDef::StringPair InputPair{ SvUl::createStdString(name), SvUl::createStdString(value) };
						InputPairVector.push_back(InputPair);
					}
					else
					{
						hr = SVObjectBuilder::OverwriteInputObject(embeddedId, SvDef::InvalidObjectId, {}, connectId, {}, ownerID); //not use old name, keep the initialized name
					}
				}
			}
			else
			{
				_variant_t objectName;
				_variant_t value;
				GetItemValue(scObjectNameTag, hInput, objectName);
				GetItemValue(scEmbeddedIDTag, hInput, value);
				SvPb::EmbeddedIdEnum embeddedId = calcEmbeddedId(value, objectName);
				GetItemValue(scUniqueReferenceIDTag, hInput, value);
				uint32_t objectID{ calcObjectId(value) };
				GetItemValue(scConntectedIDTag, hInput, value);
				uint32_t connectId = calcObjectId(value);
				GetItemValue(scConntectedDotnameTag, hInput, value);
				std::string connectedDotname {(VT_BSTR == value.vt) ? SvUl::createStdString(value) : ""};
				hr = SVObjectBuilder::OverwriteInputObject(embeddedId, objectID, SvUl::createStdString(objectName), connectId, connectedDotname, ownerID);
			}
			
			hInput = m_rTree.getNextBranch(hInputs, hInput);
		}
	} while (hInput);

	if (InputPairVector.size())
	{
		hr = SVObjectBuilder::SetInputs(ownerID, InputPairVector);
	}
	return hr;
}


HRESULT SVInspectionTreeParser::ProcessHistory(SvXml::SVXMLMaterialsTree::SVBranchHandle hHistory, uint32_t ownerID)
{
	HRESULT hr = S_OK;
	std::vector<std::pair<time_t, std::string>>  historyVector;
	SvXml::SVXMLMaterialsTree::SVBranchHandle hInput {m_rTree.getFirstBranch(hHistory)};
	do
	{
		if (hHistory && hInput)
		{
			UpdateProgress(++m_count, m_totalSize);

			std::string branchName = m_rTree.getBranchName(hInput);
			try
			{

				time_t date = std::stoll(branchName);
				_variant_t comment;
				GetItemValue(scCommentTag, hInput, comment);

				if (comment.vt == VT_BSTR)
				{
					historyVector.emplace_back(date, SvUl::createStdString(comment));
				}
			}
			catch (...)
			{
				Log_Assert(false);
			}

			hInput = m_rTree.getNextBranch(hHistory, hInput);
		}
	} while (hInput);

	if (historyVector.size())
	{
		hr = SVObjectBuilder::SetHistory(ownerID, historyVector);
	}
	return hr;
}

bool SVInspectionTreeParser::HasTag(SvXml::SVXMLMaterialsTree::SVBranchHandle hItem, const std::string& tag)
{
	bool l_Status = true;

	SvXml::SVXMLMaterialsTree::SVBranchHandle hValues( nullptr );

	hValues = m_rTree.findBranch(hItem, tag.c_str());

	if( nullptr == hValues )
	{
		SvXml::SVXMLMaterialsTree::SVLeafHandle hLeaf;

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
HRESULT SVInspectionTreeParser::CreateInspectionObject(uint32_t& rInspectionId, SvXml::SVXMLMaterialsTree& p_rTree, SvXml::SVXMLMaterialsTree::SVBranchHandle hItem)
{
	HRESULT hr = S_OK;
	
	uint32_t ownerId = SvDef::InvalidObjectId;
	_variant_t objectName;
	_variant_t classIDVariant;
	_variant_t uniqueID;
	_variant_t newDisableMethod;
	_variant_t enableAuxiliaryExtent;
	std::string name;

	name = p_rTree.getBranchName(hItem);
	SvXml::SVNavigateTree::GetItem(p_rTree, scObjectNameTag, hItem, objectName);
	SvXml::SVNavigateTree::GetItem(p_rTree, scClassIDTag, hItem, classIDVariant);
	SvPb::ClassIdEnum classId = calcClassId(classIDVariant);
	SvXml::SVNavigateTree::GetItem(p_rTree, scUniqueReferenceIDTag, hItem, uniqueID);
	SvXml::SVNavigateTree::GetItem(p_rTree, SvXml::CTAG_INSPECTION_NEW_DISABLE_METHOD, hItem, newDisableMethod);
	SvXml::SVNavigateTree::GetItem(p_rTree, SvXml::CTAG_INSPECTION_ENABLE_AUXILIARY_EXTENT, hItem, enableAuxiliaryExtent);
	rInspectionId = calcObjectId(uniqueID);
	hr = SVObjectBuilder::CreateObject(classId, rInspectionId, name, SvUl::createStdString(objectName), ownerId);
	if (S_OK == hr)
	{
		// Get Inspection Object
		SVObjectClass* pObject = nullptr;
		SVObjectManagerClass::Instance().GetObjectByIdentifier(rInspectionId, pObject);
		SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(pObject);
		if ( nullptr != pInspection )
		{
			if(VT_I4 == newDisableMethod.vt)
			{
				pInspection->SetNewDisableMethod(1 == newDisableMethod.lVal);
			}
			if (VT_I4 == enableAuxiliaryExtent.vt)
			{
				pInspection->setEnableAuxiliaryExtent(1 == enableAuxiliaryExtent.lVal);
			}
		}
		else
		{
			SvStl::MessageManager e(SvStl::MsgType::Log );
			e.setMessage( SVMSG_SVO_57_PARSERTREE_INSPECTIONCREATE_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
			hr = E_FAIL;
		}
	}
	if (S_OK != hr)
	{
		rInspectionId = SvDef::InvalidObjectId;
	}
	return hr;
}

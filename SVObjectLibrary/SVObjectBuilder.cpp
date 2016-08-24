//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectBuilder
//* .File Name       : $Workfile:   SVObjectBuilder.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 09:42:24  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#pragma warning (push)
#pragma warning (disable : 4996)
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/tokenizer.hpp>
//Moved to precompiled header: #include <boost/assign.hpp>
//Moved to precompiled header: #include <boost/foreach.hpp>
#include "SVObjectBuilder.h"
#include "SVObjectLibrary.h"
#include "SVObjectManagerClass.h"
#include "SVObjectClass.h"
#include "SVObjectAttributeClass.h"
#include "SVInputInfoListClass.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVStatusLibrary/MessageManagerResource.h"
#include "SVMessage/SVMessage.h"
#include "ObjectInterfaces/ErrorNumbers.h"
#include "TextDefinesSvOl.h"
#pragma warning (pop)
#pragma endregion Includes

typedef std::vector<std::string> split_vector_type;

typedef std::map<SVObjectTypeEnum, SVObjectScriptDataObjectTypeEnum> ObjectTypeTable;

static ObjectTypeTable typeTable = boost::assign::map_list_of<>
(SVBoolValueObjectType, SV_BOOL_Type)
(SVByteValueObjectType, SV_BYTE_Type)
(SVDWordValueObjectType, SV_DWORD_Type)
(SVLongValueObjectType, SV_LONG_Type)
(SVEnumValueObjectType, SV_LONG_Type)
(SVDoubleValueObjectType, SV_DOUBLE_Type)
(DoubleSortValueObjectType, SV_DOUBLE_Type)
(SVPointValueObjectType, SV_POINT_Type)
(SVStringValueObjectType, SV_STRING_Type)
(SVDPointValueObjectType, SV_DPOINT_Type)
(SVCharValueObjectType, SV_BYTE_Type)
(SVVariantValueObjectType, SV_VARIANT_Type)
(SVInt64ValueObjectType, SV_INT64_Type)
;

template<typename Separator>
static void SplitString(const std::string& val, split_vector_type& vec, Separator sep)
{
	boost::tokenizer<Separator> tokens(val, sep);
	BOOST_FOREACH (const std::string& t, tokens) 
	{ 
       vec.push_back(t);
	}
}

typedef boost::char_separator<char> Separator;

static POINT GetPointFromString(const SVString& strVal)
{
	POINT pointValue = { 0, 0 };
	split_vector_type SplitVec;
	std::string val(strVal.c_str());
	SplitVec.reserve(2);
	Separator sep(",");
	SplitString<Separator>(val, SplitVec, sep);
	
	if (SplitVec.size() == 2)
	{
		_stscanf( SplitVec[0].c_str(), "%d", &pointValue.x );
		_stscanf( SplitVec[1].c_str(), "%d", &pointValue.y );
	}
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
	else 
	{
		TRACE("GetPointFromString - Invalid String (Not a Point)\n");
	}
#endif
	return pointValue;
}

static SVDPointClass GetDPointFromString(const SVString& strVal)
{
	SVDPointClass pointValue(0.0, 0.0);
	split_vector_type SplitVec;
	std::string val(strVal.c_str());
	SplitVec.reserve(2);
	Separator sep(",");
	SplitString<Separator>(val, SplitVec, sep);
	if (SplitVec.size() == 2)
	{
		_stscanf( SplitVec[0].c_str(), "%lf", &pointValue.x );
		_stscanf( SplitVec[1].c_str(), "%lf", &pointValue.y );
	}
	else
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
		TRACE("GetDPointFromString - Invalid String (Not a DPoint)\n");
#endif
	}
	return pointValue;
}

HRESULT SVObjectBuilder::CreateObject(const GUID& classID, const GUID& uniqueID, const SVString& name, const SVString& objectName, const GUID& ownerUniqueID )
{
	HRESULT hr = S_OK;

	SVObjectClass* pObject = nullptr;
	// Construct new object...
	SVObjectManagerClass::Instance().ConstructObject(classID, pObject);

	if (pObject)
	{
		if (ownerUniqueID != GUID_NULL)
		{
			SVObjectClass* pOwnerObject = nullptr;
			SVObjectManagerClass::Instance().GetObjectByIdentifier(ownerUniqueID, pOwnerObject);
			if (pOwnerObject)
			{
				if (::SVSendMessage(pOwnerObject, SVM_REPLACE_OBJECT, reinterpret_cast<DWORD_PTR>(&uniqueID), reinterpret_cast<DWORD_PTR>(pObject)) != SVMR_SUCCESS)
				{
					ASSERT(false);
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
					TRACE("SVObjectBuilder::CreateObject - ReplaceObject %.80s\n", name.c_str());
#endif
					delete pObject;

#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
					TRACE("SVObjectBuilder::CreateObject - SVM_REPLACE_OBJECT was not successfully");
#endif
					hr = S_FALSE;
				}
				else
				{
					pObject->SetName(objectName.c_str());
				}
			}
			else
			{
				hr = S_FALSE;
				ASSERT(false);
				delete pObject;
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
				TRACE("SVObjectBuilder::CreateObject - Owner Object not Found");
#endif
			}
		}
		else
		{
			//
			if (SVObjectManagerClass::Instance().ChangeUniqueObjectID( pObject, uniqueID))
			{
				pObject->SetName(objectName.c_str());
			}
			else
			{
				hr = S_FALSE;
			}
		}
	}
	else
	{
		hr = S_FALSE;
		ASSERT(false);
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
		TRACE("SVObjectBuilder::CreateObject - Creation Failed");
#endif
	}
	return hr;
}

HRESULT SVObjectBuilder::DestroyFriends(const GUID& objectID)
{
	HRESULT hr = S_OK;

	SVObjectClass* pObject = nullptr;
	SVObjectManagerClass::Instance().GetObjectByIdentifier(objectID, pObject);
	if (pObject)
	{
		pObject->DestroyFriends();
	}
	return hr;
}

HRESULT SVObjectBuilder::CreateFriendObject(const GUID& classID, const GUID& uniqueID, const SVString& objectName, const GUID& ownerUniqueID, const GUID& rAddPreGuid)
{
	HRESULT hr = S_OK;

	if ( GUID_NULL == ownerUniqueID )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	SVObjectClass* pOwnerObject = nullptr;
	SVObjectManagerClass::Instance().GetObjectByIdentifier(ownerUniqueID, pOwnerObject);
	if(nullptr == pOwnerObject)
	{
		return E_FAIL;
	}

	SVObjectClass* pObject = nullptr;
	// Construct new object...
	SVObjectManagerClass::Instance().ConstructObject(classID, pObject);

	if( nullptr != pObject )
	{
		pObject->SetName(objectName.c_str());
		pObject->SetObjectOwner(ownerUniqueID);
		GUID friendId = SV_GUID_NULL;
		if (SV_GUID_NULL != uniqueID)
		{
			if( SVObjectManagerClass::Instance().ChangeUniqueObjectID( pObject, uniqueID ) )
			{
				friendId = uniqueID;
			}
		}
		else
		{
			friendId = pObject->GetUniqueObjectID();
		}
		if( SV_GUID_NULL != friendId )
		{
			BOOL Result = pOwnerObject->AddFriend(friendId, rAddPreGuid);
			if (Result)
			{
				
				::SVSendMessage(pOwnerObject, SVM_CREATE_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(pObject), 0);
			}
			else
			{
				delete pObject;
				hr = E_FAIL;
				ASSERT(FALSE);
			}
			
		}
	}
	else
	{
		hr = E_FAIL;
	}
	return hr;
}

HRESULT SVObjectBuilder::CreateEmbeddedObject(const GUID& embeddedID, const GUID& uniqueID, const SVString& name, const SVString& objectName, const GUID& ownerUniqueID)
{
	HRESULT hr = S_OK;

	if (ownerUniqueID != GUID_NULL)
	{
		SVObjectClass* pOwnerObject = nullptr;
		SVObjectManagerClass::Instance().GetObjectByIdentifier(ownerUniqueID, pOwnerObject);

		// Send to Owner of Embedded Object, Try to overwrite object...
		SVObjectClass* pObject = nullptr;
		
		if(nullptr != pOwnerObject)
		{

			if (pObject = (SVObjectClass *)::SVSendMessage(pOwnerObject, SVM_OVERWRITE_OBJECT, reinterpret_cast<DWORD_PTR>(&uniqueID), reinterpret_cast<DWORD_PTR>(&embeddedID)))
			{
				pObject->SetName(objectName.c_str());
			}
		}
		else
		{
			hr = S_FALSE;
			ASSERT(FALSE);
		}
}
	return hr;
}

static void BuildDataArray(SVObjectAttributeClass& dataObject, const SVString& itemName, const SVVariantList& ValueList, SVObjectScriptDataObjectTypeEnum dstDataType)
{
	dataObject.SetName(itemName.c_str());
	// check embedded class type for SVPointValueObject/SVDPointValueObject/SVVariantValueObject
	for (SVVariantList::const_iterator it = ValueList.begin();it != ValueList.end();++it)
	{
		const _variant_t& rValue = (*it);
		if (dstDataType == SV_VARIANT_Type)
		{
			dataObject.AddData(rValue);
		}
		else 
		{
			switch (rValue.vt)
			{
				case VT_BOOL:
					{
						BOOL bVal = (rValue.boolVal == VARIANT_TRUE) ? true : false;
						dataObject.AddData(bVal);
					}
					break;

				case VT_BSTR:
					{
						SVString strVal = SvUl_SF::createSVString(rValue.bstrVal);
						// check for Point/DPoint ??
						if (dstDataType == SV_POINT_Type)
						{
							const POINT& point = GetPointFromString(strVal);
							dataObject.AddData(point);
						}
						else if (dstDataType == SV_DPOINT_Type)
						{
							const SVDPointClass& point = GetDPointFromString(strVal);
							dataObject.AddData(point);
						}
						else
						{
							dataObject.AddData(strVal.c_str());
						}
					}
					break;

				case VT_R8:
					dataObject.AddData(rValue.dblVal);
					break;

				case VT_UI4:
					dataObject.AddData(rValue.ulVal);
					break;

				case VT_I4:
					dataObject.AddData(rValue.lVal);
					break;

				case VT_I1:
				case VT_UI1:
					dataObject.AddData(rValue.bVal);
					break;

				case VT_I8:
					dataObject.AddData(rValue.llVal);
					break;

				case VT_NULL:
				case VT_EMPTY:
					break;
			}
		}
	}
}

HRESULT SVObjectBuilder::SetObjectValue(const GUID& ownerID, const GUID& objectID, const SVString& itemName, const _variant_t& value, SVObjectScriptDataObjectTypeEnum dstDataType)
{
	SVVariantList values;
	values.push_back(value);
	return SetObjectValue(ownerID, objectID, itemName, values, dstDataType);
}

HRESULT SVObjectBuilder::SetObjectValue(const GUID& ownerID, const GUID& objectID, const SVString& itemName, const SVVariantList& values, SVObjectScriptDataObjectTypeEnum dstDataType)
{
	HRESULT hr = S_OK;

	SVObjectClass* pOwnerObject = nullptr;
	SVObjectClass* pObject = nullptr;

	SVObjectManagerClass::Instance().GetObjectByIdentifier(ownerID, pOwnerObject);
	SVObjectManagerClass::Instance().GetObjectByIdentifier(objectID, pObject);

	/////////////////////////////////////////////////////////////////////
	// If an Object and it's Owner were Found
	/////////////////////////////////////////////////////////////////////
	if (pOwnerObject && pObject)
	{
		// try to set the object's member value...
		// Note:: Send this message to the Object's Owner

		SVObjectAttributeClass dataObject;
		BuildDataArray(dataObject, itemName, values, dstDataType);

		if (::SVSendMessage(pOwnerObject, SVM_SET_OBJECT_VALUE, reinterpret_cast<DWORD_PTR>(&objectID), reinterpret_cast<DWORD_PTR>(&dataObject)) != SVMR_SUCCESS)
		{
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_ObjectBuilder_SetObjectValueError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10248 );
		}
	}
	return hr;
}

HRESULT SVObjectBuilder::SetInputs(const GUID& objectID, const SVNameGuidList& guidList)
{
	HRESULT hr = S_OK;

	SVObjectClass* pObject = nullptr;
	SVObjectManagerClass::Instance().GetObjectByIdentifier(objectID, pObject);
	
	if (pObject)
	{
		SVInputInfoListClass inputInfoList;
		::SVSendMessage(pObject, SVM_GET_INPUT_INTERFACE, reinterpret_cast<DWORD_PTR>(&inputInfoList), 0);

		// reattach inputs
		for( int i = 0; S_OK == hr && i < inputInfoList.GetSize(); i++ )
		{
			SVInObjectInfoStruct* pInInfo = inputInfoList.GetAt(i);
			if (pInInfo)
			{
				SVNameGuidList::const_iterator it = guidList.find(pInInfo->GetInputName().c_str());
				if (it != guidList.end())
				{
					SVGUID inputGuid = it->second;

					pInInfo->SetInputObject(inputGuid);
				}
			}
			else
			{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
				TRACE("SVObjectBuilder::SetInputs - Input Info pointer is not valid\n");
#endif
				hr = S_FALSE;
			}
		}
	}
	else
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
		TRACE("SVObjectBuilder::SetInputs - Not a Valid Object\n");
#endif
		hr = S_FALSE;
	}
	return hr;
}

HRESULT SVObjectBuilder::GetObjectDataType(const GUID& ownerID, const GUID& objectID, SVObjectScriptDataObjectTypeEnum& dataType)
{
	HRESULT hr = S_OK;
	
	SVObjectClass* pOwnerObject = nullptr;
	SVObjectClass* pObject = nullptr;

	SVObjectManagerClass::Instance().GetObjectByIdentifier(ownerID, pOwnerObject);
	SVObjectManagerClass::Instance().GetObjectByIdentifier(objectID, pObject);

	/////////////////////////////////////////////////////////////////////
	// If an Object and it's Owner were Found
	/////////////////////////////////////////////////////////////////////
	if (pOwnerObject && pObject)
	{
		SVObjectTypeEnum type = pObject->GetObjectType();
		dataType = SV_UNKNOWN_Type;
		ObjectTypeTable::const_iterator it = typeTable.find(type);
		if (it != typeTable.end())
		{
			dataType = it->second;
		}
		else
		{
			hr = S_FALSE;
		}
	}
	else
	{
		hr = E_FAIL;
	}
	return hr;
}


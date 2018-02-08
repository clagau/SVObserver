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
//Moved to precompiled header: #include <boost/foreach.hpp>
#include "SVObjectBuilder.h"
#include "SVObjectLibrary.h"
#include "SVObjectManagerClass.h"
#include "SVObjectClass.h"
#include "SVObjectAttributeClass.h"
#include "SVInputInfoListClass.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "TextDefinesSvOl.h"
#pragma warning (pop)
#pragma endregion Includes

typedef std::map<SvDef::SVObjectSubTypeEnum, SVObjectScriptDataObjectTypeEnum> ObjectTypeTable;

static const ObjectTypeTable cTypeTable
{
	{SvDef::SVBoolValueObjectType, SV_BOOL_Type},
	{SvDef::SVByteValueObjectType, SV_BYTE_Type},
	{SvDef::SVDWordValueObjectType, SV_DWORD_Type},
	{SvDef::SVLongValueObjectType, SV_LONG_Type},
	{SvDef::SVEnumValueObjectType, SV_LONG_Type},
	{SvDef::SVDoubleValueObjectType, SV_DOUBLE_Type},
	{SvDef::DoubleSortValueObjectType, SV_DOUBLE_Type},
	{SvDef::SVPointValueObjectType, SV_POINT_Type},
	{SvDef::SVStringValueObjectType, SV_STRING_Type},
	{SvDef::SVDPointValueObjectType, SV_DPOINT_Type},
	{SvDef::SVCharValueObjectType, SV_BYTE_Type},
	{SvDef::SVVariantValueObjectType, SV_VARIANT_Type},
	{SvDef::SVInt64ValueObjectType, SV_INT64_Type}
};

template<typename Separator>
static void SplitString(const std::string& rValue, SvDef::StringVector& rContainer, Separator sep)
{
	boost::tokenizer<Separator> tokens(rValue, sep);
	BOOST_FOREACH (const std::string& rItem, tokens) 
	{ 
       rContainer.push_back( rItem );
	}
}

typedef boost::char_separator<char> Separator;

static POINT GetPointFromString(const std::string& rValue)
{
	POINT pointValue = { 0, 0 };
	SvDef::StringVector SplitVec;
	SplitVec.reserve(2);
	Separator sep(",");
	SplitString<Separator>(rValue, SplitVec, sep);
	
	if (SplitVec.size() == 2)
	{
		_stscanf( SplitVec[0].c_str(), _T("%d"), &pointValue.x );
		_stscanf( SplitVec[1].c_str(), _T("%d"), &pointValue.y );
	}
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
	else 
	{
		::OutputDebugString( _T("GetPointFromString - Invalid String (Not a Point)\n") );
	}
#endif
	return pointValue;
}

static SVDPointClass GetDPointFromString(const std::string& rValue)
{
	SVDPointClass pointValue(0.0, 0.0);
	SvDef::StringVector SplitVec;
	SplitVec.reserve(2);
	Separator sep(",");
	SplitString<Separator>(rValue, SplitVec, sep);
	if (SplitVec.size() == 2)
	{
		_stscanf( SplitVec[0].c_str(), _T("%lf"), &pointValue.x );
		_stscanf( SplitVec[1].c_str(), _T("%lf"), &pointValue.y );
	}
	else
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
		::OutputDebugString( _T("GetDPointFromString - Invalid String (Not a DPoint)\n") );
#endif
	}
	return pointValue;
}

HRESULT SVObjectBuilder::CreateObject(const GUID& classID, const GUID& uniqueID, const std::string& name, const std::string& objectName, const GUID& ownerUniqueID )
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
				// Try to replace or add object...
				if( !pOwnerObject->replaceObject(pObject, uniqueID) )
				{
					assert(false);
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
					std::string Temp = SvUl::Format(_T("SVObjectBuilder::CreateObject - ReplaceObject %.80s\n"), name.c_str() );
					::OutputDebugString( Temp.c_str() );
#endif
					delete pObject;

#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
					::OutputDebugString( _T("SVObjectBuilder::CreateObject - replaceObject was not successfully") );
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
				assert(false);
				delete pObject;
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
				::OutputDebugString( _T("SVObjectBuilder::CreateObject - Owner Object not Found") );
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
		assert(false);
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
		::OutputDebugString( _T("SVObjectBuilder::CreateObject - Creation Failed") );
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

HRESULT SVObjectBuilder::CreateFriendObject(const GUID& classID, const GUID& uniqueID, const std::string& objectName, const GUID& ownerUniqueID, const GUID& rAddPreGuid)
{
	HRESULT hr = S_OK;

	if ( GUID_NULL == ownerUniqueID )
	{
		assert(false);
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
		GUID friendId = GUID_NULL;
		if (GUID_NULL != uniqueID)
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
		if( GUID_NULL != friendId )
		{
			bool Result = pOwnerObject->AddFriend(friendId, rAddPreGuid);
			if (Result)
			{
				pOwnerObject->CreateChildObject(pObject);
			}
			else
			{
				delete pObject;
				hr = E_FAIL;
				assert(false);
			}
			
		}
	}
	else
	{
		hr = E_FAIL;
	}
	return hr;
}

HRESULT SVObjectBuilder::OverwriteEmbeddedObject(const GUID& embeddedID, const GUID& uniqueID, const std::string& objectName, const GUID& ownerUniqueID)
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
			pObject = pOwnerObject->OverwriteEmbeddedObject(uniqueID, embeddedID);
			if (nullptr != pObject)
			{
				pObject->SetName(objectName.c_str());
			}
		}
		else
		{
			hr = S_FALSE;
			assert(false);
		}
}
	return hr;
}

static void BuildDataArray(SVObjectAttributeClass& dataObject, const std::string& itemName, const SVVariantList& ValueList, SVObjectScriptDataObjectTypeEnum dstDataType)
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
						std::string strVal = SvUl::createStdString(rValue.bstrVal);
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

HRESULT SVObjectBuilder::SetObjectValue(const GUID& ownerID, const GUID& objectID, const std::string& itemName, const _variant_t& value, SVObjectScriptDataObjectTypeEnum dstDataType)
{
	SVVariantList values;
	values.push_back(value);
	return SetObjectValue(ownerID, objectID, itemName, values, dstDataType);
}

HRESULT SVObjectBuilder::SetObjectValue(const GUID& ownerID, const GUID& objectID, const std::string& itemName, const SVVariantList& values, SVObjectScriptDataObjectTypeEnum dstDataType)
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

		if( S_OK != pOwnerObject->SetValuesForAnObject(objectID, &dataObject) )
		{
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ObjectBuilder_SetObjectValueError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10248 );
		}
	}
	return hr;
}

HRESULT SVObjectBuilder::SetInputs(const GUID& objectID, const SvDef::StringPairVector& rInputPairVector)
{
	HRESULT hr = S_OK;

	SVObjectClass* pObject = nullptr;
	SVObjectManagerClass::Instance().GetObjectByIdentifier(objectID, pObject);
	
	if (pObject)
	{
		SVInputInfoListClass inputInfoList;
		pObject->GetInputInterface(inputInfoList, false );
		
		// reattach inputs
		for( int i = 0; S_OK == hr && i < static_cast<int> (inputInfoList.size()); i++ )
		{
			SVInObjectInfoStruct* pInInfo = inputInfoList[i];
			if (pInInfo)
			{
				auto Iter = std::find_if(rInputPairVector.begin(), rInputPairVector.end(), 
					[&pInInfo](const SvDef::StringPair& rEntry) 
				{ 
					return rEntry.first == pInInfo->GetInputName(); 
				});
				if (Iter != rInputPairVector.end())
				{
					SVObjectReference ObjectRef{ Iter->second };

					pInInfo->SetInputObject(ObjectRef);
				}
			}
			else
			{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
				::OutputDebugString( _T("SVObjectBuilder::SetInputs - Input Info pointer is not valid\n") );
#endif
				hr = S_FALSE;
			}
		}
	}
	else
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
		::OutputDebugString( _T("SVObjectBuilder::SetInputs - Not a Valid Object\n") );
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
		SvDef::SVObjectSubTypeEnum type = pObject->GetObjectSubType();
		dataType = SV_UNKNOWN_Type;
		ObjectTypeTable::const_iterator it = cTypeTable.find(type);
		if (it != cTypeTable.end())
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


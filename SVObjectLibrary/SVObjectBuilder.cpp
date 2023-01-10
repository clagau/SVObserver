//******************************************************************************
//* COPYRIGHT (c) 2012 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVObjectBuilder.h"
#include "SVObjectLibrary.h"
#include "SVObjectManagerClass.h"
#include "SVObjectClass.h"
#include "SVObjectAttributeClass.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/SafeArrayHelper.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "ObjectInterfaces/ILinkedObject.h"
#include "ObjectInterfaces/ITaskObject.h"
#include "InputObject.h"
#pragma warning (pop)
#pragma endregion Includes

typedef std::map<SvPb::SVObjectSubTypeEnum, SVObjectScriptDataObjectTypeEnum> ObjectTypeTable;

static const ObjectTypeTable cTypeTable
{
	{SvPb::SVBoolValueObjectType, SV_BOOL_Type},
	{SvPb::SVByteValueObjectType, SV_BYTE_Type},
	{SvPb::SVDWordValueObjectType, SV_DWORD_Type},
	{SvPb::SVLongValueObjectType, SV_LONG_Type},
	{SvPb::SVEnumValueObjectType, SV_LONG_Type},
	{SvPb::SVDoubleValueObjectType, SV_DOUBLE_Type},
	{SvPb::DoubleSortValueObjectType, SV_DOUBLE_Type},
	{SvPb::SVPointValueObjectType, SV_POINT_Type},
	{SvPb::SVStringValueObjectType, SV_STRING_Type},
	{SvPb::SVDPointValueObjectType, SV_DPOINT_Type},
	{SvPb::SVCharValueObjectType, SV_BYTE_Type},
	{SvPb::SVVariantValueObjectType, SV_VARIANT_Type},
	{SvPb::SVInt64ValueObjectType, SV_INT64_Type},
	{SvPb::SVTimerValueObjectType, SV_INT64_Type}
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

static SVPoint<double> GetDPointFromString(const std::string& rValue)
{
	SVPoint<double> pointValue(0.0, 0.0);
	SvDef::StringVector SplitVec;
	SplitVec.reserve(2);
	Separator sep(",");
	SplitString<Separator>(rValue, SplitVec, sep);
	if (SplitVec.size() == 2)
	{
		_stscanf( SplitVec[0].c_str(), _T("%lf"), &pointValue.m_x );
		_stscanf( SplitVec[1].c_str(), _T("%lf"), &pointValue.m_y );
	}
	else
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
		::OutputDebugString( _T("GetDPointFromString - Invalid String (Not a DPoint)\n") );
#endif
	}
	return pointValue;
}

HRESULT SVObjectBuilder::CreateObject(SvPb::ClassIdEnum classID, uint32_t uniqueID, const std::string& name, const std::string& objectName, uint32_t ownerUniqueID )
{
	HRESULT hr = S_OK;
	UNREFERENCED_PARAMETER(name);

	SVObjectClass* pObject = nullptr;
	// Construct new object...
	SVObjectManagerClass::Instance().ConstructObject(classID, pObject);

	if (pObject)
	{
		if (ownerUniqueID != SvDef::InvalidObjectId)
		{
			SVObjectClass* pOwnerObject = nullptr;
			SVObjectManagerClass::Instance().GetObjectByIdentifier(ownerUniqueID, pOwnerObject);
			if (pOwnerObject)
			{
				// Try to replace or add object...
				if( !pOwnerObject->replaceObject(pObject, uniqueID) )
				{
					Log_Assert(false);
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
				Log_Assert(false);
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
		Log_Assert(false);
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
		::OutputDebugString( _T("SVObjectBuilder::CreateObject - Creation Failed") );
#endif
	}
	return hr;
}

HRESULT SVObjectBuilder::DestroyFriends(uint32_t objectID)
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

SvOi::IObjectClass* SVObjectBuilder::CreateFriendObject(SvPb::ClassIdEnum classID, uint32_t uniqueID, const std::string& objectName, uint32_t ownerUniqueID, uint32_t addPreId)
{
	if (SvDef::InvalidObjectId == ownerUniqueID)
	{
		Log_Assert(false);
		return nullptr;
	}

	SVObjectClass* pOwnerObject = nullptr;
	SVObjectManagerClass::Instance().GetObjectByIdentifier(ownerUniqueID, pOwnerObject);
	auto* pTaskOwner = dynamic_cast<SvOi::ITaskObject*>(pOwnerObject);
	if(nullptr == pTaskOwner)
	{
		return nullptr;
	}

	SVObjectClass* pObject = nullptr;
	// Construct new object...
	SVObjectManagerClass::Instance().ConstructObject(classID, pObject);

	if( nullptr != pObject )
	{
		pObject->SetName(objectName.c_str());
		pObject->SetObjectOwner(ownerUniqueID);
		uint32_t friendId = SvDef::InvalidObjectId;
		if (SvDef::InvalidObjectId != uniqueID)
		{
			if( SVObjectManagerClass::Instance().ChangeUniqueObjectID( pObject, uniqueID ) )
			{
				friendId = uniqueID;
			}
		}
		else
		{
			friendId = pObject->getObjectId();
		}
		if (SvDef::InvalidObjectId != friendId)
		{
			bool Result = pTaskOwner->AddFriend(friendId, addPreId);
			if (Result)
			{
				pOwnerObject->CreateChildObject(pObject);
			}
			else
			{
				delete pObject;
				pObject = nullptr;
				Log_Assert(false);
			}
			
		}
	}

	return pObject;
}

HRESULT SVObjectBuilder::OverwriteEmbeddedObject(SvPb::EmbeddedIdEnum embeddedID, uint32_t uniqueID, const std::string& objectName, uint32_t ownerUniqueID)
{
	HRESULT hr = S_OK;

	if (ownerUniqueID != SvDef::InvalidObjectId)
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
			Log_Assert(false);
		}
	}
	return hr;
}

HRESULT SVObjectBuilder::SetEmbeddedLinkedChildIds(uint32_t uniqueID, const std::vector<uint32_t>& rObjectIds)
{
	HRESULT hr = S_OK;

	SVObjectClass* pObject = nullptr;
	SVObjectManagerClass::Instance().GetObjectByIdentifier(uniqueID, pObject);
	SvOi::ILinkedObject* pLinkedObject = dynamic_cast<SvOi::ILinkedObject*>(pObject);
	if (nullptr != pLinkedObject)
	{
		pLinkedObject->setChildIds(rObjectIds);
	}
	else
	{
		hr = S_FALSE;
		Log_Assert(false);
	}

	return hr;
}

HRESULT SVObjectBuilder::OverwriteInputObject(SvPb::EmbeddedIdEnum embeddedID, uint32_t uniqueID, const std::string& objectName, uint32_t connectID, const std::string& rConnectedDotname, uint32_t ownerUniqueID)
{
	HRESULT hr = S_OK;

	if (ownerUniqueID != SvDef::InvalidObjectId)
	{
		SVObjectClass* pOwnerObject = nullptr;
		SVObjectManagerClass::Instance().GetObjectByIdentifier(ownerUniqueID, pOwnerObject);

		// Send to Owner of Embedded Object, Try to overwrite object...
		SVObjectClass* pObject = nullptr;

		if (nullptr != pOwnerObject)
		{
			pObject = pOwnerObject->overwriteInputObject(uniqueID, embeddedID);
			if (nullptr != pObject)
			{
				if (false == objectName.empty())
				{
					pObject->SetName(objectName.c_str());
				}
				auto* pInput = dynamic_cast<SvOl::InputObject*>(pObject);
				if (pInput)
				{
					pInput->SetInputObject(connectID);
					pInput->setDottedNameOfLoadedInput(rConnectedDotname);
				}
			}
		}
		else
		{
			hr = S_FALSE;
			Log_Assert(false);
		}
	}
	return hr;
}

static void BuildDataArray(SVObjectAttributeClass& dataObject, const std::string& itemName, const std::vector<_variant_t>& ValueList, SVObjectScriptDataObjectTypeEnum dstDataType)
{
	dataObject.SetName(itemName.c_str());
	// check embedded class type for SVPointValueObject/SVDPointValueObject/SVVariantValueObject
	for (const auto& rValue : ValueList)
	{
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
							const SVPoint<double>& point = GetDPointFromString(strVal);
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

				case VT_R8 | VT_ARRAY:
				{
					auto vec = SvUl::SafeArrayToVector<double>(rValue.parray);
					for (auto val : vec)
					{
						dataObject.AddData(val);
					}
					break;
				}

				case VT_UI4 | VT_ARRAY:
				{
					auto vec = SvUl::SafeArrayToVector<DWORD>(rValue.parray);
					for (auto val : vec)
					{
						dataObject.AddData(val);
					}
					break;
				}

				case VT_I4 | VT_ARRAY:
				{
					auto vec = SvUl::SafeArrayToVector<long>(rValue.parray);
					for (auto val : vec)
					{
						dataObject.AddData(val);
					}
					break;
				}

				case VT_I1 | VT_ARRAY:
				case VT_UI1 | VT_ARRAY:
				{
					auto vec = SvUl::SafeArrayToVector<BYTE>(rValue.parray);
					for (auto val : vec)
					{
						dataObject.AddData(val);
					}
					break;
				}

				case VT_I8 | VT_ARRAY:
				{
					auto vec = SvUl::SafeArrayToVector<long long>(rValue.parray);
					for (auto val : vec)
					{
						dataObject.AddData(val);
					}
					break;
				}

				case VT_NULL:
				case VT_EMPTY:
					break;
			}
		}
	}
}

HRESULT SVObjectBuilder::SetObjectValue(uint32_t ownerID, uint32_t objectID, const std::string& itemName, const _variant_t& value, SVObjectScriptDataObjectTypeEnum dstDataType)
{
	std::vector<_variant_t> values;
	values.push_back(value);
	return SetObjectValue(ownerID, objectID, itemName, values, dstDataType);
}

HRESULT SVObjectBuilder::SetObjectValue(uint32_t ownerID, uint32_t objectID, const std::string& itemName, const std::vector<_variant_t>& values, SVObjectScriptDataObjectTypeEnum dstDataType)
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
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ObjectBuilder_SetObjectValueError, SvStl::SourceFileParams(StdMessageParams));
		}
	}
	return hr;
}

HRESULT SVObjectBuilder::SetInputs(uint32_t objectID, const SvDef::StringPairVector& rInputPairVector)
{
	HRESULT hr = S_OK;

	SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(objectID);
	auto* pTask = dynamic_cast<SvOi::ITaskObject*>(pObject);
	if (pTask)
	{
		for (const auto& rPair : rInputPairVector)
		{
			SVObjectReference ObjectRef{ rPair.second };
			pTask->ConnectToObject(rPair.first, ObjectRef.getObjectId());
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

HRESULT SVObjectBuilder::GetObjectDataType(uint32_t ownerID, uint32_t objectID, SVObjectScriptDataObjectTypeEnum& dataType)
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
		SvPb::SVObjectSubTypeEnum type = pObject->GetObjectSubType();
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

HRESULT SVObjectBuilder::SetIndirectStringToObject(uint32_t ownerID, SvPb::EmbeddedIdEnum embeddedId, const std::vector<_variant_t>& rValueString)
{
	SVObjectClass* pOwnerObject = nullptr;
	SVObjectManagerClass::Instance().GetObjectByIdentifier(ownerID, pOwnerObject);
	if (pOwnerObject)
	{
		return pOwnerObject->setIndirectStringToObject(embeddedId, rValueString);
	}
	else
	{
		return E_FAIL;
	}
}
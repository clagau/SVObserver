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
#include "stdafx.h"
#pragma warning (push)
#pragma warning (disable : 4996)
#include <boost/config.hpp>
#include <boost/tokenizer.hpp>
#include <boost/assign.hpp>
#include <boost/foreach.hpp>
#include "SVObjectBuilder.h"
#include "SVObjectLibrary.h"
#include "SVObjectManagerClass.h"
#include "SVObjectClass.h"
#include "SVObjectAttributeClass.h"
#include "SVInputInfoListClass.h"
#include "SVUtilityLibrary/SVGUID.h"
#pragma warning (pop)

typedef std::vector<std::string> split_vector_type;

typedef std::map<SVObjectTypeEnum, SVObjectScriptDataObjectTypeEnum> ObjectTypeTable;

static ObjectTypeTable typeTable = boost::assign::map_list_of<>
(SVBoolValueObjectType, SV_BOOL_Type)
(SVByteValueObjectType, SV_BYTE_Type)
(SVDWordValueObjectType, SV_DWORD_Type)
(SVLongValueObjectType, SV_LONG_Type)
(SVEnumValueObjectType, SV_LONG_Type)
(SVDoubleValueObjectType, SV_DOUBLE_Type)
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
	else 
	{
		TRACE("GetPointFromString - Invalid String (Not a Point)\n");
	}
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
		TRACE("GetDPointFromString - Invalid String (Not a DPoint)\n");
	}
	return pointValue;
}

HRESULT SVObjectBuilder::CreateObject(const GUID& classID, const GUID& uniqueID, const SVString& name, const SVString& objectName, const GUID& ownerUniqueID )
{
	HRESULT hr = S_OK;

	SVObjectClass* pObject = NULL;
	// Construct new object...
	SVObjectManagerClass::Instance().ConstructObject(classID, pObject);

	if (pObject)
	{
		if (ownerUniqueID != GUID_NULL)
		{
			SVObjectClass* pOwnerObject = NULL;
			SVObjectManagerClass::Instance().GetObjectByIdentifier(ownerUniqueID, pOwnerObject);
			if (pOwnerObject)
			{
				if (::SVSendMessage(pOwnerObject, SVM_REPLACE_OBJECT, reinterpret_cast<DWORD_PTR>(&uniqueID), reinterpret_cast<DWORD_PTR>(pObject)) != SVMR_SUCCESS)
				{
					ASSERT(false);
					TRACE("SVObjectBuilder::CreateObject - ReplaceObject %.80s\n", name.c_str());

					delete pObject;

					TRACE("SVObjectBuilder::CreateObject - SVM_REPLACE_OBJECT was not successfully");
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
				TRACE("SVObjectBuilder::CreateObject - Owner Object not Found");
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
		TRACE("SVObjectBuilder::CreateObject - Creation Failed");
	}
	return hr;
}

HRESULT SVObjectBuilder::DestroyFriends(const GUID& objectID)
{
	HRESULT hr = S_OK;

	SVObjectClass* pObject = NULL;
	SVObjectManagerClass::Instance().GetObjectByIdentifier(objectID, pObject);
	if (pObject)
	{
		pObject->DestroyFriends();
	}
	return hr;
}

HRESULT SVObjectBuilder::CreateFriendObject(const GUID& classID, const GUID& uniqueID, const SVString& name, const SVString& objectName, const GUID& ownerUniqueID)
{
	HRESULT hr = S_OK;

	SVObjectClass* pObject = NULL;
	// Construct new object...
	SVObjectManagerClass::Instance().ConstructObject(classID, pObject);

	if( pObject && ownerUniqueID != GUID_NULL )
	{
		pObject->SetName(objectName.c_str());
		pObject->SetObjectOwner(ownerUniqueID);
		if( SVObjectManagerClass::Instance().ChangeUniqueObjectID( pObject, uniqueID ) )
		{
			SVObjectClass* pOwnerObject = NULL;
			SVObjectManagerClass::Instance().GetObjectByIdentifier(ownerUniqueID, pOwnerObject);
			if(nullptr != pOwnerObject)
			{
				pOwnerObject->AddFriend(uniqueID);
				::SVSendMessage(pOwnerObject, SVM_CREATE_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(pObject), NULL);
			}
			else
			{
				hr = S_FALSE;
				ASSERT(FALSE);
			}
			
		}
	}
	return hr;
}

HRESULT SVObjectBuilder::CreateEmbeddedObject(const GUID& embeddedID, const GUID& uniqueID, const SVString& name, const SVString& objectName, const GUID& ownerUniqueID)
{
	HRESULT hr = S_OK;

	if (ownerUniqueID != GUID_NULL)
	{
		SVObjectClass* pOwnerObject = NULL;
		SVObjectManagerClass::Instance().GetObjectByIdentifier(ownerUniqueID, pOwnerObject);

		// Send to Owner of Embedded Object, Try to overwrite object...
		SVObjectClass* pObject = NULL;
		
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

static void BuildDataArray(SVObjectAttributeClass& dataObject, const SVString& itemName, const SVVariantList& values, SVObjectScriptDataObjectTypeEnum dstDataType)
{
	dataObject.SetName(itemName.c_str());
	// check embedded class type for SVPointValueObject/SVDPointValueObject/SVVariantValueObject
	for (SVVariantList::const_iterator it = values.begin();it != values.end();++it)
	{
		const _variant_t& v = (*it);
		if (dstDataType == SV_VARIANT_Type)
		{
			dataObject.AddData(v);
		}
		else 
		{
			switch (v.vt)
			{
				case VT_BOOL:
					{
						BOOL bVal = (v.boolVal == VARIANT_TRUE) ? true : false;
						dataObject.AddData(bVal);
					}
					break;

				case VT_BSTR:
					{
						SVString strVal = SvUl_SF::createSVString(v);
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
					dataObject.AddData(v.dblVal);
					break;

				case VT_UI4:
					dataObject.AddData(v.ulVal);
					break;

				case VT_I4:
					dataObject.AddData(v.lVal);
					break;

				case VT_I1:
				case VT_UI1:
					dataObject.AddData(v.bVal);
					break;

				case VT_I8:
					dataObject.AddData(v.llVal);
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

	SVObjectClass* pOwnerObject = NULL;
	SVObjectClass* pObject = NULL;

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
			AfxMessageBox( "SVObjectBuilder::SetObjectValue Error:\n Set Object Member Value failed!" );
		}
	}
	return hr;
}

HRESULT SVObjectBuilder::SetInputs(const GUID& objectID, const SVNameGuidList& guidList)
{
	HRESULT hr = S_OK;

	SVObjectClass* pObject = NULL;
	SVObjectManagerClass::Instance().GetObjectByIdentifier(objectID, pObject);
	
	if (pObject)
	{
		SVInputInfoListClass inputInfoList;
		::SVSendMessage(pObject, SVM_GET_INPUT_INTERFACE, reinterpret_cast<DWORD_PTR>(&inputInfoList), NULL);

		// reattach inputs
		for( int i = 0; hr == S_OK && i < inputInfoList.GetSize(); i++ )
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
				TRACE("SVObjectBuilder::SetInputs - Input Info pointer is not valid\n");
				hr = S_FALSE;
			}
		}
	}
	else
	{
		TRACE("SVObjectBuilder::SetInputs - Not a Valid Object\n");
		hr = S_FALSE;
	}
	return hr;
}

HRESULT SVObjectBuilder::GetObjectDataType(const GUID& ownerID, const GUID& objectID, SVObjectScriptDataObjectTypeEnum& dataType)
{
	HRESULT hr = S_OK;
	
	SVObjectClass* pOwnerObject = NULL;
	SVObjectClass* pObject = NULL;

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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObjectLibrary\SVObjectBuilder.cpp_v  $
 * 
 *    Rev 1.2   15 May 2014 09:42:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised SVSendMessage to use DWORD_PTR instead of DWORD or LONG_PTR.
 * Revised processMessage to use DWORD_PTR instead of DWORD or LONG_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 10:09:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 16:46:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   12 Sep 2012 17:02:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Set Inputs to remove extra logic to connect inputs and only assisnged the guid.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Sep 2012 18:55:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object to allow array data to be collected.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Sep 2012 11:33:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the BuildDataArray method to fix issues with BOOL type.  Updated type conversion table to handle the Enumeration Value Object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Sep 2012 13:16:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in of new functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

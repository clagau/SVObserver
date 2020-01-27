//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// This class should managed value object which are linked to other value objects (or only a valid value).
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "LinkedValue.h"
#include "BasicValueObject.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/ObjectDefines.h"
#include "Definitions/StringTypeDef.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "SVObjectLibrary/SVInObjectInfoStruct.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary/SafeArrayHelper.h"
#pragma endregion Includes

namespace SvVol
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
LinkedValue::LinkedValue() :
	m_CircularReference(false)
{
}

LinkedValue::~LinkedValue()
{
}
#pragma endregion Constructor

#pragma region Public Methods


HRESULT LinkedValue::GetArrayValue(_variant_t& rValue) 
{
	
	//! If this flag is still set then this value is trying to reference itself!
	if (m_CircularReference)
	{
		return SVMSG_SVO_105_CIRCULAR_REFERENCE;
	}
	HRESULT Result {S_OK};
	//! When getting the value from an indirect value make sure it is not referencing this object
	if (nullptr != m_LinkedObjectRef.getValueObject())
	{
		
		m_CircularReference = true;
		if (m_LinkedObjectRef.isEntireArray())
		{
			Result = m_LinkedObjectRef.getValue(rValue);
		}
		else
		{
			_variant_t skalvalue;
			Result = m_LinkedObjectRef.getValue(skalvalue);
			rValue = SvUl::VariantToSafeArray(skalvalue);
		}

		m_CircularReference = false;
	}
	else
	{
		Result = __super::getValue(rValue, -1);
	}
	return Result;
}

HRESULT LinkedValue::SetDefaultValue(const _variant_t& rValue, bool bResetAll)
{
	if (0 == (rValue.vt & VT_ARRAY))
	{
		return __super::SetDefaultValue(rValue, bResetAll);
	}
	else
	{
		HRESULT hres;
		_variant_t value;
		value.ChangeType(rValue.vt & ~VT_ARRAY);
		hres = __super::SetDefaultValue(value, bResetAll);
		
		if (bResetAll)
		{
			long lBound(0), uBound(0);
			SafeArrayGetUBound(const_cast<SAFEARRAY *>(rValue.parray), 1, &uBound);
			SafeArrayGetLBound(const_cast<SAFEARRAY *>(rValue.parray), 1, &lBound);
			int32_t cnt_elements = uBound - lBound + 1;
			if (getArraySize() != cnt_elements)
			{
				SetArraySize(cnt_elements);
			}
			hres = __super::setValue(rValue);
		}
		return hres;
	}
	
	
}

HRESULT LinkedValue::setValue(const _variant_t& rValue, int Index /*= -1*/, bool fixArraysize)
{
	if( (rValue.vt & VT_ARRAY) && Index == -1)
	{
		long lBound(0), uBound(0);
		SafeArrayGetUBound(const_cast<SAFEARRAY *>(rValue.parray), 1, &uBound);
		SafeArrayGetLBound(const_cast<SAFEARRAY *>(rValue.parray), 1, &lBound);
		int32_t cnt_elements = uBound - lBound + 1;
		if (getArraySize() != cnt_elements && !fixArraysize)
		{
			SetArraySize(cnt_elements);
		}
	}
	return __super::setValue(rValue, Index, fixArraysize);
}
HRESULT LinkedValue::setValue(const std::string& rValue, int Index /*= -1*/)
{
	return __super::setValue(rValue, Index);
}

void LinkedValue::updateMemBlockData() const
{
	///When the linked value is an indirect value we need to always update it
	if(isIndirectValue())
	{
		setHasChanged(true);
	}
	__super::updateMemBlockData();
}

HRESULT  LinkedValue::SetValue(const _variant_t& rValue, int Index)
{
	///enabling setting Linked Value
	if ((Index == -1) && isArray())
	{
		Index = 0;
		SetArraySize(0L);
	}
	return __super::SetValue(rValue, Index);
}


HRESULT LinkedValue::GetValue(_variant_t& rValue, int Index /*= -1*/) const
{
	HRESULT Result( ValidateIndex(Index) );

	//! If this flag is still set then this value is trying to reference itself!
	if (m_CircularReference)
	{
		Result = SVMSG_SVO_105_CIRCULAR_REFERENCE;
	}
	if (S_OK == Result)
	{
		//! When getting the value from an indirect value make sure it is not referencing this object
		if (nullptr != m_LinkedObjectRef.getValueObject())
		{
			m_CircularReference = true;
			Result = m_LinkedObjectRef.getValue(rValue);
			m_CircularReference = false;
		}
		else
		{

			Result = __super::GetValue(rValue, Index);
		}
		
		if (S_OK == Result)
		{
			//If the Linked Value is of type BOOL and is to be converted to the default type then we need the absolute value
			if (VT_BOOL == rValue.vt && GetDefaultType() != rValue.vt)
			{
				rValue.boolVal = abs(rValue.boolVal);
			}
			if (S_OK != ::VariantChangeTypeEx(&rValue, &rValue, SvDef::LCID_USA, VARIANT_ALPHABOOL, GetDefaultType()))
			{
				//empty value if variant can not be converted in the right type
				::VariantClear(&rValue);
			}
		}
	}

	return Result;
}




bool LinkedValue::DisconnectObjectInput(SvOl::SVInObjectInfoStruct* pObjectInInfo)
{
	if (nullptr != pObjectInInfo && pObjectInInfo->GetInputObjectInfo().getObject() == m_LinkedObjectRef.getObject())
	{
		DisconnectInput();
	}
	return __super::DisconnectObjectInput(pObjectInInfo);
}

void LinkedValue::UpdateLinkedName()
{
	if (nullptr != m_LinkedObjectRef.getObject())
	{
		m_LinkedName.SetValue(m_LinkedObjectRef.GetObjectNameToObjectType(SvPb::SVObjectTypeEnum::SVToolSetObjectType, true));
		m_LinkedName.SetObjectAttributesAllowed(SvDef::selectableAttributes | SvPb::printable, SvOi::SetAttributeType::OverwriteAttribute);
		SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	}
	else
	{
		std::string Value = m_LinkedName.GetDefaultValue();
		m_LinkedName.SetValue(Value);
		m_LinkedName.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
		SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::AddAttribute);
	}
}

void LinkedValue::setIndirectValueSaveFlag(bool shouldSaveValue)
{
	if (nullptr != m_LinkedObjectRef.getValueObject())
	{
		m_LinkedObjectRef.getValueObject()->setSaveValueFlag(shouldSaveValue);
	}
}

DWORD LinkedValue::GetType() const
{
	DWORD result;
	if (!m_CircularReference && nullptr != m_LinkedObjectRef.getValueObject())
	{
		m_CircularReference = true;
		result = m_LinkedObjectRef.getValueObject()->GetType();
		m_CircularReference = false;
	}
	else
	{
		result = __super::GetType();
	}
	return result;
};

bool LinkedValue::isCircularReference() const
{
	if (m_CircularReference)
	{
		return true;
	}
	LinkedValue* pRefObject = dynamic_cast<LinkedValue*>(m_LinkedObjectRef.getObject());
	if (nullptr == pRefObject)
	{
		return false;
	}
	m_CircularReference = true;
	bool result = pRefObject->isCircularReference();
	m_CircularReference = false;
	return result;
}

#pragma endregion Public Methods

#pragma region Protected Methods
_variant_t LinkedValue::ConvertString2Type(const std::string& rValue) const
{
	_variant_t Result;

	SVObjectReference LinkedObjectRef(rValue);

	//If string is not GUID then check if it is a dotted name
	if(nullptr == LinkedObjectRef.getObject())
	{
		LinkedObjectRef = ConvertStringInObject(rValue);
	}

	if (nullptr != LinkedObjectRef.getObject())
	{
		if (CheckLinkedObject(LinkedObjectRef.getObject()))
		{
			//Change the value directly as the default value may have a different variant type
			Result = _variant_t(LinkedObjectRef.GetGuidAndIndexOneBased().c_str());
		}
		else
		{
			//! This means the linked object is invalid
			SvDef::StringVector msgList;
			msgList.push_back(GetName());
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_LinkedValue_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			Exception.Throw();
		}
	}
	else
	{
		_variant_t vtTemp;
		vtTemp.SetString(rValue.c_str());

		if (GetDefaultValue().vt != VT_EMPTY)
		{
			if (S_OK != ::VariantChangeTypeEx(&vtTemp, &vtTemp, SvDef::LCID_USA, 0, GetDefaultValue().vt))
			{
				SvDef::StringVector msgList;
				msgList.push_back(GetName());
				SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
				Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_LinkedValue_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
				Exception.Throw();
			}
			Result = vtTemp;
		}
		else
		{
			Result = SVVariantValueObjectClass::ConvertString2Type(rValue);
		}
	}
	return Result;
}
#pragma endregion Protected Methods

#pragma region Private Methods
bool LinkedValue::UpdateConnection(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = true;
	bool ConvertDottedName(false);
	_variant_t Value;

	//! Here we need the non linked value (SVGUID as string or constant value)
	__super::GetValue(Value);

	std::string guidAndIndexString;
	try
	{
		guidAndIndexString = SvUl::createStdString(Value);
	}
	catch (...)
	{
		guidAndIndexString.clear();
	}


	SVObjectReference LinkedObjectRef(guidAndIndexString);

	//If valid GUID then should be able to get the linked value from the object manager
	if (nullptr != LinkedObjectRef.getObject())
	{
		Result = CheckLinkedObject(LinkedObjectRef.getObject(), pErrorMessages);
		if (!Result)
		{
			LinkedObjectRef = SVObjectReference();
		}
	}
	else
	{
		//Check if current value is a GUID, but not exist anymore. In this case it is probably an deleted object. Set this value to invalid.
		std::string::size_type Pos = guidAndIndexString.find_first_of(_T("["));
		std::string guidString = guidAndIndexString.substr(0, Pos);
		if (GUID_NULL != SVGUID(_bstr_t(guidString.c_str())))
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ConnectInputFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
				pErrorMessages->push_back(Msg);
			}
		}
		else
		{
			//this part is only for backward compatibility, because in older version the name was saved and not the GUID.
			std::string ToolSetName;
			std::string ObjectName = guidAndIndexString;

			ToolSetName = SvUl::LoadStdString(IDS_CLASSNAME_SVTOOLSET);

			//Default name
		
			//If the tool set name is at the start then add the inspection name at the beginning
			if (0 == ObjectName.find(ToolSetName))
			{
				SvOi::IObjectClass* pInspection = GetAncestorInterface(SvPb::SVInspectionObjectType);
				if (nullptr != pInspection)
				{
					ObjectName = pInspection->GetName();
					ObjectName += _T(".");
					ObjectName += guidAndIndexString;
		
				}
			}
			if (S_OK == SVObjectManagerClass::Instance().GetObjectByDottedName(ObjectName, LinkedObjectRef) && nullptr != LinkedObjectRef.getObject())
			{
				ConvertDottedName = true;
			}
		}
	}
	if (nullptr != LinkedObjectRef.getObject())	// input is another VO
	{
		//disconnect old object and connect new object if old and new different.
		if (LinkedObjectRef.getObject() != m_LinkedObjectRef.getObject())
		{
			//First disconnect and then set the new Linked object
			DisconnectInput();
			m_LinkedObjectRef = LinkedObjectRef;

			//Convert old dotted name format to Unique GUID
			if (ConvertDottedName)
			{
				SVVariantValueObjectClass::setValue(m_LinkedObjectRef.GetCompleteName(true));
			}
			bool messageReturn = ConnectInput();
			assert(messageReturn);
			if (!messageReturn)
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ConnectInputFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
					pErrorMessages->push_back(Msg);
				}
			}
		}
		else
		{
			m_LinkedObjectRef = LinkedObjectRef;
		}
	}
	else	// plain data
	{
		if (Result)
		{
			DisconnectInput();

			VARTYPE defaultType = GetDefaultType();
			if (VT_EMPTY != defaultType && Value.vt != defaultType)
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_WrongType, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
					pErrorMessages->push_back(Msg);
				}
			}
		}
	}
	if (Result)
	{
		UpdateLinkedName();
	}

	return Result;
}

void LinkedValue::DisconnectInput()
{
	if (nullptr != m_LinkedObjectRef.getObject())
	{
		SvOl::SVInObjectInfoStruct InputConnectionInfo;
		InputConnectionInfo.SetInputObject(m_LinkedObjectRef.getObject());
		InputConnectionInfo.SetObject(this);
		m_LinkedObjectRef.getObject()->DisconnectObjectInput(&InputConnectionInfo);
		m_LinkedObjectRef = SVObjectReference();
	}
}

bool LinkedValue::ConnectInput()
{
	bool Result = false;
	SvOl::SVInObjectInfoStruct InputConnectionInfo;
	InputConnectionInfo.SetInputObject(m_LinkedObjectRef.getObject());
	InputConnectionInfo.SetObject(this);
	if (nullptr != m_LinkedObjectRef.getObject())
	{
		Result = m_LinkedObjectRef.getObject()->ConnectObjectInput(&InputConnectionInfo);
	}
	return Result;
}

bool LinkedValue::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVVariantValueObjectClass::ResetObject(pErrorMessages);
	Result = Result && UpdateConnection(pErrorMessages);
	if (isCircularReference())
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CircularReference, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			pErrorMessages->push_back(Msg);
		}
	}
	return Result;
}

SVObjectReference LinkedValue::ConvertStringInObject(const std::string& rValue) const
{
	std::string ToolSetName;
	std::string ObjectName;

	ToolSetName = SvUl::LoadStdString(IDS_CLASSNAME_SVTOOLSET);

	//If the tool set name is at the start then add the inspection name at the beginning
	if (0 == rValue.find(ToolSetName.c_str()))
	{
		const SvOi::IObjectClass* pInspection = GetAncestorInterface(SvPb::SVInspectionObjectType);
		if (nullptr != pInspection)
		{
			ObjectName = pInspection->GetName();
			ObjectName += _T(".");
			ObjectName += rValue;
		}
	}
	else
	{
		ObjectName = rValue;
	}

	SVObjectReference objectRef;
	SVObjectManagerClass::Instance().GetObjectByDottedName(ObjectName, objectRef);
	return objectRef;
}

bool LinkedValue::CheckLinkedObject(const SVObjectClass* const pLinkedObject, SvStl::MessageContainerVector *pErrorMessages) const
{
	bool Result(true);

	//! Check if a valid object and make sure it does not link to itself
	if (nullptr == pLinkedObject || this == pLinkedObject)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidLinkedObjectOrSame, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			pErrorMessages->push_back(Msg);
		}
	}
	else
	{
		//! This is important when copying tools that the value of another inspection is not used due to the GUID being valid
		//! That is why check that the linked value of an object is in the same inspection
		const IObjectClass* pLinkedObjectInspection = pLinkedObject->GetAncestorInterface(SvPb::SVInspectionObjectType);
		bool isSameInpection = GetAncestorInterface(SvPb::SVInspectionObjectType) == pLinkedObjectInspection;
		//! If linked object has no inspection (e.g. Global Constants) then we don't need to check that the inspections are the same
		if (nullptr != pLinkedObjectInspection && !isSameInpection)
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_WrongInspection, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
				pErrorMessages->push_back(Msg);
			}
		}
	}

	return Result;
}

const SVObjectClass* LinkedValue::GetLinkedObject() const
{
	return m_LinkedObjectRef.getObject(); 
} 
#pragma endregion Private Methods

} //namespace SvVol

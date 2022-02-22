//*****************************************************************************
/// \copyright (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "ObjectInterfaces/ITableObject.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary/SafeArrayHelper.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVProtoBuf/ConverterHelper.h"
#pragma endregion Includes

namespace SvVol
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

	//RAII class to set and reset flag
	struct LifeFlag
	{
		explicit LifeFlag(bool& flag) :
			m_rFlag(flag)
		{
			m_rFlag = true;
		}
		~LifeFlag()
		{
			m_rFlag = false;
		}
	private:
		bool& m_rFlag;
	};


SV_IMPLEMENT_CLASS(LinkedValue, SvPb::LinkedValueClassId);

#pragma region Constructor
	LinkedValue::LinkedValue() :
		m_CircularReference(false)
	{
		const LPCSTR g_strTypeEnums = _T("None=0,Value=1,Link=2,Formula=3");
		m_TypeValue.SetEnumTypes(g_strTypeEnums);

		RegisterEmbeddedObject(&m_Content, SvPb::LinkedValueContentEId, IDS_OBJECTNAME_LINKEDVALUE_CONTENT, false, SvOi::SVResetItemNone);
		RegisterEmbeddedObject(&m_TypeValue, SvPb::LinkedValueTypeEId, IDS_OBJECTNAME_LINKEDVALUE_TYPE, false, SvOi::SVResetItemNone);

		m_Content.SetDefaultValue("", true);
		m_TypeValue.SetDefaultValue(static_cast<long>(SvPb::LinkedSelectedType::None), true);
	}

	LinkedValue::~LinkedValue()
	{
		CloseObject();
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
		HRESULT Result{ S_OK };
		//! When getting the value from an indirect value make sure it is not referencing this object
		if (nullptr != m_LinkedObjectRef.getValueObject())
		{
			LifeFlag circularCheck(m_CircularReference);
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
		}
		else
		{
			Result = __super::getValue(rValue, -1);
		}
		if (rValue.vt == VT_EMPTY)
		{
			rValue = SvUl::GetEmptySafeArray(GetDefaultValue().vt | VT_ARRAY);

		}
		return Result;
	}

	HRESULT LinkedValue::SetDefaultValue(const _variant_t& rValue, bool bResetAll)
	{
		HRESULT hres(E_FAIL);
		if (rValue.vt != m_directValue.vt)
		{
			m_directValue.Clear();
		}

		if (0 == (rValue.vt & VT_ARRAY))
		{
			SetArraySize(1);
			hres = __super::SetDefaultValue(rValue, bResetAll);
		}
		else
		{
			hres = __super::SetDefaultValue(rValue, false);

			if (bResetAll)
			{
				long lBound(0), uBound(0);
				SafeArrayGetUBound(const_cast<SAFEARRAY*>(rValue.parray), 1, &uBound);
				SafeArrayGetLBound(const_cast<SAFEARRAY*>(rValue.parray), 1, &lBound);
				int32_t cnt_elements = uBound - lBound + 1;
				if (getArraySize() != cnt_elements)
				{
					SetArraySize(cnt_elements);
				}
				hres = __super::setValue(rValue);

			}
		}
		if (bResetAll)
		{
			m_directValue = rValue;
		}

		///If the memory block size is wrong  then we need to reset the TR data
		if (-1 != getMemOffset() && getMemSizeReserved() != getByteSize(false, false))
		{
			setTrData(-1L, -1L, -1L);
		}
		return hres;
	}
	void  LinkedValue::SetAllowVoidReference(bool set)
	{
		m_allowVoidReference = set;
	}
	// cppcheck-suppress unusedFunction
	bool  LinkedValue::GetAllowVoidReference() const
	{
		return m_allowVoidReference;

	}


	HRESULT LinkedValue::SetValueKeepType(LPCTSTR Value, int Index)
	{
		if (SvPb::LinkedSelectedType::DirectValue == getSelectedType())
		{
			HRESULT hr = __super::SetValueKeepType(Value, Index);
			if (S_OK == hr || SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hr)
			{
				getValue(m_directValue);
			}
			UpdateContent();
			return hr;
		}
		else
		{
			return E_FAIL;
		}
	}

	HRESULT LinkedValue::setValue(const SvPb::LinkedValue& rData)
	{
		setSelectedType(rData.type());
		SvPb::ConvertProtobufToVariant(rData.directvalue(), m_directValue);
		m_indirectValueRef = SVObjectReference{rData.indirectidstring()};
		m_formulaString = rData.formula();
		SvStl::MessageContainerVector errorMessages;
		bool isOk = UpdateConnection(&errorMessages);
		if (false == isOk)
		{
			if (errorMessages.size())
			{
				throw errorMessages[0];
			}
			return E_FAIL;
		}
		//UpdateConnnection can delete indirectValue, if it was set before and now was switched to another type
		m_indirectValueRef = SVObjectReference {rData.indirectidstring()};
		return S_OK;
	}

	HRESULT LinkedValue::setValue(const _variant_t& rValue, int Index /*= -1*/, bool fixArrasize /*= false*/)
	{
		if (SvPb::LinkedSelectedType::DirectValue == getSelectedType())
		{
			if (false == SvUl::isSameVar(rValue, m_directValue))
			{
				HRESULT hr = __super::setValue(rValue, Index, fixArrasize);
				if (S_OK == hr || SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hr)
				{
					getValue(m_directValue);
				}
				UpdateContent();
				return hr;
			}
			return S_OK;
		}
		else
		{
			assert(false);
			return E_FAIL;
		}
	}

	bool LinkedValue::setDirectValue(const _variant_t& rValue)
	{
		setSelectedType(SvPb::LinkedSelectedType::DirectValue);
		m_directValue = rValue;
		bool isOk = UpdateConnection();
		assert(isOk);
		return isOk;
	}

	bool LinkedValue::setIndirectValue(const std::string& rValueString)
	{
		SVObjectReference ref = GetObjectReferenceForDottedName(rValueString);
		return setIndirectValue(ref);
	}

	bool LinkedValue::setIndirectValue(const SVObjectReference& rReference)
	{
		m_indirectValueRef = rReference;
		setSelectedType(SvPb::LinkedSelectedType::IndirectValue);
		bool isOk = UpdateConnection();
		assert(isOk);
		return isOk;
	}

	void LinkedValue::updateMemBlockData()
	{
		///When the linked value is an indirect value we need to always update it
		if (SvPb::LinkedSelectedType::DirectValue != getSelectedType())
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
		HRESULT Result(ValidateIndex(Index));

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
				LifeFlag circularCheck(m_CircularReference);
				if (m_LinkedObjectRef.getIndex() == Index || -1 != m_LinkedObjectRef.getIndex())
				{
					Result = m_LinkedObjectRef.getValue(rValue);
				}
				else
				{
					auto refObject = m_LinkedObjectRef;
					refObject.SetArrayIndex(Index);
					Result = refObject.getValue(rValue);
				}
			}
			else
			{
				Result = __super::GetValue(rValue, Index);
			}

			if (S_OK == Result || SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == Result || E_BOUNDS == Result && VT_EMPTY != rValue.vt)
			{
				VARTYPE vtType = GetDefaultType();
				bool returnSingleValue = (0 <= Index || 0 == (vtType & VT_ARRAY));
				if (returnSingleValue)
				{
					//If the Linked Value is of type BOOL and is to be converted to the default type then we need the absolute value
					if (VT_BOOL == rValue.vt && GetDefaultType() != rValue.vt)
					{
						rValue.boolVal = rValue.boolVal ? 1 : 0;
					}

					if (0 != (vtType & VT_ARRAY))
					{
						vtType = vtType & ~VT_ARRAY;
					}
					if (S_OK != ::VariantChangeTypeEx(&rValue, &rValue, SvDef::LCID_USA, VARIANT_ALPHABOOL, vtType))
					{
						//empty value if variant can not be converted in the right type
						::VariantClear(&rValue);
					}
				}
				else
				{
					if (0 != (rValue.vt & VT_ARRAY))
					{
						rValue = SvUl::convertSafeArrayToOtherSafeArray(rValue, vtType);
					}
					else
					{
						if (S_OK != ::VariantChangeTypeEx(&rValue, &rValue, SvDef::LCID_USA, VARIANT_ALPHABOOL, vtType & ~VT_ARRAY))
						{
							//empty value if variant can not be converted in the right type
							::VariantClear(&rValue);
						}
						rValue = SvUl::VariantToSafeArray(rValue);
					}
				}
			}
		}

		return Result;
	}

	void LinkedValue::disconnectObjectInput(uint32_t objectId)
	{
		if (m_LinkedObjectRef.getObjectId() == objectId)
		{
			DisconnectInput();
			__super::disconnectObjectInput(objectId);
			if (objectId == m_indirectValueRef.getObjectId() && is_Created())
			{
				m_indirectValueRef.clear();
			}
		}
	}

	void LinkedValue::UpdateContent()
	{
		switch (getSelectedType())
		{
		case SvPb::LinkedSelectedType::DirectValue:
			m_Content.SetValue(ConvertType2String(m_directValue));
			break;
		case SvPb::LinkedSelectedType::IndirectValue:
			if (nullptr != m_LinkedObjectRef.getObject())
			{
				m_Content.SetValue(m_LinkedObjectRef.GetObjectNameToObjectType(SvPb::SVObjectTypeEnum::SVToolSetObjectType, true));
			}
			else
			{
				m_Content.SetValue("");
			}
			break;
		case SvPb::LinkedSelectedType::Formula:
			m_Content.SetValue(m_formulaString);
			break;
		default:
			break;
		}
	}

	void LinkedValue::setIndirectValueSaveFlag(bool shouldSaveValue)
	{
		if (false == m_CircularReference && nullptr != m_LinkedObjectRef.getValueObject())
		{
			LifeFlag circularCheck(m_CircularReference);
			m_LinkedObjectRef.getValueObject()->setSaveValueFlag(shouldSaveValue);
		}
	}

	DWORD LinkedValue::GetType() const
	{
		return GetDefaultType();
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
		LifeFlag circularCheck(m_CircularReference);
		bool result = pRefObject->isCircularReference();
		return result;
	}

	bool LinkedValue::isCorrectType(SvPb::ObjectSelectorType requiredType) const
	{
		try
		{
			if (VT_EMPTY != GetDefaultType())
			{
				return __super::isCorrectType(requiredType);
			}
			else if (const auto* pValue = dynamic_cast<const SVObjectClass*>(getLinkedObject()) ; nullptr != pValue)
			{
				switch (requiredType)
				{
					case SvPb::allValueObjects:
					case SvPb::allNumberValueObjects:
					case SvPb::realNumberValueOjects:
					case SvPb::stringValueObjects:
						return false; //by ValueObjects the defaultType must not be VT_EMPTY
					default:
						return pValue->isCorrectType(requiredType);
				}
			}
			return false;
		}
		catch (...)
		{
			return false;
		}
	}

	const SvOi::IObjectClass* LinkedValue::getLinkedObject() const
	{
		if (m_CircularReference)
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_CircularReference, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			Exception.Throw();
		}
		const SvOi::IObjectClass* pObject = m_LinkedObjectRef.getObject();
		const LinkedValue* pRefObject = dynamic_cast<const LinkedValue*>(pObject);
		if (nullptr == pRefObject)
		{
			return pObject;
		}

		LifeFlag circularCheck(m_CircularReference);
		const SvOi::IObjectClass* pLastObject = pRefObject->getLinkedObject();
		return nullptr != pLastObject ? pLastObject : pObject;
	}

	void LinkedValue::fillLinkedData(SvPb::LinkedValue& rLinkedValue) const
	{
		rLinkedValue.set_type(getSelectedType());
		_variant_t varValue;
		if (SvPb::LinkedSelectedType::IndirectValue != rLinkedValue.type() || nullptr != m_indirectValueRef.getFinalObject())
		{
			HRESULT result = getValue(varValue);
			if (S_OK != result)
			{
				varValue.Clear();
			}
		}

		ConvertVariantToProtobuf(varValue, rLinkedValue.mutable_value());
		ConvertVariantToProtobuf(GetDefaultValue(), rLinkedValue.mutable_defaultvalue());
		ConvertVariantToProtobuf(m_directValue, rLinkedValue.mutable_directvalue());
		rLinkedValue.set_indirectidstring(m_indirectValueRef.GetObjectIdAndIndexOneBased().c_str());
		rLinkedValue.set_formula(m_formulaString.c_str());
		rLinkedValue.set_equationid(m_equation.getObjectId());
	}

	_variant_t LinkedValue::validateValue(const SvPb::LinkedValue& rLinkedValue) const
	{
		variant_t value;
		variant_t defaultValue;
		SvPb::ConvertProtobufToVariant(rLinkedValue.defaultvalue(), defaultValue);
		switch (rLinkedValue.type())
		{
		case SvPb::LinkedSelectedType::DirectValue:
		{
			SvPb::ConvertProtobufToVariant(rLinkedValue.directvalue(), value);
			__super::validateValue(value, defaultValue);
			break;
		}
		case SvPb::LinkedSelectedType::IndirectValue:
		{
			value = validateIndirectValue(rLinkedValue.indirectidstring(), defaultValue);
			break;
		}
		case SvPb::LinkedSelectedType::Formula:
		{
			value = validateFormula(rLinkedValue.formula());
			break;
		}
		default:
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValidateValue_LinkedTypeInvalid, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10029_ValueObject_Parameter_WrongSize, getObjectId());
			Exception.Throw();
			break;
		}

		if (VT_EMPTY != defaultValue.vt && value.vt != defaultValue.vt)
		{
			if (0 == (value.vt & VT_ARRAY))
			{
				if (VT_BOOL == value.vt)
				{
					value.boolVal = value.boolVal ? 1 : 0;
				}
				if (VT_BSTR == value.vt && 0 != (defaultValue.vt & VT_BSTR))
				{
					SvStl::MessageManager Exception(SvStl::MsgType::Log);
					Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValidateValue_LinkedTypeInvalid, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10029_ValueObject_Parameter_WrongSize, getObjectId());
					Exception.Throw();
				}
				if (S_OK != ::VariantChangeTypeEx(&value, &value, SvDef::LCID_USA, VARIANT_ALPHABOOL, ~(VT_ARRAY)&defaultValue.vt))
				{
					SvStl::MessageManager Exception(SvStl::MsgType::Log);
					Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValidateValue_LinkedTypeInvalid, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10029_ValueObject_Parameter_WrongSize, getObjectId());
					Exception.Throw();
				}
				if (0 != (defaultValue.vt & VT_ARRAY))
				{
					value = SvUl::VariantToSafeArray(value);
				}
			}
			else
			{
				if (0 == (defaultValue.vt & VT_ARRAY))
				{
					SvStl::MessageManager Exception(SvStl::MsgType::Log);
					Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValidateValue_LinkedTypeInvalid, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10029_ValueObject_Parameter_WrongSize, getObjectId());
					Exception.Throw();
				}
				else
				{
					value = SvUl::convertSafeArrayToOtherSafeArray(value, defaultValue.vt);
				}
			}
		}
		return value;
	}

	void LinkedValue::fillObjectList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter, const SvDef::SVObjectTypeInfoStruct& rObjectInfo, bool addHidden /*= false*/, bool stopIfClosed /*= false*/, bool /*firstObject = false*/)
	{
		if (SvPb::NoEmbeddedId == rObjectInfo.m_EmbeddedID)
		{
			try
			{
				const auto* pObject = getLinkedObject();
				if (nullptr != pObject &&
					(SvPb::SVNotSetObjectType == rObjectInfo.m_ObjectType || rObjectInfo.m_ObjectType == pObject->GetObjectType()) &&
					(SvPb::SVNotSetSubObjectType == rObjectInfo.m_SubType || rObjectInfo.m_SubType == pObject->GetObjectSubType()))
				{
					inserter = this;
					return;
				}
			}
			catch (...)
			{
				return;
			}
		}
		
		__super::fillObjectList(inserter, rObjectInfo, addHidden, stopIfClosed);
	}

	void LinkedValue::OnObjectRenamed(const SVObjectClass&, const std::string&)
	{ 
		if (0 < ObjectAttributesAllowed())
		{
			UpdateContent();
		}
	};

	bool LinkedValue::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
	{
		bool ret = __super::CreateObject(rCreateStructure);
		long lValue = 0;
		m_TypeValue.GetValue(lValue);
		m_type = static_cast<SvPb::LinkedSelectedType>(lValue);

		m_equation.CreateObject(rCreateStructure);
		if (nullptr == m_indirectValueRef.getObject() && SvDef::InvalidObjectId != m_indirectValueRef.getObjectId())
		{
			m_indirectValueRef.update();
		}
		UpdateConnection();
		return ret;
	}

	bool LinkedValue::CloseObject()
	{
		for (auto& rLinked : m_children)
		{
			if (rLinked)
			{
				rLinked->CloseObject();
			}
		}

		m_equation.CloseObject();
		return __super::CloseObject();
	}
#pragma endregion Public Methods

#pragma region Protected Methods
	_variant_t LinkedValue::ConvertString2Type(const std::string& rValue) const
	{
		return ConvertString2Type(rValue, GetDefaultValue());
	}

	_variant_t LinkedValue::ConvertString2Type(const std::string& rValue, const _variant_t& rDefaultValue) const
	{
		_variant_t Result;

		SVObjectReference LinkedObjectRef(rValue);

		//If string is not object ID then check if it is a dotted name
		if (nullptr == LinkedObjectRef.getObject())
		{
			LinkedObjectRef = GetObjectReferenceForDottedName(rValue);
		}

		if (nullptr != LinkedObjectRef.getObject())
		{
			SvStl::MessageContainerVector errorMessages;
			if (CheckLinkedObject(LinkedObjectRef.getObject(), rDefaultValue, &errorMessages))
			{
				//Change the value directly as the default value may have a different variant type
				Result = _variant_t(LinkedObjectRef.GetObjectIdAndIndexOneBased().c_str());
			}
			else
			{
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				if (0 < errorMessages.size())
				{
					Exception.setMessage(errorMessages[0].getMessage(), getObjectId());
				}
				else
				{
					//! This means the linked object is invalid
					SvDef::StringVector msgList;
					msgList.push_back(GetName());
					Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_LinkedValue_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				}
				Exception.Throw();
			}
		}
		else
		{
			_variant_t vtTemp;
			vtTemp.SetString(rValue.c_str());

			if (rDefaultValue.vt != VT_EMPTY && VT_BSTR != rDefaultValue.vt)
			{
				if (S_OK != ::VariantChangeTypeEx(&vtTemp, &vtTemp, SvDef::LCID_USA, 0, rDefaultValue.vt))
				{
					SvDef::StringVector msgList;
					msgList.push_back(GetName());
					SvStl::MessageManager Exception(SvStl::MsgType::Log);
					Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_LinkedValue_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
					Exception.Throw();
				}
			}
			Result = vtTemp;
		}
		return Result;
	}

	void LinkedValue::disconnectAllInputs()
	{ 
		DisconnectInput(); 
		__super::disconnectAllInputs();
	}

	void LinkedValue::getOutputList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter) const
	{
		__super::getOutputList(inserter);

		for (auto& rUniqueObject : m_children)
		{
			if (nullptr != rUniqueObject)
			{
				inserter = rUniqueObject.get();
			}
		}
	}

	void LinkedValue::fillSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, SvOi::IsObjectAllowedFunc pFunctor, UINT attribute, bool wholeArray, SvPb::SVObjectTypeEnum nameToType, SvPb::ObjectSelectorType requiredType, bool stopIfClosed, bool firstObject) const
	{
		if (SvPb::noAttributes == attribute || SvPb::noAttributes != ObjectAttributesAllowed())
		{
			__super::fillSelectorList(treeInserter, pFunctor, attribute, wholeArray, nameToType, requiredType, stopIfClosed, firstObject);
			for (auto& rChild : m_children)
			{
				if (nullptr != rChild)
				{
					if (rChild->isCorrectType(requiredType))
					{
						SVObjectReference ObjectRef {rChild->getObjectId()};
						ObjectRef.fillSelectorList(treeInserter, wholeArray, pFunctor, attribute, nameToType);
					}
				}
			}
		}
	}
#pragma endregion Protected Methods

#pragma region Private Methods
	bool LinkedValue::UpdateConnection(SvStl::MessageContainerVector *pErrorMessages)
	{
		bool Result = true;
		switch (getSelectedType())
		{
		case SvPb::LinkedSelectedType::None:
			m_children.clear();
			m_childrenIds.clear();
			Result = updateFromOldStruct(pErrorMessages);
			break;
		case SvPb::LinkedSelectedType::DirectValue:
		{
			m_children.clear();
			m_childrenIds.clear();

			variant_t value;
			getValue(value);
			if (nullptr != m_LinkedObjectRef.getObject() || false == SvUl::isSameVar(value, m_directValue))
			{
				DisconnectInput();
				__super::setValue(m_directValue);
			}
			break;
		}
		case SvPb::LinkedSelectedType::IndirectValue:
		{
			Result = updateLinkedValue(m_indirectValueRef, pErrorMessages) && Result;
			break;
		}
		case SvPb::LinkedSelectedType::Formula:
		{
			m_children.clear();
			m_childrenIds.clear();
			m_equation.SetEquationText(m_formulaString);
			SvOi::EquationTestResult testResult = m_equation.Test(pErrorMessages);
			Result = testResult.bPassed;
			if (Result)
			{
				setValueFromDouble(m_equation.GetYACCResult());
			}
			DisconnectInput();
			break;
		}
		default:
			m_children.clear();
			m_childrenIds.clear();
			assert(false);
			break;
		}

		if (Result)
		{
			UpdateContent();
		}

		return Result;
	}

	bool LinkedValue::updateFromOldStruct(SvStl::MessageContainerVector* pErrorMessages)
	{
		bool Result{ true };

		std::string objectIdAndIndexString;
		auto* pValueArray = valuePtr();
		assert(nullptr != pValueArray);
		if (nullptr != pValueArray)
		{	//Here we look if the direct value is a valid objectId to find out if this object is linked to another value.
			objectIdAndIndexString = SvUl::createStdString(pValueArray[0]);
		}

		SVObjectReference LinkedObjectRef(objectIdAndIndexString);

		//If valid objectId then should be able to get the linked value from the object manager
		if (nullptr != LinkedObjectRef.getObject())
		{
			Result = CheckLinkedObject(LinkedObjectRef.getObject(), GetDefaultValue(), pErrorMessages);
			if (!Result)
			{
				LinkedObjectRef = SVObjectReference();
			}
		}
		else
		{
			if (false == m_oldIndirectString.empty())
			{
				LinkedObjectRef = GetObjectReferenceForDottedName(m_oldIndirectString);
			}
			else
			{
				//Check if current value is a objectId, but not exist anymore. In this case it is probably an deleted object. Set this value to invalid.
				std::string::size_type Pos = objectIdAndIndexString.find_first_of(_T("["));
				std::string idString = objectIdAndIndexString.substr(0, Pos);
				if (SvDef::InvalidObjectId != calcObjectId(idString))
				{
					Result = false;
					if (nullptr != pErrorMessages)
					{
						SvDef::StringVector msgList;
						std::string objectName = GetObjectNameBeforeObjectType(SvPb::SVObjectTypeEnum::SVToolObjectType);
						msgList.push_back(objectName);

						SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ConnectInputFailedLinkedValueNotFound, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
						pErrorMessages->push_back(Msg);
					}
				}
				else
				{
					//this part is only for backward compatibility, because in older version the name was saved and not the ID.
					std::string ObjectName = objectIdAndIndexString;

					//Default name

					//If the tool set name is at the start then add the inspection name at the beginning
					if (ObjectName._Starts_with(SvUl::LoadedStrings::g_ToolSetName))
					{
						SvOi::IObjectClass* pInspection = GetAncestorInterface(SvPb::SVInspectionObjectType);
						if (nullptr != pInspection)
						{
							ObjectName = pInspection->GetName();
							ObjectName += _T(".");
							ObjectName += objectIdAndIndexString;

						}
					}
					SVObjectManagerClass::Instance().GetObjectByDottedName(ObjectName, LinkedObjectRef);
				}
			}
		}
		if (nullptr != LinkedObjectRef.getObject())	// input is another VO
		{
			setSelectedType(SvPb::LinkedSelectedType::IndirectValue);
			if (VT_EMPTY == m_directValue.vt)
			{
				m_directValue = GetDefaultValue();
			}
			Result = updateLinkedValue(LinkedObjectRef, pErrorMessages) && Result;
			m_indirectValueRef = m_LinkedObjectRef;
		}
		else	// plain data
		{
			setSelectedType(SvPb::LinkedSelectedType::DirectValue);
			assert(Result);
			if (Result)
			{
				DisconnectInput();

				_variant_t value;
				__super::GetValue(value);
				if (isArray())
				{
					value = SvUl::VariantToSafeArray(value);
				}
				setDefaultDefaultIfEmpty(value);
				VARTYPE defaultType = GetDefaultType();

				if (VT_EMPTY != defaultType && value.vt != defaultType &&  ( ~VT_ARRAY & value.vt) != defaultType)
				{
					Result = false;
					if (nullptr != pErrorMessages)
					{
						SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_WrongType, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
						pErrorMessages->push_back(Msg);
					}
				}
				getValue(m_directValue);
			}
			else
			{
				m_directValue = GetDefaultValue();
			}			
		}

		return Result;
	}

	void LinkedValue::setDefaultDefaultIfEmpty(_variant_t& value)
	{
		VARTYPE defaultType = GetDefaultType();
		if (VT_EMPTY == defaultType)
		{
			if (0 != (value.vt & VT_ARRAY))
			{
				setDefaultValue(SvUl::GetEmptySafeArray(value.vt));
			}
			else
			{
				_variant_t defaultValue {0};
				if (VT_BSTR == value.vt)
				{
					defaultValue = "";
				}
				else
				{
					::VariantChangeTypeEx(&defaultValue, &defaultValue, SvDef::LCID_USA, VARIANT_ALPHABOOL, value.vt);
				}
				setDefaultValue(defaultValue);
			}
		}
	}

	bool LinkedValue::updateLinkedValue(SVObjectReference& LinkedObjectRef, SvStl::MessageContainerVector* pErrorMessages)
	{
		bool Result{ true };

		//disconnect old object and connect new object if old and new different.
		if (LinkedObjectRef.getObject() != m_LinkedObjectRef.getObject())
		{
			//First disconnect and then set the new Linked object
			DisconnectInput();
			m_LinkedObjectRef = LinkedObjectRef;
			if (nullptr != m_LinkedObjectRef.getObject()
				&& m_checkForValidDependency) // children of a linkedValue (for tableObject) should not be connected because their parent already connected.
			{
				m_LinkedObjectRef.getObject()->connectObject(getObjectId());
			}
		}
		else
		{
			m_LinkedObjectRef = LinkedObjectRef;
		}

		if (nullptr == m_LinkedObjectRef.getObject())
		{
			if (m_allowVoidReference)
			{
				return true;
			}
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_EmptyObjectForIndirectValue, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				pErrorMessages->push_back(Msg);
			}
			return false;
		}
		else if (0 == m_LinkedObjectRef.getObject()->ObjectAttributesAllowed())
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidObjectForIndirectValue, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				pErrorMessages->push_back(Msg);
			}
			return false;
		}

		auto* pLinkedObject = m_LinkedObjectRef.getFinalObject();
		if (nullptr != pLinkedObject)
		{
			switch (pLinkedObject->GetObjectType())
			{
				case SvPb::TableObjectType:
				{ //Add children-LinkedValues for TableObject
					const auto& tableValues = dynamic_cast<SvOi::ITableObject*>(pLinkedObject)->getValueList();
					m_children.resize(tableValues.size());
					SVObjectLevelCreateStruct createStruct(*this);
					for (int i = 0; i < tableValues.size(); ++i)
					{
						Result = resetChild(i, tableValues[i].get(), pErrorMessages, createStruct) && Result;
					}
					break;
				}
				case SvPb::SVToolObjectType:
				{ //Add children-LinkedValues for states of Tools
					Result = resetChildren(pLinkedObject, {SvPb::ToolEnabledEId, SvPb::PassedEId, SvPb::FailedEId, SvPb::WarnedEId, SvPb::PassedCountEId, SvPb::FailedCountEId, SvPb::WarnedCountEId, SvPb::EnabledCountEId}, pErrorMessages);
					break;
				}
				case SvPb::SVImageObjectType:
				{
					Result = resetChildren(pLinkedObject, {SvPb::ExtentWidthEId, SvPb::ExtentHeightEId}, pErrorMessages);
					break;
				}
				default:
					m_children.clear();
					m_childrenIds.clear();
					if (0 != (GetDefaultType() & VT_ARRAY))
					{
						if (auto* pValue = dynamic_cast<SvOi::IValueObject*>(pLinkedObject) ; nullptr != pValue)
						{
							SetArraySize(pValue->getArraySize());
						}
					}
					if (auto* pValue = dynamic_cast<SvOi::IValueObject*>(m_LinkedObjectRef.getObject()); nullptr != pValue && VT_BSTR == pValue->getDefaultValue().vt && 0 == (GetDefaultType() & VT_BSTR))
					{
						if (nullptr != pErrorMessages)
						{
							SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ValidateValue_LinkedTypeInvalid, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
							pErrorMessages->push_back(Msg);
						}
						return false;
					}
					break;
			}

			//! This is important when copying tools that the value of another inspection is not used due to the object ID being valid
			//! That is why check that the linked value of an object is in the same inspection
			const IObjectClass* pLinkedObjectInspection = pLinkedObject->GetAncestorInterface(SvPb::SVInspectionObjectType);
			bool isSameInpection = GetAncestorInterface(SvPb::SVInspectionObjectType) == pLinkedObjectInspection;
			//! If linked object has no inspection (e.g. Global Constants) then we don't need to check that the inspections are the same
			if (nullptr != pLinkedObjectInspection && !isSameInpection)
			{
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_WrongInspection, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
					pErrorMessages->push_back(Msg);
				}
				return false;
			}

			if (m_checkForValidDependency && false == checkIfValidDependency(m_LinkedObjectRef.getObject()))
			{
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_GroupDependencies_Wrong, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
					pErrorMessages->push_back(Msg);
				}
				return false;
			}
		}			
		return Result;
	}

	void LinkedValue::DisconnectInput()
	{
		if (nullptr != m_LinkedObjectRef.getObject())
		{
			m_LinkedObjectRef.getObject()->disconnectObject(getObjectId());
			m_LinkedObjectRef = SVObjectReference();
		}
		for (auto& rChild : m_children)
		{
			if (nullptr != rChild)
			{
				rChild->DisconnectInput();
			}
		}
	}

	UINT LinkedValue::ObjectAttributesSet(int iIndex) const
	{
		if (getObjectAttributesSetSize() <= iIndex)
		{
			assert(false);
			iIndex = 0;
		}

		return __super::ObjectAttributesSet(iIndex);
	}

	HRESULT LinkedValue::GetChildObject(SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index/* = 0*/) const
	{
		if (static_cast<const size_t> (Index) < rNameInfo.m_NameArray.size() && rNameInfo.m_NameArray[Index] == GetName())
		{
			HRESULT l_Status = __super::GetChildObject(rpObject, rNameInfo, Index);

			if (S_OK != l_Status)
			{
				for (const auto& pChild : m_children)
				{
					if (nullptr != pChild)
					{
						l_Status = pChild->GetChildObject(rpObject, rNameInfo, Index + 1);
						if (S_OK == l_Status && nullptr != rpObject)
						{
							return l_Status;
						}
					}
				}
			}
			return l_Status;
		}
		return S_FALSE;
	}

	bool LinkedValue::isArray() const
	{
		bool result{ false };

		if (nullptr != m_LinkedObjectRef.getObject())
		{
			if (false == m_CircularReference)
			{
				LifeFlag circularCheck(m_CircularReference);
				//if Reference to an index then this linkedValue is not an array
				if (0 > m_LinkedObjectRef.ArrayIndex())
				{
					result = m_LinkedObjectRef.isArray();
				}
			}
		}
		else
		{
			result = __super::isArray();
		}
		return result;
	}

	HRESULT LinkedValue::getValue(double& rValue, int Index/* = -1*/) const 
	{ 
		if (0 != (GetDefaultType() & VT_BSTR))
		{
			return E_FAIL;
		}
		return __super::getValue(rValue, Index); 
	};

	HRESULT LinkedValue::getValue(std::string& rValueString, int Index, const std::string& rFormatString) const
	{
		auto* pValue = dynamic_cast<SvOi::IValueObject*>(m_LinkedObjectRef.getFinalObject());
		if (nullptr != pValue)
		{
			if (0 <= m_LinkedObjectRef.ArrayIndex())
			{
				Index = m_LinkedObjectRef.ArrayIndex();
			}
			return pValue->getValue(rValueString, Index, rFormatString);
		}
		else
		{
			return __super::getValue(rValueString, Index, rFormatString);
		}
	}

	int32_t LinkedValue::getArraySize() const
	{
		int32_t result{ 1L };

		if (nullptr != m_LinkedObjectRef.getObject())
		{
			if (false == m_CircularReference && nullptr != m_LinkedObjectRef.getValueObject())
			{
				LifeFlag circularCheck(m_CircularReference);
				//if Reference to an index then this linkedValue is not an array
				if (0 > m_LinkedObjectRef.ArrayIndex())
				{
					result = m_LinkedObjectRef.getValueObject()->getArraySize();
				}
			}
		}
		else
		{
			result =  __super::getArraySize();
		}
		return result;
	}

	int32_t LinkedValue::getResultSize() const
	{
		int32_t result{ 1L };

		if (nullptr != m_LinkedObjectRef.getObject())
		{
			if (false == m_CircularReference && nullptr != m_LinkedObjectRef.getValueObject())
			{
				LifeFlag circularCheck(m_CircularReference);
				//if Reference to an index then this linkedValue is not an array
				if (0 > m_LinkedObjectRef.ArrayIndex())
				{
					result = m_LinkedObjectRef.getValueObject()->getResultSize();
				}
			}
		}
		else
		{
			result = __super::getResultSize();
		}
		return result;
	}

	std::string LinkedValue::getContentStr() const
	{
		std::string value;
		m_Content.getValue(value);
		return value;
	}

	void LinkedValue::Persist(SvOi::IObjectWriter& rWriter) const
	{
		if (SvPb::noAttributes == ObjectAttributesAllowed() && false == isUsed())
		{
			return;
		}

		setSaveFutherDataFlag();
		__super::Persist(rWriter);
		if (m_children.size())
		{
			rWriter.StartElement(scLinkedChildsTag);
			// Get embedded object script...
			for (const auto& pObject : m_children)
			{
				if (nullptr != pObject)
				{
					rWriter.StartElement(pObject->GetObjectName());
					pObject->PersistBaseData(rWriter);
					rWriter.EndElement();
				}
			}
			rWriter.EndElement();
		}

		rWriter.WriteAttribute(scLinkedDirectValueTag, m_directValue);
		_variant_t Value(0);
		Value = m_indirectValueRef.GetObjectIdAndIndexOneBased().c_str();
		rWriter.WriteAttribute(scLinkedIndirectValueTag, Value);
		std::string Temp = m_formulaString;
		SvUl::AddEscapeSpecialCharacters(Temp, true);
		Value = Temp.c_str();
		rWriter.WriteAttribute(scLinkedFormulaTag, Value);

		rWriter.EndElement();
	}

	HRESULT LinkedValue::SetObjectValue(SVObjectAttributeClass* pDataObject)
	{
		SvCl::SVObjectStdStringArrayClass stringList;
		if (std::string(pDataObject->GetName()) == scLinkedDirectValueTag)
		{
			m_directValue = pDataObject->getData();
			setDefaultDefaultIfEmpty(m_directValue);
			if (VT_BOOL == m_directValue.vt && GetDefaultType() != m_directValue.vt)
			{
				m_directValue.boolVal = m_directValue.boolVal ? 1 : 0;
			}
			if (GetDefaultType() != m_directValue.vt && S_OK != ::VariantChangeTypeEx(&m_directValue, &m_directValue, SvDef::LCID_USA, VARIANT_ALPHABOOL, GetDefaultType()))
			{
				//empty value if variant can not be converted in the right type
				::VariantClear(&m_directValue);
				assert(false);
			}
			return S_OK;
		}
		else if (pDataObject->GetAttributeData(scLinkedIndirectValueTag, stringList))
		{
			m_indirectValueRef = SVObjectReference(0 < stringList.size() ? stringList[0] : "");
			return S_OK;
		}
		else if (pDataObject->GetAttributeData(scLinkedFormulaTag, stringList))
		{
			m_formulaString = (0 < stringList.size() ? stringList[0] : "");
			SvUl::RemoveEscapedSpecialCharacters(m_formulaString, true);
			return S_OK;
		}
		else
		{
			return __super::SetObjectValue(pDataObject);
		}
	}

	bool LinkedValue::runEmbedded(SvIe::RunStatus& /*rRunStatus*/, SvStl::MessageContainerVector* /*pErrorMessages*/)
	{
		if (SvPb::LinkedSelectedType::Formula == getSelectedType())
		{
			return setValueFromDouble(m_equation.RunAndGetResult());
		}
		return true;
	}

	SvPb::LinkedSelectedType LinkedValue::getSelectedType() const
	{
		return m_type;
	}

	HRESULT LinkedValue::setIndirectStringForOldStruct(const std::vector<_variant_t>& rValueString)
	{
		if (0 < rValueString.size() && VT_BSTR == rValueString[0].vt)
		{
			m_oldIndirectString = SvUl::createStdString(rValueString[0].bstrVal);
			return S_OK;
		}
		return E_FAIL;		
	}

	void LinkedValue::setChildIds(const std::vector<uint32_t>& rObjectIds)
	{
		m_childrenIds = rObjectIds;
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
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CircularReference, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				pErrorMessages->push_back(Msg);
			}
		}
		return Result;
	}

	bool LinkedValue::CheckLinkedObject(const SVObjectClass* const pLinkedObject, const variant_t& rDefault, SvStl::MessageContainerVector *pErrorMessages) const
	{
		//! Check if a valid object and make sure it does not link to itself
		if (nullptr == pLinkedObject || this == pLinkedObject)
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidOrRecursiveLinkedObject, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				pErrorMessages->push_back(Msg);
			}
			return false;
		}
		
		if (m_checkForValidDependency && false == checkIfValidDependency(pLinkedObject))
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_GroupDependencies_Wrong, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				pErrorMessages->push_back(Msg);
			}
			return false;
		}

		//! This is important when copying tools that the value of another inspection is not used due to the object ID being valid
		//! That is why check that the linked value of an object is in the same inspection
		const IObjectClass* pLinkedObjectInspection = pLinkedObject->GetAncestorInterface(SvPb::SVInspectionObjectType);
		bool isSameInpection = GetAncestorInterface(SvPb::SVInspectionObjectType) == pLinkedObjectInspection;
		//! If linked object has no inspection (e.g. Global Constants) then we don't need to check that the inspections are the same
		if (nullptr != pLinkedObjectInspection && !isSameInpection)
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_WrongInspection, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				pErrorMessages->push_back(Msg);
			}
			return false;
		}

		if (VT_EMPTY != rDefault.vt)
		{
			if (SvPb::SVValueObjectType != pLinkedObject->GetObjectType() && SvPb::SVBasicValueObjectType != pLinkedObject->GetObjectType())
			{
				if (nullptr != pErrorMessages)
				{
					SvDef::StringVector msgList;
					msgList.push_back(GetName());
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_LinkedValue_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
					pErrorMessages->push_back(Msg);
				}
				return false;
			}
		}
		return true;
	}

	variant_t LinkedValue::validateIndirectValue(const std::string& rIndirectIdString, variant_t defaultValue) const
	{
		if (rIndirectIdString == "VOID" && m_allowVoidReference)
		{
			return {};
		}
		SVObjectReference refObject {rIndirectIdString};
		if (refObject.getObject())
		{
			SvStl::MessageContainerVector errorMessages;
			bool isOk = CheckLinkedObject(refObject.getObject(), defaultValue, &errorMessages);
			if (false == isOk)
			{
				if (errorMessages.size())
				{
					throw errorMessages[0];
				}
				else
				{
					SvStl::MessageManager Exception(SvStl::MsgType::Log);
					Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValidateValue_LinkedObjectInvalid, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10029_ValueObject_Parameter_WrongSize, getObjectId());
					Exception.Throw();
				}
			}
			else
			{
				variant_t value;
				refObject.getValue(value);
				return value;
			}
		}
		else
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValidateValue_LinkedObjectInvalid, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10029_ValueObject_Parameter_WrongSize, getObjectId());
			Exception.Throw();
		}
	}

	variant_t LinkedValue::validateFormula(const std::string& rFormula) const
	{
		std::string oldString;
		//save old string
		oldString = m_equation.GetEquationText();
		//To Test formula and get result the formula-text has to be set. But after the test the old formula will be set back.
		//So it is "const" if you look from outside. For this reason the const_cast is done and acceptable here.
		const_cast<SvOp::SVEquation*>(&m_equation)->SetEquationText(rFormula);

		SvStl::MessageContainerVector messageContainers;
		SvOi::EquationTestResult testResult = const_cast<SvOp::SVEquation*>(&m_equation)->Test(&messageContainers);
		variant_t value = m_equation.GetYACCResult();
		const_cast<SvOp::SVEquation*>(&m_equation)->SetEquationText(oldString);
		if (false == testResult.bPassed)
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			if (messageContainers.size())
			{
				Exception.setMessage(messageContainers[0].getMessage());
			}
			else
			{
				Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValidateValue_FormulaFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10029_ValueObject_Parameter_WrongSize, getObjectId());
			}
			Exception.Throw();
		}
		return value;
	}

	bool LinkedValue::setValueFromDouble(double value)
	{
		variant_t valueVar = value;
		if (S_OK != ::VariantChangeTypeEx(&valueVar, &valueVar, SvDef::LCID_USA, VARIANT_ALPHABOOL, GetDefaultType()))
		{
			assert(false);
			return false;
		}
		return (S_OK == __super::setValue(valueVar));
	}

	void LinkedValue::setSelectedType(SvPb::LinkedSelectedType type)
	{
		if (type != m_type)
		{
			m_type = type;
			m_TypeValue.SetValue(type);
		}
	}

	bool LinkedValue::resetChildren(const SVObjectClass* const pLinkedObject, const std::vector<SvPb::EmbeddedIdEnum>& rEmbeddedIdList, SvStl::MessageContainerVector* pErrorMessages)
	{
		std::vector<SvOi::IValueObject*> childList;
		SvDef::SVObjectTypeInfoStruct info;
		for (auto embeddedId : rEmbeddedIdList)
		{
			info.m_EmbeddedID = embeddedId;
			auto* pChildObject = dynamic_cast<SvOi::IValueObject*>(pLinkedObject->getFirstObject(info));
			if (nullptr != pChildObject)
			{
				childList.push_back(pChildObject);
			}
		}
		m_children.resize(childList.size());
		SVObjectLevelCreateStruct createStruct(*this);
		bool result = true;
		for (int i = 0; i < childList.size(); ++i)
		{
			result = resetChild(i, childList[i], pErrorMessages, createStruct) && result;
		}
		return result;
	}

	bool LinkedValue::resetChild(int pos, SvOi::IValueObject* pValue, SvStl::MessageContainerVector* pErrorMessages, const SVObjectLevelCreateStruct& rCreateStruct)
	{
		auto* pObject = dynamic_cast<SVObjectClass*>(pValue);
		if (0 <= pos && pos < m_children.size() && nullptr != pValue)
		{
			if (nullptr == m_children[pos])
			{
				m_children[pos] = std::make_unique<LinkedValue>();
				m_children[pos]->createAllObjects(rCreateStruct);
				if (pos < m_childrenIds.size() && m_childrenIds[pos] != m_children[pos]->getObjectId())
				{
					SVObjectManagerClass::Instance().ChangeUniqueObjectID(m_children[pos].get(), m_childrenIds[pos]);
				}
			}
			m_children[pos]->donotCheckForDependency();
			m_children[pos]->SetObjectOwner(this);
			m_children[pos]->SetName(pObject->GetName());
			m_children[pos]->setIndirectValue(pObject->GetObjectNameToObjectType(SvPb::SVToolSetObjectType));
			m_children[pos]->setDefaultValue(pValue->getDefaultValue());
			return m_children[pos]->resetAllObjects(pErrorMessages);
		}
		return true;
	}
#pragma endregion Private Methods

} //namespace SvVol

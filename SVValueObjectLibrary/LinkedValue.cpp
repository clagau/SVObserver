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
#include "ObjectInterfaces/ITableObject.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary/SafeArrayHelper.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
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
		if (0 == (rValue.vt & VT_ARRAY))
		{
			SetArraySize(1); //@TODO[Arvid][10.00][15.07.2020] this was added for SVB-394. 
							 //A more general solution for ensuring that array values are changed correctly to scalar ones should be found!
			hres = __super::SetDefaultValue(rValue, bResetAll);//@TODO[Arvid][10.00][16.07.2020] SetDefaultValue() (and several others) always return S_OK. they should be of type void instead!
		}
		else
		{
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
		}
		

		///If the memory block size is wrong  then we need to reset the TR data
		if (-1 != getMemOffset() && getMemSizeReserved() != getByteSize(false, false))
		{
			setTrData(-1L, -1L, -1L);
		}
		return hres;
	}

	HRESULT LinkedValue::setValue(const _variant_t& rValue, int Index /*= -1*/, bool fixArraysize)
	{
		if ((rValue.vt & VT_ARRAY) && Index == -1)
		{
			long lBound(0), uBound(0);
			SafeArrayGetUBound(const_cast<SAFEARRAY *>(rValue.parray), 1, &uBound);
			SafeArrayGetLBound(const_cast<SAFEARRAY *>(rValue.parray), 1, &lBound);
			int32_t cnt_elements = uBound - lBound + 1;
			if (getArraySize() != cnt_elements && !fixArraysize)
			{
				SetArraySize(cnt_elements);
			}
			if (!fixArraysize && cnt_elements < 2)
			{
				//make sure we have an arraysize of at least 2
				SetArraySize(2);
				SetResultSize(cnt_elements);
			}
			return __super::setValue(rValue, Index, true);
		}
		else
		{
			auto res = __super::setValue(rValue, Index, fixArraysize);
			if(S_OK == res)
			{
				res = ResetObject()? S_OK : E_FAIL; //now Linked Values are fully usable once they have been set successfully
			}
			return res;
		}
	}

	HRESULT LinkedValue::setValue(const std::string& rValueString, int Index /*= -1*/)
	{
		return __super::setValue(rValueString, Index);
	}

	void LinkedValue::updateMemBlockData()
	{
		///When the linked value is an indirect value we need to always update it
		if (isIndirectValue())
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

			if (S_OK == Result || SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == Result || E_BOUNDS == Result)
			{
				//If the Linked Value is of type BOOL and is to be converted to the default type then we need the absolute value
				if (VT_BOOL == rValue.vt && GetDefaultType() != rValue.vt)
				{
					rValue.boolVal = rValue.boolVal ? 1 : 0;
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

	void LinkedValue::disconnectObjectInput(uint32_t objectId)
	{
		if (m_LinkedObjectRef.getObjectId() == objectId)
		{
			DisconnectInput();
			__super::disconnectObjectInput(objectId);
		}
	}

	void LinkedValue::UpdateLinkedName()
	{
		if (nullptr != m_LinkedObjectRef.getObject())
		{
			m_LinkedName.SetValue(m_LinkedObjectRef.GetObjectNameToObjectType(SvPb::SVObjectTypeEnum::SVToolSetObjectType, true));
			m_LinkedName.SetObjectAttributesAllowed(SvDef::selectableAttributes | SvPb::audittrail, SvOi::SetAttributeType::OverwriteAttribute);
			SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
		}
		else
		{
			std::string Value = m_LinkedName.GetDefaultValue();
			m_LinkedName.SetValue(Value);
			m_LinkedName.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
			SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);
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
		DWORD result;
		if (false == m_CircularReference && nullptr != m_LinkedObjectRef.getValueObject())
		{
			LifeFlag circularCheck(m_CircularReference);
			result = m_LinkedObjectRef.getValueObject()->GetType();
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
		LifeFlag circularCheck(m_CircularReference);
		bool result = pRefObject->isCircularReference();
		return result;
	}

	bool LinkedValue::isCorrectType(SvPb::ObjectSelectorType requiredType, const SVObjectClass* pTestObject) const
	{
		try
		{
			return __super::isCorrectType(requiredType, (nullptr != pTestObject) ? pTestObject : dynamic_cast<const SVObjectClass*>(getLinkedObject()));
		}
		catch (...)
		{
			return false;
		}
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
			UpdateLinkedName();
		}
	};

	bool LinkedValue::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
	{
		bool ret = __super::CreateObject(rCreateStructure);
		UpdateConnection();
		return ret;
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

			if (rDefaultValue.vt != VT_EMPTY)
			{
				if (S_OK != ::VariantChangeTypeEx(&vtTemp, &vtTemp, SvDef::LCID_USA, 0, rDefaultValue.vt))
				{
					SvDef::StringVector msgList;
					msgList.push_back(GetName());
					SvStl::MessageManager Exception(SvStl::MsgType::Log);
					Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_LinkedValue_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
					Exception.Throw();
				}
				Result = vtTemp;
			}
			else
			{
				Result = SVVariantValueObjectClass::ConvertString2Type(rValue, rDefaultValue);
			}
		}
		return Result;
	}

	void LinkedValue::disconnectAllInputs()
	{ 
		DisconnectInput(); 
		__super::disconnectAllInputs();
	}

	void LinkedValue::fillSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, SvOi::IsObjectAllowedFunc pFunctor, UINT attribute, bool wholeArray, SvPb::SVObjectTypeEnum nameToType, SvPb::ObjectSelectorType requiredType, bool stopIfClosed, bool firstObject) const
	{
		__super::fillSelectorList(treeInserter, pFunctor, attribute, wholeArray, nameToType, requiredType, stopIfClosed, firstObject);
		for (auto& rChild : m_children)
		{
			if (nullptr != rChild)
			{
				if (rChild->isCorrectType(requiredType))
				{
					SVObjectReference ObjectRef{ rChild->getObjectId() };
					ObjectRef.fillSelectorList(treeInserter, wholeArray, pFunctor, attribute, nameToType);
				}
			}
		}
	}
#pragma endregion Protected Methods

#pragma region Private Methods
	bool LinkedValue::UpdateConnection(SvStl::MessageContainerVector *pErrorMessages)
	{
		bool Result = true;
		bool convertOldName(false);
		
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
			else
			{
				//check if objectIdString is from old style (GUID) with more than 30, instead of new style ({#123456}) with less than 15 
				if (15 < objectIdAndIndexString.size())
				{
					convertOldName = true;
				}
			}
		}
		else
		{
			//Check if current value is a objectId, but not exist anymore. In this case it is probably an deleted object. Set this value to invalid.
			std::string::size_type Pos = objectIdAndIndexString.find_first_of(_T("["));
			std::string idString = objectIdAndIndexString.substr(0, Pos);
			if (SvDef::InvalidObjectId != calcObjectId(idString))
			{
				//@TODO[MZA][10.10][16.06.2021] some (special child linked values) objectID are not saved. For this reason it this add this code. But this should be changed in later versions
				std::string linkedValueName;
				getLinkedName().GetValue(linkedValueName);
				LinkedObjectRef = GetObjectReferenceForDottedName(linkedValueName);
				if (nullptr == LinkedObjectRef.getObject())
				{
					Result = false;
					if (nullptr != pErrorMessages)
					{
						SvDef::StringVector msgList;
						std::string objectName = GetObjectNameBeforeObjectType(SvPb::SVObjectTypeEnum::SVToolObjectType);

						msgList.push_back(linkedValueName);
						msgList.push_back(objectName);

						SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ConnectInputFailedLinkedValueNotFound, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
						pErrorMessages->push_back(Msg);
					}
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
				if (S_OK == SVObjectManagerClass::Instance().GetObjectByDottedName(ObjectName, LinkedObjectRef) && nullptr != LinkedObjectRef.getObject())
				{
					convertOldName = true;
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

				bool messageReturn = ConnectInput();
				assert(messageReturn);
				if (!messageReturn)
				{
					Result = false;
					if (nullptr != pErrorMessages)
					{
						SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ConnectInputFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
						pErrorMessages->push_back(Msg);
					}
				}
			}
			else
			{
				m_LinkedObjectRef = LinkedObjectRef;
			}

			//Convert old dotted name format to Unique ID
			if (convertOldName)
			{
				SVVariantValueObjectClass::setValue(m_LinkedObjectRef.GetCompleteName(true));
			}

			auto* pTable = dynamic_cast<SvOi::ITableObject*>(m_LinkedObjectRef.getFinalObject());
			if (nullptr != pTable)
			{ //Add children-LinkedValues for TableObject
				const auto& tableValues = pTable->getValueList();
				m_children.resize(tableValues.size());
				SVObjectLevelCreateStruct createStruct;
				createStruct.OwnerObjectInfo.SetObject(this);
				for (int i = 0; i < tableValues.size(); ++i)
				{
					if (nullptr == m_children[i])
					{
						m_children[i] = std::make_shared<LinkedValue>();
						m_children[i]->createAllObjects(createStruct);
					}
					m_children[i]->donotCheckForDependency();
					m_children[i]->SetName(tableValues[i]->GetName());
					m_children[i]->setValue(tableValues[i]->GetObjectNameToObjectType(SvPb::SVToolSetObjectType));
					m_children[i]->setDefaultValue(tableValues[i]->getDefaultValue());
					Result = m_children[i]->resetAllObjects(pErrorMessages) && Result;
				}
			}
			else
			{
				m_children.clear();
			}
		}
		else	// plain data
		{
			if (Result)
			{
				DisconnectInput();

				VARTYPE defaultType = GetDefaultType();
				_variant_t value;
				__super::GetValue(value);
				//@TODO[mec] enable array size 0 
				//if (VT_EMPTY != defaultType && value.vt != defaultType &&  ( ~VT_ARRAY & value.vt) != defaultType)
				if (VT_EMPTY != defaultType && value.vt != defaultType)
				{
					Result = false;
					if (nullptr != pErrorMessages)
					{
						SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_WrongType, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
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
			m_LinkedObjectRef.getObject()->disconnectObject(getObjectId());
			m_LinkedObjectRef = SVObjectReference();
		}
		m_children.clear();
	}

	UINT LinkedValue::ObjectAttributesSet(int iIndex) const
	{
		UINT result{ 0 };

		if (nullptr != m_LinkedObjectRef.getObject())
		{
			if (false == m_CircularReference)
			{
				LifeFlag circularCheck(m_CircularReference);
				result = m_LinkedObjectRef.getObject()->ObjectAttributesSet(iIndex);
			}
		}
		else
		{
			result = __super::ObjectAttributesSet(iIndex);
		}
		return result;
	}

	HRESULT LinkedValue::GetChildObject(SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index/* = 0*/) const
	{
		HRESULT l_Status = __super::GetChildObject(rpObject, rNameInfo, Index);

		if (S_OK != l_Status)
		{
			for (auto pChild : m_children)
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

	bool LinkedValue::ConnectInput()
	{
		if (nullptr != m_LinkedObjectRef.getObject())
		{
			if (m_checkForValidDependency)
			{	// children of a linkedValue (for tableObject) should not be connected because their parent already connected.
				m_LinkedObjectRef.getObject()->connectObject(getObjectId());
			}
			return true;
		}
		return false;
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
#pragma endregion Private Methods

} //namespace SvVol

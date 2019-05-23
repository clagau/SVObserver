//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVValueObject
//* .File Name       : $Workfile:   SVValueObjectClass.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 16:17:06  $
//******************************************************************************

#pragma region Includes
#include "Definitions/ObjectDefines.h"
#include "Definitions/StringTypeDef.h"
#include "Definitions/TextDefineSVDef.h"
#include "ObjectInterfaces/IObjectWriter.h"
#include "SVMessage/SVMessage.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVToolsetScriptTags.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVUtilityLibrary/SVSafeArray.h"
#pragma endregion Includes

namespace SvVol
{

#pragma region Constructor
template <typename T>
SVValueObjectClass<T>::SVValueObjectClass(LPCSTR ObjectName)
	: SVObjectClass(ObjectName)
	, m_shouldSaveValue(true)
	, m_shouldSaveDefaultValue(false)
{
	Initialize();
}

template <typename T>
SVValueObjectClass<T>::SVValueObjectClass(SVObjectClass* POwner, int StringResourceID)
	: SVObjectClass(POwner, StringResourceID)
	, m_shouldSaveValue(true)
	, m_shouldSaveDefaultValue(false)
{
	Initialize();
}

template <typename T>
SVValueObjectClass<T>::~SVValueObjectClass()
{
	SVObjectManagerClass::Instance().UnregisterSubObject(GetUniqueObjectID());
}

template <typename T>
const SVValueObjectClass<T>& SVValueObjectClass<T>::operator= (const SVValueObjectClass<T>& rRhs)
{
	if (this != &rRhs)
	{
		SetArraySize(rRhs.getArraySize());
		m_DefaultValue = rRhs.GetDefaultValue();
	}

	return *this;
}
#pragma endregion Constructor

#pragma region Public Methods
template <typename T>
bool SVValueObjectClass<T>::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool l_bOk = SVObjectClass::CreateObject(rCreateStructure);

	SVObjectManagerClass::Instance().RegisterSubObject(GetUniqueObjectID());
	SetObjectAttributesAllowed(SvPb::dataDefinitionValue, SvOi::SetAttributeType::AddAttribute);	// derived classes need to reset this

	return l_bOk;
}

template <typename T>
bool SVValueObjectClass<T>::CloseObject()
{
	SVObjectManagerClass::Instance().UnregisterSubObject(GetUniqueObjectID());
	return SVObjectClass::CloseObject();
}

template <typename T>
HRESULT SVValueObjectClass<T>::SetArraySize(int iSize)
{
	HRESULT hr = S_OK;

	m_ArraySize = (std::max)(iSize, 1);	// minimum one array element //parentheses around std::max avoid problems if a max macro has beed defined somewhere (e.g. in windows.h)

	if (getArraySize() <= 1)
	{
		if (std::string::npos != m_TypeName.find(_T(" Array")))
		{
			m_TypeName = SvUl::Left(m_TypeName, m_TypeName.size() - 6);
		}
	}
	else
	{
		if (std::string::npos == m_TypeName.find(_T(" Array")))
		{
			m_TypeName += _T(" Array");
		}
	}

	m_ResultSize = iSize;

	m_ObjectAttributesSet.resize(getArraySize(), m_DefaultObjectAttributesSet);

	if (1 == getArraySize())
	{
		if (iSize == 0)
		{
			m_Value = GetDefaultValue();
		}
		m_ValueArray.clear();
	}
	else
	{
		if (iSize == 0)
		{
			m_ValueArray.clear();	// minimum array size is 1. If iSize == 0, clear the array then resize back to 1 below with the default value.
		}
		m_ValueArray.resize(getArraySize(), GetDefaultValue());
	}

	return hr;
}

template <typename T>
HRESULT SVValueObjectClass<T>::SetObjectValue(SVObjectAttributeClass* pDataObject)
{
	HRESULT Result(E_FAIL);

	SvCl::SVObjectLongArrayClass LongArray;
	std::vector<ValueVector> BucketArray;		//This is for backward compatibility
	ValueVector ValueArray;

	if (pDataObject->GetAttributeData(scDefaultTag, ValueArray))
	{
		if (0 < ValueArray.size())
		{
			DefaultValue() = ValueArray[ValueArray.size() - 1];
		}
	}
	//  BUCKET_TAG_LOAD; get buckets, not array; for backward compatibility;
	else if (pDataObject->GetAttributeData(SvDef::cBucketTag, BucketArray, DefaultValue()))
	{

		if (1 == getArraySize())
		{
			// In configurations the value are placed in bucket 1
			m_Value = BucketArray[1][0];
		}
		else
		{
			// In configurations the values are placed in bucket 1
			std::swap(m_ValueArray, BucketArray[1]);
		}
	}
	// new-style: store all array elements:
	else if (pDataObject->GetArrayData(SvDef::cArrayTag, ValueArray, DefaultValue()))
	{
		SetArraySize(static_cast<int>(ValueArray.size()));
		if (1 == getArraySize())
		{
			m_Value = ValueArray[0];
		}
		else
		{
			std::swap(m_ValueArray, ValueArray);
		}
	}
	else if (pDataObject->GetAttributeData(scArraySizeTag, LongArray))
	{
		if (0 < LongArray.size())
		{
			SetArraySize(LongArray[0]);
		}
	}
	else
	{
		Result = SVObjectClass::SetObjectValue(pDataObject);
		return Result;
	}

	Result = S_OK;	// always OK if we get here

	return Result;
}

template <typename T>
__forceinline HRESULT SVValueObjectClass<T>::SetValue(const T& rValue, int Index)
{
	HRESULT Result = ValidateIndex(Index);

	if (S_OK == Result || SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == Result)
	{
		if (1 == getArraySize())
		{
			m_Value = rValue;
		}
		else
		{
			m_ValueArray[Index] = rValue;
		}
	}

#if defined (TRACE_THEM_ALL) || defined (TRACE_VALUE_OBJECT)
	std::string DebugString = SvUl::Format(_T("SetValue, %s, %s, %d, %d\r\n"), GetName(), ConvertType2String(rValue).c_str(), Index);
	::OutputDebugString(DebugString.c_str());
#endif
	return Result;
}

template <typename T>
__forceinline HRESULT SVValueObjectClass<T>::GetValue(T& rValue, int Index) const
{
	//! When Value Object is an array and index is -1 then use first index
	if (isArray() && 0 > Index)
	{
		Index = 0;
	}
	HRESULT Result = ValidateIndex(Index);

	// is the index valid?
	if (S_OK == Result)
	{
		if (1 == getArraySize())
		{
			rValue = m_Value;
		}
		else
		{
			rValue = m_ValueArray[Index];
		}
	}
	else if (SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == Result)
	{
		// not contain a value for the current run so use
		// the default value.
		rValue = m_DefaultValue;
	}
	else
	{
		// the selected index is not in the allocated array 
		rValue = m_DefaultValue;
	}
#if defined (TRACE_THEM_ALL) || defined (TRACE_VALUE_OBJECT)
	std::string DebugString = SvUl::Format(_T("GetValue, %s, %s, %d, %d\r\n"), GetName(), ConvertType2String(rValue).c_str(), Index);
	::OutputDebugString(DebugString.c_str());
#endif
	return Result;
}

template <typename T>
HRESULT SVValueObjectClass<T>::SetDefaultValue(const T& rValue, bool bResetAll)
{
	HRESULT Result(S_OK);

	m_DefaultValue = rValue;
	if (bResetAll)
	{
		if (1 == getArraySize())
		{
			m_Value = rValue;
		}
		else
		{
			std::fill(m_ValueArray.begin(), m_ValueArray.end(), rValue);
		}
	}
	return Result;
}

template <typename T>
HRESULT SVValueObjectClass<T>::SetArrayValues(typename ValueVector::const_iterator BeginIter, typename ValueVector::const_iterator EndIter)
{
	HRESULT Result(E_FAIL);

	int Size = static_cast<int> (std::distance(BeginIter, EndIter));
	assert(Size <= getArraySize());
	if (Size <= getArraySize())
	{
		SetResultSize(Size);
		if (Size > 0)
		{
			if (1 == getArraySize())
			{
				m_Value = *BeginIter;
			}
			else
			{
				std::copy(BeginIter, EndIter, m_ValueArray.begin());
			}
		}
		Result = S_OK;
	}
	else
	{
		Result = SVMSG_SVO_33_OBJECT_INDEX_INVALID;
	}
	return Result;
}

template <typename T>
HRESULT SVValueObjectClass<T>::SetArrayValues(const ValueVector& rValues)
{
	return SetArrayValues(rValues.begin(), rValues.end());
}

template <typename T>
HRESULT SVValueObjectClass<T>::GetArrayValues(ValueVector& rValues) const
{
	HRESULT Result = S_OK;
	if (0 == getResultSize())
	{
		return Result;
	}
	assert(getResultSize() <= getArraySize());
	rValues.resize(getResultSize());

	if (1 == getArraySize())
	{
		rValues[0] = m_Value;
	}
	else
	{
		std::copy(m_ValueArray.begin(), m_ValueArray.begin() + getResultSize(), rValues.begin());
	}
	return Result;
}

template <typename T>
HRESULT SVValueObjectClass<T>::SetTypeName(LPCTSTR TypeName)
{
	m_TypeName = TypeName;
	return S_OK;
}

template <typename T>
bool SVValueObjectClass<T>::CompareWithCurrentValue(const std::string& rCompare) const
{
	bool Result(false);

	std::string CurrentValue;
	if (S_OK == getValue(CurrentValue))
	{
		Result = (CurrentValue == rCompare);
	}

	return Result;
}

template <typename T>
HRESULT SVValueObjectClass<T>::getValue(double& rValue, int Index /*= -1*/) const
{
	HRESULT Result(E_FAIL);

	ValueType Value;
	Result = GetValue(Value, Index);
	rValue = ValueType2Double(Value);

	return Result;
}

template <typename T>
HRESULT SVValueObjectClass<T>::getValues(std::vector<double>&  rValues) const
{
	HRESULT Result(S_OK);

	int ResultSize = getResultSize();
	assert(ResultSize <= getArraySize());
	rValues.resize(ResultSize);
	for (int i = 0; i < ResultSize && S_OK == Result; i++)
	{
		ValueType Value;
		Result = GetValue(Value, i);
		rValues[i] = ValueType2Double(Value);
	}

	return Result;
}

template <typename T>
void SVValueObjectClass<T>::Persist(SvOi::IObjectWriter& rWriter)
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

										   // Get the Heading (Class Info)
	__super::Persist(rWriter);

	if (shouldSaveDefaultValue())
	{
		WriteDefaultValues(rWriter);
	}

	// Get the Data Values (Member Info, Values)
	if (shouldSaveValue())
	{
		rWriter.StartElement(scArrayElementsTag);
		WriteValues(rWriter);
		rWriter.EndElement();
	}
	else
	{
		int size = getArraySize();
		if (1 < size)
		{
			_variant_t varSize = size;
			varSize.ChangeType(VT_I4);
			rWriter.WriteAttribute(scArraySizeTag, varSize);
		}
	}

	rWriter.EndElement();
}

template <typename T>
HRESULT SVValueObjectClass<T>::setValue(const _variant_t& rValue, int Index /*= -1*/)
{
	HRESULT result{S_OK};

	std::vector<T> valueVector = variant2VectorType(rValue);

	if (!isArray() || 0 == (VT_ARRAY & rValue.vt) || nullptr == rValue.parray)
	{
		if(1 == valueVector.size())
		{
			result = SetValue(valueVector[0], Index);
		}
	}
	else
	{
		int arraySize = static_cast<int> (valueVector.size());
		if(arraySize > 0)
		{
			//fit array size to safeArray-size
			if (getArraySize() != arraySize)
			{
				result = SetArraySize((arraySize));
			}
			if(S_OK == result)
			{
				for(int i=0; i < arraySize && S_OK == result; i++)
				{
					result = SetValue(valueVector[i], i);
				}
			}
		}
		else
		{
			result = SvStl::Err_10029_ValueObject_Parameter_WrongSize;
		}
	}

	return result;
}

template <typename T>
HRESULT SVValueObjectClass<T>::getValue(_variant_t& rValue, int Index /*= -1*/, bool useResultSize /*= true*/) const
{
	HRESULT Result(E_FAIL);

	//! If index is -1 and it is an array then get the whole array
	if (-1 == Index && isArray())
	{
		rValue = vectorType2SafeArray(useResultSize ? getResultSize() : getArraySize());
		Result = S_OK;
	}
	else
	{
		ValueType Value;
		Result = GetValue(Value, Index);
		if (S_OK == Result)
		{
			rValue = ValueType2Variant(Value);
		}
		else
		{
			rValue.Clear();
		}
	}

	return Result;
}

template <typename T>
HRESULT SVValueObjectClass<T>::getValues(std::vector<_variant_t>&  rValues) const
{
	HRESULT Result = E_FAIL;

	if (0 == getResultSize())
	{
		return S_OK;
	}

	assert(getResultSize() <= getArraySize());
	rValues.resize(getResultSize());
	if (1 == getArraySize())
	{
		ValueType Value;
		Result = GetValue(Value, -1);
		if (S_OK == Result)
		{
			rValues[0] = ValueType2Variant(Value);
		}
	}
	else
	{
		ValueVector ValueArray;
		Result = GetArrayValues(ValueArray);
		if (S_OK == Result)
		{
			ValueVector::const_iterator FromIter(ValueArray.begin());
			std::vector<_variant_t>::iterator ToIter(rValues.begin());

			while (ToIter != rValues.end() && FromIter != ValueArray.end() &&
				FromIter != ValueArray.begin() + getResultSize())
			{
				*ToIter = ValueType2Variant(*FromIter);

				++FromIter;
				++ToIter;
			}
		}
	}
	return Result;
}

template <typename T>
HRESULT SVValueObjectClass<T>::setValue(const std::string& rValue, int Index /*= -1*/)
{
	HRESULT Result(E_FAIL);

	try
	{
		T Value = ConvertString2Type(rValue);
		Result = SetValue(Value, Index);
	}
	catch (const SvStl::MessageContainer&)
	{
		Result = E_FAIL;
	}

	return Result;
}

template <typename T>
HRESULT SVValueObjectClass<T>::getValue(std::string& rValue, int Index /*= -1*/) const
{
	HRESULT Result(E_FAIL);

	//Whole array
	if (isArray() && -1 == Index)
	{
		ValueVector Values;
		Result = GetArrayValues(Values);
		if (S_OK == Result)
		{
			ValueVector::const_iterator Iter(Values.begin());
			for (; Values.end() != Iter; ++Iter)
			{
				if (!rValue.empty())
				{
					rValue += _T("; ");
				}
				rValue += ConvertType2String(*Iter);
			}
		}
	}
	else
	{
		ValueType Value;
		Result = GetValue(Value, Index);
		if (S_OK == Result || SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == Result)
		{
			rValue = ConvertType2String(Value);
		}
	}
	return Result;
}

template <typename T>
void SVValueObjectClass<T>::setResetOptions(bool bResetAlways, SvOi::SVResetItemEnum eResetItem)
{
	m_ResetAlways = bResetAlways;
	m_eResetItem = eResetItem;
}

template <typename T>
void SVValueObjectClass<T>::validateValue(const _variant_t& rValue) const
{
	//This function throws an exception if not valid
	variant2VectorType(rValue);
}

#pragma endregion virtual method
#pragma endregion Public Methods

#pragma region Protected Methods
template <typename T>
void SVValueObjectClass<T>::Initialize()
{
	m_ArraySize = 1;
	m_LegacyVectorObjectCompatibility = false;
	m_ResetAlways = false;
	m_eResetItem = SvOi::SVResetItemIP;
	m_ResultSize = 1;
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVValueObjectType;

	SetObjectAttributesAllowed(SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::OverwriteAttribute);
}

template <typename T>
DWORD SVValueObjectClass<T>::GetByteSize() const
{
	DWORD Result(0);

	if (VT_BSTR == ValueType2Variant(m_Value).vt)
	{
		Result = SvDef::cMaxStringSize;
	}
	else
	{
		Result = sizeof(m_Value);
	}
	return Result;
}

template <typename T>
HRESULT SVValueObjectClass<T>::CopyToMemoryBlock(BYTE* pMemoryBlock, DWORD MemByteSize, int Index /* = -1*/) const
{
	HRESULT Result = ValidateMemoryBlockParameters(pMemoryBlock, MemByteSize, Index);

	if (S_OK == Result)
	{
		if (isArray())
		{
			memcpy(pMemoryBlock, &m_ValueArray[Index], GetByteSize());
		}
		else
		{
			memcpy(pMemoryBlock, &m_Value, GetByteSize());
		}
	}
	else if (GetByteSize() == MemByteSize)
	{
		memcpy(pMemoryBlock, &m_DefaultValue, GetByteSize());
	}

	return Result;
}

template <typename T>
void SVValueObjectClass<T>::init()
{
	m_ArraySize = 1;

	m_Value = m_DefaultValue;
}

template <typename T>
inline HRESULT SVValueObjectClass<T>::ValidateIndex(int ArrayIndex) const
{
	if (isArray())
	{
		if (ArrayIndex >= 0 && ArrayIndex < getArraySize())
		{
			//Use the variable not the function getResultSize in this case
			if (1 == getArraySize() || ArrayIndex < m_ResultSize)
			{
				return S_OK;
			}
			else
			{
				return SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE;
			}
		}
		else
		{
			return SVMSG_SVO_33_OBJECT_INDEX_INVALID;
		}
	}

	return S_OK;
}

template <typename T>
inline HRESULT SVValueObjectClass<T>::ValidateMemoryBlockParameters(BYTE* pMemoryBlock, DWORD MemByteSize, int Index) const
{
	HRESULT Result = ValidateIndex(Index);

	if (S_OK == Result)
	{
		if (nullptr != pMemoryBlock)
		{
			if (MemByteSize != GetByteSize())
			{
				Result = E_FAIL;
			}
		}
		else
		{
			Result = E_POINTER;
		}
	}

	return Result;
}
template <typename T>
typename T* SVValueObjectClass<T>::getValuePointer(int Index)
{
	ValueType* pResult(nullptr);

	if (1 == getArraySize())
	{
		pResult = &m_Value;
	}
	else
	{
		pResult = &m_ValueArray[Index];
	}

	return pResult;
}

template <typename T>
std::string SVValueObjectClass<T>::FormatOutput(const T& rValue) const
{
	std::string Result;
	if (!m_OutFormat.empty())
	{
		Result = SvUl::Format(m_OutFormat.c_str(), rValue);
	}
	return Result;
}

template <typename T>
inline void SVValueObjectClass<T>::swap(SVValueObjectClass& rRhs)
{
	assert(typeid(*this) == typeid(rRhs));

	if (this != &rRhs)
	{
		std::swap(m_DefaultValue, rRhs.m_DefaultValue);
		std::swap(m_Value, rRhs.m_Value);
		std::swap(m_ValueArray, rRhs.m_ArrayValues);
		std::swap(m_ArraySize, rRhs.m_ArraySize);
		std::swap(m_ResultSize, rRhs.m_ResultSize);
		std::swap(m_bLegacyVectorObjectCompatibility, rRhs.m_bLegacyVectorObjectCompatibility);
		std::swap(m_bResetAlways, m_bResetAlways);
		std::swap(m_eResetItem, m_eResetItem);
	}
}

template <typename T>
T SVValueObjectClass<T>::convertVariantValue(const _variant_t& rValue) const
{
	T result;

	//From GUI values are set using VT_BSTR
	if (VT_BSTR == rValue.vt)
	{
		result = ConvertString2Type(SvUl::createStdString(rValue));
	}
	//!For type safety check that the VT type is either the default value main value or when not set yet (VT_EMPTY)
	else if (ValueType2Variant(m_DefaultValue).vt == rValue.vt || ValueType2Variant(m_Value).vt == rValue.vt || ValueType2Variant(m_Value).vt == VT_EMPTY)
	{
		result = Variant2ValueType(rValue);
	}
	else
	{
		//This happens if the variant to convert is of a different type then this value object!
		assert(false);
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_WrongType, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
		Exception.Throw();
	}
	return result;
}

template <typename T>
std::vector<T> SVValueObjectClass<T>::variant2VectorType(const _variant_t& rValue) const
{
	ValueVector result;

	if (!isArray() || 0 == (VT_ARRAY & rValue.vt) || nullptr == rValue.parray)
	{
		T value = convertVariantValue(rValue);
		result.emplace_back(value);
	}
	else
	{
		VARTYPE varType = rValue.vt & ~VT_ARRAY;
		//!For type safety check that the VT type is either the default value main value or when not set yet (VT_EMPTY)
		if (ValueType2Variant(m_DefaultValue).vt != varType && ValueType2Variant(m_Value).vt != varType && ValueType2Variant(m_Value).vt != VT_EMPTY)
		{
			//This happens if the variant to convert is of a different type then this value object!
			assert(false);
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_WrongType, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			Exception.Throw();
		}
		result = SvUl::getVectorFromOneDim<T>(rValue);
		if (0 >= result.size())
		{
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValidateValue_ArraySizeInvalid, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10029_ValueObject_Parameter_WrongSize, GetUniqueObjectID());
			Exception.Throw();
		}
	}

	return result;
}

template <typename T>
_variant_t SVValueObjectClass<T>::vectorType2SafeArray(long arraySize) const
{
	_variant_t result;

	if(arraySize > 0 && arraySize <= static_cast<long> (getArraySize()))
	{
		SAFEARRAYBOUND arrayBound;
		arrayBound.lLbound = 0;
		arrayBound.cElements = arraySize;
		VARTYPE varType = ValueType2Variant(m_DefaultValue).vt;
		result.parray = ::SafeArrayCreate(varType, 1, &arrayBound);
		result.vt = varType | VT_ARRAY;

		if (nullptr != result.parray)
		{
			for (long i = 0; i < arraySize; ++i)
			{
				T value;
				if (S_OK == GetValue(value, i))
				{
					if (VT_BSTR == varType)
					{
						_bstr_t stringValue{ValueType2Variant(value)};
						::SafeArrayPutElement(result.parray, &i, static_cast<void*> (stringValue.Detach()));
					}
					else
					{
						::SafeArrayPutElement(result.parray, &i, static_cast<void*> (&value));
					}
				}
			}
		}
	}

	return result;
}
#pragma endregion Protected Methods

} //namespace SvVol

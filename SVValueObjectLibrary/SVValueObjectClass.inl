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
#include "Definitions/TextDefineSVDef.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "ObjectInterfaces/IObjectWriter.h"
#include "SVMessage/SVMessage.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVToolsetScriptTags.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVUtilityLibrary/SVSafeArray.h"
#pragma endregion Includes

namespace SvVol
{
///Initialize static member variable
template <typename T>
int32_t SVValueObjectClass<T>::m_maxTextSize = SvDef::cMaxStringByteSize;

#pragma region Constructor
template <typename T>
SVValueObjectClass<T>::SVValueObjectClass(LPCTSTR ObjectName)
	: SVObjectClass(ObjectName)
{
	Initialize();
}

template <typename T>
SVValueObjectClass<T>::SVValueObjectClass(SVObjectClass* POwner, int StringResourceID)
	: SVObjectClass(POwner, StringResourceID)
{
	Initialize();
}

template <typename T>
SVValueObjectClass<T>::~SVValueObjectClass()
{
	SVObjectManagerClass::Instance().UnregisterSubObject(GetUniqueObjectID());
}

template <typename T>
SVValueObjectClass<T>& SVValueObjectClass<T>::operator= (const SVValueObjectClass<T>& rRhs)
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
HRESULT SVValueObjectClass<T>::SetArraySize(int32_t iSize)
{
	HRESULT hr = S_OK;

	int32_t newSize = std::max(iSize, 1);	// minimum one array element
	int32_t oldSize = (0 == iSize) ? 0 : m_ArraySize;
	if(newSize == oldSize)
	{
		return hr;
	}
	bool tooSmall = newSize > m_ArraySize;
	m_ArraySize = newSize;
	if(tooSmall)
	{
		m_pValue = reserveLocalMemory();
	}

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

	m_ObjectAttributesSet.resize(getArraySize(), SvPb::noAttributes);

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
		int32_t arraySize = getArraySize();
		for(int32_t i=0; i < arraySize; ++i)
		{
			// In configurations the value are placed in bucket 1
			SetValue(BucketArray[1][i], i);
		}
	}
	// new-style: store all array elements:
	else if (pDataObject->GetArrayData(SvDef::cArrayTag, ValueArray, DefaultValue()))
	{
		int32_t arraySize = static_cast<int32_t>(ValueArray.size());
		SetArraySize(arraySize);
		for (int32_t i = 0; i < arraySize; ++i)
		{
			SetValue(ValueArray[i], i);
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
HRESULT SVValueObjectClass<T>::SetValue(const T& rValue, int Index)
{
	Index = isArray() ? Index : 0;
	HRESULT Result = ValidateIndex(Index);

	if (S_OK == Result || SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == Result)
	{
		if (nullptr != m_pValue)
		{
			m_pValue[Index] = rValue;
			m_hasChanged = true;
		}
	}

#if defined (TRACE_THEM_ALL) || defined (TRACE_VALUE_OBJECT)
	std::string DebugString = SvUl::Format(_T("SetValue, %s, %s, %d, %d\r\n"), GetName(), ConvertType2String(rValue).c_str(), Index);
	::OutputDebugString(DebugString.c_str());
#endif
	return Result;
}

template <typename T>
HRESULT SVValueObjectClass<T>::GetValue(T& rValue, int Index) const
{
	//! When index is -1 then use first index
	if (0 > Index)
	{
		Index = 0;
	}
	HRESULT Result = ValidateIndex(Index);

	// is the index valid?
	if (S_OK == Result)
	{
		if (nullptr != m_pValue)
		{
			rValue = m_pValue[Index];
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
		if (nullptr != m_pValue)
		{
			std::fill(m_pValue, m_pValue + getArraySize(), rValue);
		}
	}
	return Result;
}

template <typename T>
HRESULT SVValueObjectClass<T>::SetArrayValues(const ValueVector& rValues)
{
	HRESULT Result(E_FAIL);

	int32_t Size = static_cast<int32_t> (rValues.size());
	assert(Size <= getArraySize());
	if (Size <= getArraySize() && nullptr != m_pValue)
	{
		SetResultSize(Size);
		if (0 < Size)
		{
			memcpy(m_pValue, &rValues.at(0), Size * sizeof(ValueType));
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
HRESULT SVValueObjectClass<T>::GetArrayValues(ValueVector& rValues) const
{
	HRESULT Result = S_OK;
	if (0 == getResultSize())
	{
		return Result;
	}
	assert(getResultSize() <= getArraySize());
	rValues.resize(getResultSize());
	if(nullptr != m_pValue)
	{
		std::copy(m_pValue, m_pValue + getResultSize(), rValues.begin());
	}
	return Result;
}

template <typename T>
void SVValueObjectClass<T>::SetResultSize(int32_t ResultSize)
{
	m_ResultSize = (ResultSize <= m_ArraySize) ? ResultSize : 0;
	if(nullptr != m_pResultSize)
	{
		*m_pResultSize = m_ResultSize;
	}
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
		int32_t size = getArraySize();
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
HRESULT SVValueObjectClass<T>::setValue(const _variant_t& rValue, int Index /*= -1*/, bool fixArrasize)
{
	HRESULT result {S_OK};
	std::vector<T> valueVector = variant2VectorType(rValue);

	if (!isArray() || 0 == (VT_ARRAY & rValue.vt) || nullptr == rValue.parray)
	{
		if (1 == valueVector.size())
		{
			result = SetValue(valueVector[0], Index);
		}
	}
	else
	{
		int32_t arraySize = static_cast<int32_t> (valueVector.size());
		if (arraySize > 0)
		{
			if (fixArrasize == false)
			{
				//fit array size to safeArray-size
				if (getArraySize() != arraySize)
				{
					result = SetArraySize((arraySize));
				}
				if (S_OK == result)
				{
					for (int i = 0; i < arraySize && S_OK == result; i++)
					{
						result = SetValue(valueVector[i], i);
					}
				}
			}
			else
			{
				int32_t MaxIndex = __min(arraySize, getArraySize());
				SetResultSize(MaxIndex);
				for (int32_t i = 0; i < MaxIndex && S_OK == result; i++)
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
HRESULT SVValueObjectClass<T>::getValue(_variant_t& rValue, int Index /*= -1*/) const
{
	HRESULT Result(E_FAIL);

	//! If index is -1 and it is an array then get the whole array
	if (-1 == Index && isArray())
	{
		rValue = vectorType2SafeArray(getResultSize());
		Result = S_OK;
	}
	else
	{
		ValueType Value;
		Result = GetValue(Value, Index);
		if (S_OK == Result)
		{
			rValue = ValueType2Variant(&Value);
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
			rValues[0] = ValueType2Variant(&Value);
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
				*ToIter = ValueType2Variant(&*FromIter);

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
	m_LegacyVectorObjectCompatibility = false;
	m_ResetAlways = false;
	m_eResetItem = SvOi::SVResetItemIP;
	m_ResultSize = 1;
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVValueObjectType;
	SetObjectAttributesAllowed(SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::OverwriteAttribute);
}

template <typename T>
int32_t SVValueObjectClass<T>::getByteSize(bool useResultSize, bool memBlockData) const
{
	int32_t result(0L);

	///When for memory block and has no attributes then no memory requirements and return 0
	if (memBlockData && 0 == ObjectAttributesAllowed())
	{
		return result;
	}

	result = sizeof(ValueType);
	int32_t numberOfElements = useResultSize ? getResultSize() : getArraySize();
	result *= numberOfElements;
	//For memory block data that is an array the first value shall contain the result size which can be variable
	if(memBlockData && isArray())
	{
		result += sizeof(int32_t);
	}

	return result;
}

template <typename T>
void SVValueObjectClass<T>::setTrData(int32_t memOffset, int32_t memSize, int32_t pos)
{
	///This is required to make sure that if memory data block needs to be reallocated the pointer is not invalid
	clearMemoryBlockPointer();
	///Reset all parameters when pos is -1
	if(-1 == pos)
	{
		m_trPos = -1L;
		m_memOffset = -1L;
		m_memSizeReserved = 0L;
		return;
	}
	m_trPos = pos;
	///When memOffset and memSize are -1 then only the trigger record position is changed
	if(-1 == memOffset && -1 == memSize)
	{
		return;
	}
	m_memOffset = memOffset;
	///If array then the reserved size needs to be reduced by the result size which is of type int32_t
	m_memSizeReserved = isArray() ? memSize - sizeof(int32_t) : memSize;
}

template <typename T>
void SVValueObjectClass<T>::setMemBlockPointer(uint8_t* pMemBlockBase)
{
	if(nullptr != pMemBlockBase && -1 != m_memOffset)
	{
		if(isArray())
		{
			setResultSizePointer(reinterpret_cast<int32_t*> (pMemBlockBase + m_memOffset));
			m_pValue = reinterpret_cast<ValueType*> (pMemBlockBase + m_memOffset + sizeof(int32_t));
		}
		else
		{
			setResultSizePointer(nullptr);
			m_pValue = reinterpret_cast<ValueType*> (pMemBlockBase + m_memOffset);
		}
		///If local value data size is not 0 then we need to copy this to the memory block
		if(0 < m_valueData.size())
		{
			memcpy(m_pValue, &m_valueData.at(0), m_valueData.size());
			m_valueData.clear();
		}
	}
}

template <typename T>
void SVValueObjectClass<T>::init()
{
	///This also automatically allocates the local memory required
	SetArraySize(1);
}

template <typename T>
T* SVValueObjectClass<T>::reserveLocalMemory()
{
	ValueType* pResult{nullptr};

	bool isBlockMemory = (nullptr != m_pValue) && (0 == m_valueData.size());
	///Determine old array size
	int32_t oldArraySize = isBlockMemory ? m_memSizeReserved : static_cast<int32_t> (m_valueData.size());
	oldArraySize /= sizeof(ValueType);

	m_valueData.resize(getByteSize(false, false), 0);

	if (0 < m_valueData.size())
	{
		pResult = reinterpret_cast<ValueType*> (&m_valueData.at(0));
		///Need to copy block memory back to local memory when resizing
		if(isBlockMemory)
		{
			memcpy(pResult, m_pValue, m_memSizeReserved);
			m_memSizeReserved = 0L;
		}
		if(oldArraySize < getArraySize())
		{
			//Set all new elements to the default value
			std::fill(pResult + oldArraySize, pResult + getArraySize() - 1, DefaultValue());
		}
	}
	///Setting memory offset to -1 insures that the values will be redirected to block memory if necessary
	m_memOffset = -1;

	return pResult;
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
T SVValueObjectClass<T>::convertVariantValue(const _variant_t& rValue) const
{
	T result;

	//From GUI values are set using VT_BSTR
	if (VT_BSTR == rValue.vt)
	{
		result = ConvertString2Type(SvUl::createStdString(rValue));
	}
	//!For type safety check that the VT type is either the default value main value or when not set yet (VT_EMPTY)
	else if (ValueType2Variant(&m_DefaultValue).vt == rValue.vt || ValueType2Variant(m_pValue).vt == rValue.vt || ValueType2Variant(m_pValue).vt == VT_EMPTY)
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
		if (ValueType2Variant(&m_DefaultValue).vt != varType && ValueType2Variant(m_pValue).vt != varType && ValueType2Variant(m_pValue).vt != VT_EMPTY)
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

	if (arraySize > 0 && arraySize <= static_cast<long> (getArraySize()))
	{
		SAFEARRAYBOUND arrayBound;
		arrayBound.lLbound = 0;
		arrayBound.cElements = arraySize;
		VARTYPE varType = ValueType2Variant(&m_DefaultValue).vt;

		//Take the typ of the first element when no defaultvalue is set
		if (varType == VT_EMPTY)
		{
			T value;
			if (S_OK == GetValue(value, 0))
			{
				varType = ValueType2Variant(&value).vt;

			}
			if (varType == VT_EMPTY)
			{
				varType = VT_VARIANT;
			}
		}

		result.parray = ::SafeArrayCreate(varType, 1, &arrayBound);
		result.vt = varType | VT_ARRAY;

		if (nullptr != result.parray)
		{
			for (long i = 0; i < arraySize; ++i)
			{
				T value;
				if (S_OK == GetValue(value, i))
				{

					switch (varType)
					{
						case VT_I8:
						{
							__int64 llValue = static_cast<__int64> (ValueType2Variant(&value));
							::SafeArrayPutElement(result.parray, &i, static_cast<void*>(&llValue));
							break;
						}

						case VT_BSTR:
						{
							::SafeArrayPutElement(result.parray, &i, static_cast<void*> (V_BSTR(ValueType2Variant(&value).GetAddress())));
							break;
						}
						case  VT_R8:
						{
							double dValue = static_cast<double> (ValueType2Variant(&value));
							::SafeArrayPutElement(result.parray, &i, static_cast<void*>(&dValue));
							break;
						}
						case  VT_R4:
						{
							float fValue = static_cast<float> (ValueType2Variant(&value));
							::SafeArrayPutElement(result.parray, &i, static_cast<void*>(&fValue));
							break;
						}

						case VT_I4:
						{
							long lValue = static_cast<long> (ValueType2Variant(&value));
							::SafeArrayPutElement(result.parray, &i, static_cast<void*>(&lValue));
							break;
						}

						case VT_I2:
						{
							short sValue = static_cast<short> (ValueType2Variant(&value));
							::SafeArrayPutElement(result.parray, &i, static_cast<void*>(&sValue));
							break;
						}

						case VT_UI2:
						{
							unsigned short sValue = static_cast<unsigned short> (ValueType2Variant(&value));
							::SafeArrayPutElement(result.parray, &i, static_cast<void*>(&sValue));
							break;
						}

						case VT_UI4:
						{
							unsigned long lValue = static_cast<unsigned long> (ValueType2Variant(&value));
							::SafeArrayPutElement(result.parray, &i, static_cast<void*>(&lValue));
							break;
						}
						default:
						{
							//this works only if varType = T
							::SafeArrayPutElement(result.parray, &i, static_cast<void*> (&value));
							break;
						}

					}

				}
			}
		}
	}
	return result;
}

template <typename T>
void SVValueObjectClass<T>::setResultSizePointer(int32_t* pResultSize)
{
	m_pResultSize = pResultSize;
	if(nullptr != m_pResultSize)
	{
		*m_pResultSize = getResultSize();
	}
}

#pragma endregion Protected Methods

} //namespace SvVol

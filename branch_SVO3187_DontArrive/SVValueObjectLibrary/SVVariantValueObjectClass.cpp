//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVVariantValueObjectClass
//* .File Name       : $Workfile:   SVVariantValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   22 Apr 2014 07:05:58  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVVariantValueObjectClass.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary\SafeArrayHelper.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/TextDefineSVDef.h"
#pragma endregion Includes

namespace SvVol
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVVariantValueObjectClass, SvPb::VariantValueClassId);

SVVariantValueObjectClass::SVVariantValueObjectClass( LPCTSTR ObjectName )
: SVValueObjectClass<_variant_t>( ObjectName ) 
{
	LocalInitialize();
}

SVVariantValueObjectClass::SVVariantValueObjectClass( SVObjectClass* pOwner, int StringResourceID )
: SVValueObjectClass<_variant_t>( pOwner, StringResourceID )
{
	LocalInitialize();
}

SVVariantValueObjectClass::SVVariantValueObjectClass( const SVVariantValueObjectClass& rhs )
: SVValueObjectClass<_variant_t>()
{
	LocalInitialize();
	*this = rhs;
}

SVVariantValueObjectClass& SVVariantValueObjectClass::operator = (const SVVariantValueObjectClass& rhs )
{
	__super::operator = (rhs);
	return *this;
}

SVVariantValueObjectClass::~SVVariantValueObjectClass()
{
}

HRESULT SVVariantValueObjectClass::SetObjectValue(SVObjectAttributeClass* pDataObject)
{
	HRESULT Result( E_FAIL );

	std::vector<ValueVector> BucketArray;		//This is for backward compatibility
	ValueVector ValueArray;
	
	bool bOk{pDataObject->GetArrayData( SvDef::cArrayTag, ValueArray, DefaultValue())};
	// new-style: store all array elements:
	if (bOk)
	{
		int32_t arraySize = static_cast<int32_t> (ValueArray.size());
		SetArraySize(arraySize);
		for (int32_t i = 0; i < arraySize; i++)
		{
			_variant_t& rValue = ValueArray[i];
			if( rValue.vt == VT_BSTR )
			{
				std::string Temp = SvUl::createStdString( rValue.bstrVal );
				if ( SvUl::RemoveEscapedSpecialCharacters(Temp, true) )
				{
					rValue.SetString( Temp.c_str() );
				}
			}
			SetValue(rValue, i);
		}
	}
	else if(true == (bOk = pDataObject->GetAttributeData(_T("m_vtDefault"), ValueArray)))
	{
		if ( 0 < ValueArray.size() )
		{
			DefaultValue() = ValueArray[ValueArray.size()-1];
		}
	}
	else if(true == (bOk = pDataObject->GetAttributeData(_T("m_pavtArray"), BucketArray, DefaultValue())))
	{
		int32_t arraySize = static_cast<int32_t> (ValueArray.size());
		SetArraySize(arraySize);
		for(int32_t i = 0 ; i < BucketArray.size(); i++ )
		{
			_variant_t& rValue = BucketArray[i][0];
			if( rValue.vt == VT_BSTR )
			{
				std::string Temp = SvUl::createStdString( rValue.bstrVal );
				if ( SvUl::RemoveEscapedSpecialCharacters( Temp, true ) )
				{
					rValue.SetString( Temp.c_str() );
				}
			}
			SetValue(rValue, i);
		}
	}
	else
	{
		Result = __super::SetObjectValue(pDataObject);
		return Result;
	}

	Result = bOk ? S_OK : E_FAIL;
	return Result;
}

HRESULT SVVariantValueObjectClass::SetValue(const _variant_t& rValue, int Index /*= -1*/)
{
	///Note that the call to SetValue must be the SetValue(const T& rValue, int Index) version
	///otherwise it becomes an endless recursive call!
	HRESULT result = __super::SetValue(rValue, Index);
	///If the memory block size is to small then we need to reset the TR data
	if (-1 != getMemOffset() && getMemSizeReserved() < getByteSize(false, false))
	{
		setTrData(-1L, -1L, -1L);
	}
	return result;
}

HRESULT SVVariantValueObjectClass::SetArrayValues(const ValueVector& rValues)
{
	HRESULT Result(E_FAIL);

	int32_t Size = static_cast<int32_t> (rValues.size());
	assert(Size <= getArraySize());
	if (Size <= m_variantData.size())
	{
		SetResultSize(Size);
		if(0 < Size)
		{
			std::copy(rValues.begin(), rValues.end(), m_variantData.begin());
			setHasChanged(true);
			///If the memory block size is to small then we need to reset the TR data
			if (-1 != getMemOffset() && getMemSizeReserved() < getByteSize(false, false))
			{
				setTrData(-1L, -1L, -1L);
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

HRESULT SVVariantValueObjectClass::SetDefaultValue(const _variant_t& rValue, bool bResetAll)
{
	HRESULT hres = __super::SetDefaultValue(rValue, bResetAll);
	
	if (-1 != getMemOffset() && getMemSizeReserved() != getByteSize(false, false))
	{
		setTrData(-1L, -1L, -1L);
	}
	return hres;
}
void SVVariantValueObjectClass::setMemBlockPointer(uint8_t* pMemBlockBase)
{
	int32_t memOffset = getMemOffset();
	if (nullptr != pMemBlockBase && -1 != memOffset)
	{
		if (isArray())
		{
			setResultSizePointer(reinterpret_cast<int32_t*> (pMemBlockBase + memOffset));
			m_pMemBlockData = (pMemBlockBase + memOffset + sizeof(int32_t));
		}
		else
		{
			setResultSizePointer(nullptr);
			m_pMemBlockData = pMemBlockBase + memOffset;
		}
		updateMemBlockData();
	}
}

void SVVariantValueObjectClass::updateMemBlockData()
{
	///Here we only want the data byte size
	int32_t dataByteSize = getByteSize(true, false);
	///This is to make sure that enough space has been reserved for memory block data
	if (0 < dataByteSize && dataByteSize <= getMemSizeReserved() && nullptr != m_pMemBlockData)
	{
		if (hasChanged())
		{
			uint8_t* pMemoryLocation = m_pMemBlockData;
			int32_t byteSize = dataByteSize / getResultSize();
			int resultSize = isArray() ? getResultSize() : 1;
			for (int i = 0; i < resultSize; ++i)
			{
				_variant_t value;
				//!Note this calls the virtual function important for Linked values
				GetValue(value, i);
				const void* pValue(nullptr);

				switch (value.vt)
				{
					case VT_BOOL:
					case VT_I1:
					case VT_UI1:
					case VT_I2:
					case VT_UI2:
					case VT_I4:
					case VT_UI4:
					case VT_I8:
					case VT_UI8:
					case VT_INT:
					case VT_UINT:
					case VT_R4:
					case VT_R8:
					{
						///we take the address of any type in the union as all have the same address!
						pValue = &value.boolVal;
						break;
					}
					case VT_BSTR:
					{
						std::string tempString = SvUl::createStdString(value.bstrVal);
						pValue = nullptr;
						//Copy also the ending \0 of the string
						memcpy(pMemoryLocation, tempString.c_str(), tempString.size() + 1);
						pMemoryLocation += tempString.size() + 1;
						break;
					}
					default:
					{
						break;
					}
				}
				//This is for all types except VT_BSTR
				if (nullptr != pValue)
				{
					memcpy(pMemoryLocation, pValue, byteSize);
					pMemoryLocation += byteSize;
				}
			}
			setHasChanged(false);
		}
	}
	else
	{
		if (0 < getMemSizeReserved() && nullptr != m_pMemBlockData)
		{
			///Clear the memory block data
			memset(m_pMemBlockData, 0, getMemSizeReserved());
			setHasChanged(false);
			if (0 < dataByteSize)
			{
				///Memory block reserved for value object is to small. This should not happen!
				assert(false);
				#ifdef _DEBUG
				OutputDebugString("Memory block reserved for value object is to small. This should not happen!");
				#endif 
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				Exception.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorMemoryBlockDataReservedSize, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			}
		}
	}
}

HRESULT SVVariantValueObjectClass::SetValueKeepType(LPCTSTR Value, int Index)
{
	HRESULT hr = S_OK;

	_variant_t vtTemp;
	vtTemp = Value;

	if( VT_EMPTY != DefaultValue().vt )
	{
		hr = ::VariantChangeTypeEx( &vtTemp, &vtTemp, SvDef::LCID_USA, 0, DefaultValue().vt );
	}

	if( S_OK == hr)
	{
		hr = __super::SetValue( vtTemp, Index );
	}

	return hr;
}

bool SVVariantValueObjectClass::isArray() const
{
	return __super::isArray() || 0 != (GetDefaultType() & VT_ARRAY);
}

std::string SVVariantValueObjectClass::ToString(const VARIANT& rValue, bool bScript )
{
	std::string Result;
	_variant_t vt = rValue;


	switch ( vt.vt )
	{
		case VT_EMPTY:
		{
			if ( bScript )
			{
				Result = SvUl::Format( _T("%d, "), vt.vt );
				Result += _T("0");
			}
			break;
		}

		case VT_BSTR:
		{
			if ( bScript )
			{
				std::string Temp = SvUl::createStdString( rValue.bstrVal );
				SvUl::AddEscapeSpecialCharacters( Temp, true );
				Result = SvUl::Format(_T("%d, \"%s\""), vt.vt, Temp.c_str());
			}
			else
			{
				Result += SvUl::createStdString( rValue.bstrVal );
			}
		}
		break;

		default:
		{
			if( VT_ARRAY == (vt.vt & VT_ARRAY) )
			{
				if( bScript)
				{
					Result = SvUl::Format(_T("%d, 0"), VT_EMPTY);
				}
			}
			else
			{
				VARTYPE l_OldType = vt.vt;
				HRESULT hr = ::VariantChangeTypeEx(&vt, &vt, SvDef::LCID_USA, VARIANT_ALPHABOOL, VT_BSTR);	// use United States locale
				if ( S_OK == hr )
				{
					if( bScript)
					{
						Result = SvUl::Format(_T("%d, "), l_OldType);
					}

					Result += SvUl::createStdString( rValue.bstrVal );
					if( VT_BOOL == rValue.vt )
					{
						Result = SvUl::MakeUpper(Result.c_str());
					}
				}
				else
				{
					if( bScript)
					{
						Result = SvUl::Format(_T("%d, 0"), VT_EMPTY);
					}
				}
			}
		}
	}

	return Result;
}

_variant_t* SVVariantValueObjectClass::reserveLocalMemory()
{
	_variant_t* pResult{nullptr};

	if (m_variantData.size() != static_cast<size_t> (getArraySize()))
	{
		m_variantData.resize(getArraySize(), GetDefaultValue());
	}
	if (0 < m_variantData.size())
	{
		pResult = &m_variantData.at(0);
	}
	return pResult;
}

double SVVariantValueObjectClass::ValueType2Double(const _variant_t& rValue) const
{
	double Result(0.0);

	switch (rValue.vt)
	{
	case VT_BOOL:
		Result = rValue.boolVal ? 1.0 : 0.0;
		break;
	case VT_I1:
	case VT_UI1:
	case VT_I2:
	case VT_UI2:
	case VT_I4:
	case VT_UI4:
	case VT_I8:
	case VT_UI8:
	case VT_INT:
	case VT_UINT:
		Result = static_cast<double> (rValue);
		break;
	case VT_R8:
		Result = rValue.dblVal;
		break;
	case VT_BSTR:
		std::string Value = SvUl::createStdString(rValue);
		Result = atof( Value.c_str() );
		break;
	}
	return Result;
}

_variant_t SVVariantValueObjectClass::Variant2ValueType(const _variant_t& rValue) const
{
	_variant_t result {rValue};
	if (VT_EMPTY != GetDefaultType() && VT_EMPTY != rValue.vt)
	{
		result.ChangeType(GetDefaultType(), &rValue);
	}
	return result;
}

_variant_t SVVariantValueObjectClass::ConvertString2Type( const std::string& rValue ) const
{
	return ConvertString2Type(rValue, GetDefaultValue());
}

_variant_t SVVariantValueObjectClass::ConvertString2Type(const std::string& rValue, const _variant_t& rDefaultValue) const
{
	_variant_t Result(rValue.c_str());

	if (VT_EMPTY != rDefaultValue.vt)
	{
		if (S_OK != ::VariantChangeTypeEx(&Result, &Result, SvDef::LCID_USA, 0, rDefaultValue.vt))
		{
			SvDef::StringVector msgList;
			msgList.push_back(GetName());
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			Exception.Throw();
		}
	}

	return Result;
}

std::string SVVariantValueObjectClass::ConvertType2String( const _variant_t& rValue ) const
{
	std::string Result;

	switch ( rValue.vt )
	{
	case VT_BSTR:
		Result = SvUl::createStdString( rValue );
		break;

	default:
		{
			if (0 == (rValue.vt & VT_ARRAY))
			{
				_variant_t Value;
				if (S_OK == ::VariantChangeTypeEx(&Value, &rValue, SvDef::LCID_USA, VARIANT_ALPHABOOL, VT_BSTR))
				{
					Result = SvUl::createStdString(Value);
					if (VT_BOOL == rValue.vt)
					{
						Result = SvUl::MakeUpper(Result.c_str());
					}
				}
			}
			else
			{
				Result = SvUl::VariantToString(rValue);
			}
		}
	}
	return Result;
}

int32_t SVVariantValueObjectClass::getByteSize(bool useResultSize, bool memBlockData) const
{
	int32_t result(0L);

	///When for memory block and has no attributes then no memory requirements and return 0
	if (memBlockData && 0 == ObjectAttributesAllowed())
	{
		return result;
	}

	_variant_t value;
	GetValue(value, 0);
	switch (value.vt)
	{
	case VT_BOOL:
		result = sizeof(VARIANT::boolVal);
		break;
	case VT_I1:
		result = sizeof(VARIANT::cVal);
		break;
	case VT_UI1:
		result = sizeof(VARIANT::bVal);
		break;
	case VT_I2:
		result = sizeof(VARIANT::iVal);
		break;
	case VT_UI2:
		result = sizeof(VARIANT::uiVal);
		break;
	case VT_I4:
		result = sizeof(VARIANT::lVal);
		break;
	case VT_UI4:
		result = sizeof(VARIANT::ulVal);
		break;
	case VT_I8:
		result = sizeof(VARIANT::llVal);
		break;
	case VT_UI8:
		result = sizeof(VARIANT::ullVal);
		break;
	case VT_INT:
		result = sizeof(VARIANT::intVal);
		break;
	case VT_UINT:
		result = sizeof(VARIANT::uintVal);
		break;
	case VT_R4:
		result = sizeof(VARIANT::fltVal);
		break;
	case VT_R8:
		result = sizeof(VARIANT::dblVal);
		break;
	case VT_BSTR:
		result = getMaxTextSize();
		break;
	default:
		break;
	}

	int32_t numberOfElements = useResultSize ? getResultSize() : getArraySize();
	result *= numberOfElements;

	//For memory block data that is an array the first value shall contain the result size which can be variable
	if (memBlockData && isArray())
	{
		result += sizeof(int32_t);
	}

	return result;
}

void SVVariantValueObjectClass::WriteValues(SvOi::IObjectWriter& rWriter) const
{
	// Object Depth is implicit (it's the count of the values)
	std::vector<_variant_t> list;

	// Get the Data Values (Member Info, Values)
	_variant_t Value;

	// for all elements in the array
	for (int32_t i = 0; i < SVVariantValueObjectClass::getArraySize(); i++)
	{
		//Make sure this is not a derived virtual method which is called
		SVVariantValueObjectClass::GetValue(Value, i);

		if (VT_BSTR == Value.vt)
		{
			std::string temp = SvUl::createStdString(Value);
			SvUl::AddEscapeSpecialCharacters(temp, true);
			Value.SetString(temp.c_str());
		}
			
		// The parser does not like reading in empty safe array.
		// Therefore if an empty array is detected then set the variant type to VT_EMPTY.
		// 
		if (VT_ARRAY == (Value.vt & VT_ARRAY))
		{
			bool bEmpty = true;
			unsigned int dim = ::SafeArrayGetDim(Value.parray);
			long lBound = 0;
			long uBound = 0;
			if (dim > 0)
			{
				HRESULT hr = ::SafeArrayGetLBound(Value.parray, 1, &lBound);
				if(S_OK == hr)
				{
					hr = ::SafeArrayGetUBound(Value.parray, 1, &uBound);
				}
				long lSize = uBound - lBound + 1;
				if (S_OK == hr && lSize > 0)
				{
					bEmpty = false;
				}
			}
			if (bEmpty)
			{
				Value.Clear();
			}
		}
		list.push_back(Value);
	}
	rWriter.WriteAttribute(scElementTag, list);
}

void SVVariantValueObjectClass::WriteDefaultValues(SvOi::IObjectWriter& rWriter) const
{
	_variant_t Value(GetDefaultValue());
	rWriter.WriteAttribute(scDefaultTag, Value);
}

void SVVariantValueObjectClass::LocalInitialize()
{
	m_ObjectTypeInfo.m_SubType = SvPb::SVVariantValueObjectType;
	DefaultValue().Clear();

	SetTypeName( _T("Variant") );

	init();
}

} //namespace SvVol

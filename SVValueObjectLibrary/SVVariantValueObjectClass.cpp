//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
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
#include "SVObjectLibrary\SVClsids.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/TextDefineSVDef.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations


SV_IMPLEMENT_CLASS(SVVariantValueObjectClass, SVVariantValueObjectClassGuid);

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

const SVVariantValueObjectClass& SVVariantValueObjectClass::operator = (const SVVariantValueObjectClass& rhs )
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
	bool	bOk( false );

	std::vector<ValueType> ObjectArray;	// for default values
	BucketVector BucketArray;
	ValueVector ReadValueArray;
	
	// new-style: store all array elements:
	if ( bOk = pDataObject->GetArrayData( SvDef::cArrayTag, ReadValueArray, DefaultValue() ) )
	{
		for (size_t i = 0; i < ReadValueArray.size(); i++)
		{
			_variant_t& rValue = ReadValueArray[i];
			if( rValue.vt == VT_BSTR )
			{
				std::string Temp = SvUl::createStdString( rValue.bstrVal );
				if ( SvUl::RemoveEscapedSpecialCharacters(Temp, true) )
				{
					rValue.SetString( Temp.c_str() );
				}
			}
		}

		SetArraySize( static_cast< int >( ReadValueArray.size() ) );
		if( !isBucketized() )
		{
			if ( 1 == getArraySize() )
			{
				Value() = ReadValueArray[0];
			}
			else
			{
				std::swap( ValueArray(), ReadValueArray );
			}
		}
	}
	else if ( bOk = pDataObject->GetAttributeData(_T("m_vtDefault"), ObjectArray) )
	{
		if ( 0 < ObjectArray.size() )
		{
			DefaultValue() = ObjectArray[ObjectArray.size()-1];
		}
	}
	else if ( bOk = pDataObject->GetAttributeData(_T("m_pavtArray"), BucketArray, DefaultValue() ) )
	{
		for( size_t i = 0 ; i < BucketArray.size(); i++ )
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
		}

		if( !isBucketized() )
		{
			if ( 1 == getArraySize() )
			{
				// In configurations the value are placed in bucket 1
				Value() = BucketArray[1][0];
			}
			else
			{
				// In configurations the values are placed in bucket 1
				std::swap( ValueArray(), BucketArray[1] );
			}
		}
		else
		{
			if ( 1 == getArraySize() )
			{
				if( nullptr != getBucket().get() )
				{
					getBucket()->at(0) = BucketArray[0][0];
					getBucket()->at(1) = BucketArray[1][0];
				}
			}
			else
			{
				if( nullptr != getBucketArray() )
				{
					std::swap( *getBucketArray(), BucketArray );
				}
			}
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

VARTYPE SVVariantValueObjectClass::GetDefaultType( ) const
{
	return GetDefaultValue().vt;
}


HRESULT SVVariantValueObjectClass::SetValueKeepType(LPCTSTR Value, int Index)
{
	HRESULT hr = S_OK;

	_variant_t vtTemp;
	vtTemp = Value;

	if( VT_EMPTY != DefaultValue().vt )
	{
		hr = ::VariantChangeType( &vtTemp, &vtTemp, 0, DefaultValue().vt );
	}

	if( S_OK == hr)
	{
		hr = __super::SetValue( vtTemp, Index );
	}

	return hr;
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
						SvUl::MakeUpper( Result );
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

_variant_t SVVariantValueObjectClass::ConvertString2Type( const std::string& rValue ) const
{
	_variant_t Result(rValue.c_str());

	if (VT_EMPTY != GetDefaultType())
	{
		if (S_OK != ::VariantChangeType(&Result, &Result, 0, GetDefaultType()))
		{
			SvDef::StringVector msgList;
			msgList.push_back(GetName());
			SvStl::MessageMgrStd Exception(SvStl::LogOnly);
			Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
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
			_variant_t Value;
			if ( S_OK == ::VariantChangeType(&Value, &rValue, VARIANT_ALPHABOOL, VT_BSTR) )
			{
				Result = SvUl::createStdString( Value );
				if( VT_BOOL == rValue.vt )
				{
					SvUl::MakeUpper( Result );
				}
			}
		}
	}
	return Result;
}

DWORD SVVariantValueObjectClass::GetByteSize() const
{
	DWORD Result(0);

	switch (GetValueType())
	{
	case VT_BOOL:
		Result = sizeof(VARIANT::boolVal);
		break;
	case VT_I1:
		Result = sizeof(VARIANT::cVal);
		break;
	case VT_UI1:
		Result = sizeof(VARIANT::bVal);
		break;
	case VT_I2:
		Result = sizeof(VARIANT::iVal);
		break;
	case VT_UI2:
		Result = sizeof(VARIANT::uiVal);
		break;
	case VT_I4:
		Result = sizeof(VARIANT::lVal);
		break;
	case VT_UI4:
		Result = sizeof(VARIANT::ulVal);
		break;
	case VT_I8:
		Result = sizeof(VARIANT::llVal);
		break;
	case VT_UI8:
		Result = sizeof(VARIANT::ullVal);
		break;
	case VT_INT:
		Result = sizeof(VARIANT::intVal);
		break;
	case VT_UINT:
		Result = sizeof(VARIANT::uintVal);
		break;
	case VT_R4:
		Result = sizeof(VARIANT::fltVal);
		break;
	case VT_R8:
		Result = sizeof(VARIANT::dblVal);
		break;
	case VT_BSTR:
		Result = SvDef::cMaxStringSize;
		break;
	default:
		break;
	}
	return Result;
}

HRESULT SVVariantValueObjectClass::CopyToMemoryBlock(BYTE* pMemoryBlock, DWORD MemByteSize, int Index /* = -1*/) const
{
	HRESULT Result = ValidateMemoryBlockParameters(pMemoryBlock, MemByteSize, Index);

	if (S_OK == Result)
	{
		_variant_t Value;
		SVVariantValueObjectClass::GetValue(Value, Index);
		const void* pValue(nullptr);

		switch (Value.vt)
		{
		case VT_BOOL:
			pValue = &Value.boolVal;
			break;
		case VT_I1:
			pValue = &Value.cVal;
			break;
		case VT_UI1:
			pValue = &Value.bVal;
			break;
		case VT_I2:
			pValue = &Value.iVal;
			break;
		case VT_UI2:
			pValue = &Value.uiVal;
			break;
		case VT_I4:
			pValue = &Value.lVal;
			break;
		case VT_UI4:
			pValue = &Value.ulVal;
			break;
		case VT_I8:
			pValue = &Value.llVal;
			break;
		case VT_UI8:
			pValue = &Value.ullVal;
			break;
		case VT_INT:
			pValue = &Value.intVal;
			break;
		case VT_UINT:
			pValue = &Value.uintVal;
			break;
		case VT_R4:
			pValue = &Value.fltVal;
			break;
		case VT_R8:
			pValue = &Value.dblVal;
			break;
		case VT_BSTR:
			{
				std::string TempString = SvUl::createStdString(Value.bstrVal);
				size_t Size = std::min(static_cast<size_t> (GetByteSize() - 1), TempString.size());
				pValue = nullptr;
				memcpy(pMemoryBlock, TempString.c_str(), Size);
			}
			break;
		default:
			break;
		}
		//This is for all types except VT_BSTR
		if (nullptr != pValue)
		{
			memcpy(pMemoryBlock, pValue, GetByteSize());
		}
	}

	return Result;
}

void SVVariantValueObjectClass::WriteValues(SVObjectWriter& rWriter)
{
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;

	// Get the Data Values (Member Info, Values)
	_variant_t Value;

	// for all elements in the array
	for (int i = 0; i < getArraySize(); i++)
	{
		//Make sure this is not a derived virtual method which is called
		SVVariantValueObjectClass::GetValue(Value, i);

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
				hr = ::SafeArrayGetUBound(Value.parray, 1, &uBound);
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

void SVVariantValueObjectClass::WriteDefaultValues(SVObjectWriter& rWriter)
{
	_variant_t Value(GetDefaultValue());
	rWriter.WriteAttribute(scDefaultTag, Value);
}

void SVVariantValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvDef::SVVariantValueObjectType;
	DefaultValue().Clear();

	SetTypeName( _T("Variant") );

	InitializeBuckets();
}


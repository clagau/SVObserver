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
#include "SVUtilityLibrary/SVStringConversions.h"
#include "SVObjectLibrary/GlobalConst.h"
#include "ObjectInterfaces/TextDefineSvOi.h"
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
	BOOL	bOk( false );

	SvCl::SVObjectArrayClassTemplate<ValueType> ObjectArray;	// for default values
	BucketVector BucketArray;
	ValueVector ReadValueArray;
	
	// new-style: store all array elements:
	if ( bOk = pDataObject->GetArrayData( SvOi::cArrayTag, ReadValueArray, DefaultValue() ) )
	{
		for (size_t i = 0; i < ReadValueArray.size(); i++)
		{
			_variant_t& rValue = ReadValueArray[i];
			if( rValue.vt == VT_BSTR )
			{
				SVString Temp = SvUl_SF::createSVString( rValue.bstrVal );
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
		setLastSetIndex( 1 );

	}
	else if ( bOk = pDataObject->GetAttributeData(_T("m_vtDefault"), ObjectArray) )
	{
		if ( 0 < ObjectArray.GetSize() )
		{
			DefaultValue() = ObjectArray[ObjectArray.GetSize()-1];
		}
	}
	else if ( bOk = pDataObject->GetAttributeData(_T("m_pavtArray"), BucketArray, DefaultValue() ) )
	{
		for( size_t i = 0 ; i < BucketArray.size(); i++ )
		{
			_variant_t& rValue = BucketArray[i][0];
			if( rValue.vt == VT_BSTR )
			{
				SVString Temp = SvUl_SF::createSVString ( rValue.bstrVal );
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
		if ( 1 < getNumberOfBuckets() )
		{
			setLastSetIndex( 1 );
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


BOOL SVVariantValueObjectClass::SetType( int vt )
{
	if ( DefaultValue().vt != vt )
	{
		DefaultValue().Clear();
		DefaultValue().ChangeType( vt );
	}
	if ( 1 == getArraySize() )
	{
		if ( Value().vt != vt )
		{
			Value().Clear();
			Value().ChangeType( vt );
		}
	}
	else
	{
		for (int i = 0; i < getNumberOfBuckets(); i++)
		{
			for (int j=0; j < getArraySize(); j++)
			{
				_variant_t* pValue = getValuePointer( i, j );
				if ( nullptr != pValue && pValue->vt != vt )
				{
					pValue->Clear();
					pValue->ChangeType( vt );
				}
			}
		}
	}
	return true;
}

HRESULT SVVariantValueObjectClass::SetValueKeepType(LPCTSTR Value, int Bucket, int Index)
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
		hr = __super::SetValue( vtTemp, Bucket, Index );
	}

	return hr;
}

SVString SVVariantValueObjectClass::ToString(const VARIANT& rValue, bool bScript )
{
	SVString Result;
	_variant_t vt = rValue;


	switch ( vt.vt )
	{
		case VT_EMPTY:
		{
			if ( bScript )
			{
				Result = SvUl_SF::Format( _T("%d, "), vt.vt );
				Result += _T("0");
			}
			break;
		}

		case VT_BSTR:
		{
			if ( bScript )
			{
				SVString Temp = SvUl_SF::createSVString( rValue.bstrVal );
				SvUl::AddEscapeSpecialCharacters( Temp, true );
				Result = SvUl_SF::Format(_T("%d, \"%s\""), vt.vt, Temp.c_str());
			}
			else
			{
				Result += SvUl_SF::createSVString( rValue.bstrVal );
			}
		}
		break;

		default:
		{
			if( VT_ARRAY == (vt.vt & VT_ARRAY) )
			{
				if( bScript)
				{
					Result = SvUl_SF::Format(_T("%d, 0"), VT_EMPTY);
				}
			}
			else
			{
				VARTYPE l_OldType = vt.vt;
				HRESULT hr = ::VariantChangeTypeEx(&vt, &vt, SvOl::LCID_USA, VARIANT_ALPHABOOL, VT_BSTR);	// use United States locale
				if ( S_OK == hr )
				{
					if( bScript)
					{
						Result = SvUl_SF::Format(_T("%d, "), l_OldType);
					}

					Result += SvUl_SF::createSVString( rValue.bstrVal );
					if( VT_BOOL == rValue.vt )
					{
						SvUl_SF::MakeUpper( Result );
					}
				}
				else
				{
					if( bScript)
					{
						Result = SvUl_SF::Format(_T("%d, 0"), VT_EMPTY);
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
		SVString Value = SvUl_SF::createSVString(rValue);
		Result = atof( Value.c_str() );
		break;
	}
	return Result;
}

_variant_t SVVariantValueObjectClass::ConvertString2Type( const SVString& rValue ) const
{
	_variant_t Result(rValue.c_str());

	if (VT_EMPTY != GetDefaultType())
	{
		if (S_OK != ::VariantChangeType(&Result, &Result, 0, GetDefaultType()))
		{
			SVStringVector msgList;
			msgList.push_back(GetName());
			SvStl::MessageMgrStd Exception(SvStl::LogOnly);
			Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			Exception.Throw();
		}
	}

	return Result;
}

SVString SVVariantValueObjectClass::ConvertType2String( const _variant_t& rValue ) const
{
	SVString Result;

	switch ( rValue.vt )
	{
	case VT_BSTR:
		Result = SvUl_SF::createSVString( rValue );
		break;

	default:
		{
			_variant_t Value;
			if ( S_OK == ::VariantChangeType(&Value, &rValue, VARIANT_ALPHABOOL, VT_BSTR) )
			{
				Result = SvUl_SF::createSVString( Value );
				if( VT_BOOL == rValue.vt )
				{
					SvUl_SF::MakeUpper( Result );
				}
			}
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
		SVVariantValueObjectClass::GetValue(Value, GetLastSetIndex(), i);

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

void SVVariantValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVVariantValueObjectType;
	DefaultValue().Clear();

	SetTypeName( _T("Variant") );

	InitializeBuckets();
}


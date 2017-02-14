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

SVVariantValueObjectClass::SVVariantValueObjectClass(LPCTSTR ObjectName)
: base(ObjectName) 
{
	LocalInitialize();
}

SVVariantValueObjectClass::SVVariantValueObjectClass(SVObjectClass* POwner, int StringResourceID)
: base(POwner, StringResourceID) 
{
	LocalInitialize();
}

SVVariantValueObjectClass::SVVariantValueObjectClass( const SVVariantValueObjectClass& rhs )
: base()
{
	LocalInitialize();
	*this = rhs;
}

const SVVariantValueObjectClass& SVVariantValueObjectClass::operator = (const SVVariantValueObjectClass& rhs )
{
	base::operator = (rhs);
	return *this;
}

SVVariantValueObjectClass::~SVVariantValueObjectClass()
{
}

void SVVariantValueObjectClass::Persist(SVObjectWriter& rWriter)
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	SVValueObjectClass::Persist(rWriter);
	
	// Get the Data Values (Member Info, Values)
	_variant_t value(DefaultValue());
	rWriter.WriteAttribute( scDefaultTag, value  );
	
	rWriter.StartElement(scArrayElementsTag);

	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;

	// for all elements in the array (m_iNumberOfBuckets)
	for (int i = 0; i < m_iArraySize; i++)
	{
		value = Element(m_iLastSetIndex, i);

		// The parser does not like reading in empty safe array.
		// Therefore if an empty array is detected then set the variant type to VT_EMPTY.
		// 
		if( VT_ARRAY == (value.vt & VT_ARRAY) )
		{
			bool bEmpty = true;
			unsigned int dim = ::SafeArrayGetDim(value.parray);
			long lBound = 0;
			long uBound = 0;
			if( dim > 0 )
			{
				HRESULT hr = ::SafeArrayGetLBound( value.parray,1,&lBound);
				hr = ::SafeArrayGetUBound( value.parray, 1, &uBound);
				long lSize = uBound-lBound+1;
				if( S_OK == hr && lSize > 0 )
				{
					bEmpty = false;
				}
			}
			if( bEmpty)
			{
				value.Clear();
			}
		}
		list.push_back( value );
	}
	rWriter.WriteAttribute( scElementTag, list );
	rWriter.EndElement();

	rWriter.EndElement();
}

HRESULT SVVariantValueObjectClass::SetObjectValue(SVObjectAttributeClass* pDataObject)
{
	HRESULT hr = S_FALSE;
	BOOL bOk = FALSE;
	SvCl::SVObjectVariantArrayClass svArray;
	bucket_type l_Buckets(BucketNoAssert());
	array_type l_Array;
	
	if ( bOk = pDataObject->GetAttributeData(SvOi::cDefaultTag, svArray) )
	{
		for (int i = 0; i < svArray.GetSize(); i++)
		{
			DefaultValue() = svArray[i];
		}
	}
	else if ( bOk = pDataObject->GetAttributeData(SvOi::cBucketTag, l_Buckets, DefaultValue() ) )
	{
		if ( 1 == ArraySize() )
		{
			ScalarBucket(0)=l_Buckets[0][0];
			ScalarBucket(1)=l_Buckets[1][0];
		}
		else
		{
			std::swap(Buckets(), l_Buckets);
		}
		if ( 1 < m_iNumberOfBuckets )
		{
			m_iLastSetIndex = 1;
		}
	}
	// new-style: store all array elements:
	else if ( bOk = pDataObject->GetArrayData(SvOi::cArrayTag, l_Array, DefaultValue() ) )
	{
		for ( size_t i = 0; i < l_Array.size(); i++ )
		{
			_variant_t& rValue = l_Array[ i ];
			if( rValue.vt == VT_BSTR )
			{
				SVString Temp = SvUl_SF::createSVString( rValue.bstrVal );
				if ( SvUl::RemoveEscapedSpecialCharacters(Temp, true) )
				{
					rValue = Temp.c_str();
				}
			}
		}

		SetArraySize( static_cast<int>(l_Array.size()) );
		if ( ArraySize() == 1 )
		{
			ScalarBucket(1)=l_Array[0];
		}
		else
		{
			std::swap(Array(1), l_Array);
		}
		m_iLastSetIndex = 1;
	}
	else if ( bOk = pDataObject->GetAttributeData(_T("m_vtDefault"), svArray) )
	{
		if ( svArray.GetSize() > 0 )
		{
			DefaultValue() = svArray[svArray.GetSize()-1];
		}
	}
	else if ( bOk = pDataObject->GetAttributeData(_T("m_pavtArray"), l_Buckets, DefaultValue() ) )
	{
		for( size_t i = 0 ; i < l_Buckets.size(); i++ )
		{
			_variant_t& rValue = l_Buckets[i][0];
			if( rValue.vt == VT_BSTR )
			{
				SVString Temp = SvUl_SF::createSVString ( rValue.bstrVal );
				if ( SvUl::RemoveEscapedSpecialCharacters( Temp, true ) )
				{
					rValue = Temp.c_str();
				}
			}
		}

		if ( 1 == ArraySize()  )
		{
			ScalarBucket(0) = l_Buckets[0][0];
			ScalarBucket(1) = l_Buckets[1][0];
		}
		else
		{
			std::swap(Buckets(), l_Buckets);
		}
		if ( 1 < m_iNumberOfBuckets )
		{
			m_iLastSetIndex = 1;
		}
	}
	else
	{
		hr = base::SetObjectValue(pDataObject);
		return hr;
	}

	hr = bOk ? S_OK : S_FALSE;
	return hr;
}

HRESULT SVVariantValueObjectClass::SetValueAt( int iBucket, int iIndex, const VARIANT& rvtValue )
{
	return base::SetValueAt(iBucket, iIndex, _variant_t(rvtValue));
}

HRESULT SVVariantValueObjectClass::SetValueAt(int iBucket, int iIndex, const BOOL bValue)
{
	HRESULT hr = ValidateIndexes(iBucket, iIndex);

	if ( S_OK == hr || SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hr )
	{
		Element(iBucket, iIndex).Clear();
		Element(iBucket, iIndex).ChangeType(VT_BOOL);
		Element(iBucket, iIndex) = (short) bValue;
		m_iLastSetIndex  = iBucket;

		hr = S_OK;
	}
	
	return hr;
}

HRESULT SVVariantValueObjectClass::SetValueAt( int iBucket, int iIndex, const double dValue )
{
	return base::SetValueAt( iBucket, iIndex, _variant_t(dValue) );
}

HRESULT SVVariantValueObjectClass::SetValueAt( int iBucket, int iIndex, const BYTE byValue )
{
	return base::SetValueAt( iBucket, iIndex, _variant_t(byValue) );
}

HRESULT SVVariantValueObjectClass::SetValueAt( int iBucket, int iIndex, const long lValue )
{
	return base::SetValueAt( iBucket, iIndex, _variant_t(lValue) );
}

HRESULT SVVariantValueObjectClass::SetValueAt( int iBucket, int iIndex, const DWORD dwValue )
{
	HRESULT hr = ValidateIndexes(iBucket, iIndex);

	if ( S_OK == hr || SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hr )
	{
		Element(iBucket, iIndex).Clear();
		Element(iBucket, iIndex).ChangeType(VT_UI4);
		Element(iBucket, iIndex).ulVal = dwValue;
		m_iLastSetIndex  = iBucket;
		
		hr = S_OK;
	}
	return hr;
}

HRESULT SVVariantValueObjectClass::SetValueAt( int iBucket, int iIndex, const SVString& strValue )
{
	return base::SetValueAt( iBucket, iIndex, _variant_t(strValue.c_str()) );
}

VARTYPE SVVariantValueObjectClass::GetDefaultType( ) const
{
	return DefaultValue().vt;
}


BOOL SVVariantValueObjectClass::SetType( int vt )
{
	if ( DefaultValue().vt != vt )
	{
		DefaultValue().Clear();
		DefaultValue().ChangeType( vt );
	}
	if ( 1 == ArraySize() )
	{
		for (int i = 0; i < m_iNumberOfBuckets; ++ i)
		{
			if ( ScalarBucket(i).vt != vt )
			{
				ScalarBucket(i).Clear();
				ScalarBucket(i).ChangeType( vt );
			}
		}
	}
	else
	{
		for (int i = 0; i < m_iNumberOfBuckets; i++)
		{
			for (int j=0; j < ArraySize(); j++)
			{
				if ( Element(i, j).vt != vt )
				{
					Element(i, j).Clear();
					Element(i, j).ChangeType( vt );
				}
			}
		}
	}
	return true;
}

HRESULT SVVariantValueObjectClass::SetValueKeepType( int iBucket, int iIndex, LPCTSTR strValue )
{
	HRESULT hr = S_OK;

	_variant_t vtTemp;
	vtTemp = strValue;

	if( VT_EMPTY != DefaultValue().vt )
	{
		hr = ::VariantChangeType( &vtTemp, &vtTemp, 0, DefaultValue().vt );
	}

	if( S_OK == hr)
	{
		hr = base::SetValueAt( iBucket, iIndex, vtTemp );
	}

	return hr;
}

HRESULT SVVariantValueObjectClass::SetDefaultValue(const VARIANT& rvtValue, bool bResetAll)
{
	HRESULT hr = ::VariantCopy( &(DefaultValue()), const_cast <VARIANT*>( &rvtValue) );
	if ( bResetAll )
	{
		if ( 1 == ArraySize() )
		{
			for (int i = 0; i < m_iNumberOfBuckets; ++ i)
			{
				ScalarBucket(i) = rvtValue;
			}
		}
		else
		{
			for (int i = 0; i < m_iNumberOfBuckets; ++ i)
			{
				for (size_t j=0; j < Buckets()[i].size(); ++j)
				{
					Buckets()[i][j] = rvtValue;
				}
			}
		}
	}
	return hr;
}

HRESULT SVVariantValueObjectClass::GetDefaultValue(_variant_t& rValue) const
{
	HRESULT hr = ::VariantCopy( &rValue, &(DefaultValue()) );
	return IsCreated() ? hr : S_FALSE;
}

HRESULT SVVariantValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rValue) const
{
	HRESULT hr = ValidateIndexes(iBucket,iIndex);

	if ( S_OK == hr )
	{
		hr = ::VariantCopy( &rValue, const_cast <_variant_t*> (&(Element(iBucket, iIndex))) );
		m_isObjectValid = true;
	}
	else if ( SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hr )
	{
		rValue = DefaultValue();
		m_isObjectValid = true;
	}
	else	//	BAD INDEX
	{
		rValue = DefaultValue();
		m_isObjectValid = false;
	}
	return hr;
}

HRESULT SVVariantValueObjectClass::GetValueAt( int iBucket, int iIndex, BOOL& rbValue) const
{
	HRESULT hr = ValidateIndexes(iBucket,iIndex);

	if ( S_OK == hr )
	{
		m_isObjectValid = true;
		_variant_t vtTemp = Element(iBucket, iIndex);
		switch ( vtTemp.vt )
		{
			case VT_BSTR:
			{
				SVString sTemp = SvUl_SF::createSVString( vtTemp.bstrVal );
				if ( 0 == SvUl_SF::CompareNoCase( sTemp, SvOi::cTrue) )
				{
					rbValue = true;
					hr = S_OK;
				}
				else if ( 0 == SvUl_SF::CompareNoCase( sTemp, SvOi::cFalse) )
				{
					rbValue = false;
					hr = S_OK;
				}
				else if ( 0 == SvUl_SF::CompareNoCase( sTemp, _T("1")) )
				{
					rbValue = true;
					hr = S_OK;
				}
				else if ( 0 == SvUl_SF::CompareNoCase( sTemp, _T("0")) )
				{
					rbValue = false;
					hr = S_OK;
				}
				else
				{
					rbValue = false;
				}
			}
			break;

			default:
			{
				hr = ::VariantChangeType(&vtTemp, &vtTemp, 0, VT_BOOL);
				if ( S_OK == hr )
				{
					rbValue = vtTemp.boolVal;
				}
				else
				{
					if ( VT_BOOL == DefaultValue().vt )
					{
						rbValue = DefaultValue().boolVal;
					}
					else
					{
						rbValue = false;
					}
				}
			}
		}// end switch ( vtTemp.vt )
	}// end if (iBucket >= 0 && iBucket < m_iNumberOfBuckets)
	else
	{
		if ( VT_BOOL == DefaultValue().vt )
		{
			rbValue = DefaultValue().boolVal;
		}
		else
		{
			rbValue = false;
		}
		m_isObjectValid = (SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hr);
	}

	return hr;
}


HRESULT SVVariantValueObjectClass::GetValueAt( int iBucket, int iIndex, SVString& rValue) const
{
	HRESULT hr = ValidateIndexes(iBucket,iIndex);

	_variant_t vtTemp;
	if ( S_OK == hr )
	{
		vtTemp = Element(iBucket, iIndex);
		m_isObjectValid = true;
	}
	else
	{
		vtTemp = DefaultValue();
		m_isObjectValid = (SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hr);
	}

	switch ( vtTemp.vt )
	{
		case VT_BSTR:
		{
			rValue = SvUl_SF::createSVString( vtTemp.bstrVal );
			hr = S_OK;
		}
		break;

		default:
		{
			VARTYPE vtOrig = vtTemp.vt;
			hr = ::VariantChangeType(&vtTemp, &vtTemp, VARIANT_ALPHABOOL, VT_BSTR);
			if ( S_OK == hr )
			{
				rValue = SvUl_SF::createSVString( vtTemp.bstrVal );
				if( VT_BOOL == vtOrig )
				{
					SvUl_SF::MakeUpper( rValue );
				}
			}
			else
			{
				rValue.clear();
			}
		}
	}// end switch ( vtTemp.vt )

	return hr;
}

HRESULT SVVariantValueObjectClass::GetValueAt( int iBucket, int iIndex, double& rdValue) const
{
	HRESULT hr = ValidateIndexes(iBucket, iIndex);

	_variant_t vtTemp;
	if ( S_OK == hr )
	{
		vtTemp = Element(iBucket, iIndex);
		m_isObjectValid = true;
	}
	else
	{
		vtTemp = DefaultValue();
		m_isObjectValid = (SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hr);
	}

	HRESULT hrChange = ::VariantChangeType(&vtTemp, &vtTemp, 0, VT_R8);
	if ( S_OK == hrChange )
	{
		rdValue = vtTemp.dblVal;
	}
	else
	{
		rdValue = 0;
		if ( S_OK == hr )
		{
			hr = hrChange;
		}
	}

	return hr;
}

HRESULT SVVariantValueObjectClass::GetValueAt( int iBucket, int iIndex, long& rlValue ) const
{
	HRESULT hr = ValidateIndexes(iBucket, iIndex);

	_variant_t vtTemp;
	if ( S_OK == hr )
	{
		vtTemp = Element(iBucket, iIndex);
		m_isObjectValid = true;
	}
	else
	{
		vtTemp = DefaultValue();
		m_isObjectValid = (SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hr);
	}

	HRESULT hrChange = ::VariantChangeType(&vtTemp, &vtTemp, 0, VT_I4);
	if ( S_OK == hrChange )
	{
		rlValue = vtTemp.lVal;
	}
	else
	{
		rlValue = 0;

		if ( S_OK == hr )
		{
			hr = hrChange;
		}
	}

	return hr;
}

HRESULT SVVariantValueObjectClass::GetValueAt( int iBucket, int iIndex, DWORD& rdwValue ) const
{
	HRESULT hr = ValidateIndexes(iBucket, iIndex);

	_variant_t vtTemp;
	if ( S_OK == hr )
	{
		vtTemp = Element(iBucket, iIndex);
		m_isObjectValid = true;
	}
	else
	{
		vtTemp = DefaultValue();
		m_isObjectValid = (SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hr);
	}

	HRESULT hrChange = ::VariantChangeType(&vtTemp, &vtTemp, 0, VT_UI4);
	if ( S_OK == hrChange  )
	{
		rdwValue = vtTemp.ulVal;
	}
	else
	{
		rdwValue = 0;
		if ( S_OK == hr )
		{
			hr = hrChange;
		}
	}
	return hr;
}

HRESULT SVVariantValueObjectClass::GetValueAt( int iBucket, int iIndex, BYTE& rbyValue ) const
{
	HRESULT hr = ValidateIndexes(iBucket, iIndex);

	_variant_t vtTemp;
	if ( S_OK == hr )
	{
		vtTemp = Element(iBucket, iIndex);
		m_isObjectValid = true;
	}
	else
	{
		vtTemp = DefaultValue();
		m_isObjectValid = (SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hr );
	}

	HRESULT hrChange = ::VariantChangeType(&vtTemp, &vtTemp, 0, VT_UI1);
	if ( S_OK == hrChange )
	{
		rbyValue = vtTemp.bVal;
	}
	else
	{
		rbyValue = 0;
		if ( S_OK == hr )
		{
			hr = hrChange;
		}
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

void SVVariantValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVVariantValueObjectType;
	DefaultValue().Clear();

	SetTypeName( _T("Variant") );

	InitializeBuckets();
}


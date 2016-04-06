//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVByteValueObjectClass
//* .File Name       : $Workfile:   SVByteValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Oct 2013 12:16:26  $
//******************************************************************************

#include "stdafx.h"
#include "SVByteValueObjectClass.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVOMFCLibrary/StringMunge.h"

namespace	// only for this file
{
	const CString DEFAULT_TAG_SAVE(_T(".Default"));
	const CString BUCKET_TAG_SAVE(_T(".Array"));	// for backwards compatibility
	const CString ARRAY_TAG_SAVE(_T(".Array_Elements"));	// new style; one bucket, all array values

	const CString DEFAULT_TAG_LOAD(_T("Default"));
	const CString BUCKET_TAG_LOAD(_T("Array"));	// for backwards compatibility
	const CString ARRAY_TAG_LOAD(_T("Array_Elements"));	// new style; one bucket, all array values
}	// end file scope namespace

SV_IMPLEMENT_CLASS(SVByteValueObjectClass, SVByteValueObjectClassGuid);

SVByteValueObjectClass::SVByteValueObjectClass(LPCTSTR ObjectName)
				       : base(ObjectName) 
{
	LocalInitialize();
}


SVByteValueObjectClass::SVByteValueObjectClass(SVObjectClass* POwner, int StringResourceID)
				       : base(POwner, StringResourceID) 
{
	LocalInitialize();
}

SVByteValueObjectClass::SVByteValueObjectClass( const SVByteValueObjectClass& rhs )
	: base()
{
	LocalInitialize();
	*this = rhs;
}


const SVByteValueObjectClass& SVByteValueObjectClass::operator = ( const SVByteValueObjectClass& rhs )
{
	base::operator = (rhs);
	return *this;
}

SVByteValueObjectClass::~SVByteValueObjectClass()
{
	// Delete all object's data arrays...
	SetObjectDepth(0);
	CreateBuckets();
}

void SVByteValueObjectClass::Persist(SVObjectWriter& rWriter)
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	// Get the Heading (Class Info)
	SVValueObjectClass::Persist( rWriter );

	// Get the Data Values (Member Info, Values)
	_variant_t value(DefaultValue());
	
	rWriter.WriteAttribute(scDefaultTag, value);

	rWriter.StartElement(scArrayElementsTag);
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;	

	// for all elements in the array (m_iArraySize)
	for( int i = 0; i < m_iArraySize; i++ )
	{
		value.bVal = Element(m_iLastSetIndex, i);
		list.push_back(value);
	}
	rWriter.WriteAttribute(scElementTag, list);
	rWriter.EndElement();

	rWriter.EndElement();
}

// This override provides the ability to correctly load script data from the legacy SVByteVectorObjectClass
HRESULT SVByteValueObjectClass::SetObjectValue( SVObjectAttributeClass* pDataObject )
{
	HRESULT hr = S_FALSE;
	
	SvCl::SVObjectArrayClassTemplate<value_type> svArray;	// for default values

	if ( m_bLegacyVectorObjectCompatibility )
	{
		if (   SVObjectAttributeClassHelper::GetAttributeData(pDataObject, "pArray", svArray)
			|| SVObjectAttributeClassHelper::GetAttributeData(pDataObject, BUCKET_TAG_LOAD, svArray) )
		{
			SetArraySize( static_cast< int >( svArray.size() ) );
			hr = SetArrayValues(1, svArray.begin(), svArray.end());
			return hr;
		}
	}

	hr = base::SetObjectValue( pDataObject );
	return hr;
}

HRESULT SVByteValueObjectClass::SetValueAt( int iBucket, int iIndex, const VARIANT& vtValue )
{
	if ( vtValue.vt == VT_UI1 || vtValue.vt == VT_I1 )
		return base::SetValueAt(iBucket, iIndex, vtValue.bVal);
	ASSERT(FALSE);
	return S_FALSE;
}

HRESULT SVByteValueObjectClass::SetValueAt( int iBucket, int iIndex, CString strValue )
{
	CString strDigits( strValue );
	StringMunge::KeepChars( &strDigits, _T("0123456789 .xXabcdefABCDEF") );
	if ( strDigits == strValue )
	{
		strDigits.MakeLower();
		TCHAR* p = NULL;
		DWORD ulValue;
		if ( strDigits.Find(_T('x')) != -1  )
			ulValue = _tcstoul(strDigits, &p, 16);
		else
			ulValue = _tcstoul(strDigits, &p, 10);

		if ( ulValue <= 255 )
			return base::SetValueAt(iBucket, iIndex, static_cast <BYTE> (ulValue) );
	}
	ASSERT(FALSE);
	return S_FALSE;
}

HRESULT SVByteValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rvtValue ) const
{
	BYTE l_byValue=0;
	_variant_t l_Temp;
	l_Temp.Attach( rvtValue );
	HRESULT hr = base::GetValueAt( iBucket, iIndex, l_byValue );
	if( hr == S_OK )
	{
		l_Temp = l_byValue;
	}
	else
	{
		l_Temp.Clear();
	}
	rvtValue = l_Temp.Detach();
	return hr;
}

HRESULT SVByteValueObjectClass::GetValueAt( int iBucket, int iIndex, CString& rstrValue) const
{
	BYTE value=0;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	rstrValue.Format(_T("0x%2.2x"), value);

	return hr;
}

HRESULT SVByteValueObjectClass::GetValueAt( int iBucket, int iIndex, double& rdValue) const
{
	BYTE value=0;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	rdValue = (double) value;

	return hr;
}

void SVByteValueObjectClass::LocalInitialize()
{
	outObjectInfo.ObjectTypeInfo.ObjectType = SVByteValueObjectType;
	DefaultValue() = 0;
	if ( m_sLegacyScriptDefaultName.IsEmpty() )
	{
		m_sLegacyScriptDefaultName = "bDefault";
		m_sLegacyScriptArrayName = "pBArray";
	}
	m_strTypeName = "Integer8";
	InitializeBuckets();
}


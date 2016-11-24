//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInt64ValueObjectClass
//* .File Name       : $Workfile:   SVInt64ValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   30 Jul 2013 12:24:32  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVInt64ValueObjectClass.h"
#include "SVObjectLibrary\SVClsids.h"
#include "SVObjectLibrary/SVToolsetScriptTags.h"
#include "SVOMFCLibrary/StringMunge.h"
#pragma endregion Includes

namespace	// only for this file
{
	const CString DEFAULT_TAG_SAVE(_T(".Default"));
	const CString BUCKET_TAG_SAVE(_T(".Array"));	// for backwards compatibility
	const CString ARRAY_TAG_SAVE(_T(".Array_Elements"));	// new style; one bucket, all array values

	const CString DEFAULT_TAG_LOAD(_T("Default"));
	const CString BUCKET_TAG_LOAD(_T("Array"));	// for backwards compatibility
	const CString ARRAY_TAG_LOAD(_T("Array_Elements"));	// new style; one bucket, all array values
}	// end file scope namespace

SV_IMPLEMENT_CLASS(SVInt64ValueObjectClass, SVInt64ValueObjectClassGuid);

SVInt64ValueObjectClass::SVInt64ValueObjectClass(LPCTSTR ObjectName)
						: base(ObjectName)
{
	LocalInitialize();
}

SVInt64ValueObjectClass::SVInt64ValueObjectClass(SVObjectClass* pOwner, int StringResourceID)
						: base(pOwner, StringResourceID)
{
	LocalInitialize();
}

SVInt64ValueObjectClass::~SVInt64ValueObjectClass()
{
	SetObjectDepth( 0 );
	ResetObject();
}

const SVInt64ValueObjectClass& SVInt64ValueObjectClass::operator = (const SVInt64ValueObjectClass& rhs)
{
	base::operator = (rhs);
	return *this;
}

void SVInt64ValueObjectClass::Persist(SVObjectWriter& rWriter)
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	// Get the Heading (Class Info)
	SVValueObjectClass::Persist( rWriter );

	// Get the Data Values (Member Info, Values)
	_variant_t value;
	value.ChangeType(VT_I8);
	value.llVal = DefaultValue();

	rWriter.WriteAttribute(scDefaultTag, value);

	rWriter.StartElement(scArrayElementsTag);
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;	

	// for all elements in the array (m_iArraySize)
	for( int i = 0; i < m_iArraySize; i++ )
	{
		value.llVal = Element(m_iLastSetIndex, i);
		list.push_back(value);
	}
	rWriter.WriteAttribute(scElementTag, list);
	rWriter.EndElement();

	rWriter.EndElement();
}
HRESULT SVInt64ValueObjectClass::SetValueAt(int iBucket, int iIndex, const int iValue)
{
    return base::SetValueAt(iBucket, iIndex, static_cast <__int64> (iValue) );
}

HRESULT SVInt64ValueObjectClass::SetValueAt(int iBucket, int iIndex, const CString& strValue)
{
	CString strDigits( strValue );
	StringMunge::KeepChars( &strDigits, _T("0123456789- ") );
	if ( strDigits == strValue )
	{
		__int64 iValue = _atoi64( strDigits );
		return base::SetValueAt(iBucket, iIndex, iValue);
	}
	assert( false );
	return S_FALSE;
}

HRESULT SVInt64ValueObjectClass::SetValueAt( int iBucket, int iIndex, double value )
{
	return base::SetValueAt(iBucket, iIndex, static_cast <__int64> (value) );
}

HRESULT SVInt64ValueObjectClass::GetValueAt(int iBucket, int iIndex, double& rdValue) const
{
	__int64 value=0;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	rdValue = (double) value;

	return hr;
}

HRESULT SVInt64ValueObjectClass::GetValueAt(int iBucket, int iIndex, CString& rstrValue) const
{
	__int64 value=0;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	rstrValue.Format("%d",value);

	return hr;
}

HRESULT SVInt64ValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const
{
	__int64 l_i64Value=0;
	_variant_t l_Temp;
	l_Temp.Attach( rValue );
	HRESULT hr = base::GetValueAt( iBucket, iIndex, l_i64Value );
	if( S_OK == hr )
	{
		l_Temp = l_i64Value;
	}
	else
	{
		l_Temp.Clear();
	}
	rValue = l_Temp.Detach();

	return hr;
}

void SVInt64ValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVInt64ValueObjectType;
	DefaultValue() = 0;
	m_sLegacyScriptDefaultName = _T("Default");
	m_sLegacyScriptArrayName = _T("Array");
	SetTypeName( _T("Integer64") );

	InitializeBuckets();
}


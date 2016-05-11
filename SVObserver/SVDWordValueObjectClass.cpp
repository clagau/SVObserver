//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDWordValueObjectClass
//* .File Name       : $Workfile:   SVDWordValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   30 Jul 2013 12:28:30  $
//******************************************************************************

#include "stdafx.h"
#include "SVDWordValueObjectClass.h"
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

SV_IMPLEMENT_CLASS(SVDWordValueObjectClass, SVDWordValueObjectClassGuid);

SVDWordValueObjectClass::SVDWordValueObjectClass(LPCTSTR ObjectName)
				        : base(ObjectName) 
{
	LocalInitialize();
}


SVDWordValueObjectClass::SVDWordValueObjectClass(SVObjectClass* POwner, int StringResourceID)
				        : base(POwner, StringResourceID) 
{
	LocalInitialize();
}

SVDWordValueObjectClass::SVDWordValueObjectClass( const SVDWordValueObjectClass& rhs )
	: base()
{
	LocalInitialize();
	*this = rhs;
}

const SVDWordValueObjectClass& SVDWordValueObjectClass::operator = ( const SVDWordValueObjectClass& rhs )
{
	base::operator = (rhs);
	return *this;
}

SVDWordValueObjectClass::~SVDWordValueObjectClass()
{
	// Delete all object's data arrays...
	SetObjectDepth(0);
	CreateBuckets();
}

void SVDWordValueObjectClass::Persist(SVObjectWriter& rWriter)
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	// Get the Heading (Class Info)
	SVValueObjectClass::Persist( rWriter );

	// Get the Data Values (Member Info, Values)
	_variant_t value(DefaultValue());
	value.ChangeType(VT_UI4);
	
	rWriter.WriteAttribute(scDefaultTag, value);

	rWriter.StartElement(scArrayElementsTag);

	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;	

	// for all elements in the array (m_iArraySize)
	for( int i = 0; i < m_iArraySize; i++ )
	{
		value.ulVal = Element(m_iLastSetIndex, i);
		list.push_back(value);
	}
	rWriter.WriteAttribute(scElementTag, list);
	rWriter.EndElement();

	rWriter.EndElement();
}

HRESULT  SVDWordValueObjectClass::SetValueAt( int iBucket, int iIndex, const VARIANT& rvtValue )
{
	if ( rvtValue.vt == VT_UI4 )
		return base::SetValueAt(iBucket, iIndex, (DWORD) rvtValue.ulVal);
	ASSERT(FALSE);
	return S_FALSE;
}

HRESULT SVDWordValueObjectClass::SetValueAt( int iBucket, int iIndex, CString strValue )
{
	CString strDigits (strValue);
	StringMunge::KeepChars( &strDigits, _T("0123456789 .xXabcdefABCDEF") );
	if ( strDigits == strValue )
	{
		strDigits.MakeLower();
		TCHAR* p = nullptr;
		DWORD ulValue;
		if ( strDigits.Find(_T('x')) != -1  )
			ulValue = _tcstoul(strDigits, &p, 16);
		else
			ulValue = _tcstoul(strDigits, &p, 10);

		return base::SetValueAt(iBucket, iIndex, static_cast <DWORD> (ulValue) );
	}
	ASSERT(FALSE);
	return S_FALSE;
}


HRESULT SVDWordValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rvtValue ) const
{
	DWORD l_dwValue = 0;
	_variant_t l_Temp;
	l_Temp.Attach( rvtValue );
	HRESULT hr = base::GetValueAt( iBucket, iIndex, l_dwValue );
	if( S_OK == hr )
	{
		l_Temp = l_dwValue ;
	}
	else
	{
		l_Temp.Clear();
	}
	rvtValue = l_Temp.Detach();

	return hr;
}

HRESULT SVDWordValueObjectClass::GetValueAt( int iBucket, int iIndex, CString& rstrValue) const
{
	DWORD dwValue=0;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, dwValue);
	rstrValue.Format(_T("0x%8.8x"), dwValue);

	return hr;
}


HRESULT SVDWordValueObjectClass::GetValueAt( int iBucket, int iIndex, double& rdValue) const
{
	DWORD dwValue=0;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, dwValue);
	rdValue = (double) dwValue;

	return hr;
}

void SVDWordValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVDWordValueObjectType;
	DefaultValue() = 0;
	if ( m_sLegacyScriptDefaultName.IsEmpty() )
	{
		m_sLegacyScriptDefaultName = "dwDefault";
		m_sLegacyScriptArrayName = "pDwArray";
	}
	m_strTypeName = "Integer32Hex";
	InitializeBuckets();
}


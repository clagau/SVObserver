//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCharValueObjectClass
//* .File Name       : $Workfile:   SVCharValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   30 Jul 2013 12:28:30  $
//******************************************************************************

#include "stdafx.h"
#include "SVCharValueObjectClass.h"
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

SV_IMPLEMENT_CLASS(SVCharValueObjectClass, SVCharValueObjectClassGuid);

SVCharValueObjectClass::SVCharValueObjectClass(LPCTSTR ObjectName)
				       : base(ObjectName) 
{
	LocalInitialize();
}


SVCharValueObjectClass::SVCharValueObjectClass(SVObjectClass* POwner, int StringResourceID)
				       : base(POwner, StringResourceID) 
{
	LocalInitialize();
}

SVCharValueObjectClass::SVCharValueObjectClass( const SVCharValueObjectClass& rhs )
	: base()
{
	LocalInitialize();
	*this = rhs;
}

const SVCharValueObjectClass& SVCharValueObjectClass::operator = ( const SVCharValueObjectClass& rhs )
{
	base::operator = (rhs);
	return *this;
}

SVCharValueObjectClass::~SVCharValueObjectClass()
{
	// Delete all object's data arrays...
	SetObjectDepth(0);
	CreateBuckets();
}

void SVCharValueObjectClass::Persist(SVObjectWriter& rWriter)
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
		value.cVal = Element(m_iLastSetIndex, i);
		list.push_back(value);
	}
	rWriter.WriteAttribute(scElementTag, list);
	rWriter.EndElement();

	rWriter.EndElement();
}

HRESULT SVCharValueObjectClass::SetValueAt( int iBucket, int iIndex, const VARIANT& rvtValue )
{
	if ( rvtValue.vt == VT_I1 )
		return base::SetValueAt(iBucket, iIndex, rvtValue.cVal);
	ASSERT(FALSE);
	return S_FALSE;
}

HRESULT SVCharValueObjectClass::SetValueAt( int iBucket, int iIndex, CString strValue )
{
	if ( strValue.GetLength() == 1 )
	{
		return base::SetValueAt(iBucket, iIndex, static_cast <char> (strValue[0]) );
	}
	else
	{
		CString strDigits( strValue );
		StringMunge::KeepChars( &strDigits, _T("0123456789 .xXabcdefABCDEF") );
		if ( strDigits == strValue )
		{
			strDigits.MakeLower();
			TCHAR* p = NULL;
			long lValue;
			if ( strDigits.Find(_T('x')) != -1  )
				lValue = _tcstol(strDigits, &p, 16);
			else
				lValue = _tcstol(strDigits, &p, 10);

			if ( lValue >= 0 && lValue <= 255 )
				return base::SetValueAt(iBucket, iIndex, static_cast <char> (lValue) );
		}
		else
		{
			// do we want to use the array capabilities to create an array of chars for the string?
			//STATIC_CHECK( sizeof(TCHAR) == sizeof(char), Need_to_decide_what_to_do_here );
		}
	}
	ASSERT(FALSE);
	return S_FALSE;
}


HRESULT SVCharValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rvtValue ) const
{
	char l_cValue=0;
	_variant_t l_Temp;
	l_Temp.Attach( rvtValue );
	HRESULT hr = base::GetValueAt( iBucket, iIndex, l_cValue );
	if( hr == S_OK )
	{
		l_Temp = l_cValue;
	}
	else
	{
		l_Temp.Clear();
	}
	rvtValue = l_Temp.Detach();
	return hr;
}

HRESULT SVCharValueObjectClass::GetValueAt( int iBucket, int iIndex, CString& rstrValue) const
{
	char value=0;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	rstrValue.Format(_T("%c"), value);

	return hr;
}

HRESULT SVCharValueObjectClass::GetValueAt( int iBucket, int iIndex, double& rdValue) const
{
	char value=0;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	rdValue = (double) value;

	return hr;
}

void SVCharValueObjectClass::LocalInitialize()
{
	outObjectInfo.ObjectTypeInfo.ObjectType = SVCharValueObjectType;
	DefaultValue() = 0;
	if ( m_sLegacyScriptDefaultName.IsEmpty() )
	{
		m_sLegacyScriptDefaultName = "bDefault";
		m_sLegacyScriptArrayName = "pBArray";
	}
	m_strTypeName = "Char";
	InitializeBuckets();
}


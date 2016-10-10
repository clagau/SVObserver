//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLongValueObjectClass
//* .File Name       : $Workfile:   SVLongValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   30 Jul 2013 12:29:28  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVLongValueObjectClass.h"
#include "SVObjectLibrary\SVClsids.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVOMFCLibrary/StringMunge.h"
#include "SVStatusLibrary/MessageManager.h"
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

SV_IMPLEMENT_CLASS(SVLongValueObjectClass, SVLongValueObjectClassGuid);

SVLongValueObjectClass::SVLongValueObjectClass(LPCTSTR ObjectName)
				       : base(ObjectName) 
{
	LocalInitialize();
}


SVLongValueObjectClass::SVLongValueObjectClass(SVObjectClass* POwner, int StringResourceID)
				       : base(POwner, StringResourceID) 
{
	LocalInitialize();
}

SVLongValueObjectClass::SVLongValueObjectClass( const SVLongValueObjectClass& rhs )
	: base()
{
	LocalInitialize();
	*this = rhs;
}

const SVLongValueObjectClass& SVLongValueObjectClass::operator = (const SVLongValueObjectClass& rhs)
{
	base::operator = (rhs);
	return *this;
}

SVLongValueObjectClass::~SVLongValueObjectClass()
{
	// Delete all object's data arrays...
	SetObjectDepth(0);
	CreateBuckets();
}

void SVLongValueObjectClass::Persist(SVObjectWriter& rWriter)
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	// Get the Heading (Class Info)
	SVValueObjectClass::Persist( rWriter );

	// Get the Data Values (Member Info, Values)
	_variant_t value(DefaultValue());
	value.ChangeType(VT_I4);
	
	rWriter.WriteAttribute(scDefaultTag, value);

	rWriter.StartElement(scArrayElementsTag);

	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;	

	// for all elements in the array (m_iArraySize)
	for( int i = 0; i < m_iArraySize; i++ )
	{
		value.lVal = Element(m_iLastSetIndex, i);
		list.push_back(value);
	}
	rWriter.WriteAttribute(scElementTag, list);
	rWriter.EndElement();

	rWriter.EndElement();
}

HRESULT SVLongValueObjectClass::SetValueAt(int iBucket, int iIndex, const double dValue)
{
    return base::SetValueAt(iBucket, iIndex, (long) dValue);
}

HRESULT SVLongValueObjectClass::SetValueAt(int iBucket, int iIndex, const int iValue)
{
    return base::SetValueAt(iBucket, iIndex, (long) iValue);
}

HRESULT SVLongValueObjectClass::SetValueAt( int iBucket, int iIndex, const VARIANT& rvtValue )
{
	if ( VT_I4 == rvtValue.vt )
	{
		return base::SetValueAt(iBucket, iIndex, (long) rvtValue.lVal);
	}
	assert(false);
	return S_FALSE;
}

HRESULT SVLongValueObjectClass::SetValueAt( int iBucket, int iIndex, const CString& strValue )
{
	try
	{
		long value = convertString2Long(strValue);
		return base::SetValueAt(iBucket, iIndex, value );
	}
	catch (const SvStl::MessageContainer&)
	{
		ASSERT(FALSE);
		return S_FALSE;
	}
}

HRESULT SVLongValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rvtValue ) const
{
	long l_lValue=0;
	_variant_t l_Temp;
	l_Temp.Attach( rvtValue );
	HRESULT hr = base::GetValueAt( iBucket, iIndex, l_lValue );
	if( S_OK == hr )
	{
		l_Temp = l_lValue;
	}
	else
	{
		l_Temp.Clear();
	}
	rvtValue = l_Temp.Detach();

	return hr;
}

HRESULT SVLongValueObjectClass::GetValueAt( int iBucket, int iIndex, CString& rstrValue) const
{
	long lValue=0;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, lValue);
	rstrValue.Format(_T("%d"), lValue);

	return hr;
}

HRESULT SVLongValueObjectClass::GetValueAt( int iBucket, int iIndex, double& rdValue) const
{
	long lValue=0;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, lValue);
	rdValue = (double) lValue;

	return hr;
}

void SVLongValueObjectClass::ValidateValue( int iBucket, int iIndex, const SVString& rValue ) const
{
	convertString2Long(rValue.c_str());
	base::ValidateValue( iBucket, iIndex, rValue );
}

void SVLongValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVLongValueObjectType;
	DefaultValue() = 0;
	if ( m_sLegacyScriptDefaultName.IsEmpty() )
	{
		m_sLegacyScriptDefaultName = "lDefault";
		m_sLegacyScriptArrayName = "pLArray";
	}
	m_strTypeName = "Integer32";
	InitializeBuckets();
}

long SVLongValueObjectClass::convertString2Long(const CString& rValue ) const
{
	CString strDigits (rValue);
	StringMunge::KeepChars( &strDigits, _T("-0123456789 .xXabcdefABCDEF") );
	if ( strDigits == rValue )
	{
		strDigits.MakeLower();
		TCHAR* p = nullptr;
		long lValue;
		if ( strDigits.Find(_T('x')) != -1 )
		{
			lValue = _tcstol(strDigits, &p, 16);
		}
		else
		{
			lValue = _tcstol(strDigits, &p, 10);
		}

		return lValue;
	}
	SVStringArray msgList;
	msgList.push_back(SVString(rValue));
	msgList.push_back(GetName());
	SvStl::MessageMgrStd Exception( SvStl::LogOnly );
	Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
	Exception.Throw();
	return 0; //will never reached, because the exception will throw before. But this line avoid a warning
}
//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDoubleValueObjectClass
//* .File Name       : $Workfile:   SVDoubleValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Oct 2013 14:12:22  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVDoubleValueObjectClass.h"
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

SV_IMPLEMENT_CLASS(SVDoubleValueObjectClass, SVDoubleValueObjectClassGuid);

SVDoubleValueObjectClass::SVDoubleValueObjectClass(LPCTSTR ObjectName)
				         : base(ObjectName) 
{
	LocalInitialize();
}

SVDoubleValueObjectClass::SVDoubleValueObjectClass(SVObjectClass* POwner, int StringResourceID)
				         : base(POwner, StringResourceID) 
{
	LocalInitialize();
}

SVDoubleValueObjectClass::SVDoubleValueObjectClass( const SVDoubleValueObjectClass& rhs )
	: base()
{
	LocalInitialize();
	*this = rhs;
}

const SVDoubleValueObjectClass& SVDoubleValueObjectClass::operator = (const SVDoubleValueObjectClass& rhs)
{
	base::operator = (rhs);
	return *this;
}

SVDoubleValueObjectClass::~SVDoubleValueObjectClass()
{
	// Delete all object's data arrays...
	SetObjectDepth(0);
	CreateBuckets();
}

void SVDoubleValueObjectClass::Persist(SVObjectWriter& rWriter)
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
		value.dblVal = Element(m_iLastSetIndex, i);
		list.push_back(value);
	}
	rWriter.WriteAttribute(scElementTag, list);
	rWriter.EndElement();

	rWriter.EndElement();
}

// This override provides the ability to correctly load script data from the legacy SVDoubleVectorObjectClass
HRESULT SVDoubleValueObjectClass::SetObjectValue( SVObjectAttributeClass* pDataObject )
{
	HRESULT hr = S_FALSE;
	
	SvCl::SVObjectArrayClassTemplate<value_type> svArray;	// for default values

	if ( m_bLegacyVectorObjectCompatibility )
	{
		if ( SVObjectAttributeClassHelper::GetAttributeData(pDataObject, "bDefault", svArray) )
		{
			if ( svArray.GetSize() > 0 )
				DefaultValue() = svArray[ svArray.GetSize()-1 ];
			hr = S_OK;
			return hr;
		}
		else if (    SVObjectAttributeClassHelper::GetAttributeData(pDataObject, "pArray", svArray)
		          || SVObjectAttributeClassHelper::GetAttributeData(pDataObject, BUCKET_TAG_LOAD, svArray) )
		{
			SetArraySize( static_cast<int>(svArray.size()) );
			hr = SetArrayValues(1, svArray.begin(), svArray.end());
			return hr;
		}
	}
	hr = base::SetObjectValue( pDataObject );
	return hr;
}

HRESULT SVDoubleValueObjectClass::SetValueAt( int iBucket, int iIndex, const VARIANT& rvtValue )
{
	if ( VT_R8 == rvtValue.vt )
	{
		return base::SetValueAt(iBucket, iIndex, rvtValue.dblVal);
	}
	assert(false);
	return S_FALSE;
}

HRESULT SVDoubleValueObjectClass::SetValueAt(int iBucket, int iIndex, const int value)
{
    return base::SetValueAt(iBucket, iIndex, (double) value);
}

HRESULT SVDoubleValueObjectClass::SetValueAt(int iBucket, int iIndex, const long value)
{
    return base::SetValueAt(iBucket, iIndex, (double) value);
}

HRESULT SVDoubleValueObjectClass::SetValueAt( int iBucket, int iIndex, const CString& strValue )
{
	try
	{
		ValidateValue( iBucket, iIndex, SVString(strValue) );
		double dValue = atof(strValue);
		return base::SetValueAt(iBucket, iIndex, dValue );
	}
	catch(const SvStl::MessageContainer&)
	{
		ASSERT(FALSE);
		return S_FALSE;
	}
}

HRESULT SVDoubleValueObjectClass::GetValueAt( int iBucket, int iIndex, long& rlValue ) const
{
	double dValue=0.0;

	HRESULT hr = base::GetValueAt( iBucket, iIndex, dValue );
	rlValue = static_cast<long>(dValue);

	return hr;
}

HRESULT SVDoubleValueObjectClass::GetValueAt( int iBucket, int iIndex, DWORD& rdwValue ) const
{
	double dValue=0.0;

	HRESULT hr = base::GetValueAt( iBucket, iIndex, dValue );
	rdwValue = static_cast<unsigned long>(dValue);

	return hr;
}

HRESULT SVDoubleValueObjectClass::GetValueAt( int iBucket, int iIndex, CString& rstrValue) const
{
	double value = 0.0;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	rstrValue.Format(_T("%lf"), value);

	return hr;
}

HRESULT SVDoubleValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rvtValue ) const
{
	double l_dValue=0;
	_variant_t l_Temp;
	l_Temp.Attach( rvtValue );
	HRESULT hr = base::GetValueAt( iBucket, iIndex, l_dValue );
	if( S_OK == hr )
	{
		l_Temp = l_dValue;
	}
	else
	{
		l_Temp.Clear();
	}
	rvtValue = l_Temp.Detach();
	return hr;
}

void SVDoubleValueObjectClass::ValidateValue( int iBucket, int iIndex, const SVString& rValue ) const
{
	CString strDigits (rValue.c_str());
	StringMunge::KeepChars( &strDigits, _T("-0123456789. ") );
	if ( strDigits != rValue.c_str() )
	{
		SVStringArray msgList;
		msgList.push_back(GetName());
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_LinkedValue_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
		Exception.Throw();
	}
	base::ValidateValue( iBucket, iIndex, rValue );
}

void SVDoubleValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVDoubleValueObjectType;
	DefaultValue() = 0.0;
	if ( m_sLegacyScriptDefaultName.IsEmpty() )
	{
		m_sLegacyScriptDefaultName = "dDefault";
		m_sLegacyScriptArrayName = "pDArray";
	}
	m_strTypeName = "Decimal";
	InitializeBuckets();
}


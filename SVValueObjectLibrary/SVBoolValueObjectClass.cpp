//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBoolValueObjectClass
//* .File Name       : $Workfile:   SVBoolValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   30 Oct 2013 15:18:36  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVBoolValueObjectClass.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVObjectLibrary\SVClsids.h"
#include "SVStatusLibrary\MessageManager.h"
#pragma endregion Includes

namespace	// only for this file
{
	const CString DEFAULT_TAG_SAVE(_T(".Default"));
	const CString BUCKET_TAG_SAVE(_T(".Array"));	// for backwards compatibility
	const CString ARRAY_TAG_SAVE(_T(".Array_Elements"));	// new style; one bucket, all array values

	const CString DEFAULT_TAG_LOAD(_T("Default"));
	const CString BUCKET_TAG_LOAD(_T("Array"));	// for backwards compatibility
	const CString ARRAY_TAG_LOAD(_T("Array_Elements"));	// new style; one bucket, all array values

	const CString scTrue = _T("TRUE");
	const CString scFalse = _T("FALSE");
}	// end file scope namespace

SV_IMPLEMENT_CLASS(SVBoolValueObjectClass, SVBoolValueObjectClassGuid);

/*static*/
HRESULT SVBoolValueObjectClass::GetNormalizedValue( const CString& p_strValue, CString& rstrNormalized )
{
	HRESULT hr = S_FALSE;
	CString strValue(p_strValue);
	strValue.TrimLeft();
	strValue.TrimRight();
	if ( strValue == _T("1") || strValue == _T("-1") /*for VB weenies*/ || (strValue.CompareNoCase(scTrue) == 0) )
	{
		rstrNormalized = scTrue;
		hr = S_OK;
	}
	else if ( strValue == _T("0") || (strValue.CompareNoCase(scFalse) == 0) )
	{
		rstrNormalized = scFalse;
		hr = S_OK;
	}
	return hr;
}

SVBoolValueObjectClass::SVBoolValueObjectClass(LPCTSTR ObjectName)
				       :base(ObjectName) 
{
	LocalInitialize();
}

SVBoolValueObjectClass::SVBoolValueObjectClass(SVObjectClass* POwner, int StringResourceID)
				       :base(POwner, StringResourceID) 
{
	LocalInitialize();
}

SVBoolValueObjectClass::SVBoolValueObjectClass(const SVBoolValueObjectClass& rhs)
	:base()
{
	LocalInitialize();
	*this = rhs;
}

SVBoolValueObjectClass::~SVBoolValueObjectClass()
{
	// Delete all object's data arrays...
	SetObjectDepth(0);
	CreateBuckets();
}

const SVBoolValueObjectClass& SVBoolValueObjectClass::operator = (const SVBoolValueObjectClass& rhs)
{
	base::operator = (rhs);
	return *this;
}

void SVBoolValueObjectClass::Persist(SVObjectWriter& rWriter)
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	// Get the Heading (Class Info)
	SVValueObjectClass::Persist( rWriter );

	// Get the Data Values (Member Info, Values)
	_variant_t value;
	value.ChangeType(VT_BOOL);
	value.boolVal = (DefaultValue() ? VARIANT_TRUE : VARIANT_FALSE);

	rWriter.WriteAttribute(scDefaultTag, value);

	rWriter.StartElement(scArrayElementsTag);
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;	

	// for all elements in the array (m_iArraySize)
	for( int i = 0; i < m_iArraySize; i++ )
	{
		value.boolVal = (Element(m_iLastSetIndex, i) ? VARIANT_TRUE : VARIANT_FALSE);
		list.push_back(value);
	}
	rWriter.WriteAttribute(scElementTag, list);
	rWriter.EndElement();

	rWriter.EndElement();
}

HRESULT SVBoolValueObjectClass::GetValidTypes( SVValidTypesVector& p_astrTypes ) const
{
	p_astrTypes.push_back(scTrue);
	p_astrTypes.push_back(scFalse);
	return S_OK;
}

HRESULT SVBoolValueObjectClass::SetValueAt( int iBucket, int iIndex, const double dValue )
{
	BOOL bValue = dValue != 0.0f;
	return base::SetValueAt(iBucket, iIndex, bValue);
}

HRESULT SVBoolValueObjectClass::SetValueAt( int iBucket, int iIndex, const long lValue )
{
	BOOL bValue = lValue != 0;
	return base::SetValueAt(iBucket, iIndex, bValue);
}

HRESULT SVBoolValueObjectClass::SetValueAt( int iBucket, int iIndex, const bool bValue )
{
	BOOL l_bValue = bValue ? TRUE : FALSE;
	return base::SetValueAt(iBucket, iIndex, l_bValue);
}

HRESULT SVBoolValueObjectClass::SetValueAt( int iBucket, int iIndex, const CString& strValue )
{
	try
	{
		BOOL l_bValue = ConvertString2Bool(strValue);
		return base::SetValueAt(iBucket, iIndex, l_bValue);
	}
	catch (const SvStl::MessageContainer&)
	{
		assert(false);
		return S_FALSE;
	}
}

HRESULT SVBoolValueObjectClass::SetValueAt( int iBucket, int iIndex, const VARIANT& rvtValue )
{
	if ( VT_BOOL == rvtValue.vt )
	{
		return base::SetValueAt(iBucket, iIndex, (BOOL) rvtValue.boolVal);
	}
	assert(false);
	return S_FALSE;
}

HRESULT SVBoolValueObjectClass::GetValueAt( int iBucket, int iIndex, bool& rbValue) const
{
	BOOL bValue = FALSE;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, bValue);
	rbValue = bValue != FALSE;

	return hr;
}

HRESULT SVBoolValueObjectClass::GetValueAt( int iBucket, int iIndex, CString& rstrValue) const
{
	BOOL bValue = FALSE;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, bValue);
	rstrValue = (bValue ? scTrue : scFalse);

	return hr;
}

HRESULT SVBoolValueObjectClass::GetValueAt( int iBucket, int iIndex, double& rdValue) const
{
	BOOL bValue = FALSE;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, bValue);
	rdValue = (double) bValue;

	return hr;
}
	
HRESULT SVBoolValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rvtValue ) const
{
	BOOL l_bValue=0;
	_variant_t l_Temp;
	l_Temp.Attach( rvtValue );
	HRESULT hr = base::GetValueAt( iBucket, iIndex, l_bValue );
	if( S_OK == hr )
	{
		l_Temp = static_cast<VARIANT_BOOL>(l_bValue) ;
	}
	else
	{
		l_Temp.Clear();
	}
	rvtValue = l_Temp.Detach();

	return hr;
}

void SVBoolValueObjectClass::ValidateValue( int iBucket, int iIndex, const SVString& rValue ) const
{
	ConvertString2Bool(rValue.c_str());
	base::ValidateValue( iBucket, iIndex, rValue );
}

HRESULT SVBoolValueObjectClass::CompareWithCurrentValueImpl( const CString& rstrCompare ) const
{
	CString strValue;
	GetValue( strValue );
	return S_OK;
}

HRESULT SVBoolValueObjectClass::GetNormalizedValueImpl( const CString& strValue, CString& rstrNormalized ) const
{
	return SVBoolValueObjectClass::GetNormalizedValue(strValue, rstrNormalized);
}

void SVBoolValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVBoolValueObjectType;
	DefaultValue() = false;
	if ( m_sLegacyScriptDefaultName.IsEmpty() )
	{
		m_sLegacyScriptDefaultName = "bDefault";
		m_sLegacyScriptArrayName = "pBArray";
	}
	m_strTypeName = "Bool";

	InitializeBuckets();
}

BOOL SVBoolValueObjectClass::ConvertString2Bool( const CString &rValue ) const
{
	bool Result(TRUE);
	CString value = rValue;
	value.TrimLeft();
	value.TrimRight();
	if ( _T("1") == value  || _T("-1") == value /*for VB weenies*/ || (value.CompareNoCase(scTrue) == 0) )
	{
		Result = TRUE;
	}
	else if ( _T("0") == value || (value.CompareNoCase(scFalse) == 0) )
	{
		Result = FALSE;
	}
	else
	{
		SVStringArray msgList;
		msgList.push_back(SVString(value));
		msgList.push_back(GetName());
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
		Exception.Throw();
	}

	return Result;
}


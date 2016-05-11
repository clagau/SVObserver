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

#include "stdafx.h"
#include "SVBoolValueObjectClass.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"

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

HRESULT SVBoolValueObjectClass::SetValueAt( int iBucket, int iIndex, CString strValue )
{
	BOOL l_bValue;
	strValue.TrimLeft();
	strValue.TrimRight();
	if ( strValue == _T("1") || strValue == _T("-1") /*for VB weenies*/ || (strValue.CompareNoCase(scTrue) == 0) )
	{
		l_bValue = TRUE;
	}
	else if ( strValue == _T("0") || (strValue.CompareNoCase(scFalse) == 0) )
	{
		l_bValue = FALSE;
	}
	else
	{
		ASSERT(FALSE);
		return S_FALSE;
	}

	return base::SetValueAt(iBucket, iIndex, l_bValue);
}

HRESULT SVBoolValueObjectClass::SetValueAt( int iBucket, int iIndex, const VARIANT& rvtValue )
{
	if ( rvtValue.vt == VT_BOOL )
		return base::SetValueAt(iBucket, iIndex, (BOOL) rvtValue.boolVal);
	ASSERT(FALSE);
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


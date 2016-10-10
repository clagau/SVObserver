//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPointValueObjectClass
//* .File Name       : $Workfile:   SVPointValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Aug 2013 11:21:42  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVPointValueObjectClass.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVObjectLibrary\SVClsids.h"
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

SV_IMPLEMENT_CLASS(SVPointValueObjectClass, SVPointValueObjectClassGuid);

SVPointValueObjectClass::SVPointValueObjectClass(LPCTSTR ObjectName)
				        : base(ObjectName) 
{
	LocalInitialize();
}


SVPointValueObjectClass::SVPointValueObjectClass(SVObjectClass* POwner, int StringResourceID)
				        : base(POwner, StringResourceID) 
{
	LocalInitialize();
}

SVPointValueObjectClass::SVPointValueObjectClass( const SVPointValueObjectClass& rhs )
	: base()
{
	LocalInitialize();
	*this = rhs;
}

const SVPointValueObjectClass& SVPointValueObjectClass::operator = (const SVPointValueObjectClass& rhs)
{
	base::operator = (rhs);
	return *this;
}

SVPointValueObjectClass::~SVPointValueObjectClass()
{
	// Delete all object's data arrays...
	SetObjectDepth(0);
	CreateBuckets();
}

void SVPointValueObjectClass::Persist(SVObjectWriter& rWriter)
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	// Get the Heading (Class Info)
	SVValueObjectClass::Persist(rWriter);
	
	// Get the Data Values (Member Info, Values)
	CString tmp;
	tmp.Format(_T("%d, %d"), DefaultValue().x(), DefaultValue().y());
	_variant_t value;
	value.SetString(tmp);
	rWriter.WriteAttribute(scDefaultTag, value);
	value.Clear();
	
	rWriter.StartElement(scArrayElementsTag);

	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;

	// for all elements in the array (objectDepth)
	for (int i = 0; i < m_iArraySize; i++)
	{
		tmp.Format(_T("%d, %d"), Element(m_iLastSetIndex, i).x(), Element(m_iLastSetIndex, i).y());
		value.SetString(tmp);
		list.push_back(value);
		value.Clear();
	}
	rWriter.WriteAttribute(scElementTag, list);
	rWriter.EndElement();

	rWriter.EndElement();
}

HRESULT SVPointValueObjectClass::GetDefaultValue(POINT& rPoint) const
{
	SVPOINT value;
	HRESULT hrOk = base::GetDefaultValue(value);
	rPoint = value;
	return hrOk;
}

HRESULT SVPointValueObjectClass::SetValueAt( int iBucket, int iIndex, const POINT value )
{
	return base::SetValueAt(iBucket, iIndex, SVPOINT(value));
}

HRESULT SVPointValueObjectClass::SetValueAt( int iBucket, int iIndex, double value )
{
	assert(false);
	return S_FALSE;
}

HRESULT SVPointValueObjectClass::SetValueAt( int iBucket, int iIndex, const CString& strValue )
{
	try
	{
		SVPOINT value = convertString2Point(strValue);
		return base::SetValueAt(iBucket, iIndex, value );
	}
	catch (const SvStl::MessageContainer&)
	{
		assert(false);
		return S_FALSE;
	}
}

HRESULT SVPointValueObjectClass::GetValueAt( int iBucket, int iIndex, POINT& rPoint) const
{
	SVPOINT value;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	rPoint = value;

	return hr;
}

HRESULT SVPointValueObjectClass::GetValueAt( int iBucket, int iIndex, CString& rstrValue) const
{
	SVPOINT value;
	
	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	rstrValue.Format(_T("( %d, %d)"), value.x(), value.y());

	return hr;
}

HRESULT SVPointValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const
{
	SVPOINT l_Point;
	_variant_t l_Temp;
	l_Temp.Attach( rValue );

	HRESULT hr = base::GetValueAt( iBucket, iIndex, l_Point );
	if( S_OK == hr )
	{
		CString l_strTmp;
		l_strTmp.Format(  _T( "( %d, %d )"),l_Point.x(), l_Point.y());
		l_Temp = l_strTmp;
	}
	else
	{
		l_Temp.Clear();
	}
	rValue = l_Temp.Detach();

	return hr;
}

HRESULT SVPointValueObjectClass::GetValueAt( int iBucket, int iIndex, double& rValue ) const
{
	assert(false);
	return S_FALSE;
}

void SVPointValueObjectClass::ValidateValue( int iBucket, int iIndex, const SVString& rValue ) const
{
	convertString2Point(rValue.c_str());
	base::ValidateValue( iBucket, iIndex, rValue );
}

void SVPointValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVPointValueObjectType;
	if ( m_sLegacyScriptDefaultName.IsEmpty() )
	{
		m_sLegacyScriptDefaultName = "defaultPoint";
		m_sLegacyScriptArrayName = "pArray";
	}	
	ObjectAttributesAllowedRef() = SV_VIEWABLE | SV_ARCHIVABLE | SV_EMBEDABLE | SV_PRINTABLE | SV_DD_VALUE;

	m_strTypeName = "Point";
	InitializeBuckets();
}

SVPOINT SVPointValueObjectClass::convertString2Point(const CString& rValue ) const
{
	CString strLegalChars (rValue);
	StringMunge::KeepChars( &strLegalChars, _T("0123456789()-, ") );	// only integers
	if ( strLegalChars == rValue )
	{
		StringMunge::StripChars( &strLegalChars, _T("()") );
		int iComma = strLegalChars.Find(_T(','));
		if ( iComma > 0 && iComma < strLegalChars.GetLength() - 1 )
		{
			CString sX = strLegalChars.Left( iComma );
			CString sY = strLegalChars.Mid( iComma + 1 );
			return SVPOINT(atol(sX), atol(sY));
		}
	}
	SVStringArray msgList;
	msgList.push_back(SVString(rValue));
	msgList.push_back(GetName());
	SvStl::MessageMgrStd Exception( SvStl::LogOnly );
	Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
	Exception.Throw();
	return SVPOINT(); //will never reached, because the exception will throw before. But this line avoid a warning
}

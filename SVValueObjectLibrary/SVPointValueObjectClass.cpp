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
#include "SVStatusLibrary/MessageManager.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
}

void SVPointValueObjectClass::Persist(SVObjectWriter& rWriter)
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	// Get the Heading (Class Info)
	SVValueObjectClass::Persist(rWriter);
	
	// Get the Data Values (Member Info, Values)
	SVString tmp;
	tmp = SvUl_SF::Format(_T("%d, %d"), DefaultValue().x(), DefaultValue().y());
	_variant_t value;
	value.SetString(tmp.c_str());
	rWriter.WriteAttribute(scDefaultTag, value);
	value.Clear();
	
	rWriter.StartElement(scArrayElementsTag);

	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;

	// for all elements in the array (objectDepth)
	for (int i = 0; i < m_iArraySize; i++)
	{
		tmp = SvUl_SF::Format(_T("%d, %d"), Element(m_iLastSetIndex, i).x(), Element(m_iLastSetIndex, i).y());
		value.SetString(tmp.c_str());
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

HRESULT SVPointValueObjectClass::SetValueAt( int iBucket, int iIndex, const SVString& strValue )
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

HRESULT SVPointValueObjectClass::GetValueAt( int iBucket, int iIndex, SVString& rValue) const
{
	SVPOINT value;
	
	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	rValue = SvUl_SF::Format( _T("( %d, %d)"), value.x(), value.y() );

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
		SVString l_strTmp = SvUl_SF::Format( _T( "( %d, %d )"), l_Point.x(), l_Point.y());
		l_Temp = l_strTmp.c_str();
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
	if ( m_sLegacyScriptDefaultName.empty() )
	{
		m_sLegacyScriptDefaultName = _T("defaultPoint");
		m_sLegacyScriptArrayName = _T("pArray");
	}	
	ObjectAttributesAllowedRef() = SV_VIEWABLE | SV_ARCHIVABLE | SV_EMBEDABLE | SV_PRINTABLE | SV_DD_VALUE;

	SetTypeName( _T("Point") );
	InitializeBuckets();
}

SVPOINT SVPointValueObjectClass::convertString2Point(const SVString& rValue ) const
{
	SVString LegalChars = SvUl_SF::ValidateString( rValue, _T("0123456789()-, ") );	// only integers
	if ( LegalChars == rValue )
	{
		SvUl_SF::RemoveCharacters( LegalChars, _T("()") );
		size_t Pos = LegalChars.find(_T(','));
		if ( SVString::npos != Pos )
		{
			SVString sX = SvUl_SF::Left( LegalChars, Pos );
			SVString sY = SvUl_SF::Mid( LegalChars, Pos + 1 );
			return SVDPointClass(atof(sX.c_str()), atof(sY.c_str()));
		}
	}
	SVStringVector msgList;
	msgList.push_back(SVString(rValue));
	msgList.push_back(GetName());
	SvStl::MessageMgrStd Exception( SvStl::LogOnly );
	Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
	Exception.Throw();
	return SVPOINT(); //will never reached, because the exception will throw before. But this line avoid a warning
}

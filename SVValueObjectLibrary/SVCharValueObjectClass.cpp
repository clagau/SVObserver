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

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVCharValueObjectClass.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVObjectLibrary\SVClsids.h"
#include "SVStatusLibrary/MessageManager.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

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
	if ( VT_I1 == rvtValue.vt )
	{
		return base::SetValueAt(iBucket, iIndex, rvtValue.cVal);
	}
	assert(false);
	return S_FALSE;
}

HRESULT SVCharValueObjectClass::SetValueAt( int iBucket, int iIndex, const SVString& rValue )
{
	try
	{
		TCHAR value = convertString2Char(rValue);
		return base::SetValueAt(iBucket, iIndex, value );
	}
	catch (const SvStl::MessageContainer&)
	{
		assert(FALSE);
		return S_FALSE;
	}
}

HRESULT SVCharValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const
{
	TCHAR l_cValue=0;
	_variant_t l_Temp;
	l_Temp.Attach( rValue );
	HRESULT hr = base::GetValueAt( iBucket, iIndex, l_cValue );
	if( S_OK == hr )
	{
		l_Temp = l_cValue;
	}
	else
	{
		l_Temp.Clear();
	}
	rValue = l_Temp.Detach();
	return hr;
}

HRESULT SVCharValueObjectClass::GetValueAt( int iBucket, int iIndex, SVString& rValue) const
{
	TCHAR value=0;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	rValue = SvUl_SF::Format(_T("%c"), value);

	return hr;
}

HRESULT SVCharValueObjectClass::GetValueAt( int iBucket, int iIndex, double& rdValue) const
{
	TCHAR value=0;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	rdValue = (double) value;

	return hr;
}

void SVCharValueObjectClass::ValidateValue( int iBucket, int iIndex, const SVString& rValue ) const
{
	convertString2Char(rValue.c_str());
	base::ValidateValue( iBucket, iIndex, rValue );
}

void SVCharValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVCharValueObjectType;
	DefaultValue() = 0;
	if ( m_sLegacyScriptDefaultName.empty() )
	{
		m_sLegacyScriptDefaultName = _T("bDefault");
		m_sLegacyScriptArrayName = _T("pBArray");
	}
	SetTypeName( _T("Char") );
	InitializeBuckets();
}

TCHAR SVCharValueObjectClass::convertString2Char( const SVString& rValue ) const
{
	if ( 1 == rValue.size() )
	{
		return rValue[0];
	}
	else
	{
		SVString Digits = SvUl_SF::ValidateString( rValue, _T("0123456789 .xXabcdefABCDEF") );
		if ( Digits == rValue )
		{
			Digits = SvUl_SF::MakeLower(Digits);
			TCHAR* p = nullptr;
			long lValue;
			if ( SVString::npos != Digits.find( 'x' ) )
			{
				lValue = _tcstol(Digits.c_str(), &p, 16);
			}
			else
			{
				lValue = _tcstol(Digits.c_str(), &p, 10);
			}

			if ( lValue >= 0 && lValue <= 255 )
			{
				return static_cast <char> (lValue);
			}
		}
	}
	SVStringVector msgList;
	msgList.push_back(SVString(rValue));
	msgList.push_back(GetName());
	SvStl::MessageMgrStd Exception( SvStl::LogOnly );
	Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
	Exception.Throw();
	return 0; //will never reached, because the exception will throw before. But this line avoid a warning
}


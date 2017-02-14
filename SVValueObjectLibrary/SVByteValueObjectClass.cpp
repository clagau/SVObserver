//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVByteValueObjectClass
//* .File Name       : $Workfile:   SVByteValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Oct 2013 12:16:26  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVByteValueObjectClass.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVObjectLibrary\SVClsids.h"
#include "SVStatusLibrary/MessageManager.h"
#include "ObjectInterfaces/TextDefineSvOi.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVByteValueObjectClass, SVByteValueObjectClassGuid);

SVByteValueObjectClass::SVByteValueObjectClass(LPCTSTR ObjectName)
				       : base(ObjectName) 
{
	LocalInitialize();
}

SVByteValueObjectClass::SVByteValueObjectClass(SVObjectClass* POwner, int StringResourceID)
				       : base(POwner, StringResourceID) 
{
	LocalInitialize();
}

SVByteValueObjectClass::SVByteValueObjectClass( const SVByteValueObjectClass& rhs )
	: base()
{
	LocalInitialize();
	*this = rhs;
}

const SVByteValueObjectClass& SVByteValueObjectClass::operator = ( const SVByteValueObjectClass& rhs )
{
	base::operator = (rhs);
	return *this;
}

SVByteValueObjectClass::~SVByteValueObjectClass()
{
}

void SVByteValueObjectClass::Persist(SVObjectWriter& rWriter)
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
		value.bVal = Element(m_iLastSetIndex, i);
		list.push_back(value);
	}
	rWriter.WriteAttribute(scElementTag, list);
	rWriter.EndElement();

	rWriter.EndElement();
}

// This override provides the ability to correctly load script data from the legacy SVByteVectorObjectClass
HRESULT SVByteValueObjectClass::SetObjectValue( SVObjectAttributeClass* pDataObject )
{
	HRESULT hr = S_FALSE;
	
	SvCl::SVObjectArrayClassTemplate<value_type> svArray;	// for default values

	if ( m_bLegacyVectorObjectCompatibility )
	{
		if (   SVObjectAttributeClassHelper::GetAttributeData(pDataObject, "pArray", svArray)
			|| SVObjectAttributeClassHelper::GetAttributeData(pDataObject, SvOi::cBucketTag, svArray) )
		{
			SetArraySize( static_cast< int >( svArray.size() ) );
			hr = SetArrayValues(1, svArray.begin(), svArray.end());
			return hr;
		}
	}

	hr = base::SetObjectValue( pDataObject );
	return hr;
}

HRESULT SVByteValueObjectClass::SetOutputFormat(OutputFormat outputFormat)
{
	HRESULT Result = S_OK;
	switch (outputFormat)
	{
	case OutputFormat_int:
		m_outFormat = _T("%d");
		break;
	case OutputFormat_hex:
		m_outFormat = _T("0x%2.2x");
		break;
	default:
		Result = E_INVALIDARG;
		break;
	}
	return Result;
}

HRESULT SVByteValueObjectClass::SetValueAt( int iBucket, int iIndex, const VARIANT& vtValue )
{
	if ( VT_UI1 == vtValue.vt || VT_I1 == vtValue.vt )
	{
		return base::SetValueAt(iBucket, iIndex, vtValue.bVal);
	}
	assert(false);
	return S_FALSE;
}

HRESULT SVByteValueObjectClass::SetValueAt( int iBucket, int iIndex, const SVString& strValue )
{
	try
	{
		BYTE value = convertString2Byte(strValue);
		return base::SetValueAt(iBucket, iIndex, value );
	}
	catch (const SvStl::MessageContainer&)
	{
		assert(false);
		return S_FALSE;
	}
}

HRESULT SVByteValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rvtValue ) const
{
	BYTE l_byValue=0;
	_variant_t l_Temp;
	l_Temp.Attach( rvtValue );
	HRESULT hr = base::GetValueAt( iBucket, iIndex, l_byValue );
	if( S_OK == hr )
	{
		l_Temp = l_byValue;
	}
	else
	{
		l_Temp.Clear();
	}
	rvtValue = l_Temp.Detach();
	return hr;
}

HRESULT SVByteValueObjectClass::GetValueAt( int iBucket, int iIndex, SVString& rValue) const
{
	BYTE value=0;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	rValue = SvUl_SF::Format( m_outFormat.c_str(), value );

	return hr;
}

HRESULT SVByteValueObjectClass::GetValueAt( int iBucket, int iIndex, double& rdValue) const
{
	BYTE value=0;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	rdValue = (double) value;

	return hr;
}

void SVByteValueObjectClass::ValidateValue( int iBucket, int iIndex, const SVString& rValue ) const
{
	convertString2Byte(rValue.c_str());
	base::ValidateValue( iBucket, iIndex, rValue );
}

void SVByteValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVByteValueObjectType;
	DefaultValue() = 0;
	if ( m_sLegacyScriptDefaultName.empty() )
	{
		m_sLegacyScriptDefaultName = _T("bDefault");
		m_sLegacyScriptArrayName = _T("pBArray");
	}
	SetTypeName( _T("Integer8") );
	InitializeBuckets();

	SetOutputFormat(OutputFormat_hex);
}

BYTE SVByteValueObjectClass::convertString2Byte(const SVString& rValue ) const
{
	SVString Digits = SvUl_SF::ValidateString( rValue, _T("0123456789 .xXabcdefABCDEF") );
	if ( Digits == rValue )
	{
		Digits == SvUl_SF::MakeLower( Digits );
		TCHAR* p = nullptr;
		DWORD ulValue;
		if ( SVString::npos != Digits.find( 'x' ) )
		{
			ulValue = _tcstoul( Digits.c_str(), &p, 16 );
		}
		else
		{
			ulValue = _tcstoul( Digits.c_str(), &p, 10 );
		}

		if ( ulValue <= 255 )
		{
			return static_cast <BYTE> (ulValue);
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


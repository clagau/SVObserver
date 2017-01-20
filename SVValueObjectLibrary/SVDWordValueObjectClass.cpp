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

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVDWordValueObjectClass.h"
#include "SVObjectLibrary\SVClsids.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVStatusLibrary/MessageManager.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

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

HRESULT SVDWordValueObjectClass::SetOutputFormat(OutputFormat outputFormat)
{
	HRESULT Result = S_OK;
	switch (outputFormat)
	{
	case OutputFormat_int:
		m_outFormat = _T("%d");
		break;
	case OutputFormat_hex:
		m_outFormat = _T("0x%8.8x");
		break;
	default:
		Result = E_INVALIDARG;
		break;
	}
	return Result;
}

HRESULT  SVDWordValueObjectClass::SetValueAt( int iBucket, int iIndex, const VARIANT& rvtValue )
{
	if ( VT_UI4 == rvtValue.vt )
	{
		return base::SetValueAt(iBucket, iIndex, (DWORD) rvtValue.ulVal);
	}
	assert(false);
	return S_FALSE;
}

HRESULT SVDWordValueObjectClass::SetValueAt( int iBucket, int iIndex, const SVString& strValue )
{
	try
	{
		DWORD value = convertString2DWord(strValue);
		return base::SetValueAt(iBucket, iIndex, value );
	}
	catch (const SvStl::MessageContainer&)
	{
		assert(false);
		return S_FALSE;
	}
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

HRESULT SVDWordValueObjectClass::GetValueAt( int iBucket, int iIndex, SVString& rstrValue) const
{
	DWORD dwValue=0;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, dwValue);
	rstrValue = SvUl_SF::Format( m_outFormat.c_str(), dwValue );

	return hr;
}


HRESULT SVDWordValueObjectClass::GetValueAt( int iBucket, int iIndex, double& rdValue) const
{
	DWORD dwValue=0;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, dwValue);
	rdValue = (double) dwValue;

	return hr;
}

void SVDWordValueObjectClass::ValidateValue( int iBucket, int iIndex, const SVString& rValue ) const
{
	convertString2DWord(rValue.c_str());
	base::ValidateValue( iBucket, iIndex, rValue );
}

void SVDWordValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVDWordValueObjectType;
	DefaultValue() = 0;
	if ( m_sLegacyScriptDefaultName.empty() )
	{
		m_sLegacyScriptDefaultName = _T("dwDefault");
		m_sLegacyScriptArrayName = _T("pDwArray");
	}
	SetTypeName( _T("Integer32Hex") );
	InitializeBuckets();

	SetOutputFormat(OutputFormat_hex);
}

DWORD SVDWordValueObjectClass::convertString2DWord(const SVString& rValue ) const
{
	SVString Digits = SvUl_SF::ValidateString( rValue, _T("0123456789 .xXabcdefABCDEF") );
	if ( Digits == rValue )
	{
		Digits = SvUl_SF::MakeLower( Digits );
		TCHAR* p = nullptr;
		DWORD ulValue;
		if ( SVString::npos != Digits.find( 'x' ) )
		{
			ulValue = _tcstoul(Digits.c_str(), &p, 16);
		}
		else
		{
			ulValue = _tcstoul(Digits.c_str(), &p, 10);
		}

		return ulValue;
	}
	SVStringVector msgList;
	msgList.push_back(SVString(rValue));
	msgList.push_back(GetName());
	SvStl::MessageMgrStd Exception( SvStl::LogOnly );
	Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
	Exception.Throw();
	return 0; //will never reached, because the exception will throw before. But this line avoid a warning
}


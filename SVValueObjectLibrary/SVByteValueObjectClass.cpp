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

SVByteValueObjectClass::SVByteValueObjectClass( LPCTSTR ObjectName )
: SVValueObjectClass<BYTE>( ObjectName )
{
	LocalInitialize();
}

SVByteValueObjectClass::SVByteValueObjectClass( SVObjectClass* pOwner, int StringResourceID )
: SVValueObjectClass<BYTE>( pOwner, StringResourceID )
{
	LocalInitialize();
}

SVByteValueObjectClass::SVByteValueObjectClass( const SVByteValueObjectClass& rhs )
: SVValueObjectClass<BYTE>()
{
	LocalInitialize();
	*this = rhs;
}

const SVByteValueObjectClass& SVByteValueObjectClass::operator = ( const SVByteValueObjectClass& rhs )
{
	__super::operator = (rhs);
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
	_variant_t Value( GetDefaultValue() );
	
	rWriter.WriteAttribute(scDefaultTag, Value);

	rWriter.StartElement(scArrayElementsTag);
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;	

	// for all elements in the array
	for( int i = 0; i < getArraySize(); i++ )
	{
		//Make sure this is not a derived virtual method which is called
		SVByteValueObjectClass::GetValue( Value.bVal, GetLastSetIndex(), i );
		list.push_back(Value);
	}
	rWriter.WriteAttribute(scElementTag, list);
	rWriter.EndElement();

	rWriter.EndElement();
}

// This override provides the ability to correctly load script data from the legacy SVByteVectorObjectClass
HRESULT SVByteValueObjectClass::SetObjectValue( SVObjectAttributeClass* pDataObject )
{
	HRESULT Result( E_FAIL );
	
	if ( isLegacyVectorObjectCompatibility() )
	{
		SvCl::SVObjectArrayClassTemplate<ValueType> svArray;	// for default values

		if (   pDataObject->GetAttributeData( _T("pArray"), svArray )
			|| pDataObject->GetAttributeData( SvOi::cBucketTag, svArray ) )
		{
			SetArraySize( static_cast< int >( svArray.size() ) );
			Result = SetArrayValues( svArray.begin(), svArray.end(), 1 );
			return Result;
		}
	}

	Result = __super::SetObjectValue( pDataObject );
	return Result;
}

HRESULT SVByteValueObjectClass::SetOutputFormat(OutputFormat outputFormat)
{
	HRESULT Result = S_OK;
	switch (outputFormat)
	{
	case OutputFormat_int:
		setOutputFormat( _T("%d") );
		break;
	case OutputFormat_hex:
		setOutputFormat( _T("0x%2.2x") );
		break;
	default:
		Result = E_INVALIDARG;
		break;
	}
	return Result;
}

BYTE SVByteValueObjectClass::ConvertString2Type(const SVString& rValue ) const
{
	SVString Digits = SvUl_SF::ValidateString( rValue, _T("0123456789 .xXabcdefABCDEF") );
	if ( Digits == rValue )
	{
		SvUl_SF::MakeLower( Digits );
		TCHAR* p = nullptr;
		DWORD Value;
		if ( SVString::npos != Digits.find( 'x' ) )
		{
			Value = _tcstoul( Digits.c_str(), &p, 16 );
		}
		else
		{
			Value = _tcstoul( Digits.c_str(), &p, 10 );
		}

		if ( Value <= 255 )
		{
			return static_cast<BYTE> (Value);
		}
	}
	SVStringVector msgList;
	msgList.push_back(SVString(rValue));
	msgList.push_back(GetName());
	SvStl::MessageMgrStd Exception( SvStl::LogOnly );
	Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
	Exception.Throw();
	return 0; //will never be reached, because the exception will throw before. But this line avoids a warning
}

void SVByteValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVByteValueObjectType;
	DefaultValue() = 0;
	if ( m_sLegacyScriptDefaultName.empty() )
	{
		m_sLegacyScriptDefaultName = _T("bDefault");
		m_sLegacyScriptArrayName = _T("pBArray");
	}
	SetTypeName( _T("Integer8") );
	InitializeBuckets();

	SetOutputFormat( OutputFormat_hex );
}


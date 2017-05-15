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

SVDWordValueObjectClass::SVDWordValueObjectClass( LPCTSTR ObjectName )
: SVValueObjectClass<DWORD>( ObjectName ) 
{
	LocalInitialize();
}


SVDWordValueObjectClass::SVDWordValueObjectClass( SVObjectClass* pOwner, int StringResourceID )
: SVValueObjectClass<DWORD>( pOwner, StringResourceID )
{
	LocalInitialize();
}

SVDWordValueObjectClass::SVDWordValueObjectClass( const SVDWordValueObjectClass& rhs )
: SVValueObjectClass<DWORD>()
{
	LocalInitialize();
	*this = rhs;
}

const SVDWordValueObjectClass& SVDWordValueObjectClass::operator = ( const SVDWordValueObjectClass& rhs )
{
	__super::operator = (rhs);
	return *this;
}

SVDWordValueObjectClass::~SVDWordValueObjectClass()
{
}

void SVDWordValueObjectClass::Persist(SVObjectWriter& rWriter)
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	// Get the Heading (Class Info)
	SVValueObjectClass::Persist( rWriter );

	// Get the Data Values (Member Info, Values)
	_variant_t Value( GetDefaultValue() );
	Value.ChangeType(VT_UI4);
	
	rWriter.WriteAttribute(scDefaultTag, Value);

	rWriter.StartElement(scArrayElementsTag);

	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;	

	// for all elements in the array
	for( int i = 0; i < getArraySize(); i++ )
	{
		DWORD Temp( 0 );
		//Make sure this is not a derived virtual method which is called
		SVDWordValueObjectClass::GetValue( Temp, GetLastSetIndex(), i );
		Value.ulVal = static_cast<ULONG> (Temp);
		list.push_back(Value);
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
		setOutputFormat( _T("%d") );
		break;
	case OutputFormat_hex:
		setOutputFormat( _T("0x%8.8x") );
		break;
	default:
		Result = E_INVALIDARG;
		break;
	}
	return Result;
}

DWORD SVDWordValueObjectClass::ConvertString2Type( const SVString& rValue ) const
{
	SVString Digits = SvUl_SF::ValidateString( rValue, _T("0123456789 .xXabcdefABCDEF") );
	if ( Digits == rValue )
	{
		SvUl_SF::MakeLower( Digits );
		TCHAR* p = nullptr;
		DWORD Value( 0 );
		if ( SVString::npos != Digits.find( 'x' ) )
		{
			Value = _tcstoul(Digits.c_str(), &p, 16);
		}
		else
		{
			Value = _tcstoul(Digits.c_str(), &p, 10);
		}

		return Value;
	}
	SVStringVector msgList;
	msgList.push_back(SVString(rValue));
	msgList.push_back(GetName());
	SvStl::MessageMgrStd Exception( SvStl::LogOnly );
	Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
	Exception.Throw();
	return 0; //will never be reached, because the exception will throw before. But this line avoids a warning
}

void SVDWordValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVDWordValueObjectType;
	DefaultValue() = 0;
	if ( m_sLegacyScriptDefaultName.empty() )
	{
		m_sLegacyScriptDefaultName = _T("dwDefault");
		m_sLegacyScriptArrayName = _T("pDwArray");
	}
	SetTypeName( _T("Integer32Hex") );
	InitializeBuckets();

	SetOutputFormat( OutputFormat_hex );
}



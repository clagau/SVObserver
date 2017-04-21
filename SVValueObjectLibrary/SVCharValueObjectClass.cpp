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

SVCharValueObjectClass::SVCharValueObjectClass( LPCTSTR ObjectName )
: SVValueObjectClass<TCHAR>( ObjectName )
{
	LocalInitialize();
}


SVCharValueObjectClass::SVCharValueObjectClass( SVObjectClass* pOwner, int StringResourceID )
: SVValueObjectClass<TCHAR>( pOwner, StringResourceID )
{
	LocalInitialize();
}

SVCharValueObjectClass::SVCharValueObjectClass( const SVCharValueObjectClass& rhs )
: SVValueObjectClass<TCHAR>()
{
	LocalInitialize();
	*this = rhs;
}

const SVCharValueObjectClass& SVCharValueObjectClass::operator = ( const SVCharValueObjectClass& rhs )
{
	__super::operator = (rhs);
	return *this;
}

SVCharValueObjectClass::~SVCharValueObjectClass()
{
}

void SVCharValueObjectClass::Persist(SVObjectWriter& rWriter)
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
		SVCharValueObjectClass::GetValue( Value.cVal, GetLastSetIndex(), i );
		list.push_back(Value);
	}
	rWriter.WriteAttribute(scElementTag, list);
	rWriter.EndElement();

	rWriter.EndElement();
}

TCHAR SVCharValueObjectClass::ConvertString2Type( const SVString& rValue ) const
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
			SvUl_SF::MakeLower(Digits);
			TCHAR* p = nullptr;
			long Value;
			if ( SVString::npos != Digits.find( 'x' ) )
			{
				Value = _tcstol(Digits.c_str(), &p, 16);
			}
			else
			{
				Value = _tcstol(Digits.c_str(), &p, 10);
			}

			if ( 0 <= Value && 255 >= Value )
			{
				return static_cast<TCHAR> (Value);
			}
		}
	}
	SVStringVector msgList;
	msgList.push_back(SVString(rValue));
	msgList.push_back(GetName());
	SvStl::MessageMgrStd Exception( SvStl::LogOnly );
	Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
	Exception.Throw();
	return 0; //will never reached, because the exception will throw before. But this line avoids a warning
}

void SVCharValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVCharValueObjectType;
	DefaultValue() = 0;
	if ( m_sLegacyScriptDefaultName.empty() )
	{
		m_sLegacyScriptDefaultName = _T("bDefault");
		m_sLegacyScriptArrayName = _T("pBArray");
	}
	SetTypeName( _T("Char") );
	setOutputFormat( _T("%c") );
	InitializeBuckets();
}



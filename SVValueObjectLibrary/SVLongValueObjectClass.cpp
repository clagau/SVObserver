//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLongValueObjectClass
//* .File Name       : $Workfile:   SVLongValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   30 Jul 2013 12:29:28  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVLongValueObjectClass.h"
#include "SVObjectLibrary\SVClsids.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVStatusLibrary/MessageManager.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS(SVLongValueObjectClass, SVLongValueObjectClassGuid);

SVLongValueObjectClass::SVLongValueObjectClass( LPCTSTR ObjectName )
: SVValueObjectClass<long>( ObjectName )
{
	LocalInitialize();
}

SVLongValueObjectClass::SVLongValueObjectClass(SVObjectClass* pOwner, int StringResourceID)
: SVValueObjectClass<long>( pOwner, StringResourceID )
{
	LocalInitialize();
}

SVLongValueObjectClass::SVLongValueObjectClass( const SVLongValueObjectClass& rhs )
: SVValueObjectClass<long>()
{
	LocalInitialize();
	*this = rhs;
}

const SVLongValueObjectClass& SVLongValueObjectClass::operator = (const SVLongValueObjectClass& rhs)
{
	__super::operator = (rhs);
	return *this;
}

SVLongValueObjectClass::~SVLongValueObjectClass()
{
}

void SVLongValueObjectClass::Persist(SVObjectWriter& rWriter)
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
		SVLongValueObjectClass::GetValue( Value.lVal, GetLastSetIndex(), i );
		list.push_back(Value);
	}
	rWriter.WriteAttribute(scElementTag, list);
	rWriter.EndElement();

	rWriter.EndElement();
}

long SVLongValueObjectClass::ConvertString2Type( const SVString& rValue ) const
{
	SVString Digits = SvUl_SF::ValidateString( rValue, _T("-0123456789 .xXabcdefABCDEF") );
	if( Digits == rValue )
	{
		SvUl_SF::MakeLower(Digits);
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

		return lValue;
	}
	SVStringVector msgList;
	msgList.push_back( rValue );
	msgList.push_back( GetName() );
	SvStl::MessageMgrStd Exception( SvStl::LogOnly );
	Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
	Exception.Throw();
	return 0; //will never reached, because the exception will throw before. But this line avoid a warning
}

void SVLongValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVLongValueObjectType;
	DefaultValue() = 0;
	if ( m_sLegacyScriptDefaultName.empty() )
	{
		m_sLegacyScriptDefaultName = _T("lDefault");
		m_sLegacyScriptArrayName = _T("pLArray");
	}
	SetTypeName( _T("Integer32") );

	setOutputFormat( _T("%d") );
	InitializeBuckets();
}

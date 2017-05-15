//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInt64ValueObjectClass
//* .File Name       : $Workfile:   SVInt64ValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   30 Jul 2013 12:24:32  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVInt64ValueObjectClass.h"
#include "SVObjectLibrary\SVClsids.h"
#include "SVObjectLibrary/SVToolsetScriptTags.h"
#include "ObjectInterfaces/TextDefineSvOi.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS(SVInt64ValueObjectClass, SVInt64ValueObjectClassGuid);

SVInt64ValueObjectClass::SVInt64ValueObjectClass( LPCTSTR ObjectName )
: SVValueObjectClass<__int64>( ObjectName )
{
	LocalInitialize();
}

SVInt64ValueObjectClass::SVInt64ValueObjectClass( SVObjectClass* pOwner, int StringResourceID )
: SVValueObjectClass<__int64>( pOwner, StringResourceID )
{
	LocalInitialize();
}

SVInt64ValueObjectClass::~SVInt64ValueObjectClass()
{
}

const SVInt64ValueObjectClass& SVInt64ValueObjectClass::operator = (const SVInt64ValueObjectClass& rhs)
{
	__super::operator = (rhs);
	return *this;
}

void SVInt64ValueObjectClass::Persist(SVObjectWriter& rWriter)
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
		SVInt64ValueObjectClass::GetValue( Value.llVal, GetLastSetIndex(), i );
		list.push_back(Value);
	}
	rWriter.WriteAttribute(scElementTag, list);
	rWriter.EndElement();

	rWriter.EndElement();
}

__int64 SVInt64ValueObjectClass::ConvertString2Type( const SVString& rValue ) const
{
	__int64 Result(0LL);
	SVString Digits = SvUl_SF::ValidateString( rValue, _T("0123456789- ") );
	if( Digits == rValue )
	{
		return _atoi64( Digits.c_str() );
	}

	SVStringVector msgList;
	msgList.push_back( rValue );
	msgList.push_back( GetName() );
	SvStl::MessageMgrStd Exception( SvStl::LogOnly );
	Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
	Exception.Throw();

	return Result;
}

void SVInt64ValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVInt64ValueObjectType;
	DefaultValue() = 0;
	m_sLegacyScriptDefaultName = SvOi::cDefaultTag;
	m_sLegacyScriptArrayName = SvOi::cBucketTag;
	SetTypeName( _T("Integer64") );

	setOutputFormat( _T("%I64d") );
	InitializeBuckets();
}


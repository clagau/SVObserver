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

SVPointValueObjectClass::SVPointValueObjectClass( LPCTSTR ObjectName )
: SVValueObjectClass<SVPOINT>( ObjectName )
{
	LocalInitialize();
}


SVPointValueObjectClass::SVPointValueObjectClass( SVObjectClass* pOwner, int StringResourceID )
: SVValueObjectClass<SVPOINT>( pOwner, StringResourceID )
{
	LocalInitialize();
}

SVPointValueObjectClass::SVPointValueObjectClass( const SVPointValueObjectClass& rhs )
: SVValueObjectClass<SVPOINT>()
{
	LocalInitialize();
	*this = rhs;
}

const SVPointValueObjectClass& SVPointValueObjectClass::operator = (const SVPointValueObjectClass& rhs)
{
	__super::operator = (rhs);
	return *this;
}

SVPointValueObjectClass::~SVPointValueObjectClass()
{
}

_variant_t SVPointValueObjectClass::ValueType2Variant( const SVPOINT& rValue ) const
{
	_variant_t Result;

	Result.SetString( ConvertType2String(rValue).c_str() );

	return Result;
}

SVPOINT SVPointValueObjectClass::Variant2ValueType( const _variant_t& rValue ) const
{
	SVPOINT Result;

	if( VT_BSTR == rValue.vt )
	{
		try
		{
			SVString Value = SvUl_SF::createSVString( rValue );
			Result = ConvertString2Type( Value );
		}
		catch( const SvStl::MessageContainer& )
		{
		}
	}

	return Result;
}

SVPOINT SVPointValueObjectClass::ConvertString2Type( const SVString& rValue ) const
{
	SVString LegalChars = SvUl_SF::ValidateString( rValue, _T("0123456789()-, ") );	// only integers
	if ( LegalChars == rValue )
	{
		SvUl_SF::RemoveCharacters( LegalChars, _T("()") );
		size_t Pos = LegalChars.find(_T(','));
		if ( SVString::npos != Pos )
		{
			SVString XValue = SvUl_SF::Left( LegalChars, Pos );
			SVString YValue = SvUl_SF::Mid( LegalChars, Pos + 1 );
			return SVPOINT( atol(XValue.c_str()), atol(YValue.c_str()) );
		}
	}
	SVStringVector msgList;
	msgList.push_back(SVString(rValue));
	msgList.push_back(GetName());
	SvStl::MessageMgrStd Exception( SvStl::LogOnly );
	Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
	Exception.Throw();
	return SVPOINT(); //will never reached, because the exception will throw before. But this line avoid a warning
}

SVString SVPointValueObjectClass::ConvertType2String( const SVPOINT& rValue ) const
{
	SVString Result;
	//This is faster than SvUl_SF::Format
	TCHAR Text[100];
	sprintf_s(Text, 100, _T("( %d, %d)"), rValue.x(), rValue.y());
	Result = Text;
	return Result;
}

void SVPointValueObjectClass::WriteValues(SVObjectWriter& rWriter)
{
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;

	// Get the Data Values (Member Info, Values)
	SVString TempValue;
	_variant_t Value;
	Value.Clear();

	// for all elements in the array
	for (int i = 0; i < getArraySize(); i++)
	{
		SVPOINT PointValue;
		//Make sure this is not a derived virtual method which is called
		SVPointValueObjectClass::GetValue(PointValue, GetLastSetIndex(), i);
		TempValue = SvUl_SF::Format(_T("%d, %d"), PointValue.x(), PointValue.y());
		Value.SetString(TempValue.c_str());
		list.push_back(Value);
		Value.Clear();
	}
	rWriter.WriteAttribute(scElementTag, list);
}

void SVPointValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVPointValueObjectType;
	SetObjectAttributesAllowed( SvOi::SV_VIEWABLE | SvOi::SV_ARCHIVABLE | SvOi::SV_EMBEDABLE | SvOi::SV_PRINTABLE | SvOi::SV_DD_VALUE, SvOi::SetAttributeType::OverwriteAttribute );

	SetTypeName( _T("Point") );
	InitializeBuckets();
}

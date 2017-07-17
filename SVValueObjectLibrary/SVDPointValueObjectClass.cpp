//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDPointValueObjectClass
//* .File Name       : $Workfile:   SVDPointValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Aug 2013 11:21:42  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVDPointValueObjectClass.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVObjectLibrary\SVClsids.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVUtilityLibrary/SetBits.h"
#include "SVStatusLibrary/MessageManager.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVDPointValueObjectClass, SVDPointValueObjectClassGuid );

SVDPointValueObjectClass::SVDPointValueObjectClass( LPCTSTR ObjectName )
: SVValueObjectClass<SVDPointClass>( ObjectName )
{
	LocalInitialize();
}

SVDPointValueObjectClass::SVDPointValueObjectClass( SVObjectClass* POwner, int StringResourceID )
: SVValueObjectClass<SVDPointClass>( POwner, StringResourceID ) 
{
	LocalInitialize();
}

const SVDPointValueObjectClass& SVDPointValueObjectClass::operator = (const SVDPointValueObjectClass& rhs)
{
	__super::operator = (rhs);
	return *this;
}
  
SVDPointValueObjectClass::~SVDPointValueObjectClass()
{
}

_variant_t SVDPointValueObjectClass::ValueType2Variant( const SVDPointClass& rValue ) const
{
	_variant_t Result;

	Result.SetString( ConvertType2String(rValue).c_str() );

	return Result;
}

SVDPointClass SVDPointValueObjectClass::Variant2ValueType( const _variant_t& rValue ) const
{
	SVDPointClass Result;

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

SVDPointClass SVDPointValueObjectClass::ConvertString2Type( const SVString& rValue ) const
{
	SVString LegalChars = SvUl_SF::ValidateString( rValue, _T("0123456789()-., ") );	// floats
	if ( LegalChars == rValue )
	{
		SvUl_SF::RemoveCharacters( LegalChars, _T("()") );
		size_t Pos = LegalChars.find(_T(','));
		if ( SVString::npos != Pos )
		{
			SVString sX = SvUl_SF::Left( LegalChars, Pos );
			SVString sY = SvUl_SF::Mid( LegalChars, Pos + 1 );
			return SVDPointClass( atof(sX.c_str()), atof(sY.c_str()) );
		}
	}
	SVStringVector msgList;
	msgList.push_back( rValue );
	msgList.push_back( GetName() );
	SvStl::MessageMgrStd Exception( SvStl::LogOnly );
	Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
	Exception.Throw();
	return SVDPointClass(); //will never reached, because the exception will throw before. But this line avoid a warning
}

SVString SVDPointValueObjectClass::ConvertType2String( const SVDPointClass& rValue ) const
{
	SVString Result;
	//This is faster than SvUl_SF::Format
	TCHAR Text[100];
	sprintf_s(Text, 100, _T("( %lf, %lf )"), rValue.x, rValue.y);
	Result = Text;
	return Result;
}

HRESULT SVDPointValueObjectClass::CopyToMemoryBlock(BYTE* pMemoryBlock, DWORD MemByteSize, int Index /* = -1*/) const
{
	HRESULT Result = ValidateMemoryBlockParameters(pMemoryBlock, MemByteSize, Index);

	if (S_OK == Result)
	{
		SVDPointClass Value;
		SVDPointValueObjectClass::GetValue(Value, Index);

		memcpy(pMemoryBlock, &Value.x, sizeof(Value.x));
		memcpy(pMemoryBlock + sizeof(Value.x), &Value.y, sizeof(Value.y));
	}

	return Result;
}

void SVDPointValueObjectClass::WriteValues(SVObjectWriter& rWriter)
{
	// Get the Data Values (Member Info, Values)
	SVString tmp;
	_variant_t value;
	value.Clear();

	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;

	// for all elements in the array
	for (int i = 0; i < getArraySize(); i++)
	{
		SVDPointClass Value;
		//Make sure this is not a derived virtual method which is called
		SVDPointValueObjectClass::GetValue(Value, i);
		tmp = SvUl_SF::Format(_T("%lf, %lf"), Value.x, Value.y);
		value.SetString(tmp.c_str());
		list.push_back(value);
		value.Clear();
	}
	rWriter.WriteAttribute(scElementTag, list);
}

void SVDPointValueObjectClass::WriteDefaultValues(SVObjectWriter& rWriter)
{
	SVString tmp;
	tmp = SvUl_SF::Format(_T("%lf, %lf"), GetDefaultValue().x, GetDefaultValue().y);
	_variant_t value;
	value.SetString(tmp.c_str());
	rWriter.WriteAttribute(scDefaultTag, value);
}

void SVDPointValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVDPointValueObjectType;

	SetObjectAttributesAllowed( SvOi::SV_VIEWABLE | SvOi::SV_ARCHIVABLE | SvOi::SV_EMBEDABLE | SvOi::SV_PRINTABLE | SvOi::SV_DD_VALUE, SvOi::SetAttributeType::OverwriteAttribute );

	SetTypeName( _T("Point") );

	InitializeBuckets();
}


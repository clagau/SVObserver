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
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvVol
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVDPointValueObjectClass, SVDPointValueObjectClassGuid );

SVDPointValueObjectClass::SVDPointValueObjectClass( LPCTSTR ObjectName )
: SVValueObjectClass<SVPoint<double>>( ObjectName )
{
	LocalInitialize();
}

SVDPointValueObjectClass::SVDPointValueObjectClass( SVObjectClass* POwner, int StringResourceID )
: SVValueObjectClass<SVPoint<double>>( POwner, StringResourceID )
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

_variant_t SVDPointValueObjectClass::ValueType2Variant( const SVPoint<double>& rValue ) const
{
	_variant_t Result;

	Result.SetString( ConvertType2String(rValue).c_str() );

	return Result;
}

SVPoint<double> SVDPointValueObjectClass::Variant2ValueType( const _variant_t& rValue ) const
{
	SVPoint<double> Result;

	if( VT_BSTR == rValue.vt )
	{
		try
		{
			std::string Value = SvUl::createStdString( rValue );
			Result = ConvertString2Type( Value );
		}
		catch( const SvStl::MessageContainer& )
		{
		}
	}

	return Result;
}

SVPoint<double> SVDPointValueObjectClass::ConvertString2Type( const std::string& rValue ) const
{
	std::string LegalChars = SvUl::ValidateString( rValue, _T("0123456789()-., ") );	// floats
	if ( LegalChars == rValue )
	{
		SvUl::RemoveCharacters( LegalChars, _T("()") );
		size_t Pos = LegalChars.find(_T(','));
		if ( std::string::npos != Pos )
		{
			std::string sX = SvUl::Left( LegalChars, Pos );
			std::string sY = SvUl::Mid( LegalChars, Pos + 1 );
			return SVPoint<double>( atof(sX.c_str()), atof(sY.c_str()) );
		}
	}
	SvDef::StringVector msgList;
	msgList.push_back( rValue );
	msgList.push_back( GetName() );
	SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
	Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
	Exception.Throw();
	return SVPoint<double>(); //will never reached, because the exception will throw before. But this line avoid a warning
}

HRESULT SVDPointValueObjectClass::CopyToMemoryBlock(BYTE* pMemoryBlock, DWORD MemByteSize, int Index /* = -1*/) const
{
	HRESULT Result = ValidateMemoryBlockParameters(pMemoryBlock, MemByteSize, Index);

	if (S_OK == Result)
	{
		SVPoint<double> Value;
		SVDPointValueObjectClass::GetValue(Value, Index);

		memcpy(pMemoryBlock, &Value.m_x, sizeof(Value.m_x));
		memcpy(pMemoryBlock + sizeof(Value.m_x), &Value.m_y, sizeof(Value.m_y));
	}

	return Result;
}

void SVDPointValueObjectClass::WriteValues(SvOi::IObjectWriter& rWriter)
{
	// Get the Data Values (Member Info, Values)
	std::string tmp;
	_variant_t value;
	value.Clear();

	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;

	// for all elements in the array
	for (int i = 0; i < getArraySize(); i++)
	{
		SVPoint<double> Value;
		//Make sure this is not a derived virtual method which is called
		SVDPointValueObjectClass::GetValue(Value, i);
		tmp = SvUl::Format(_T("%lf, %lf"), Value.m_x, Value.m_y);
		value.SetString(tmp.c_str());
		list.push_back(value);
		value.Clear();
	}
	rWriter.WriteAttribute(scElementTag, list);
}

void SVDPointValueObjectClass::WriteDefaultValues(SvOi::IObjectWriter& rWriter)
{
	std::string tmp;
	tmp = SvUl::Format(_T("%lf, %lf"), GetDefaultValue().m_x, GetDefaultValue().m_y);
	_variant_t value;
	value.SetString(tmp.c_str());
	rWriter.WriteAttribute(scDefaultTag, value);
}

void SVDPointValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVDPointValueObjectType;

	SetObjectAttributesAllowed( SvPb::viewable | SvPb::archivable | SvPb::embedable | SvPb::printable | SvPb::dataDefinitionValue, SvOi::SetAttributeType::OverwriteAttribute );

	SetTypeName( _T("Point") );

	init();
}

} //namespace SvVol

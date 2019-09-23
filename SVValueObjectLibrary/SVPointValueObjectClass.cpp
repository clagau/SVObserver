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

SV_IMPLEMENT_CLASS(SVPointValueObjectClass, SVPointValueObjectClassGuid);

SVPointValueObjectClass::SVPointValueObjectClass( LPCTSTR ObjectName )
: SVValueObjectClass<SVPoint<long>>( ObjectName )
{
	LocalInitialize();
}


SVPointValueObjectClass::SVPointValueObjectClass( SVObjectClass* pOwner, int StringResourceID )
: SVValueObjectClass<SVPoint<long>>( pOwner, StringResourceID )
{
	LocalInitialize();
}

SVPointValueObjectClass::SVPointValueObjectClass( const SVPointValueObjectClass& rhs )
: SVValueObjectClass<SVPoint<long>>()
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

_variant_t SVPointValueObjectClass::ValueType2Variant( const SVPoint<long>& rValue ) const
{
	_variant_t Result;

	Result.SetString( ConvertType2String(rValue).c_str() );

	return Result;
}

SVPoint<long> SVPointValueObjectClass::Variant2ValueType( const _variant_t& rValue ) const
{
	SVPoint<long> Result;

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

SVPoint<long> SVPointValueObjectClass::ConvertString2Type( const std::string& rValue ) const
{
	std::string LegalChars = SvUl::ValidateString( rValue, _T("0123456789()-, ") );	// only integers
	if ( LegalChars == rValue )
	{
		SvUl::RemoveCharacters( LegalChars, _T("()") );
		size_t Pos = LegalChars.find(_T(','));
		if ( std::string::npos != Pos )
		{
			std::string XValue = SvUl::Left( LegalChars, Pos );
			std::string YValue = SvUl::Mid( LegalChars, Pos + 1 );
			return SVPoint<long>( atol(XValue.c_str()), atol(YValue.c_str()) );
		}
	}
	SvDef::StringVector msgList;
	msgList.push_back(std::string(rValue));
	msgList.push_back(GetName());
	SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
	Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
	Exception.Throw();
	return SVPoint<long>(); //will never reached, because the exception will throw before. But this line avoid a warning
}

long SVPointValueObjectClass::GetByteSize(bool useResultSize) const
{
	long result(0L);

	//Attribute must be set otherwise do not consider for memory requirements
	if (0 != ObjectAttributesAllowed())
	{
		//SVPointValueObject has 2 long values for each point
		long numberOfElements = useResultSize ? getResultSize() : getArraySize();
		result = 2 * sizeof(double) * numberOfElements;
		//If the value object is an array the first value shall contain the result size which is variable
		if (isArray())
		{
			result += sizeof(int);
		}
	}

	return result;
}


long SVPointValueObjectClass::CopyToMemoryBlock(BYTE* pMemoryBlock, long MemByteSize) const
{
	long result {GetByteSize(false)};

	//Attribute must be set otherwise do not consider for memory requirements
	if (0 != ObjectAttributesAllowed() && -1 != GetMemOffset())
	{
		result = GetByteSize(false);
		if (result <= MemByteSize)
		{
			BYTE* pMemoryLocation = pMemoryBlock + GetMemOffset();
			if (isArray())
			{
				//For arrays we need to write the result size at the start of the memory as an int
				*(reinterpret_cast<int*> (pMemoryLocation)) = getResultSize();
				pMemoryLocation += sizeof(int);
			}
			for (int i = 0; i < getResultSize(); ++i)
			{
				SVPoint<long> Value;
				SVPointValueObjectClass::GetValue(Value, i);
				memcpy(pMemoryLocation, &Value.m_x, sizeof(Value.m_x));
				pMemoryLocation += sizeof(Value.m_x);
				memcpy(pMemoryLocation, &Value.m_y, sizeof(Value.m_y));
				pMemoryLocation += sizeof(Value.m_y);
			}
		}
		else
		{
			result = -1L;
		}
	}
	else
	{
		result = 0L;
	}

	return result;
}

void SVPointValueObjectClass::WriteValues(SvOi::IObjectWriter& rWriter)
{
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	std::vector<_variant_t> list;

	// Get the Data Values (Member Info, Values)
	std::string TempValue;
	_variant_t Value;
	Value.Clear();

	// for all elements in the array
	for (int i = 0; i < getArraySize(); i++)
	{
		SVPoint<long> PointValue;
		//Make sure this is not a derived virtual method which is called
		SVPointValueObjectClass::GetValue(PointValue, i);
		TempValue = SvUl::Format(_T("%d, %d"), PointValue.m_x, PointValue.m_y);
		Value.SetString(TempValue.c_str());
		list.push_back(Value);
		Value.Clear();
	}
	rWriter.WriteAttribute(scElementTag, list);
}

void SVPointValueObjectClass::WriteDefaultValues(SvOi::IObjectWriter& rWriter)
{
	std::string TempValue;
	TempValue = SvUl::Format(_T("%d, %d"), GetDefaultValue().m_x, GetDefaultValue().m_y);
	_variant_t Value;
	Value.SetString(TempValue.c_str());
	rWriter.WriteAttribute(scDefaultTag, Value);
}

void SVPointValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVPointValueObjectType;
	SetObjectAttributesAllowed( SvPb::viewable | SvPb::archivable | SvPb::embedable | SvPb::printable | SvPb::dataDefinitionValue, SvOi::SetAttributeType::OverwriteAttribute );

	SetTypeName( _T("Point") );
	init();
}

} //namespace SvVol
